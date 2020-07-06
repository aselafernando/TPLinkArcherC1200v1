/*!Copyright(c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 *\file		libledctrl.h
 *\brief	 
 *
 *\author	Zhang Zhongwei
 *\version	1.0.0
 *\date		29Apr14
 *
 *\history \arg 1.0.0, 29Apr14, Zhang Zhongwei, Create the file. 	
 */


#ifndef _LIBLEDCTRL_H_
#define _LIBLEDCTRL_H_

/***************************************************************************/
/*						CONFIGURATIONS					 */
/***************************************************************************/

/***************************************************************************/
/*						INCLUDE_FILES					 */
/***************************************************************************/
#include "log.h"

/***************************************************************************/
/*						DEFINES						 */
/***************************************************************************/
#define LEDCTRL_VERSION   0x10001
#define LEDCTRL_MAX_BUFF  1024
#define LEDCTRL_MAX_NAME  128

#if 0
#define LED_DEBUG  1

#define LEDCTRL_ERROR(format, args...) fprintf(stderr, "%s(%d): "format"\r\n",__FILE__,__LINE__,##args)
#if LED_DEBUG
#define LEDCTRL_DEBUG(format, args...) fprintf(stdout, "%s(%d): "format"\r\n",__FILE__,__LINE__,##args)
#else
#define LEDCTRL_DEBUG(format, args...) while(0) {}
#endif
#endif

#if 0
#define LED_DEBUG  1
#define LED_LOG_ID 288

#define LEDCTRL_INFO(msg_id, args...) log(LED_LOG_ID, msg_id, ##args)
#if LED_DEBUG
#define LEDCTRL_DBG(msg_id, args...) log(LED_LOG_ID, msg_id, ##args)
#else
#define LEDCTRL_DBG(msg_id, args...) while(0) {}
#endif
#endif
/***************************************************************************/
/*						TYPES							 */
/***************************************************************************/
typedef struct _LEDCTRL_MSG
{
    int  version;
    char name[LEDCTRL_MAX_NAME];
} LEDCTRL_MSG;


/***************************************************************************/
/*						VARIABLES						 */
/***************************************************************************/

/***************************************************************************/
/*						FUNCTIONS						 */
/***************************************************************************/
int ledctrl_run(const char *name);

#endif

