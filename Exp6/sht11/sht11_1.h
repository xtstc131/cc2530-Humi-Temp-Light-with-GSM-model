#ifndef SENSOR_1
#define SERSOR_1
#include <ioCC2530.h>

typedef union
{ 
  unsigned int i;
  float f;
} value_1;
//----------------------------------------------------------------------------------
// modul-var
//----------------------------------------------------------------------------------
enum {TEMP_1,HUMI_1};
enum {TEMPERATURE_1,HUMIDITY_1};
#define noACK_1 0
#define ACK_1 1
//adr command r/w
#define STATUS_REG_W_1 0x06 //000 0011 0
#define STATUS_REG_R_1 0x07 //000 0011 1
#define MEASURE_TEMP_1 0x03 //000 0001 1
#define MEASURE_HUMI_1 0x05 //000 0010 1
#define RESET_1 0x1e //000 1111 0

// Enables/disables the io interface

// CLK ->P0_2
// DATA ->P0_3
#define Sensor_DATA_IN_1()                do{P0DIR &= ~(0X80<<0);P0INP &= ~0X80;P2INP &= ~(0x01<<5);}while(0);                
#define Sensor_CLK_IN_1()                 do{P0DIR &= ~(0X40<<0);}while(0); 

#define Sensor_DATA_OUT_1()               do{P0DIR |= (0X80<<0);P0INP &= ~0X80;P2INP &= ~(0x01<<5);}while(0);
#define Sensor_CLK_OUT_1()                do{P0DIR |= (0X40<<0);}while(0);

#define set_DATA_1_1()                (P0_7 = 1)
#define set_DATA_0_1()                (P0_7 = 0)
#define set_CLK_1_1()             (P0_6 = 1)
#define set_CLK_0_1()               (P0_6 = 0)

#define IS_DATA_1_1()               ( P0_7 )
#define IS_CLK_1_1()              ( P0_6 )

typedef unsigned int uint1;

void uDelay_1(uint1 n);
void _nop_1(void);

void s_connectionreset_1(void);
char s_measure_1(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode);
void calc_sth11_1(float *p_humidity ,float *p_temperature);
float calc_dewpoint_1(float h,float t);
char s_read_byte_1(unsigned char ack);
char s_write_byte_1(unsigned char value);

extern void Sht11Delay_1(uint1 n);
extern void Sht11Init_1(void);
extern char GetHumiAndTemp_1(float *humi, float *temp);

#endif
