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
