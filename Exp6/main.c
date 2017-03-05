/* 包含头文件 */
/********************************************************************/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "./uart/hal_board_cfg.h"
#include "./uart/hal_uart.h"
#include "./sht11/sht11.h"
#include "./sht11/sht11_1.h"
#include "./gy30/gy30.h"
#include "./LCD/LCD.h"
/********************************************************************/
//定义控制灯的端口
#define LED1 P1_0
#define LED2 P1_1

#define uint16 unsigned int
#define uchar unsigned char


void LedInit(void)
{
  P1DIR |= 0x03; //设置LED
  LED1 = 1;
  LED2 = 1;
}
void delay(unsigned int time)
{ unsigned int i;
unsigned char j;
for(i = 0; i < time; i++)
{ for(j = 0; j < 240; j++)
{ asm("NOP"); 
asm("NOP");
asm("NOP");
} 
} 
}
void initUARTSEND(void)
{

    CLKCONCMD &= ~0x40;                          //设置系统时钟源为32MHZ晶振
    while(CLKCONSTA & 0x40);                     //等待晶振稳定
    CLKCONCMD &= ~0x47;                          //设置系统主时钟频率为32MHZ
   
  
    PERCFG = 0x00;				//位置1 P0口
    P0SEL = 0x0c;				//P0_2,P0_3用作串口
    P2DIR &= ~0XC0;                             //P0优先作为UART0

    U0CSR |= 0x80;				//UART方式
    U0GCR |= 11;				//U0GCR.BAUD_E
    U0BAUD |= 216;				//波特率设为115200 UxBAUD.BAUD_M
    UTX0IF = 0;                                 //UART0 TX中断标志初始置位0
}
/****************************************************************
串口发送字符串函数			
****************************************************************/
void UartTX_Send_String(char *Data,int len)
{
  int j;
  for(j=0;j<len;j++)
  {
    U0DBUF = *Data++;
    while(UTX0IF == 0);
    UTX0IF = 0;
  }
}

void UartTX_Send_Hex(char hex)
{ 
  
    U0DBUF = hex;
    while(UTX0IF == 0);
    UTX0IF = 0;
  
}
void Light(char* ch)
{
    uint32 temp_data;
    temp_data = (uint32)(get_light()/1.2);
    uint8 wan=(uint8)temp_data/10000 ;
    temp_data=temp_data%10000; 
    uint8 qian=(uint8)temp_data/1000 ;
    temp_data=temp_data%1000;  
    uint8 bai=(uint8)temp_data/100;
    temp_data=temp_data%100;    
    uint8 shi=(uint8)temp_data/10;
    temp_data=temp_data%10;      
    uint8 ge=(uint8)temp_data;
  
 
    ch[0] = bai + 48;
    ch[1] = shi + 48;
    ch[2] = ge + 48;
    ch[3]= '\0';
}

void connect2TCP()
{
      UartTX_Send_String("AT\r\n",4);
     delay(10000);
      UartTX_Send_String("AT+CGCLASS=\"B\"\r\n",16);
          delay(10000);
    UartTX_Send_String("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n",27);
     delay(10000);
    UartTX_Send_String("AT+CGATT=1\r\n",13); 
     delay(10000);
    UartTX_Send_String("AT+CIPCSGP=1,\"CMNET\"\r\n",23);
     delay(10000);
     UartTX_Send_String("AT+CIPSTART=\"TCP\",\"www.yyydata.com\",\"9000\"\r\n",sizeof("AT+CIPSTART=\"TCP\",\"www.yyydata.com\",\"9000\"\r\n"));
      delay(10000); delay(10000);    delay(10000);

}
/*********************************************************************
 * 函数名称：main
 * 功    能：main函数入口
 * 入口参数：无
 * 出口参数：无
 * 返 回 值：无
 ********************************************************************/
 void main(void)
{
    unsigned char temp_buf[10];
    unsigned char humi_buf[10];
    unsigned char tempStr[10];
    unsigned char humiStr[10];
    float humi,temp;
    
    unsigned char temp_buf_1[10];
    unsigned char humi_buf_1[10];
    unsigned char tempStr_1[10];
    unsigned char humiStr_1[10];
    float humi_1,temp_1;
      
     char ch[4]="";
    unsigned char *light_buf;
    char data_buf[80]= "";
    char DATA_HEAD[] = "&&yyy&& 10000002 ";
    char DATA_END[]=" &&data&&";
    LED1 = 0;       //LED1点亮，表示程序启动
    initUARTSEND();     // 初始化串口   bautrate:115200
    LCD_Init();                      //oled 初始化  
    LCD_CLS(); 
    Sht11Init_1();   
    Sht11Init();
    LIGHT_INIT();
   delay(1000);

    while(1)
    {    
       connect2TCP();
        if(GetHumiAndTemp(&humi,&temp) == 0)
        {
             sprintf(temp_buf,"%f",temp);
             sprintf(humi_buf,"%f",humi);
             sprintf(tempStr,"%.1f",temp);
             sprintf(humiStr,"%.f",humi);
             LCD_temp("t0:",temp_buf);
             LCD_humi("h0:",humi_buf); 
           //  prints(humi_buf);
             LED2 = !LED2;           //LED2闪烁，表示温湿度采样正常
            
        }
        if(GetHumiAndTemp_1(&humi_1,&temp_1) == 0)
        {
            sprintf(temp_buf_1,"%f",temp_1);
            sprintf(humi_buf_1,"%f",humi_1);
              sprintf(tempStr_1,"%.1f",temp_1);
             sprintf(humiStr_1,"%.f",humi_1);
            LCD_temp_1("t1:",temp_buf_1);
            LCD_humi_1("h1:",humi_buf_1);
            LED2 = !LED2;           //LED2闪烁，表示温湿度采样正常
           
        }
        Light(ch);
        LCD_light("l:",ch);
        
        strcat(data_buf,DATA_HEAD);
        strcat(data_buf,"T1:");
        strcat(data_buf,tempStr);
        strcat(data_buf," H1:");
        strcat(data_buf,humiStr);
        strcat(data_buf," I:");
        strcat(data_buf,ch);
        strcat(data_buf," T2:");
           strcat(data_buf,tempStr_1);       
        strcat(data_buf," H2:");
             strcat(data_buf,humiStr_1);
        strcat(data_buf,DATA_END);
      //  memset(temp_buf,0,sizeof(temp_buf));
      //  memset(humi_buf,0,sizeof(humi_buf));
          
         UartTX_Send_String("AT+CIPSEND\r\n",12);
       // UartTX_Send_String(data_buf,35);
              delay(10000);     delay(10000);
        UartTX_Send_String(data_buf,59);
       // 
        UartTX_Send_Hex(0x1A);  
        data_buf[0] = '\0';
             delay(10000);
        UartTX_Send_String("\r\n",2);     
        UartTX_Send_String("AT+CIPSHUT\r\n",12);
        
        
        LED1 = !LED1;
             delay(50000);  delay(50000);  
  }
}
