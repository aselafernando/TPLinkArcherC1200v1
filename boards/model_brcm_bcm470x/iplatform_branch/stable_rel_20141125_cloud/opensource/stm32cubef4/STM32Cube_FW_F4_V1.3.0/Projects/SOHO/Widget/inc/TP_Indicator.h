/*TP_Indicator.h*/

#ifndef __TP_INDICATOR_H__
#define __TP_INDICATOR_H__

#include "GUI.h"
#include "WM.h"




/*********************************************************************
*
*  Indicator pics indices
*/
#define TP_INDICATOR_HIGHLIGHT					0
#define TP_INDICATOR_DARK  		                1
#define TP_INDICATOR_BKGROUND					2
#define TP_INDICATOR_MAX					    3

typedef WM_HMEM TP_INDICATOR_Handle;


typedef struct {
	
	int id;
	
	const GUI_BITMAP * pBitmap[TP_INDICATOR_MAX];/*ѡ��ǰ����δѡ��ǰ��������ͼƬ*/

    U8 indicatorAlpha[TP_INDICATOR_MAX];         /*ѡ��ǰ����δѡ��ǰ��������͸��ϵ��*/

    GUI_COLOR colorIndex[TP_INDICATOR_MAX];      /*ѡ��ǰ����δѡ��ǰ����������ɫ*/

    U8 aaFactor;        /*���������*/
	
	U8 page;	        /*��ǰҳ���*/

    U8 num;             /*ҳ�����*/

    U8 size;            /*ָʾ��Բֱ����С*/
        
    U8 space;	        /*ָʾ��Բ���*/

    int numExtraBytes;
	
}TP_INDICATOR_Obj;

GUI_COLOR TP_Indicator_GetDefaultColorIndex(int index);
void      TP_Indicator_SetDefaultColorIndex(GUI_COLOR color, int index);
U8   TP_Indicator_GetDefaultIndicatorAlpha(int index);
void TP_Indicator_SetDefaultIndicatorAlpha(U8 alpha, int index);
U8	TP_Indicator_GetDefaultAaFactor(void);
void	TP_Indicator_SetDefaultAaFactor(U8 factor);
U8	TP_Indicator_GetDefaultPage(void);
void	TP_Indicator_SetDefaultPage(U8 page);
U8	TP_Indicator_GetDefaultNum(void);
void	TP_Indicator_SetDefaultNum(U8 num);
U8	TP_Indicator_GetDefaultSize(void);
void	TP_Indicator_SetDefaultSize(U8 size);
U8	TP_Indicator_GetDefaultSpace(void);
void	TP_Indicator_SetDefaultSpace(U8 space);

int  TP_Indicator_GetId(TP_INDICATOR_Handle hObj);
void TP_Indicator_SetId(TP_INDICATOR_Handle hObj, int id);

/*���û���indicatorʹ�õ�ͼƬ*/
const GUI_BITMAP* TP_Indicator_GetBitmap(TP_INDICATOR_Handle hObj, int index);
void TP_Indicator_SetBitmapEx           (TP_INDICATOR_Handle hObj,  int index,  \
                                                const GUI_BITMAP *pBitmap, int x, int y) ;
void TP_Indicator_SetBitmap             (TP_INDICATOR_Handle hObj,  int index,  \
                                                const GUI_BITMAP *pBitmap) ;
/*��ú����û���indicatorʹ�õ���ɫ*/
GUI_COLOR TP_Indicator_GetColorIndex(TP_INDICATOR_Handle hObj,  int index);
void      TP_Indicator_SetColorIndex(TP_INDICATOR_Handle hObj,  int index, GUI_COLOR color);
/*��ú����û���indicatorʹ�õĿ��������*/
U8   TP_Indicator_GetAaFactor(TP_INDICATOR_Handle hObj);
void TP_Indicator_SetAaFactor(TP_INDICATOR_Handle hObj, U8 factor);
/*��ú����û���indicatorʹ�õ�͸��ϵ��*/
U8   TP_Indicator_GetIndicatorAlpha (TP_INDICATOR_Handle hObj, int index);
void TP_Indicator_SetIndicatorAlpha (TP_INDICATOR_Handle hObj, int index, U8 Alpha);
/*��ú�����indicator��ǰָʾ��ҳ�棬��1��ʼΪ��һҳ*/
U8   TP_Indicator_GetPage (TP_INDICATOR_Handle hObj);
void TP_Indicator_SetPage (TP_INDICATOR_Handle hObj, U8 page);
/*��ú�����indicator��ҳ�����*/
U8   TP_Indicator_GetNum  (TP_INDICATOR_Handle hObj);
void TP_Indicator_SetNum  (TP_INDICATOR_Handle hObj, U8 num);
/*��ú����û���indicatorʹ�õ�Բ��ֱ��*/
U8   TP_Indicator_GetSize (TP_INDICATOR_Handle hObj);
void TP_Indicator_SetSize (TP_INDICATOR_Handle hObj, U8 size);
/*��ú����û���indicatorԲ�ļ�ࣨ��Բ���Ե���룩*/
U8   TP_Indicator_GetSpace(TP_INDICATOR_Handle hObj);
void TP_Indicator_SetSpace(TP_INDICATOR_Handle hObj, U8 space);
int TP_Indicator_GetUserData(TP_INDICATOR_Handle hWin, void * pDest, int sizeOfBuffer);
int TP_Indicator_SetUserData(TP_INDICATOR_Handle hWin, void * pSrc, int sizeOfBuffer);
/*indicatormĬ��ʹ�õĻص�����*/
void TP_Indicator_Callback(WM_MESSAGE * pMsg);


TP_INDICATOR_Handle TP_Indicator_Create(int x0, int y0, int xSize, int ySize,   \
									WM_HWIN hWinParent, U32 style,              \
									WM_CALLBACK * pfCallback, int numExtraBytes, int id);


#endif/* __TP_INDICATOR_H__ */