#ifndef BUTTON_H
#define BUTTON_H

#ifndef BUTTON_DEBUG
#define BUTTON_DEBUG  1
#endif

#define DI_165B_Pin GPIO_PIN_3
#define DI_165B_GPIO_Port GPIOB
#define CLK_165B_Pin GPIO_PIN_4
#define CLK_165B_GPIO_Port GPIOB
#define LATCH_165B_Pin GPIO_PIN_5
#define LATCH_165B_GPIO_Port GPIOB

#define DI_RIR_Pin GPIO_PIN_15
#define DI_RIR_GPIO_Port GPIOA
#define CLK_RIR_Pin GPIO_PIN_12
#define CLK_RIR_GPIO_Port GPIOC
#define LATCH_RIR_Pin GPIO_PIN_2
#define LATCH_RIR_GPIO_Port GPIOD


//#define BUTTON_CAMERA_OFFSET			0x01
#define BUTTON_LONG_PRESS 1

#define BUTTON_NUMBER 16

#define BUTTON_1		0				//
#define BUTTON_2 		1				//
#define BUTTON_3 		2				//
#define BUTTON_4 		3				//
#define BUTTON_5 		4				//
#define BUTTON_6 		5				//

#define BUTTON_DUMMY 	6

#define BUTTON_RIR1			8
#define BUTTON_RIR2			9
#define BUTTON_RIR3			10
#define BUTTON_RIR4			11
#define BUTTON_RIR5			12
#define BUTTON_RIR6			13
#define BUTTON_RIR7			14
#define BUTTON_RIR8			15

#define BUTTON_DOWN 0
#define BUTTON_UP	1
#define	BUTTON_LONG_PRESSED 2
#define BUTTON_LONG_RELEASE 3

#define BUTTON_LONG_TIMER	200	//长按延时时间 200*10ms =2S

#define CHANNEL_RIR_MAX 7

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

#define MAX_KIN_COL			14

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

void bt_rir_1( unsigned char flag );
void bt_rir_2( unsigned char flag );
void bt_rir_3( unsigned char flag );
void bt_rir_4( unsigned char flag );
void bt_rir_5( unsigned char flag );
void bt_rir_6( unsigned char flag );
void bt_rir_7( unsigned char flag );


int thread_button_init(void);
#endif


