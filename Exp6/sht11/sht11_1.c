/***********************************************************************************
Project: SHT11 demo program (V2.0)
Filename: SHT11.c
Prozessor: 80C51 family
Compiler: Keil Version 6.14
Autor: MST
Copyrigth: (c) Sensirion AG
***********************************************************************************/

#include <math.h>
#include "../uart/hal_uart.h"
#include "sht11_1.h"

const float C1_1=-4.0; // for 12 Bit
const float C2_1= 0.0405; // for 12 Bit
const float C3_1=-0.0000028; // for 12 Bit
const float T1_1=0.01; // for 14 Bit @ 5V
const float T2_1=0.00008; // for 14 Bit @ 5V

void _nop_1(void)
{
  int i;
  for(i=0;i<100;i++);
}

//----------------------------------------------------------------------------------
// writes a byte on the Sensibus and checks the acknowledge
char s_write_byte_1(unsigned char value)
{
    unsigned char i,error=0;
    
    for (i=0x80;i>0;i/=2) //shift bit for masking
    { 
            if (i & value) set_DATA_1_1(); //masking value with i , write to SENSI-BUS
            else set_DATA_0_1();
            set_CLK_1_1(); //clk for SENSI-BUS
            _nop_1();_nop_1();_nop_1();    //pulswith approx. 5 us		
            set_CLK_0_1();		
    }
    set_DATA_1_1(); //release DATA-line
    Sensor_DATA_IN_1();//改为输入模式
    set_CLK_1_1(); //clk #9 for ack

    error=IS_DATA_1_1(); //check ack (DATA will be pulled down by SHT11)
    Sensor_DATA_OUT_1();//改为输出模式，还原
    set_CLK_0_1();
    
    return error; //error=1 in case of no acknowledge
}
//----------------------------------------------------------------------------------
// reads a byte form the Sensibus and gives an acknowledge in case of "ack=1"
char s_read_byte_1(unsigned char ack)
{
	unsigned char i,val=0;
        
	set_DATA_1_1(); //release DATA-line
	Sensor_DATA_IN_1();//改为输入模式

	for (i=0x80;i>0;i/=2) //shift bit for masking
	{ 
		set_CLK_1_1(); //clk for SENSI-BUS
		if (IS_DATA_1_1()) val=(val | i); //read bit
		set_CLK_0_1();
	}
	Sensor_DATA_OUT_1();//还原
	if(ack)
		set_DATA_0_1();
	else
		set_DATA_1_1();
                           
//	DATA=!ack; //in case of "ack==1" pull down DATA-Line
	set_CLK_1_1(); //clk #9 for ack
	_nop_1();_nop_1();_nop_1(); //pulswith approx. 5 us

	set_CLK_0_1();
	set_DATA_1_1(); //release DATA-line
	return val;
}
//----------------------------------------------------------------------------------
void s_transstart_1(void)
//----------------------------------------------------------------------------------
// generates a transmission start
// _____ ________
// DATA: |_______|
// ___ ___
// SCK : ___| |___| |______
{  
  
	set_DATA_1_1(); set_CLK_0_1(); //Initial state
	_nop_1();      
	set_CLK_1_1();
	_nop_1();         
	set_DATA_0_1();
	_nop_1();          
	set_CLK_0_1();
	_nop_1();_nop_1();_nop_1();   
	set_CLK_1_1();
	_nop_1();      
	set_DATA_1_1();
	_nop_1();     
	set_CLK_0_1();
         
}
//----------------------------------------------------------------------------------
void s_connectionreset_1(void)
//----------------------------------------------------------------------------------
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
// _____________________________________________________ ________
// DATA: |_______|
// _ _ _ _ _ _ _ _ _ ___ ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______| |___| |______
{
	unsigned char i;
        
	set_DATA_1_1(); set_CLK_0_1(); //Initial state
	for(i=0;i<9;i++) //9 SCK cycles
	{ 
		set_CLK_1_1();
		_nop_1();    
		set_CLK_0_1();
		_nop_1();       
	}
	s_transstart_1(); //transmission start
}
//----------------------------------------------------------------------------------
char s_softreset_1(void)
//----------------------------------------------------------------------------------
// resets the sensor by a softreset
{
    unsigned char error=0;
    s_connectionreset_1(); //reset communication
    error+=s_write_byte_1(RESET_1); //send RESET-command to sensor
    return error; //error=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------
char s_measure_1(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode)
//----------------------------------------------------------------------------------
// makes a measurement (humidity/temperature) with checksum
{
    unsigned error=0;
    unsigned int i;
    s_transstart_1(); //transmission start
    switch(mode){ //send command to sensor
            case TEMP_1 : error+=s_write_byte_1(MEASURE_TEMP_1); break;
            case HUMI_1 : error+=s_write_byte_1(MEASURE_HUMI_1); break;
            default : break;
    }
    
    Sensor_DATA_IN_1();//改为输入模式

    for (i=0;i<65535;i++) if(!IS_DATA_1_1()) {break;} //wait until sensor has finished the measurement
    for (i=0;i<65535;i++) if(!IS_DATA_1_1()) {break;}
    for (i=0;i<65535;i++) if(!IS_DATA_1_1()) {break;}
    for (i=0;i<65535;i++) if(!IS_DATA_1_1()) {break;}

    
    if(IS_DATA_1_1()) error+=1; // or timeout (~2 sec.) is reached
    
    
    if(error >0){
              P1_1 =0;
             // prints("s_measure error");
    }
    
    Sensor_DATA_OUT_1();//恢复
    *(p_value+1) =s_read_byte_1(ACK_1); //read the first byte (MSB)
    *(p_value)=s_read_byte_1(ACK_1); //read the second byte (LSB)
    *p_checksum =s_read_byte_1(noACK_1); //read checksum
    return error;
}

/****************************************************************
*函数功能 ：延时						
*入口参数 ：定性延时									
*返 回 值 ：无												
*说    明 ：													
****************************************************************/
void Sht11Delay_1(uint1 n)
{
	uint1 i,t;
        for(i = 0;i<5;i++)
	for(t = 0;t<n;t++);
}

/*****************************************
//Sht11初始化
*****************************************/
void Sht11Init_1(void)
{
    P0SEL &= ~(0xC0<<0); // set general io mode for p0
    P0DIR |= 0XC0;   //  设置P0_6,P0_7为输出
    P0INP &= ~0X02;  //设置P0_0(CLK)为三态口
    //P2INP &= ~(0x01<<5);
    
    Sensor_DATA_OUT_1();
    set_DATA_0_1();
    Sensor_CLK_OUT_1();
    set_CLK_0_1();
    s_connectionreset_1();

    Sht11Delay_1(20);
};


//----------------------------------------------------------------------------------------
// calculates temperature [.C] and humidity [%RH]
// input : humi [Ticks] (12 bit)
// temp [Ticks] (14 bit)
// output: humi [%RH]
// temp [.C]
//----------------------------------------------------------------------------------------
void calc_sth11_1(float *p_humidity ,float *p_temperature)
{
    float rh=*p_humidity; // rh: Humidity [Ticks] 12 Bit
    float t=*p_temperature; // t: Temperature [Ticks] 14 Bit
    float rh_lin; // rh_lin: Humidity linear
    float rh_true; // rh_true: Temperature compensated humidity
    float t_C; // t_C : Temperature [.C]
    t_C=t*0.01 - 40; //calc. Temperature from ticks to [.C]

    rh_lin=C3_1*rh*rh + C2_1*rh + C1_1; //calc. Humidity from ticks to [%RH]
    rh_true=(t_C-25)*(T1_1+T2_1*rh)+rh_lin; //calc. Temperature compensated humidity [%RH]
    if(rh_true>100)rh_true=100; //cut if the value is outside of
    if(rh_true<0.1)rh_true=0.1; //the physical possible range

    *p_temperature=t_C; //return temperature [.C]
    *p_humidity=rh_true; //return humidity[%RH]
}

//--------------------------------------------------------------------
// calculates dew point
// input: humidity [%RH], temperature [.C]
// output: dew point [.C]
//--------------------------------------------------------------------
float calc_dewpoint_1(float h,float t)
{
    float logEx,dew_point ;
    logEx=0.66077+7.5*t/(237.3+t)+(log10(h)-2) ;
    dew_point = (logEx - 0.66077)*237.3/(0.66077+7.5-logEx) ;
    return dew_point;
}

/****************************************************************
*函数功能 ：get the value of both humidity and temperature 		
*入口参数 ：humi, return the value of humidity
          ：temp, return the value of temperature 
*返 回 值 ：0:success,   >0:error												
*说    明 ：													
****************************************************************/
char GetHumiAndTemp_1(float *humi, float *temp)
{
  unsigned char error=0, checksum;
  value_1 humi_val,temp_val;
  volatile float dew_point;
  error+=s_measure_1((unsigned char*) &humi_val.i,&checksum,HUMI_1);      //measure humidity
  error+=s_measure_1((unsigned char*) &temp_val.i,&checksum,TEMP_1);      //measure temperature
  if(error!=0){ s_connectionreset_1();  Sht11Delay_1(50000); }   
  else
  {
      humi_val.f=(float)humi_val.i;
      temp_val.f=(float)temp_val.i;

      calc_sth11_1(&humi_val.f,&temp_val.f); //calculate humidity, temperature
      dew_point=calc_dewpoint_1(humi_val.f,temp_val.f); //calculate dew point
      
      *humi = humi_val.f;
      *temp = temp_val.f;
  }
  return error;
}

