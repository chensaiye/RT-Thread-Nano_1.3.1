#ifndef _MENU_H
#define _MENU_H
#include "stm32f1xx_hal.h"
//-----------------------------------------------------------------------------
// Constant Definitions
//-----------------------------------------------------------------------------
#define  MAX_MENU_LEVELS 	4			//菜单层数
#define  MENUITEMVALUE_NO	26          //菜单项总数目
//菜单项参数结构
typedef struct _sMenuItemValue
{
	uint32_t 	ItemValue;					//当前菜单项设定值
	uint32_t 	DefaultValue;				//当前菜单项默认值
	uint32_t 	MinValue;						//当前菜单项最小值
	uint32_t 	MaxValue;						//当前菜单项最大值
	uint32_t 	Gain;								//当前菜单项值增益
}sMenuItemValue,*psMenuItemValue;
	

//菜单项结构
typedef struct _sMenuItem
{
	//uint32_t		count;						//当前层的菜单数量
	uint8_t  	psName[8];						//菜单项名称
	void  *psFarMenuGroup;					//父菜单
	void  *psSonMenuGroup;					//子菜单
	void 	(* cbFunc)(void);			//菜单回调函数
	uint32_t		Value_Flag;					//当前菜单项值进入标志
	sMenuItemValue *psValue;			//当前菜单项的值
}sMenuItem,*psMenuItem;

//菜单组
typedef struct _sMenuGroup
{
	uint32_t		Count;						//当前层的菜单数量
	uint32_t  	Position;					//指向当前菜单组的菜单项位置
	sMenuItem* psMenuItems;				//指向当前的菜单组
}sMenuGroup,*psMenuGroup;

extern sMenuGroup* CurrentMenuGroup;	//当前菜单组指针
extern sMenuItem* CurrentMenuItem;		//当前菜单项指针
extern sMenuItem MainMenu[];			//主菜单，一级菜单
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
