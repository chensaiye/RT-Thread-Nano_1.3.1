#ifndef _MENU_H
#define _MENU_H
#include "stm32f1xx_hal.h"
//-----------------------------------------------------------------------------
// Constant Definitions
//-----------------------------------------------------------------------------
#define  MAX_MENU_LEVELS 	4			//�˵�����
#define  MENUITEMVALUE_NO	26          //�˵�������Ŀ
//�˵�������ṹ
typedef struct _sMenuItemValue
{
	uint32_t 	ItemValue;					//��ǰ�˵����趨ֵ
	uint32_t 	DefaultValue;				//��ǰ�˵���Ĭ��ֵ
	uint32_t 	MinValue;						//��ǰ�˵�����Сֵ
	uint32_t 	MaxValue;						//��ǰ�˵������ֵ
	uint32_t 	Gain;								//��ǰ�˵���ֵ����
}sMenuItemValue,*psMenuItemValue;
	

//�˵���ṹ
typedef struct _sMenuItem
{
	//uint32_t		count;						//��ǰ��Ĳ˵�����
	uint8_t  	psName[8];						//�˵�������
	void  *psFarMenuGroup;					//���˵�
	void  *psSonMenuGroup;					//�Ӳ˵�
	void 	(* cbFunc)(void);			//�˵��ص�����
	uint32_t		Value_Flag;					//��ǰ�˵���ֵ�����־
	sMenuItemValue *psValue;			//��ǰ�˵����ֵ
}sMenuItem,*psMenuItem;

//�˵���
typedef struct _sMenuGroup
{
	uint32_t		Count;						//��ǰ��Ĳ˵�����
	uint32_t  	Position;					//ָ��ǰ�˵���Ĳ˵���λ��
	sMenuItem* psMenuItems;				//ָ��ǰ�Ĳ˵���
}sMenuGroup,*psMenuGroup;

extern sMenuGroup* CurrentMenuGroup;	//��ǰ�˵���ָ��
extern sMenuItem* CurrentMenuItem;		//��ǰ�˵���ָ��
extern sMenuItem MainMenu[];			//���˵���һ���˵�
extern sMenuItemValue MenuIVS[];

//#define MENU_MODE			0
extern void Show_Version(void);

void Menu_Display_Ctl(void);
void Menu_Init(void);
void MenuSelectFunc(void);
void MenuDownFunc(void);
void MenuUpFunc(void);
void MenuReturnFunc(void);
void IdleFunc(void);
void Menu_IVS_Reset_To_Factory(void);

void Enter_Menu_Set(void);
void Exit_Menu_Set(void);
uint8_t Get_Menu_Set_Flag(void);
void Enter_Menu_View(void);
void Exit_Menu_View(void);
uint8_t Get_Menu_View_Flag(void);
void menu_display(void);


void CopyData_To_MenuItemsValue(uint16_t *pBuffer,uint16_t len);
void Readata_From_MenuItemsValue(uint16_t *pBuffer,uint16_t len);	

uint8_t MenuItems_Value_Read(uint16_t *addr,uint16_t start_addr,uint16_t len );
uint8_t MenuItems_Value_Write(uint16_t *addr,uint16_t start_addr,uint16_t len );
void MenuItemsValue_WriteToEEprom(void);
void MenuItemsValue_ReadFromEEprom(void);

#endif
