#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "Si7021.h"


 int main(void)
 {
	u8 i=0;

	delay_init();			//延时函数初始化	
	uart_init(115200);		//串口1初始化波特率为115200
	IIC_Init();				//IIC初始化 
	 
	while(1)
	{	//每50ms读取一次数据
		measure_Si7021();

		i++;
		delay_ms(50);
		if(i==20)
		{	//串口每1s打印一次数据
			printf("\r\nTemp:%.2f\r\n",TEMP_buf);//打印温度数据，保留两位小数
			printf("Humi:%.2f\r\n",Humi_buf);//打印湿度数据，保留两位小数	
			i=0;
		}
	}
}
