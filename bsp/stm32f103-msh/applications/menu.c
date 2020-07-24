//-----------------------------------------------------------------------------
// menu.c
//-----------------------------------------------------------------------------
/********************************************************************************/
//////////////////////////////////////////////////////////////////////////////////	
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//STM32开发板
//CSY
//修改日期:
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 启东应成医疗器械有限公司
//All rights reserved									   
//////////////////////////////////////////////////////////////////////////////////
/********************************************************************************/
//修改说明 


////////////////////////////////////////////////////////////////////////////////// 
/********************************************************************************/

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "menu.h"
#define PARA_MAX 2800
//#include "port.h"
//extern USHORT  usRegHoldingBuf[REG_HOLDING_NREGS]; //保持寄存器

//uint8_t SET_FACTORY_FLAG;	//回复出厂设置标志

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t MenuItemIndex = 0, nMenuLevel = 0;
uint32_t ItemPos[MAX_MENU_LEVELS];		//每层的当前菜单位置
uint32_t Menu_Set_Flag; //菜单设置标志
uint32_t Menu_View_Flag; //菜单预览标志

//菜单项值数组
//sMenuItemValue MainMenuIVS[2] = {
//	{0x0005,0x0005,0x0001,0x002D,1},// 当前值， 默认值， 最小值， 最大值 ，放大倍数
//	{0x001B,0x001B,0x0001,0x002D,1},
//};

//604Plus
sMenuItemValue MenuIVS[MENUITEMVALUE_NO] = {
	{900,0x0064,0x0046,PARA_MAX,1},// 当前值， 默认值， 最小值， 最大值 ，放大倍数
	{1900,0x0708,0x0046,PARA_MAX,1},	
	{900,0x0064,0x0046,PARA_MAX,1},
	{1900,0x0708,0x0046,PARA_MAX,1},
	{900,0x0064,0x0046,PARA_MAX,1},
	{1900,0x0708,0x0046,PARA_MAX,1},
	{800,0x0064,0x0046,PARA_MAX-300,1},
	{1800,0x0708,0x0046,PARA_MAX-300,1},
	
	{900,0x0064,0x0046,PARA_MAX,1},// 当前值， 默认值， 最小值， 最大值 ，放大倍数
	{1900,0x0708,0x0046,PARA_MAX,1},	
	{900,0x0064,0x0046,PARA_MAX,1},
	{1900,0x0708,0x0046,PARA_MAX,1},
	{900,0x0064,0x0046,PARA_MAX,1},
	{1900,0x0708,0x0046,PARA_MAX,1},
	{800,0x0064,0x0046,PARA_MAX-300,1},
	{1800,0x0708,0x0046,PARA_MAX-300,1},
	
	{500,0x0064,0x0046,PARA_MAX,1},// 当前值， 默认值， 最小值， 最大值 ，放大倍数
	{1000,0x0708,0x0046,PARA_MAX,1},	
	{500,0x0064,0x0046,PARA_MAX,1},
	{1000,0x0708,0x0046,PARA_MAX,1},
	{500,0x0064,0x0046,PARA_MAX,1},
	{1000,0x0708,0x0046,PARA_MAX,1},
	{1000,0x0064,0x0046,PARA_MAX-300,1},
	{2000,0x0708,0x0046,PARA_MAX-300,1},
	
	{0x0000,0x0000,0x0000,PARA_MAX,1},// 当前值， 默认值， 最小值， 最大值 ，放大倍数
	{0x0000,0x0000,0x0000,PARA_MAX,1},	
	{0x0000,0x0000,0x0000,PARA_MAX,1},
	{0x0000,0x0000,0x0000,PARA_MAX,1},
	{0x0000,0x0000,0x0000,PARA_MAX,1},
	{0x0000,0x0000,0x0000,PARA_MAX,1},
	{30,0x0064,0x0046,PARA_MAX-300,1},
	{2000,0x0708,0x0046,PARA_MAX-300,1},
	
	{0x000a,0x0005,0x0001,0x002D,1},
	{0x001B,0x001B,0x0001,0x002D,1},
};

//菜单项值数组
//sMenuItemValue MODE_GP1_Item[8] = {
//	{0x0064,0x0064,0x0046,PARA_MAX,1},// 当前值， 默认值， 最小值， 最大值 ，放大倍数
//	{0x0708,PARA_MAX,0x0046,PARA_MAX,1},	
//	{0x0064,0x0064,0x0046,PARA_MAX,1},
//	{0x0708,PARA_MAX,0x0046,PARA_MAX,1},
//	
//	{0x0064,0x0064,0x0046,PARA_MAX,1},
//	{0x0708,PARA_MAX,0x0046,PARA_MAX,1},
//	{0x0064,0x0064,0x0046,PARA_MAX,1},
//	{0x0708,PARA_MAX,0x0046,PARA_MAX,1},
//};
//sMenuItemValue MODE_GP2_Item[8] = {
//	{0x0064,0x0064,0x0046,PARA_MAX,1},// 当前值， 默认值， 最小值， 最大值 ，放大倍数
//	{0x0708,PARA_MAX,0x0046,PARA_MAX,1},	
//	{0x0064,0x0064,0x0046,PARA_MAX,1},
//	{0x0708,PARA_MAX,0x0046,PARA_MAX,1},
//	
//	{0x0064,0x0064,0x0046,PARA_MAX,1},
//	{0x0708,PARA_MAX,0x0046,PARA_MAX,1},
//	{0x0064,0x0064,0x0046,PARA_MAX,1},
//	{0x0708,PARA_MAX,0x0046,PARA_MAX,1},
//};
//sMenuItemValue MODE_GP3_Item[8] = {
//	{0x0064,0x0064,0x0046,PARA_MAX,1},// 当前值， 默认值， 最小值， 最大值 ，放大倍数
//	{0x0708,PARA_MAX,0x0046,PARA_MAX,1},	
//	{0x0064,0x0064,0x0046,PARA_MAX,1},
//	{0x0708,PARA_MAX,0x0046,PARA_MAX,1},
//	
//	{0x0064,0x0064,0x0046,PARA_MAX,1},
//	{0x0708,PARA_MAX,0x0046,PARA_MAX,1},
//	{0x0064,0x0064,0x0046,PARA_MAX,1},
//	{0x0708,PARA_MAX,0x0046,PARA_MAX,1},
//};
//sMenuItemValue MODE_GP4_Item[8] = {
//	{0x0064,0x0064,0x0046,PARA_MAX,1},// 当前值， 默认值， 最小值， 最大值 ，放大倍数
//	{0x0708,PARA_MAX,0x0046,PARA_MAX,1},	
//	{0x0064,0x0064,0x0046,PARA_MAX,1},
//	{0x0708,PARA_MAX,0x0046,PARA_MAX,1},
//	
//	{0x0064,0x0064,0x0046,PARA_MAX,1},
//	{0x0708,PARA_MAX,0x0046,PARA_MAX,1},
//	{0x0064,0x0064,0x0046,PARA_MAX,1},
//	{0x0708,PARA_MAX,0x0046,PARA_MAX,1},
//};

sMenuGroup* CurrentMenuGroup;//当前指向的菜单组
sMenuItem*  CurrentMenuItem; //当前指向的菜单项
sMenuItem MainMenu[];		//主菜单，一级菜单
//level 2****************************
//sMenuItem MODE_Gp1[] = {
//	{"CH1-MIN ", MainMenu, NULL , NULL,0,&MODE_GP1_Item[0]},
//	{"CH1-MAX ", MainMenu, NULL , NULL,0,&MODE_GP1_Item[1]},
//	{"CH2-MIN ", MainMenu, NULL , NULL,0,&MODE_GP1_Item[2]},
//	{"CH2-MAX ", MainMenu, NULL , NULL,0,&MODE_GP1_Item[3]},
//	{"CH3-MIN ", MainMenu, NULL , NULL,0,&MODE_GP1_Item[4]},
//	{"CH3-MAX ", MainMenu, NULL , NULL,0,&MODE_GP1_Item[5]},
//	{"CH4-MIN ", MainMenu, NULL , NULL,0,&MODE_GP1_Item[6]},
//	{"CH4-MAX ", MainMenu, NULL , NULL,0,&MODE_GP1_Item[7]},
////	{"CH5-MIN ", NULL, NULL , NULL,0,&MenuIVS[8]},
////	{"CH5-MAX ", NULL, NULL , NULL,0,&MenuIVS[9]},
////	{"CH6-MIN ", NULL, NULL , NULL,0,&MenuIVS[10]},
////	{"CH6-MAX ", NULL, NULL , NULL,0,&MenuIVS[11]}
//};
//sMenuGroup MODE1Group1={8,0,MODE_Gp1};

//sMenuItem MODE_Gp2[] = {
//	{"CH1-MIN ", MainMenu, NULL , NULL,0,&MODE_GP2_Item[0]},
//	{"CH1-MAX ", MainMenu, NULL , NULL,0,&MODE_GP2_Item[1]},
//	{"CH2-MIN ", MainMenu, NULL , NULL,0,&MODE_GP2_Item[2]},
//	{"CH2-MAX ", MainMenu, NULL , NULL,0,&MODE_GP2_Item[3]},
//	{"CH3-MIN ", MainMenu, NULL , NULL,0,&MODE_GP2_Item[4]},
//	{"CH3-MAX ", MainMenu, NULL , NULL,0,&MODE_GP2_Item[5]},
//	{"CH4-MIN ", MainMenu, NULL , NULL,0,&MODE_GP2_Item[6]},
//	{"CH4-MAX ", MainMenu, NULL , NULL,0,&MODE_GP2_Item[7]},
//};
//sMenuItem MODE_Gp3[] = {
//	{"CH1-MIN ", MainMenu, NULL , NULL,0,&MODE_GP3_Item[0]},
//	{"CH1-MAX ", MainMenu, NULL , NULL,0,&MODE_GP3_Item[1]},
//	{"CH2-MIN ", MainMenu, NULL , NULL,0,&MODE_GP3_Item[2]},
//	{"CH2-MAX ", MainMenu, NULL , NULL,0,&MODE_GP3_Item[3]},
//	{"CH3-MIN ", MainMenu, NULL , NULL,0,&MODE_GP3_Item[4]},
//	{"CH3-MAX ", MainMenu, NULL , NULL,0,&MODE_GP3_Item[5]},
//	{"CH4-MIN ", MainMenu, NULL , NULL,0,&MODE_GP3_Item[6]},
//	{"CH4-MAX ", MainMenu, NULL , NULL,0,&MODE_GP3_Item[7]},
//};
//sMenuItem MODE_Gp4[] = {
//	{"CH1-MIN ", MainMenu, NULL , NULL,0,&MODE_GP4_Item[0]},
//	{"CH1-MAX ", MainMenu, NULL , NULL,0,&MODE_GP4_Item[1]},
//	{"CH2-MIN ", MainMenu, NULL , NULL,0,&MODE_GP4_Item[2]},
//	{"CH2-MAX ", MainMenu, NULL , NULL,0,&MODE_GP4_Item[3]},
//	{"CH3-MIN ", MainMenu, NULL , NULL,0,&MODE_GP4_Item[4]},
//	{"CH3-MAX ", MainMenu, NULL , NULL,0,&MODE_GP4_Item[5]},
//	{"CH4-MIN ", MainMenu, NULL , NULL,0,&MODE_GP4_Item[6]},
//	{"CH4-MAX ", MainMenu, NULL , NULL,0,&MODE_GP4_Item[7]},
//};

//level 1****************************
//sMenuItem MainMenu[] = {
//	{"MODE1-GP", NULL, MODE_Gp1 , NULL,0,NULL},
//	{"MODE2-GP", NULL, MODE_Gp2 , NULL,0,NULL},
//	{"MODE3-GP", NULL, MODE_Gp3 , NULL,0,NULL},
//	{"MODE4-GP", NULL, MODE_Gp4 , NULL,0,NULL},
//	{"GAIN_MIN", NULL, NULL , NULL,0,&MainMenuIVS[0]},
//	{"GAIN_MAX", NULL, NULL , NULL,0,&MainMenuIVS[1]},
//	{"Reset VL", NULL, NULL , Menu_IVS_Reset_To_Factory,0,NULL},
//	{"Version ", NULL, NULL , Show_Version,0,NULL},
//	{"Return  ", NULL, NULL , MenuReturnFunc,0,NULL}};

sMenuItem MainMenu[] = {
	{"M1-1-MIN", NULL, NULL , NULL,0,&MenuIVS[0]},
	{"M1-1-MAX", NULL, NULL , NULL,0,&MenuIVS[1]},
	{"M1-2-MIN", NULL, NULL , NULL,0,&MenuIVS[2]},
	{"M1-2-MAX", NULL, NULL , NULL,0,&MenuIVS[3]},
	{"M1-3-MIN", NULL, NULL , NULL,0,&MenuIVS[0]},
	{"M1-3-MAX", NULL, NULL , NULL,0,&MenuIVS[1]},
	{"M1-4-MIN", NULL, NULL , NULL,0,&MenuIVS[2]},
	{"M1-4-MAX", NULL, NULL , NULL,0,&MenuIVS[3]},
	
	{"M2-1-MIN", NULL, NULL , NULL,0,&MenuIVS[4]},
	{"M2-1-MAX", NULL, NULL , NULL,0,&MenuIVS[5]},
	{"M2-2-MIN", NULL, NULL , NULL,0,&MenuIVS[6]},
	{"M2-2-MAX", NULL, NULL , NULL,0,&MenuIVS[7]},
	{"M2-3-MIN", NULL, NULL , NULL,0,&MenuIVS[4]},
	{"M2-3-MAX", NULL, NULL , NULL,0,&MenuIVS[5]},
	{"M2-4-MIN", NULL, NULL , NULL,0,&MenuIVS[6]},
	{"M2-4-MAX", NULL, NULL , NULL,0,&MenuIVS[7]},
	
	{"M3-1-MIN", NULL, NULL , NULL,0,&MenuIVS[8]},
	{"M3-1-MAX", NULL, NULL , NULL,0,&MenuIVS[9]},
	{"M3-2-MIN", NULL, NULL , NULL,0,&MenuIVS[10]},
	{"M3-2-MAX", NULL, NULL , NULL,0,&MenuIVS[11]},
	{"M3-3-MIN", NULL, NULL , NULL,0,&MenuIVS[8]},
	{"M3-3-MAX", NULL, NULL , NULL,0,&MenuIVS[9]},
	{"M3-4-MIN", NULL, NULL , NULL,0,&MenuIVS[10]},
	{"M3-4-MAX", NULL, NULL , NULL,0,&MenuIVS[11]},
	
	{"M4-1-MIN", NULL, NULL , NULL,0,&MenuIVS[12]},
	{"M4-1-MAX", NULL, NULL , NULL,0,&MenuIVS[13]},
	{"M4-2-MIN", NULL, NULL , NULL,0,&MenuIVS[14]},
	{"M4-2-MAX", NULL, NULL , NULL,0,&MenuIVS[15]},
	{"M4-3-MIN", NULL, NULL , NULL,0,&MenuIVS[12]},
	{"M4-3-MAX", NULL, NULL , NULL,0,&MenuIVS[13]},
	{"M4-4-MIN", NULL, NULL , NULL,0,&MenuIVS[14]},
	{"M4-4-MAX", NULL, NULL , NULL,0,&MenuIVS[15]},
	
	{"GAIN_MIN", NULL, NULL , NULL,0,&MenuIVS[32]},
	{"GAIN_MAX", NULL, NULL , NULL,0,&MenuIVS[33]},
	{"Reset VL", NULL, NULL , Menu_IVS_Reset_To_Factory,0,NULL},
	{"Version ", NULL, NULL , Show_Version,0,NULL},
	{"Return  ", NULL, NULL , MenuReturnFunc,0,NULL}};

sMenuGroup MainMenuGroup={MENUITEMVALUE_NO+3,0,&MainMenu[0]};


uint8_t SOFT_VERSION[] = "V1.1.0";
void Show_Version(void)
{
		//OLED_ShowString(0,CHARSIZE_6X8, SOFT_VERSION,CHARSIZE_6X8);
}

void Enter_Menu_View(void)
{
	Menu_View_Flag = 1;
}

void Exit_Menu_View(void)
{
	Menu_View_Flag = 0;
}
 
uint8_t Get_Menu_View_Flag(void)
{
	return (uint8_t)Menu_View_Flag;
}

void Enter_Menu_Set(void)
{
	Menu_Set_Flag = 1;
}

void Exit_Menu_Set(void)
{
	Menu_Set_Flag = 0;
}
 
uint8_t Get_Menu_Set_Flag(void)
{
	return (uint8_t)Menu_Set_Flag;
}
/*******************************************************************************
* Function Name  : CopyData_To_MenuItemsValue
* Description    : CopyData To MenuItemsValue
* Input          : *pBuffer - date address want to write;len - write len
* Output         : None
* Return         : None
*******************************************************************************/
void CopyData_To_MenuItemsValue(uint16_t *pBuffer,uint16_t len)
{
	uint8_t i;
	for(i=0;i<len;i++,pBuffer++)
		MenuIVS[i].ItemValue = *pBuffer;
}
/*******************************************************************************
* Function Name  : Readata_From_MenuItemsValue
* Description    : Readata_From_MenuItemsValue.
* Input          : *pBuffer - date address want to read;len - write len
* Output         : None
* Return         : None
*******************************************************************************/
void Readata_From_MenuItemsValue(uint16_t *pBuffer,uint16_t len)
{
	uint8_t i;
	for(i=0;i<len;i++,pBuffer++)
		*pBuffer = (uint16_t)MenuIVS[i].ItemValue;
}

/*******************************************************************************
* Function Name  : MenuItemsValue_WriteToEEprom
* Description    : Initializes the navigation menu.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MenuItemsValue_WriteToEEprom(void)
{
//	uint8_t i;
//	uint32_t buf[MENUITEMVALUE_NO];
//	for(i=0;i<MENUITEMVALUE_NO;i++)
//		buf[i] = MenuIVS[i].ItemValue;
//	AT24CXX_WriteOnePage(EEPROM_ARRD1,(uint8_t *)buf);
//	AT24CXX_WriteOnePage(EEPROM_ARRD1+8,(uint8_t *)&buf[2]);
//	AT24CXX_WriteOnePage(EEPROM_ARRD1+16,(uint8_t *)&buf[4]);
//	AT24CXX_WriteOnePage(EEPROM_ARRD1+24,(uint8_t *)&buf[6]);
//	
//	AT24CXX_WriteOnePage(EEPROM_ARRD1+32,(uint8_t *)&buf[8]);
//	AT24CXX_WriteOnePage(EEPROM_ARRD1+40,(uint8_t *)&buf[10]);
//	AT24CXX_WriteOnePage(EEPROM_ARRD1+48,(uint8_t *)&buf[12]);
//	AT24CXX_WriteOnePage(EEPROM_ARRD1+56,(uint8_t *)&buf[14]);
}
/*******************************************************************************
* Function Name  : MenuItemsValue_ReadFromEEprom
* Description    : Initializes the navigation menu.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MenuItemsValue_ReadFromEEprom(void)
{
//	uint8_t i;
//	uint32_t buf[MENUITEMVALUE_NO];
//	AT24CXX_Read(EEPROM_ARRD1,(uint8_t *)buf,MENUITEMVALUE_NO*4);
//	for(i=0;i<MENUITEMVALUE_NO;i++)
//		 MenuIVS[i].ItemValue = buf[i];
}

/*******************************************************************************
* Function Name  : SelFunc
* Description    : This function is executed when "SEL" push-buttton is pressed.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MenuSelectFunc(void)
{
	if(CurrentMenuItem->psSonMenuGroup != NULL)
	{//有子菜单进入
		ItemPos[nMenuLevel] = MenuItemIndex;
		MenuItemIndex = 0;		//下级菜单从第一项开始
		nMenuLevel++;		
		CurrentMenuGroup = CurrentMenuItem->psSonMenuGroup;
		CurrentMenuItem = (sMenuItem *)CurrentMenuGroup->psMenuItems;
	} 
	else
	{//修改菜单值
		
		if(CurrentMenuItem->psValue != NULL)	//已初始化了结构
		{//
			CurrentMenuItem->Value_Flag++;
			if(CurrentMenuItem->psValue->ItemValue > CurrentMenuItem->psValue->MaxValue)
				CurrentMenuItem->psValue->ItemValue = CurrentMenuItem->psValue->DefaultValue;
			if(CurrentMenuItem->psValue->ItemValue < CurrentMenuItem->psValue->MinValue)
				CurrentMenuItem->psValue->ItemValue = CurrentMenuItem->psValue->DefaultValue;
		}
	}
	if(CurrentMenuItem->cbFunc != NULL)	//如果定义了回调函数即执行
		CurrentMenuItem->cbFunc();				//通常用于单选的处理
}
/*******************************************************************************
* Function Name  : DownFunc
* Description    : This function is executed when any of "Down" push-butttons
*                  is pressed.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MenuDownFunc(void)
{
	if(CurrentMenuItem->Value_Flag)
	{//修改参数值
		if(Menu_Set_Flag==1)//允许值更改标志
		{
		if(++CurrentMenuItem->psValue->ItemValue > CurrentMenuItem->psValue->MaxValue)
			CurrentMenuItem->psValue->ItemValue = CurrentMenuItem->psValue->MaxValue;
		}
	}
	else
	{//菜单项切换
		/* Test on the MenuItemIndex value before incrementing it */
		if(MenuItemIndex >= CurrentMenuGroup->Count-1)//((CurrentMenuItem->count)-1))
		{
			//CurrentMenuItem = &CurrentMenuGroup->psMenuItems;//CurrentMenuGroup;
			MenuItemIndex = MenuItemIndex;
		}
		else
		{
			MenuItemIndex++;
			CurrentMenuItem++;//= &CurrentMenuGroup[MenuItemIndex];
		}
	}
}
  

/*******************************************************************************
* Function Name  : UpFunc
* Description    : This function is executed when any of "UP" push-butttons
*                  is pressed.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MenuUpFunc(void)
{
	if(CurrentMenuItem->Value_Flag)
	{//菜单项值层
		if(Menu_Set_Flag==1)//允许值更改标志
		{
			if(CurrentMenuItem->psValue->ItemValue)
				if(--CurrentMenuItem->psValue->ItemValue < CurrentMenuItem->psValue->MinValue)
					CurrentMenuItem->psValue->ItemValue = CurrentMenuItem->psValue->MinValue;
		}
	}
	else
	{//菜单项切换层
		if(MenuItemIndex > 0)
		{
			MenuItemIndex--;
			CurrentMenuItem--;// = &CurrentMenuGroup[MenuItemIndex];
		}
		else
		{
			MenuItemIndex = 0;
			CurrentMenuItem = (sMenuItem *)CurrentMenuGroup->psMenuItems;
		}
	}
}
/*******************************************************************************
* Function Name  : ReturnFunc
* Description    : This function is executed when the "RETURN" menu is selected.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MenuReturnFunc(void)
{
	if((CurrentMenuItem->Value_Flag ==0)&&(nMenuLevel==0))
	{
		Exit_Menu_View();
		if(Get_Menu_Set_Flag())
		{
			MenuItemsValue_WriteToEEprom();
			//MenuItems_Value_Read(usRegHoldingBuf,0,MENUITEMVALUE_NO);//读取系统参数到保持寄存器
			Exit_Menu_Set();
		}
		return;
	}
	if(CurrentMenuItem->Value_Flag)
	{
		CurrentMenuItem->Value_Flag = 0;	//先退出菜单值修改
	}
	else
	{
		if(nMenuLevel)
		{
			nMenuLevel--;												//菜单层减1
			MenuItemIndex = ItemPos[nMenuLevel];						//恢复上次进入时的菜单
			CurrentMenuGroup = (sMenuGroup*)CurrentMenuItem->psFarMenuGroup;				//恢复组
			CurrentMenuItem  = (sMenuItem *)(CurrentMenuGroup->psMenuItems);// + MenuItemIndex;		//恢复项
		}
	}
}

//void Get_Factory_Test( void )
//{
//	unsigned char button_status;
//	
//	button_status = get_165_data();
//	// check if button_1 pressed at the reset time, 
//	if( button_status == 0xFE )
//	{
//		SET_FACTORY_FLAG = IS_TRUE;
//	}
//	
//}

void Menu_IVS_Reset_To_Factory(void)
{
	uint8_t i;
	for(i=0;i<MENUITEMVALUE_NO;i++)
	{
		MenuIVS[i].ItemValue = MenuIVS[i].DefaultValue;
	}
}

/*******************************************************************************
* Function Name  : Menu_Init
* Description    : Initializes the navigation menu.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Menu_Init(void)
{
//	uint8_t i;
	nMenuLevel = 0;
	MenuItemIndex = 0;
	Menu_View_Flag = 0;
	Menu_Set_Flag=0;
	
	CurrentMenuGroup = &MainMenuGroup;
	CurrentMenuItem  = (sMenuItem *)(CurrentMenuGroup->psMenuItems);
	
}

/*******************************************************************************
* Function Name  : IdleFunc
* Description    : Idle function.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void IdleFunc(void)
{
  /* Nothing to execute: return */
  return;
}


/*******************************************************************************
* Function Name  : MenuItems_Value_Read
* Description    : read menu items value
* Input          : addr-读取数据的目的地址 start_addr-读取的起始寄存器地址，len-读取的寄存器个数
* Output         : None
* Return         : 1 address error  , 0 no error
*******************************************************************************/
uint8_t MenuItems_Value_Read(uint16_t *addr,uint16_t start_addr,uint16_t len )
{
	uint8_t i;
	if((start_addr + len) > MENUITEMVALUE_NO )
		return 1;//
	for(i=start_addr;i<(start_addr + len);i++)
	{
		*addr = (uint16_t)(MenuIVS[i].ItemValue);
		addr++;
	}
	return 0;
}
/*******************************************************************************
* Function Name  : MenuItems_Value_Write
* Description    : write menu items value
* Input          : addr-写入数据的源地址 start_addr-写入的起始寄存器地址，len-写入的寄存器个数
* Output         : None
* Return         : 1 address error  , 0 no error
*******************************************************************************/
uint8_t MenuItems_Value_Write(uint16_t *addr,uint16_t start_addr,uint16_t len )
{
	uint8_t i;
	if((start_addr + len) > MENUITEMVALUE_NO )
		return 1;//
	for(i=start_addr;i<(start_addr + len);i++)
	{
		MenuIVS[i].ItemValue =*addr;
		addr++;
	}
	//write eeprom
	return 0;
	
}
 void menu_display(void)
 {	
//	char str[10];
//	if(Get_Menu_View_Flag())
//	{
//		OLED_ShowString(0,0, CurrentMenuItem->psName,CHARSIZE_6X8);
//		if(CurrentMenuItem->Value_Flag)
//		{
//			sprintf(str,":%d",CurrentMenuItem->psValue->ItemValue);
//			OLED_ShowString(0,CHARSIZE_6X8, (uint8_t *)str,CHARSIZE_6X8);
//		}
//	}
	
 }

void Menu_Display_Ctl(void)
{	
//		//OLED_Clear();
//		if(Get_Menu_View_Flag())
//		{
//			menu_display();
//		}
//		else
//		{
//			//OLED_ShowString(0,0, "0123456789ABCDEFGHIJK",CHARSIZE_8X6);
//			//Channel_display();
//		}
//		OLED_Refresh_Gram();
}
