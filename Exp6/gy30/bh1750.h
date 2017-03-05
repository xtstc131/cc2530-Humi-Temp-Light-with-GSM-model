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
#define DPOWR  0X00         //断电
#define POWER  0X01         //SHANG DIAN
#define RESET_L    0X07         //CHONG ZHI
#define CHMODE  0X10        //连续H分辨率
#define CHMODE2 0X11         //连续H分辨率2
#define CLMODE   0X13           //连续低分辨
#define HMODE   0X20           //一次H分辨率
#define HMODE2 0X21          //一次H分辨率2
#define LMODE    0X23           //一次L分辨率模式

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
启动I2C


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

结束I2C


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

字节发送成功收到ACK 返回0


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
			LIGHT_SCK_0();          //个人感觉要不要也行，到时候试试
			delay();
			LIGHT_SCK_1() ;       //设置时钟线为高通知设备开始收据数据
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

接受数据函数ack 

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
测量光张强度

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
	send_byte(0x01);   //初始化
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
	//delay();//最大等待180ms
	
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


























