// Enables/disables the io interface
// CLK ->P0_2
// DATA ->P0_3
#define Sensor_DATA_IN()                do{P0DIR &= ~(0X08<<0);P0INP &= ~0X08;P2INP &= ~(0x01<<5);}while(0);                
#define Sensor_CLK_IN()                 do{P0DIR &= ~(0X04<<0);}while(0); 

#define Sensor_DATA_OUT()               do{P0DIR |= (0X08<<0);P0INP &= ~0X08;P2INP &= ~(0x01<<5);}while(0);
#define Sensor_CLK_OUT()                do{P0DIR |= (0X04<<0);}while(0);

#define set_DATA_1()                (P0_3 = 1)
#define set_DATA_0()                (P0_3 = 0)
#define set_CLK_1()                 (P0_2 = 1)
#define set_CLK_0()                 (P0_2 = 0)

#define IS_DATA_1()                 ( P0_3 )
#define IS_CLK_1()                  ( P0_2 )
