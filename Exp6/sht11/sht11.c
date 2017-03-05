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
#include "sht11.h"

const float C1=-4.0; // for 12 Bit
const float C2= 0.0405; // for 12 Bit
const float C3=-0.0000028; // for 12 Bit
const float T1=0.01; // for 14 Bit @ 5V
const float T2=0.00008; // for 14 Bit @ 5V

void _nop_(void)
{
  int i;
  for(i=0;i<100;i++);
}

//----------------------------------------------------------------------------------
// writes a byte on the Sensibus and checks the acknowledge
char s_write_byte(unsigned char value)
{
    unsigned char i,error=0;
    
    for (i=0x80;i>0;i/=2) //shift bit for masking
    { 
            if (i & value) set_DATA_1(); //masking value with i , write to SENSI-BUS
            else set_DATA_0();
            set_CLK_1(); //clk for SENSI-BUS
            _nop_();_nop_();_nop_();    //pulswith approx. 5 us		
            set_CLK_0();		
    }
    set_DATA_1(); //release DATA-line
    Sensor_DATA_IN();//改为输入模式
    set_CLK_1(); //clk #9 for ack

    error=IS_DATA_1(); //check ack (DATA will be pulled down by SHT11)
    Sensor_DATA_OUT();//改为输出模式，还原
    set_CLK_0();
    
    return error; //error=1 in case of no acknowledge
}
//----------------------------------------------------------------------------------
// reads a byte form the Sensibus and gives an acknowledge in case of "ack=1"
char s_read_byte(unsigned char ack)
{
	unsigned char i,val=0;
        
	set_DATA_1(); //release DATA-line
	Sensor_DATA_IN();//改为输入模式

	for (i=0x80;i>0;i/=2) //shift bit for masking
	{ 
		set_CLK_1(); //clk for SENSI-BUS
		if (IS_DATA_1()) val=(val | i); //read bit
		set_CLK_0();
	}
	Sensor_DATA_OUT();//还原
	if(ack)
		set_DATA_0();
	else
		set_DATA_1();
                           
//	DATA=!ack; //in case of "ack==1" pull down DATA-Line
	set_CLK_1(); //clk #9 for ack
	_nop_();_nop_();_nop_(); //pulswith approx. 5 us

	set_CLK_0();
	set_DATA_1(); //release DATA-line
	return val;
}
//----------------------------------------------------------------------------------
void s_transstart(void)
//----------------------------------------------------------------------------------
// generates a transmission start
// _____ ________
// DATA: |_______|
// ___ ___
// SCK : ___| |___| |______
{  
  
	set_DATA_1(); set_CLK_0(); //Initial state
	_nop_();      
	set_CLK_1();
	_nop_();         
	set_DATA_0();
	_nop_();          
	set_CLK_0();
	_nop_();_nop_();_nop_();   
	set_CLK_1();
	_nop_();      
	set_DATA_1();
	_nop_();     
	set_CLK_0();
         
}
//----------------------------------------------------------------------------------
void s_connectionreset(void)
//----------------------------------------------------------------------------------
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
// _____________________________________________________ ________
// DATA: |_______|
// _ _ _ _ _ _ _ _ _ ___ ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______| |___| |______
{
	unsigned char i;
        
	set_DATA_1(); set_CLK_0(); //Initial state
	for(i=0;i<9;i++) //9 SCK cycles
	{ 
		set_CLK_1();
		_nop_();    
		set_CLK_0();
		_nop_();       
	}
	s_transstart(); //transmission start
}
//----------------------------------------------------------------------------------
char s_softreset(void)
//----------------------------------------------------------------------------------
// resets the sensor by a softreset
{
    unsigned char error=0;
    s_connectionreset(); //reset communication
    error+=s_write_byte(RESET); //send RESET-command to sensor
    return error; //error=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------
char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode)
//----------------------------------------------------------------------------------
// makes a measurement (humidity/temperature) with checksum
{
    unsigned error=0;
    unsigned int i;
    s_transstart(); //transmission start
    switch(mode){ //send command to sensor
            case TEMP : error+=s_write_byte(MEASURE_TEMP); break;
            case HUMI : error+=s_write_byte(MEASURE_HUMI); break;
            default : break;
    }
    
    Sensor_DATA_IN();//改为输入模式

    for (i=0;i<65535;i++) if(!IS_DATA_1()) {break;} //wait until sensor has finished the measurement
    for (i=0;i<65535;i++) if(!IS_DATA_1()) {break;}
    for (i=0;i<65535;i++) if(!IS_DATA_1()) {break;}
    for (i=0;i<65535;i++) if(!IS_DATA_1()) {break;}

    
    if(IS_DATA_1()) error+=1; // or timeout (~2 sec.) is reached
    
    
    if(error >0){
              P1_1 =0;
            //  prints("s_measure error");
    }
    
    Sensor_DATA_OUT();//恢复
    *(p_value+1) =s_read_byte(ACK); //read the first byte (MSB)
    *(p_value)=s_read_byte(ACK); //read the second byte (LSB)
    *p_checksum =s_read_byte(noACK); //read checksum
    return error;
}

/****************************************************************
*函数功能 ：延时						
*入口参数 ：定性延时									
*返 回 值 ：无												
*说    明 ：													
****************************************************************/
void Sht11Delay(uint n)
{
	uint i,t;
       
	for(t = 0;t<n;t++)
	  for(t = 0;t<n;t++)
	    for(t = 0;t<n;t++)
	      for(t = 0;t<n;t++);
}

/*****************************************
//Sht11初始化
*****************************************/
void Sht11Init(void)
{
    P0SEL &= ~(0x03<<0); // set general io mode for p0
    P0DIR |= 0X03;   //  设置P0_0,P0_1为输出
    P0INP &= ~0X02;  //设置P0_0(CLK)为三态口
    //P2INP &= ~(0x01<<5);
    
    Sensor_DATA_OUT();
    set_DATA_0();
    Sensor_CLK_OUT();
    set_CLK_0();
    s_connectionreset();

    Sht11Delay(20);
};


//----------------------------------------------------------------------------------------
// calculates temperature [.C] and humidity [%RH]
// input : humi [Ticks] (12 bit)
// temp [Ticks] (14 bit)
// output: humi [%RH]
// temp [.C]
//----------------------------------------------------------------------------------------
void calc_sth11(float *p_humidity ,float *p_temperature)
{
    float rh=*p_humidity; // rh: Humidity [Ticks] 12 Bit
    float t=*p_temperature; // t: Temperature [Ticks] 14 Bit
    float rh_lin; // rh_lin: Humidity linear
    float rh_true; // rh_true: Temperature compensated humidity
    float t_C; // t_C : Temperature [.C]
    t_C=t*0.01 - 40; //calc. Temperature from ticks to [.C]

    rh_lin=C3*rh*rh + C2*rh + C1; //calc. Humidity from ticks to [%RH]
    rh_true=(t_C-25)*(T1+T2*rh)+rh_lin; //calc. Temperature compensated humidity [%RH]
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
float calc_dewpoint(float h,float t)
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
char GetHumiAndTemp(float *humi, float *temp)
{
  unsigned char error=0, checksum;
  value humi_val,temp_val;
  volatile float dew_point;
  error+=s_measure((unsigned char*) &humi_val.i,&checksum,HUMI);      //measure humidity
  error+=s_measure((unsigned char*) &temp_val.i,&checksum,TEMP);      //measure temperature
  if(error!=0){ s_connectionreset();  Sht11Delay(50000); }   
  else
  {
      humi_val.f=(float)humi_val.i;
      temp_val.f=(float)temp_val.i;

      calc_sth11(&humi_val.f,&temp_val.f); //calculate humidity, temperature
      dew_point=calc_dewpoint(humi_val.f,temp_val.f); //calculate dew point
      
      *humi = humi_val.f;
      *temp = temp_val.f;
  }
  return error;
}

