/*TP_Toast.h*/

#ifndef __TP_TOAST_H__
#define __TP_TOAST_H__

#include "GUI.h"
#include "WM.h"




/*********************************************************************
*
*  Toast pics indices
*/
#define TP_TOAST_BKGROUND					0
#define TP_TOAST_TEXT 		                1
#define TP_TOAST_MAX					    2


//#define GUI_DEFAULT_FONT          &GUI_Font24_ASCII

typedef WM_HMEM TP_TOAST_Handle;


typedef struct {
	
	int id;
	
	const GUI_BITMAP * pBkBitmap;           /*ѡ��ǰ����δѡ��ǰ��������ͼƬ*/

    U8 toastAlpha[TP_TOAST_MAX];            /*ѡ��ǰ����δѡ��ǰ��������͸��ϵ��*/

    GUI_COLOR colorIndex[TP_TOAST_MAX];      /*ѡ��ǰ����δѡ��ǰ����������ɫ*/

    U8 aaFactor;                            /*���������*/

    U8 radius;                              /*���ڵ�Բ�ǰ뾶*/

    int lastTime;                           /*toast ����ʱ��,��λ����*/

    WM_HTIMER hTimer;

    int fadeTime;                           /*toast ����ʱ��,��λ����*/
	
	const GUI_FONT* pFont;                        /*toast �ı�����*/

    int textAlign;                          /*toast �ı����뷽ʽ*/

    char *pText;

    int numExtraBytes;                      /*toast����洢�ռ�*/
	
}TP_TOAST_Obj;
const GUI_FONT* TP_Toast_GetDefaultFont(void);;
const GUI_FONT* TP_Toast_SetDefaultFont(const GUI_FONT* pFont);
GUI_COLOR TP_Toast_GetDefaultColorIndex(int index);
void      TP_Toast_SetDefaultColorIndex(GUI_COLOR color, int index);
U8   	TP_Toast_GetDefaultToastAlpha(int index);
void 	TP_Toast_SetDefaultToastAlpha(U8 alpha, int index);
int	TP_Toast_GetDefaultTextAlign(void);
void	TP_Toast_SetDefaultTextAlign(int align);
int	TP_Toast_GetDefaultAaFactor(void);
void	TP_Toast_SetDefaultAaFactor(int factor);
U8	TP_Toast_GetDefaultRadius(void);
void	TP_Toast_SetDefaultRadius(int radius);
int	TP_Toast_GetDefaultLastTime(void);
void	TP_Toast_SetDefaultLastTime(int time);
int	TP_Toast_GetDefaultFadeTime(void);
void	TP_Toast_SetDefaultFadeTime(int time);
int	TP_Toast_GetDefaultNumExtraBytes(void);
void	TP_Toast_SetDefaultNumExtraBytes(int bytes);
int 	TP_Toast_GetId(TP_TOAST_Handle hObj);
void 	TP_Toast_SetId(TP_TOAST_Handle hObj,int id);
const GUI_BITMAP* TP_Toast_GetBitmap(TP_TOAST_Handle hObj);
void 	          TP_Toast_SetBitmapEx(TP_TOAST_Handle hObj, const GUI_BITMAP *pBitmap, int x, int y);
void 	          TP_Toast_SetBitmap(TP_TOAST_Handle hObj, const GUI_BITMAP *pBitmap) ;
U8 	      TP_Toast_GetToastAlpha(TP_TOAST_Handle hObj,U8 index);
void 	  TP_Toast_SetToastAlpha (TP_TOAST_Handle hObj, U8 index, U8 Alpha);
GUI_COLOR TP_Toast_GetColorIndex(TP_TOAST_Handle hObj, int index);
void 	  TP_Toast_SetColorIndex(TP_TOAST_Handle hObj, int index, GUI_COLOR color);
U8 	      TP_Toast_GetAaFactor(TP_TOAST_Handle hObj);
void 	  TP_Toast_SetAaFactor(TP_TOAST_Handle hObj, U8 factor);
U8        TP_Toast_GetRadius(TP_TOAST_Handle hObj);
void TP_Toast_SetRadius(TP_TOAST_Handle hObj, U8 radius);
void TP_Toast_SetLastTime(TP_TOAST_Handle hObj, int time);
int  TP_Toast_GetFadeTime(TP_TOAST_Handle hObj);
void TP_Toast_SetFadeTime(TP_TOAST_Handle hObj, int time);
const GUI_FONT *  TP_Toast_GetFont(TP_TOAST_Handle hObj, const GUI_FONT * pFont);
const GUI_FONT *  TP_Toast_SetFont(TP_TOAST_Handle hObj, const GUI_FONT * pFont);
int  TP_Toast_GetTextAlign(TP_TOAST_Handle hObj);
void TP_Toast_SetTextAlign(TP_TOAST_Handle hObj, int align);
int  TP_Toast_GetText(TP_TOAST_Handle hObj, char * pBuffer, unsigned int maxLen);
int  TP_Toast_SetText(TP_TOAST_Handle hObj, const char* pText);
int TP_Toast_GetUserData(TP_TOAST_Handle hWin, void * pDest, int sizeOfBuffer);
int TP_Toast_SetUserData(TP_TOAST_Handle hWin, void * pSrc, int sizeOfBuffer);
void TP_Toast_Callback(WM_MESSAGE * pMsg);

TP_TOAST_Handle TP_TOAST_Create(int x0, int y0, int xSize, int ySize,                  \
								WM_HWIN hWinParent, U32 style,char *pText,             \
								WM_CALLBACK * pfCallback,  int numExtraBytes, int id);


#endif/* __TP_TOAST_H__ */