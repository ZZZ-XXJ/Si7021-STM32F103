#include "SI7021.h"
#include "delay.h"
#include "usart.h"

//�ṹ�嶨��
_si7021_value si7021;//���ݻ������ṹ��
_si7021_filter si7021_filter;//ƽ��ֵ�˲����ṹ��

//�������壬�˲�������ս������ʹ�ô��ڴ�ӡ
float TEMP_buf,Humi_buf;

//�������ƣ�single_write_Si7021
//�������ܣ����ֽ�д�봫����
//����������
//�� �� ֵ��
void single_write_Si7021(u8 REG_address)
{
	IIC_Start();
	
	IIC_Send_Byte((SLAVE_ADDR<<1)|0);
	IIC_Wait_Ack();
	
	IIC_Send_Byte(REG_address);
	IIC_Wait_Ack();
	
	IIC_Stop();
}

//�������ƣ�Multiple_read_Si7021
//�������ܣ����ֽڶ�ȡ������
//����������
//�� �� ֵ��
void Multiple_read_Si7021(u8 REG_address, u16 *value)
{
	u8 Si7021_BUF[2]={0};
	
	IIC_Start();
	
	IIC_Send_Byte((SLAVE_ADDR<<1)|0);
	IIC_Wait_Ack();
	
	IIC_Send_Byte(REG_address);
	IIC_Wait_Ack();
	
	delay_ms(19);
	
	IIC_Start();
	IIC_Send_Byte((SLAVE_ADDR<<1)|1);
	IIC_Wait_Ack();	
	Si7021_BUF[0] = IIC_Read_Byte(1);
	Si7021_BUF[1] = IIC_Read_Byte(0);
	
	IIC_Stop();
	
	*value=((Si7021_BUF[0]<<8)+Si7021_BUF[1]);
}

//�������ƣ�measure_si7021
//�������ܣ�NO HOLD MASTERģʽ�¶�ȡ��ʪ�� 
//������������
//�� �� ֵ����
void measure_Si7021(void)
{
	//�����������
	u16 TEMP,HUMI;
	u8 curI;
	
	//��ȡ�¶�
	Multiple_read_Si7021(TEMP_NOHOLD_MASTER,&TEMP);
	si7021.temp=(((((float)TEMP)*175.72f)/65536.0f) - 46.85f);//��ԭʼ�¶����ݼ���Ϊʵ���¶����ݲ����ݸ�����������λ ��
//	TEMP_buf=(((((float)TEMP)*175.72f)/65536.0f) - 46.85f);
	
	Multiple_read_Si7021(HUMI_NOHOLD_MASTER,&HUMI);
	si7021.humi=(((((float)HUMI)*125.0f)/65535.0f) - 6.0f);//��ԭʼʪ�����ݼ���Ϊʵ��ʪ�����ݲ����ݸ�����������λ %RH
//	Humi_buf=(((((float)HUMI)*125.0f)/65535.0f) - 6.0f);
	
	//����Ϊƽ��ֵ�˲����룬ѭ������10�ε����ݣ�����һ��measure_Si7021()�ʹ�һ��
	if(MEAN_NUM > si7021_filter.curI)//��MEAN_NUM==10ʱ�����10�ζ�ȡ
	{
		si7021_filter.tBufs[si7021_filter.curI] = si7021.temp;
		si7021_filter.hBufs[si7021_filter.curI] = si7021.humi;

		si7021_filter.curI++;
	}
	else
	{
		si7021_filter.curI = 0;

		si7021_filter.tBufs[si7021_filter.curI] = si7021.temp;
		si7021_filter.hBufs[si7021_filter.curI] = si7021.humi;

		si7021_filter.curI++;
	}
	
	if(MEAN_NUM <= si7021_filter.curI) 
    {
        si7021_filter.thAmount = MEAN_NUM;
    }

	//�ж��Ƿ����ѭ��
    if(0 == si7021_filter.thAmount) 
    {
        //����ɼ���10������֮ǰ��ƽ��ֵ
        for(curI = 0; curI < si7021_filter.curI; curI++)
        {
            si7021.temp += si7021_filter.tBufs[curI];
            si7021.humi += si7021_filter.hBufs[curI];
        }

        si7021.temp = si7021.temp / si7021_filter.curI;
        si7021.humi = si7021.humi / si7021_filter.curI; 
        
        TEMP_buf = si7021.temp;
        Humi_buf = si7021.humi;
    }
    else if(MEAN_NUM == si7021_filter.thAmount) 
    {
        //����ɼ���10������֮���ƽ��ֵ
        for(curI = 0; curI < si7021_filter.thAmount; curI++) 
        {
            si7021.temp += si7021_filter.tBufs[curI];
            si7021.humi += si7021_filter.hBufs[curI];
        }

        si7021.temp = si7021.temp / si7021_filter.thAmount; 
        si7021.humi = si7021.humi / si7021_filter.thAmount; 
        
        TEMP_buf = si7021.temp; 
        Humi_buf = si7021.humi; 
    }
}
