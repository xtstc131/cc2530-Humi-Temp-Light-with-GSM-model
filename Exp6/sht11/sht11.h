#ifndef SENSOR
#define SERSOR
#include <ioCC2530.h>

typedef union
{ 
  unsigned int i;
  float f;
} value;
//----------------------------------------------------------------------------------
// modul-var
//----------------------------------------------------------------------------------
enum {TEMP,HUMI};
enum {TEMPERATURE,HUMIDITY};
#define noACK 0
#define ACK 1
//adr command r/w
#define STATUS_REG_W 0x06 //000 0011 0
#define STATUS_REG_R 0x07 //000 0011 1
#define MEASURE_TEMP 0x03 //000 0001 1
#define MEASURE_HUMI 0x05 //000 0010 1
#define RESET 0x1e //000 1111 0

// Enables/disables the io interface

// CLK ->P0_0
// DATA ->P0_1
#define Sensor_DATA_IN()                do{P0DIR &= ~(0X02<<0);P0INP &= ~0X02;P2INP &= ~(0x01<<5);}while(0);                
#define Sensor_CLK_IN()                 do{P0DIR &= ~(0X01<<0);}while(0); 

#define Sensor_DATA_OUT()               do{P0DIR |= (0X02<<0);P0INP &= ~0X02;P2INP &= ~(0x01<<5);}while(0);
#define Sensor_CLK_OUT()                do{P0DIR |= (0X01<<0);}while(0);

#define set_DATA_1()                (P0_1 = 1)
#define set_DATA_0()                (P0_1 = 0)
#define set_CLK_1()                 (P0_0 = 1)
#define set_CLK_0()                 (P0_0 = 0)

#define IS_DATA_1()                 ( P0_1 )
#define IS_CLK_1()                  ( P0_0 )


typedef unsigned int uint;

void uDelay(uint n);
void _nop_(void);

void s_connectionreset(void);
char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode);
void calc_sth11(float *p_humidity ,float *p_temperature);
float calc_dewpoint(float h,float t);
char s_read_byte(unsigned char ack);
char s_write_byte(unsigned char value);

extern void Sht11Delay(uint n);
extern void Sht11Init(void);
extern char GetHumiAndTemp(float *humi, float *temp);

#endif
