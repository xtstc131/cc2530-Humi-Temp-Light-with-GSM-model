#include <stdio.h>
#include "ioCC2530.h"




#define st(x)      do { x } while (__LINE__ == -1)
#define HAL_IO_SET(port, pin, val)        HAL_IO_SET_PREP(port, pin, val)
#define HAL_IO_SET_PREP(port, pin, val)   st( P##port##_##pin## = val; )
#define HAL_IO_GET(port, pin)   HAL_IO_GET_PREP( port,pin)
#define HAL_IO_GET_PREP(port, pin)   ( P##port##_##pin)

#define LIGHT_SCK_0() 	HAL_IO_SET(1,4,0)
#define LIGHT_SCK_1() 	HAL_IO_SET(1,4,1)
#define LIGHT_DTA_0() 	HAL_IO_SET(1,3,0)
#define LIGHT_DTA_1() 	HAL_IO_SET(1,3,1)

#define LIGHT_DTA()     HAL_IO_GET(1,3)

#define SDA_W() (P1DIR |=BV(3)  )
#define SDA_R() (P1DIR &=~BV(3) )
#define delay() {asm("nop");asm("nop");asm("nop");asm("nop");}



			/****command********/
#define DPOWR  0X00         //�ϵ�
#define POWER  0X01         //SHANG DIAN
#define RESET_L    0X07         //CHONG ZHI
#define CHMODE  0X10        //����H�ֱ���
#define CHMODE2 0X11         //����H�ֱ���2
#define CLMODE   0X13           //�����ͷֱ�
#define HMODE   0X20           //һ��H�ֱ���
#define HMODE2 0X21          //һ��H�ֱ���2
#define LMODE    0X23           //һ��L�ֱ���ģʽ

void delay_nms(int n)
{	
	while(n--)
		{
	asm("nop");asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");asm("nop");
}
}






/****************************
����I2C


*****************************/

void start_i2c(void)
{
	SDA_W() ;
	LIGHT_DTA_1();
	delay() ;
	LIGHT_SCK_1() ;
	delay() ;
	LIGHT_DTA_0() ;
	delay() ;
	LIGHT_SCK_0() ;
	delay() ;
	
}


/********************************

����I2C


********************************/

void stop_i2c(void)
{
	SDA_W() ;
	LIGHT_DTA_0() ;
	delay() ;
	LIGHT_SCK_1() ;
	delay() ;
	LIGHT_DTA_1();
	delay() ;
	
}



/*********************************

�ֽڷ��ͳɹ��յ�ACK ����0


********************************/
static int  send_byte(unsigned char c)
{
	unsigned char i,error=0;
	//SDA_W() ;
	for(i=0x80;i>0;i/=2)
		{
			if(i&c)
				LIGHT_DTA_1();
			else
				LIGHT_DTA_0();
			LIGHT_SCK_0();          //���˸о�Ҫ��ҪҲ�У���ʱ������
			delay();
			LIGHT_SCK_1() ;       //����ʱ����Ϊ��֪ͨ�豸��ʼ�վ�����
			delay();
			LIGHT_SCK_0() ;   
			}
	delay();
	LIGHT_DTA_1();
	SDA_R();
	delay();
	LIGHT_SCK_1() ; 
	delay();
	if(LIGHT_DTA())
		error=1;
		delay();
	LIGHT_SCK_0();
	return error;
	
	
}


/***********************

�������ݺ���ack 

*******************************/





static unsigned 
read_byte(int ack)
{
	int  i;
	unsigned char val=0;
	LIGHT_DTA_1();
	SDA_R() ;
	for(i=0x80;i>0;i/=2)
		{
			LIGHT_SCK_1();
			delay();
			if(LIGHT_DTA())
				val=(val | i);
			
			LIGHT_SCK_0();
			delay();
		}
	
	SDA_W();
	if(ack)
		LIGHT_DTA_0();
	LIGHT_SCK_1();
	delay();
	LIGHT_SCK_0();
	LIGHT_DTA_1();
	return val;
	
	
	
}


/**************************
��������ǿ��

***************************/


unsigned char 
get_light(void)
{	
	unsigned char ack1=1;
	unsigned char ack2=1;
	unsigned char ack3=1;
	unsigned char ack4 =1;
	unsigned char t0;
	unsigned char t1;
	unsigned char t;
	start_i2c();
	send_byte(0x01);   //��ʼ��
	delay();
	while(ack4)
		{
			ack4=send_byte(0x01);                     //power on
		}
	while(ack1)
	{
		ack1=send_byte(0x46);               //write address
	}
	while(ack2)
		{
			ack2=send_byte(0x10);              //h lianxu     
		}
	stop_i2c();
	delay_nms(180);
	//delay();
	//delay();
	//delay();
	//delay();
	//delay();
	//delay();
	//delay();
	//delay();
	//delay();
	//delay();
	//delay();
	//delay();
	//delay();
	//delay();//���ȴ�180ms
	
	start_i2c();
	while(ack3)
		{
			ack3=send_byte(0x47);
		}
	t0=read_byte(1);
	t1=read_byte(0);
	stop_i2c();
	t=(((t0<<8)|t1)/1.2);
	return t; 
	
}


























