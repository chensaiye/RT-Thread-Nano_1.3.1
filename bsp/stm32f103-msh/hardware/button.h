#ifndef BUTTON_H
#define BUTTON_H

//#define BUTTON_CAMERA_OFFSET			0x01
//#define BUTTON_LONG_PRESS 1

#define BUTTON_NUMBER 8

#define BUTTON_1		0				//
#define BUTTON_2 		1				//
#define BUTTON_3 		2				//
#define BUTTON_4 		3				//
#define BUTTON_DUMMY 	4

#define BUTTON_DOWN 0
#define BUTTON_UP	1
#define	BUTTON_LONG_PRESSED 2
#define BUTTON_LONG_RELEASE 3

#define BUTTON_LONG_TIMER	40	//长按延时时间 40*50ms =2S


//#define KEY1 	GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)
//#define KEY2 	GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14)
//#define KEY3 	GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_15)
//#define KEY4 	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)

/* button type define */
typedef struct
{
	unsigned char status;

/* bit 0: controller;		T/F, must sent controll event after trigger,
								e.g. after voice up, send 0xcX, */
/* bit 1: domain_button;	T/F, is a domain button ? */
/* bit 2: domain_plus;		T: press domain button to plus;
							F: press domain button to minus */
/* bit 3: domain_change;	T/F, affect switch to default domain */
/* bit 4: pressed;			T/F, the key is pressed ? */
/* bit 5: enable;			T/F, if enable the button operation */
	
	unsigned char count;	//long press x
} button_type;

/* button struct */
extern button_type  buttons[BUTTON_NUMBER];

#define	BUTTON_CONTROLLER		0x01
#define	BUTTON_DOMAIN_BUTTON	0x02
#define	BUTTON_DOMAIN_PLUS		0x04
#define	BUTTON_DOMAIN_CHANGE	0x08
#define	BUTTON_PRESSED			0x10
#define	BUTTON_ENABLE			0x20
#define	BUTTON_DISABLE			0xdf

#define MAX_KIN_COL			4

//void button_init(void);
//uint8_t get_button(void);
//void button_scan(void);
void button_1( unsigned char flag );
void button_2( unsigned char flag );
void button_3( unsigned char flag );
void button_4( unsigned char flag );
void button_5( unsigned char flag );
void button_6( unsigned char flag );
void button_7( unsigned char flag );
void button_8( unsigned char flag );
void button_dummy( unsigned char flag );


#endif


