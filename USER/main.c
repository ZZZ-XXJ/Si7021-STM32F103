#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "Si7021.h"


 int main(void)
 {
	u8 i=0;

	delay_init();			//��ʱ������ʼ��	
	uart_init(115200);		//����1��ʼ��������Ϊ115200
	IIC_Init();				//IIC��ʼ�� 
	 
	while(1)
	{	//ÿ50ms��ȡһ������
		measure_Si7021();

		i++;
		delay_ms(50);
		if(i==20)
		{	//����ÿ1s��ӡһ������
			printf("\r\nTemp:%.2f\r\n",TEMP_buf);//��ӡ�¶����ݣ�������λС��
			printf("Humi:%.2f\r\n",Humi_buf);//��ӡʪ�����ݣ�������λС��	
			i=0;
		}
	}
}
