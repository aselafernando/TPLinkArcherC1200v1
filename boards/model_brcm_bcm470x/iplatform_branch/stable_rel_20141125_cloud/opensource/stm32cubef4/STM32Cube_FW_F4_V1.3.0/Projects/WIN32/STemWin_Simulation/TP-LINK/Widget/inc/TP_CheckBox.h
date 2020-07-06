#ifndef __TP_CHECKBOX_H__
#define __TP_CHECKBOX_H__

#include "GUI.h"
#include "WM.h"

/*********************************************************************
*
*  Text Background Color indices
*/
#define TP_CHECKBOX_TB_CI_NORMAL			0
#define TP_CHECKBOX_TB_CI_CLICKED			1
#define TP_CHECKBOX_TB_CI_DISABLED			2
#define TP_CHECKBOX_TB_CI_MAX				3


/* Box Line Color indices */
#define TP_CHECKBOX_BOX_CI_NORMAL		0
#define TP_CHECKBOX_BOX_CI_CLICKED		1
#define TP_CHECKBOX_BOX_CI_MAX			2

#define TP_CHECKBOX_STATE_UNCHECKED		0
#define TP_CHECKBOX_STATE_CHECKED		1

#define TP_CHECKBOX_ALIGN_H_LEFT		(1 << 0)
#define TP_CHECKBOX_ALIGN_H_RIGHT		(1 << 1)

typedef int	TP_CHECKBOX_STATE;

typedef WM_HMEM TP_CHECKBOX_Handle;


/*********************************************************************
*
* TP_Checkbox_Obj
*//**/
typedef struct 
{
	int id;
	GUI_COLOR boxBkColor;	/*����ı�����ɫ*/
	GUI_COLOR boxColor;		/*�����ɫ*/
	GUI_COLOR textBkColor[TP_CHECKBOX_TB_CI_MAX];	/*�ı��ı�����ɫ*/
	GUI_COLOR textColor;	/*�ı�����ɫ*/
	GUI_COLOR hookColor;	/*���ӵ���ɫ*/
	CHAR * pText;		/*Ҫ��ʾ���ı�*/
	const GUI_FONT	*pFont;		/*�ı�������*/
	int			space;		/*�����ı�֮��ľ���*/
	TP_CHECKBOX_STATE state;	/*��ǰ��ѡ���״̬*/
	int numExtraBytes;			/* ����Ĵ洢�ռ����� */
	int	size;	/*���Ĭ�Ͽ��*/
	//int ySize;	/*���Ĭ�ϸ߶�*/
	//GUI_COLOR touchColor;	/*����ʱ����ʾ����ɫ*/
	int pressed;
	int textAlign;
	int boxAlign;
}TP_CHECKBOX_Obj;
	

void TP_CheckBox_Callback(WM_MESSAGE * pMsg);

int TP_CheckBox_GetUserData(TP_CHECKBOX_Handle hWin, void * pDest, int sizeOfBuffer);
int TP_CheckBox_SetUserData(TP_CHECKBOX_Handle hWin, void * pSrc, int sizeOfBuffer);
TP_CHECKBOX_Handle TP_CheckBox_Create(int x0, int y0, int xSize, int ySize,
									WM_HWIN hWinParent, U32 style, const char * pText,
									WM_CALLBACK * pfCallback, int numExtraBytes, int id);



const GUI_FONT* TP_CheckBox_GetDefaultFont(void);
const GUI_FONT* TP_CheckBox_SetDefaultFont(const GUI_FONT* pFont);
GUI_COLOR TP_CheckBox_GetDefaultTextBkColor(int index);
void TP_CheckBox_SetDefaultTextBkColor(GUI_COLOR color, int index);
int	TP_CheckBox_GetDefaultSpacing(void);
int	TP_CheckBox_SetDefaultSpacing(int space);
int	TP_CheckBox_GetDefaultTextAlign(void);
int	TP_CheckBox_SetDefaultTextAlign(int align);
GUI_COLOR TP_CheckBox_GetDefaultTextColor(void);
int TP_CheckBox_SetDefaultTextColor(GUI_COLOR color);
GUI_COLOR TP_CheckBox_GetTextBkColor(TP_CHECKBOX_Handle hObj, int index);
void  TP_CheckBox_SetTextBkColor(TP_CHECKBOX_Handle hObj, GUI_COLOR color,int index);
GUI_COLOR TP_CheckBox_GetTextColor(TP_CHECKBOX_Handle hObj);
GUI_COLOR TP_CheckBox_SetTextColor(TP_CHECKBOX_Handle hObj, GUI_COLOR color);
GUI_COLOR TP_CheckBox_GetBoxBkColor(TP_CHECKBOX_Handle hObj);
GUI_COLOR TP_CheckBox_SetBoxBkColor(TP_CHECKBOX_Handle hObj, GUI_COLOR Color);
TP_CHECKBOX_STATE TP_CheckBox_GetState(TP_CHECKBOX_Handle hObj);
void TP_CheckBox_SetState (TP_CHECKBOX_Handle hObj, TP_CHECKBOX_STATE state);
int	TP_CheckBox_GetText(TP_CHECKBOX_Handle hObj, char * pBuffer, unsigned int maxLen);
int TP_CheckBox_SetText(TP_CHECKBOX_Handle hObj, const char* pText);

const GUI_FONT *  TP_CheckBox_GetFont(TP_CHECKBOX_Handle hObj, const GUI_FONT * pFont);

const GUI_FONT *  TP_CheckBox_SetFont(TP_CHECKBOX_Handle hObj, const GUI_FONT * pFont);


#endif/* __TP_CHECKBOX_H__ */
