/*
**	scripts/kconfig/dep_common.h

--	*********************************************************************
||	common head file for dep_*.c
||
||	1. define/declare Macros, data structure, routines, which will used to find
||	dependency/depent tree for some kernel config. 
||
||	2. parse ".deptables" and apply it to rewrite default value of config item
||	into .config file.
||	In this process, warning && error message and integrated config item dependency
||	tree of .deptables are recorded in "deptab.log" file of kernel source code dir.
--	*********************************************************************

@@	By TengFei, 13/07/01
*/

#ifndef	DEP_COMMON_H
#define	DEP_COMMON_H

/* feature Macros supported */
#define	IPT_DEP_TABLE_SUPPORT
//#define	IPT_DEP_TREE_ONLY_SPT
#define	IPT_DEPT_TAB_SUPPORT
#define	IPT_LIST_ALL_CFG_SPT

#define	DEP_TAB_LINE_LEN_MAX		256

/*
**	define operation modes.
*/
typedef enum
{
	MODE_INV_VAL = 0x0,
		
	MODE_DEP_TAB = 0x1,
	MODE_TREE_ONLY = 0x2,
	MODE_DEPT_TAB = 0x3,

	MODE_MAX_VAL = 0xff
}IPT_DEP_MODE;


struct deptab_node
{
	struct deptab_node *list;
	struct deptab_node *next;
	struct deptab_node *pa;
	
	char* cfg_name;
	char* cfg_value;

	unsigned char phase_flag;
	unsigned char item_flag;
	unsigned char val_ymn;
	
	struct symbol *sym;
	void* range;

	int be_select;

#ifdef	IPT_DEPT_TAB_SUPPORT
	struct menu* src_menu;
#endif	/* IPT_DEPT_TAB_SUPPORT */
};


#ifdef IPT_DEP_TABLE_SUPPORT
struct range_data 
{
	unsigned tri_mask;
};

void deptab_check_node
(
	struct symbol *sym, 
	const char* val, 
	int type
);
int deptab_checker(int);
int deptab_handler(IPT_DEP_MODE m);
#endif	/* IPT_DEP_TABLE_SUPPORT */

#ifdef	IPT_DEPT_TAB_SUPPORT
int dept_tab_hdlr(IPT_DEP_MODE m);
#endif	/* IPT_DEPT_TAB_SUPPORT */


/*
**	Seek through the whole menu to find all the config items.
*/
#ifdef  IPT_LIST_ALL_CFG_SPT
void dep_find_all_configs(void);
#endif	/* IPT_LIST_ALL_CFG_SPT */

#endif	/* DEP_COMMON_H */

