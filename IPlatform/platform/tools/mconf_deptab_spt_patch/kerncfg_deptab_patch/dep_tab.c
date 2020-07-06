/*
**	scripts/xconfig/dep_tab.c

--	*********************************************************************
||	source file to parse ".deptables" and apply it to rewrite default value of config item
||	into .config file.
||	In this process, warning && error message and integrated config item dependency
||	tree of .deptables are recorded in "deptab.log" file of kernel source code dir.
||
||	The following is one example of "deptab.log":
||
||	[warning] __code:2__ Can not find corresponding symbol for config item NET_IPGRE_DEMUX.
||	[warning] __code:2__ Can not find corresponding symbol for config item PPTP.
||	###############################################################
||	#  Integrated kernel config item dependency tree for deptables.
||	###############################################################
||	|-- CRC_CCITT=m(tristate, *)
||	|-- NET_IPGRE=m(tristate, *)
||	|   |-- NET=y(boolean, *)
||	|   `-- IPV6=m(tristate, *)
||	|       |-- NET=y(boolean)
||	|       `-- INET=y(boolean)
||	|-- NET_IPGRE_DEMUX=m(none, *)
||	|-- PPP=m(tristate, *)
||	|   `-- NETDEVICES=y(boolean, *)
||	|       `-- NET=y(boolean)
||	|-- PPPOE=m(tristate, *)
||	|   |-- NETDEVICES=y(boolean)
||	|   |-- EXPERIMENTAL=y(boolean, *)
||	|   `-- PPP=m(tristate)
||	|-- PPP_ASYNC=m(tristate, *)
||	|   |-- NETDEVICES=y(boolean)
||	|   `-- PPP=m(tristate)
||	|-- PPTP=m(none, *)
||	`-- SLHC=m(tristate, *)
||	    `-- NETDEVICES=y(boolean)
||	[error] __code:6__ Just no symbol for config item NET_IPGRE_DEMUX.
||	[error] __code:6__ Just no symbol for config item PPTP.
--	*********************************************************************

@@	By TengFei, 14/07/04
*/



/*
**	DEFINITIONS AND DECLARATIONS
*/
#define	DEPTAB_FILE_NAME					".deptables"
#define	DEPTAB_ASS_FILE					".ass_deptable"
#define	DEPTAB_OUTPUT_FILE				"deptables.log"

#define	TREE_ONLY_FILE_NAME				"dep.table"
#define	TREE_ONLY_OUTPUT_FILE			"dep_table.log"

#ifdef	IPT_DEPT_TAB_SUPPORT
#define	DEPT_TAB_FILE_NAME				"dept.table"
#define	DEPT_TAB_OUTPUT_FILE				"dept_table.log"
#endif	/* IPT_DEPT_TAB_SUPPORT */

#define	DEPTAB_CFG_ITEM_PREFIX			"CONFIG_"

#define	DEPTAB_RET_VAL_IGN			(-1)
#define	DEPTAB_RET_VAL_OK				0

#define	DEPTAB_COMMON_CODE			0

#define	deptab_message(lv, code, fmt, args...)		\
	do {		\
		if (lv == LV_ERROR)	\
		{					\
			show_set_err_event(OP_SET, code);	\
		}					\
		show_message(gset_dep_mode(OP_GET, 0), lv, code, fmt, ##args);	\
	} while (0)

#define	deptab_summary(lv, fmt, args...)		\
	do {		\
		show_message(gset_dep_mode(OP_GET, 0), lv, DEPTAB_COMMON_CODE, fmt, ##args);	\
	} while (0)


/*
**	Define a enumerated type that indicates the warning number of deptab operations. 
*/
typedef	enum
{
	DEPTAB_WARN_NONE = 0x0,

	DEPTAB_WARN_ARGS = 0x1,
	DEPTAB_WARN_SYM_NONE = 0x2,
	DEPTAB_WARN_MALLOC = 0x3,
	DEPTAB_WARN_SYM_ANOTHER = 0x4,
	DEPTAB_WARN_EXP_SYM_ENTITY_NONE = 0x5,
	DEPTAB_WARN_EXP_SYM_UNEQUAL = 0x6,
	DEPTAB_WARN_SYM_TYPE_UNKNOWN = 0x7,
	DEPTAB_WARN_DEPTAB_ADJ_PA_NONE = 0x8,
	DEPTAB_WARN_DEPTAB_LIST_EXIST = 0x9,
	DEPTAB_WARN_SYM_ATTACH_MULNODE = 0xa,

	DEPTAB_WARN_CHK_SYM_NONE = 0xb,
	DEPTAB_WARN_CHK_NAME_DIFF = 0xc,
	DEPTAB_WARN_CHK_VAL_NULL = 0xd,
	DEPTAB_WARN_CHK_VAL_DIFF = 0xe,
	DEPTAB_WARN_CHK_STR_VAL_DIFF = 0xf,
	DEPTAB_WARN_CHK_NAME_VAL_FAIL = 0x10,

	DEPTAB_WARN_OTHERS = 0xfe,
	DEPTAB_WARN_MAX =0xff
}DEPTAB_WARNO;


/*
**	Define a enumerated type that indicates the error number of deptab operations. 
*/
typedef	enum
{
	DEPTAB_ERR_NONE = 0x0,
	DEPTAB_ERR_COUNTS = 0x0,

	DEPTAB_ERR_FILE_LOST = 0x1,
	DEPTAB_ERR_NO_CFG = 0x2,
	DEPTAB_ERR_MALLOC = 0x3,
	DEPTAB_ERR_CFG_VAL_DISMATCH = 0x4,
	DEPTAB_ERR_CFG_VAL_SET = 0x5,
	DEPTAB_ERR_CFG_SYM_NULL = 0x6,
	DEPTAB_ERR_CFG_VISIBLE_NONE = 0x7,
	DEPTAB_ERR_SYM_NODE_NULL = 0x8,
	DEPTAB_ERR_CFG_VAL_CLSN = 0x9,
	DEPTAB_ERR_CFG_VAL_NULL = 0xa,
	DEPTAB_ERR_CHK_NAME_VAL_FAIL = 0xb,
	DEPTAB_ERR_ITEM_TYPE_OR_FAKE = 0xc,

	DEPTAB_ERR_NUM,

	DEPTAB_ERR_OTHERS = 0xfe,
	DEPTAB_ERR_MAX =0xff
}DEPTAB_ERRNO;


/*
**	Define a enumerated type that indicates the phase of node in deptab tree. 
*/
enum
{
	PH_INITED = 0x0,
	PH_AJTED = 0x1,
	PH_SETED = 0x2,

	PH_CHK_OK = 0x3,
	PH_CHK_ERR = 0x4,
	PH_CHK_UNSURE = 0x5,

	PH_MAX
};


/*
**	Define a enumerated type that indicates the config item flag & type of node in deptab tree. 
*/
enum
{
	ITEM_FLAG_NORMAL = 0x0,
	ITEM_FLAG_OR_PA = 0x1,
	ITEM_FLAG_OR_IGN = 0x2,	/* must be used with ITEM_TYPE_OR_FAKE, otherwise it's no sence. */
	ITEM_FLAG_INVISIBLE = 0x4,
	
	ITEM_FLAG_MAX =0xf,

	
	ITEM_TYPE_CHOICE = 0x10,
	ITEM_TYPE_OR_FAKE = 0x20,
	ITEM_TYPE_OR_IGN = (ITEM_TYPE_OR_FAKE | ITEM_FLAG_OR_IGN),
	
	ITEM_TYPE_MAX = 0xff
};


/*
**	Define a enumerated type that indicates the range for value of config item in deptab tree. 
*/
enum
{
	RANGE_VAL_IGN = 0x0,
	RANGE_VAL_N = 0x1,
	RANGE_VAL_M = 0x2,
	RANGE_VAL_Y = 0x4,
	RANGE_VAL_YM = (RANGE_VAL_M | RANGE_VAL_Y),

	RANGE_VAL_MAX
};


/*
**	define operation types.
*/
typedef	enum
{
	OP_SHOW = 0x0,
	
	OP_GET = 0x0,
	OP_SET = 0x1,
	OP_GET_DIR = 0x2,

	OP_INV
}DEP_TAB_OP;


/*
**	Define a enumerated type that indicates the error number of deptab operations. 
*/
typedef	enum
{
	LV_INFO,
	LV_NOTICE,
	LV_WARN,
	LV_ERROR,

	/* As a summary description */
	LV_SUM_LOW,
	LV_SUM_MID,
	LV_SUM_HIGH,

	LV_MAX

}MSG_LEVEL;


/*
**	global and local variables
*/
extern struct menu rootmenu;

static char* deptab_val_y = "y";
static char* deptab_val_m = "m";
static char* deptab_val_n = "n";
static char* deptab_choice = "<choice>";
static char* deptab_OR_item = "item_or_fake";


/*
**	PRIVATE FUNTIONS
*/
/*
**	get/set operation mode via arguments
*/
static IPT_DEP_MODE gset_dep_mode(DEP_TAB_OP op, IPT_DEP_MODE mode)
{
	static IPT_DEP_MODE dep_mode = MODE_INV_VAL;

	switch (op)
	{
	case OP_GET:
		return dep_mode;
	case OP_SET:
		dep_mode = mode;
		break;
	default:
		/* Nothing to do */;
	}

	return MODE_INV_VAL;
}


/*
**	get pointer of dbg_file
*/
static FILE* get_dbg_fp(IPT_DEP_MODE mode)
{
	static FILE *dbg_fp1 = NULL, *dbg_fp2 = NULL, *dbg_fp3 = NULL;

	switch (mode)
	{
	case MODE_DEP_TAB:
		if (!dbg_fp1)
		{
			dbg_fp1 = fopen(DEPTAB_OUTPUT_FILE, "w+");
			if (!dbg_fp1)
			{
				fprintf(stdout, "open "DEPTAB_OUTPUT_FILE" failed!\n");
				dbg_fp1 = stdout;
			}
		}
		return dbg_fp1;

	case MODE_TREE_ONLY:
		if (!dbg_fp2)
		{
			dbg_fp2 = fopen(TREE_ONLY_OUTPUT_FILE, "w+");
			if (!dbg_fp2)
			{
				fprintf(stdout, "open "TREE_ONLY_OUTPUT_FILE" failed!\n");
				dbg_fp2 = stdout;
			}
		}
		return dbg_fp2;

	case MODE_DEPT_TAB:
		if (!dbg_fp3)
		{
			dbg_fp3 = fopen(DEPT_TAB_OUTPUT_FILE, "w+");
			if (!dbg_fp3)
			{
				fprintf(stdout, "open "DEPT_TAB_OUTPUT_FILE" failed!\n");
				dbg_fp3 = stdout;
			}
		}
		return dbg_fp3;

	default:
		/* Nothing to do! */;
	}

	return stdout;
}


/*
**	API to show some significative messages to help usr to find what the problem is. 
*/
static void show_message(IPT_DEP_MODE mode, int level, int code, const char *fmt, ...)
{
	FILE *fp = get_dbg_fp(mode);
	static unsigned int sum_count = 0;
	
	if (LV_NOTICE >= level)
	{
		return;
	}

	if (LV_SUM_LOW == level)
	{
		return;
	}
	
	va_list ap;
	va_start(ap, fmt);
	switch (level)
	{
	case LV_INFO:
		fprintf(fp, "[info] __code:%d__ ", code);
		break;

	case LV_NOTICE:
		fprintf(fp, "[notice] __code:%d__ ", code);
		break;
		
	case LV_WARN:
		fprintf(fp, "[warning] __code:%d__ ", code);
		break;

	case LV_ERROR:
		fprintf(fp, "[error] __code:%d__ ", code);
		break;

	case LV_SUM_LOW:
		fprintf(fp, "_ ");
		break;
		
	case LV_SUM_HIGH:
		fprintf(fp, "* ");
	case LV_SUM_MID:
		fprintf(fp, "%d. ", ++sum_count);
		break;

	default:
		fprintf(fp, "[lv%d] __code:%d__ ", level, code);
	}
	
	vfprintf(fp, fmt, ap);
	fprintf(fp, "\n");
	va_end(ap);
}


/*
**	Show errors for various events OR Set various error events.
*/
static void show_set_err_event(DEP_TAB_OP op, unsigned code)
{
	static unsigned char err_list[DEPTAB_ERR_NUM];
	unsigned char idx = 0x0, count = 0x0;

	if (op != OP_SHOW && op != OP_SET)
	{
		/* Just return without complainning! */
		return;
	}

	if (op == OP_SET)
	{
		if (code > DEPTAB_ERR_NONE && code < DEPTAB_ERR_NUM)
		{
			err_list[code] = err_list[code] % 0xff + 0x1;
			err_list[DEPTAB_ERR_COUNTS] = err_list[DEPTAB_ERR_COUNTS] % 0xff + 0x1;
		}
		return;
	}

	if (!err_list[DEPTAB_ERR_COUNTS])
	{
		return;
	}

	printf("\n|=== Total %d Error%s when parsing & applying "DEPTAB_FILE_NAME" ===\n",
		err_list[DEPTAB_ERR_COUNTS], (err_list[DEPTAB_ERR_COUNTS] > 1)  ? "s occur" : " occurs");
	for (idx = DEPTAB_ERR_NONE + 1; idx < DEPTAB_ERR_NUM; idx++)
	{
		if (err_list[idx])
		{
			count++;
			printf("+ %d. Error(code:0x%x, times:%d)\t", count, idx, err_list[idx]);
			switch (idx)
			{	
			case DEPTAB_ERR_FILE_LOST:
				puts("No deptable file: "DEPTAB_FILE_NAME".");
				break;
			case DEPTAB_ERR_MALLOC:
				puts("No enough memory to alloc var.");
				break;
			case DEPTAB_ERR_CFG_VAL_DISMATCH:
				printf("Some config item%s dismatch %s.\n", 
					(err_list[idx] > 1) ? "s whose values" : " whose value", (err_list[idx] > 1) ? "them" : "it");
				break;
			case DEPTAB_ERR_CFG_VAL_SET:
				printf("It's failed to set value for some config item%s.\n", (err_list[idx] > 1) ? "s" : "");
				break;

			case DEPTAB_ERR_NO_CFG:
			case DEPTAB_ERR_CFG_SYM_NULL:
				printf("Just no symbol for some config item%s.\n", (err_list[idx] > 1) ? "s" : "");
				break;
			case DEPTAB_ERR_CFG_VISIBLE_NONE:
				printf("It is not visible for some config item%s.\n", (err_list[idx] > 1) ? "s" : "");
				break;
			case DEPTAB_ERR_SYM_NODE_NULL:
				printf("The symbol%s without one node in deptab.\n", (err_list[idx] > 1) ? "s appear" : " appears");
				break;
			case DEPTAB_ERR_CFG_VAL_CLSN:
				printf("The config item%s and %s value%s are in collision.\n", 
					(err_list[idx] > 1) ? "s" : "", (err_list[idx] > 1) ? "their" : "its", (err_list[idx] > 1) ? "s" : "");
				break;
			case DEPTAB_ERR_CFG_VAL_NULL:
				printf("The value of some config item%s null, perhaps %s in E_UNEQUAL expression case.\n", 
					(err_list[idx] > 1) ? "s are" : " is", (err_list[idx] > 1) ? "these config items" : "this config item");
				break;
			case DEPTAB_ERR_CHK_NAME_VAL_FAIL:
				printf("Some config %s can not be set to .config.\n", (err_list[idx] > 1) ? "items with their values" : "item with its value");
				break;
			case DEPTAB_ERR_ITEM_TYPE_OR_FAKE:
				printf("Some config item%s on \"OR\" expression(s).\n", (err_list[idx] > 1) ? "s  depend" : " depends");
				break;

			default:
				printf("Other error%s.\n", (err_list[idx] > 1) ? "s" : "");
			}
		}
	}
	printf("|=== For more detailed information please refer to "DEPTAB_OUTPUT_FILE" ===\n\n");
}



/*
**	Just create && initialize first node of deptab.
*/
static struct deptab_node* get_root_deptab_node(void)
{
	static struct deptab_node *root_deptab_node = NULL;

	if (!root_deptab_node)
	{
		root_deptab_node = malloc(sizeof(struct deptab_node));
		if (!root_deptab_node)
		{
			deptab_message(LV_ERROR, DEPTAB_ERR_MALLOC, 
				"No enough memory to alloc root_deptab_node, just exit running.");
			exit(DEPTAB_ERR_MALLOC);
		}
		memset(root_deptab_node, 0x0, sizeof(struct deptab_node));
	}
	return root_deptab_node;
}


/*
**	Print integrated config item dependency tree based DEPTAB_FILE_NAME & DEPTAB_ASS_FILE
*/
static void print_deptab_tree(struct deptab_node* p_node, bool prt_head)
{
	int layer = 1, i = 0;
	unsigned	long showmap = 0;
	IPT_DEP_MODE dep_mode = gset_dep_mode(OP_GET, 0);
	FILE *fp = get_dbg_fp(dep_mode);
	struct deptab_node* node = p_node;

	if (node && prt_head)
	{
		fprintf(fp, "\n");
		fprintf(fp, "#########################################################\n");
		fprintf(fp, "#  Integrated config item dependency tree for deptables.\n");
		fprintf(fp, "#########################################################\n");
	}
	
	while (node)
	{
		for (i = 0; i < layer; i++)
		{
			if (i + 1 == layer)
			{
				if (node->next)
				{
					if (node->item_flag & ITEM_TYPE_OR_FAKE)
						fprintf(fp, "|~~ ");
					else
						fprintf(fp, "|-- ");
					/* set bit_$(layer) of showmap to 0 */
					showmap &= (~(1 << i));
				}
				else
				{
					if (node->item_flag & ITEM_TYPE_OR_FAKE)
						fprintf(fp, "`~~ ");
					else
						fprintf(fp, "`-- ");
					/* set bit_$(layer) of showmap to 1 */
					showmap |= (1 << i);
				}
				break;
			}
			if (!((showmap >> i) & 0x1))
			{
				fprintf(fp, "|   ");
			}
			else
			{
				fprintf(fp, "    ");
			}
		}

		switch (dep_mode)
		{
		case MODE_DEPT_TAB:
			fprintf(fp, "%s%s%s(%s, %s%s)"
			#ifdef	IPT_DEPT_TAB_SUPPORT
				" {%s:%d}"
			#endif	/* IPT_DEPT_TAB_SUPPORT */
				"\n", 
				(node->be_select == -2 || node->be_select == 2) ? "+ " : "",
				(node->be_select < 0) ? "!" : "",
				node->cfg_name ? node->cfg_name : (node->sym ? node->sym->name : "none"),
				node->sym ? sym_type_name(node->sym->type): "none",
				node->sym ? (sym_get_string_value(node->sym) ? sym_get_string_value(node->sym) : "unknown") : "none",
				node->cfg_name ? ", *" : ""
			#ifdef	IPT_DEPT_TAB_SUPPORT
				, (node->src_menu && node->src_menu->file && node->src_menu->file->name) ? node->src_menu->file->name : "none",
				(node->src_menu) ? node->src_menu->lineno : 0x0
			#endif	/* IPT_DEPT_TAB_SUPPORT */
				);
			break;

		case MODE_TREE_ONLY:
			fprintf(fp, "%s%s%s%s(%s, %s%s) %s\n", 
				node->be_select ? "+ " : "",
				strcmp(node->cfg_value, deptab_val_n) ? "" : "!",
				(node->sym && node->sym->node && (node->sym->node->item_flag & ITEM_FLAG_INVISIBLE)) ? ". " : "",
				node->cfg_name ? node->cfg_name : (node->sym ? node->sym->name : "none"),
				node->sym ? sym_type_name(node->sym->type): "none",
				node->sym ? (sym_get_string_value(node->sym) ? sym_get_string_value(node->sym) : "unknown") : "none",
				node->cfg_name ? ", *" : "",
				((node->item_flag & ITEM_TYPE_OR_IGN) == ITEM_TYPE_OR_IGN) ? "Ignored" : "");
			break;

		case MODE_DEP_TAB:
			fprintf(fp, "%s%s=%s(%s, %s%s) %s\n", 
				(node->sym && node->sym->node && (node->sym->node->item_flag & ITEM_FLAG_INVISIBLE)) ? ". " : "",
				node->cfg_name ? node->cfg_name : (node->sym ? node->sym->name : "none"),
				node->cfg_value, node->sym ? sym_type_name(node->sym->type): "none",
				(node->sym  && sym_get_string_value(node->sym)) ? sym_get_string_value(node->sym) : "unknown",
				node->cfg_name ? ", *" : "",
				((node->item_flag & ITEM_TYPE_OR_IGN) == ITEM_TYPE_OR_IGN) ? "Ignored" : "");
			break;

		default:
			/* Nothing to do. */;
		}
		
		if (node->list) 
		{
			node = node->list;
			layer++;
			continue;
		}
		if (node->next)
		{
			node = node->next;
		}
		else while ((node = node->pa)) 
		{
			layer--;
			if (node->next)
			{
				if (!prt_head && node == p_node)
				{
					/* Just return! */
					return;
				}
				node = node->next;
				break;
			}
		}
	}
}


/*
**	Judge this node contains one or more unknown type child node?
**	Yes return true, otherwise return false. 
*/
static bool have_ukn_type_node(struct deptab_node* n)
{
	struct deptab_node* t = NULL;

	if (n->sym && n->sym->type == S_UNKNOWN
		&& n->item_flag <= ITEM_FLAG_MAX )
	{
		return true;
	}

	for (t = n->list; t; t = t->next)
	{
		if (have_ukn_type_node(t))
		{
			return true;
		}
	}

	return false;
}


/*
**	Print node dependency tree for unknown type config item.
*/
static void print_ukn_dep_tree(struct deptab_node* node)
{
	struct deptab_node* t = NULL;
	FILE *fp = get_dbg_fp(gset_dep_mode(OP_GET, 0));
	unsigned int counts = 0x0;
	
	for (t = node; t; t = t->next)
	{
		if (have_ukn_type_node(t))
		{
			if (!counts)
			{
				fprintf(fp, "\n");
				fprintf(fp, "#########################################################\n");
				fprintf(fp, "#  Integrated dependency tree for ukn type config item.\n");
				fprintf(fp, "#########################################################\n");
			}
			++counts;
			print_deptab_tree(t, false);
		}
	}
}


/*
**	One of background funtion to adjust each sub-tree of deptab.
**	Make sure the more basic config item node, the earlier be found in LRD of deptab tree.
*/
static void up_bsc_node_deptab_tree(struct deptab_node* n)
{
	struct deptab_node* t = n->list;
	struct deptab_node *t2 = NULL;
	struct deptab_node node1, node2;

	if (t == NULL)
	{
		if (n->phase_flag == PH_AJTED)
		{
			return;
		}

		if (n->sym && n->sym->node && n->sym->node != n 
			&& !n->cfg_name)
		{
			t2 = n->sym->node;
			if (t2->phase_flag == PH_AJTED)
			{
				n->phase_flag = PH_AJTED;
				return;
			}

			deptab_message(LV_INFO, DEPTAB_COMMON_CODE, 
				"Should adjust node position of config item %s whose parent is %s and younger brother is %s.",
				n->sym->name ? n->sym->name : "unknown",
				(n->pa && n->pa->cfg_name) ? n->pa->cfg_name : "none",
				n->next ? (n->next->cfg_name ? n->next->cfg_name : "unknown") : "none");
			
			node1 = *n;
			node2 = *t2;
			
			n->sym->node = n;

			/* exchange with each other */
			*n = node2;
			*t2 = node1;

			/* revert following fileds to orignal value */
			t2->next = node2.next;
			t2->pa = node2.pa;
			t2->cfg_value = node2.cfg_value;
			t2->val_ymn = node2.val_ymn;
			t2->cfg_name = NULL;
		
			n->cfg_value = node1.cfg_value;
			n->next = node1.next;
			n->pa = node1.pa;
			n->val_ymn = node1.val_ymn;
			n->cfg_name = n->sym->name;

			/* mark phase_flag to  PH_AJTED */
			n->phase_flag = PH_AJTED;
			t2->phase_flag = PH_AJTED;
			
			for (t2 = n->list; t2; t2 = t2->next)
			{
				t2->pa = n;
			}
		}
	}

	for (t = n->list; t; t = t->next)
	{
		up_bsc_node_deptab_tree(t);
	}

	n->phase_flag = PH_AJTED;
}


/*
**	Does this node contains a ITEM_TYPE_OR_FAKE type child node?
**	Yes return true, otherwise return false. 
*/
static bool have_child_OR_node(struct deptab_node* n)
{
	struct deptab_node* t = NULL;

	if ((n->item_flag & ITEM_TYPE_OR_IGN) == ITEM_TYPE_OR_FAKE)
	{
		return true;
	}

	for (t = n->list; t; t = t->next)
	{
		if (have_child_OR_node(t))
		{
			return true;
		}
	}

	return false;
}


/*
**	One of background funtion to adjust each sub-tree of deptab.
**	If a trunk node appears as child node of another trunk node, put former node ahead of later node.
*/
static bool  up_trunk_node_deptab_tree(struct deptab_node* n)
{
	struct deptab_node *t, *actual = NULL, *former = NULL, *formest = NULL;
	struct deptab_node tmp_node;
	bool has_child_or = true;

	if (!n->sym || !n->sym->node || n->sym->node == n)
	{
		/* Just return without warnning! */
		return false;
	}

	actual = n->sym->node;
	/* same trunk node? */
	if (!actual->pa)
	{
		return false;
	}

	has_child_or = have_child_OR_node(actual);

	if (has_child_or)
	{
		former = actual->pa;
		while (former->pa)
		{
			former = former->pa;
		}
		
		formest = get_root_deptab_node();
		while (formest->next != former)
		{
			formest = formest->next;
		}
	}
	else
	{
		formest = get_root_deptab_node();
		while (!have_child_OR_node(formest->next))
		{
			formest = formest->next;
		}
		former = formest->next;
	}
	
	if (!formest || former == n)
	{
		/* Just return without warnning! */
		return false;
	}

	n->sym->node = n;

	tmp_node= *n;
	*n = *actual;
	n->cfg_value = tmp_node.cfg_value;
	n->val_ymn = tmp_node.val_ymn;
	n->pa = tmp_node.pa;
	n->cfg_name = (tmp_node.sym)->name;
	n->next = former;
	
	for (t = actual->list; t; t = t->next)
	{
		t->pa = n;
	}
	formest->next = n;

	actual->list = NULL;
	actual->cfg_name = NULL;

	return true;
}


/*
**	Judge whether the node and its children node are all ITEM_TYPE_OR_FAKE type?
**	Yes, return true, else return false.
*/
static inline bool is_OR_fake_family(struct deptab_node* n)
{
	struct deptab_node* t = n->list;
		
	if (!(n->item_flag & ITEM_TYPE_OR_FAKE) || !t)
	{
		return false;
	}

	for (t = n->list; t; t = t->next)
	{
		if(!(t->item_flag & ITEM_TYPE_OR_FAKE))
		{
			return false;
		}
	}

	return true;
}


/*
**	One of background funtion to adjust each sub-tree of deptab.
**	Merge ITEM_TYPE_OR_FAKE type nodes to make deptab tree more clearly.
*/
static void merge_ORnode_deptab_tree(struct deptab_node* n)
{
	struct deptab_node* t = n->list;
	struct deptab_node *farther_ch = NULL, *farthest_ch = NULL;
	struct deptab_node node;

	if (!t)
	{
		return;
	}
	
	while (is_OR_fake_family(n))
	{
		while (true)
		{
			farther_ch = n->list;
			farthest_ch = farther_ch->next;

			while (farthest_ch && farthest_ch->next)
			{
				farther_ch = farthest_ch;
				farthest_ch = farthest_ch->next;
			}
			
			if (!farthest_ch)
			{
			#if	1
				/* treat its first child as itself! */
				node = *n;
				*n = *farther_ch;
				n->pa = node.pa;
				n->next = node.next;

				for (t = farther_ch->list; t; t = t->next)
				{
					t->pa = n;
				}

				free(farther_ch);
			#endif	
				break;
			}

		#if	1
			/* treat farthest child as its younger brother! */
			farthest_ch->next = n->next;
			farthest_ch->pa = n->pa;
			n->next = farthest_ch;
			farther_ch->next = NULL;
		#endif
		}
	}


	for (t = n->list; t; t = t->next)
	{
		merge_ORnode_deptab_tree(t);
	}

}


/*
**	One of background funtion to adjust each sub-tree of deptab.
**	Put node of ITEM_TYPE_OR_FAKE type farthest among its brother nodes.
*/
static void far_ORnode_deptab_tree(struct deptab_node* n)
{
	struct deptab_node* t = n->list;
	struct deptab_node *prev = NULL, *curr = NULL;

	if (!t)
	{
		return;
	}
	
	if (n->item_flag & ITEM_FLAG_OR_PA)
	{
		prev = NULL;
		t = n->list;
		while (t)
		{
			if (t->item_flag & ITEM_TYPE_OR_FAKE)
			{
				break;
			}
			prev = t;
			t = t->next;
		}

		if (!t)
		{
			goto next;
		}
		
		for (curr = t, t = t->next; t; t = t->next)
		{
			if (t->item_flag & ITEM_TYPE_OR_FAKE)
			{
				curr = t;
				continue;
			}
			/* move nor-ITEM_TYPE_OR_FAKE node up! */
			curr->next = t->next;
			if (!prev)
			{
				t->next = n->list;
				n->list = t;
			}
			else
			{
				t->next = prev->next;
				prev->next = t;
			}

			prev = t;
			t = curr;
		}
	}

next:
	for (t = n->list; t; t = t->next)
	{
		far_ORnode_deptab_tree(t);
	}
}


/*
**	One of background funtion to adjust each sub-tree of deptab.
**	Calc the value of ITEM_TYPE_OR_FAKE type brother nodes, if the value equal 0x1, just ignore them!
*/
static void ign_ORnode_deptab_tree(struct deptab_node* n)
{
	struct deptab_node* t = n->list;
	struct deptab_node *t2 = NULL, *t3 = NULL;

	if (!t)
	{
		if (n->item_flag & ITEM_TYPE_OR_FAKE)
		{
			n->item_flag |= ITEM_FLAG_OR_IGN;
		}
		return;
	}

	if ((n->item_flag & ITEM_TYPE_OR_IGN) == ITEM_TYPE_OR_IGN)
	{
		return;
	}
	
	if ((n->item_flag & ITEM_TYPE_OR_FAKE) && !t->next
		&& (t->item_flag <= ITEM_FLAG_MAX))
	{
		/* Improve it later! */
		t2 = n->next;
		if (t2 && (t2->item_flag & ITEM_TYPE_OR_FAKE) && t2->list && !t2->list->next
			&& (t2->list->item_flag <= ITEM_FLAG_MAX))
		{
			t3 = t2->list;
			if (t->sym && t->sym == t3->sym && strcmp(t->cfg_value, t3->cfg_value)
				&& (t->sym->type == S_BOOLEAN || t->sym->type == S_TRISTATE))
			{
				/* one is "y", the other one is "n" */
				if (t->sym->type == S_BOOLEAN)
				{
					n->item_flag |= ITEM_FLAG_OR_IGN;
					t2->item_flag |= ITEM_FLAG_OR_IGN;
				}
				else
				{
					if ((!strcmp(t->cfg_value, deptab_val_n) && (t3->val_ymn == RANGE_VAL_YM))
						|| ((!strcmp(t3->cfg_value, deptab_val_n) && (t->val_ymn == RANGE_VAL_YM))))
					{
						n->item_flag |= ITEM_FLAG_OR_IGN;
						t2->item_flag |= ITEM_FLAG_OR_IGN;
					}
				}
			}
		}
	}

	if ((n->item_flag & ITEM_TYPE_OR_IGN) == ITEM_TYPE_OR_IGN)
	{
		return;
	}

	for (t = n->list; t; t = t->next)
	{
		if ((t->item_flag & ITEM_TYPE_OR_IGN) != ITEM_TYPE_OR_IGN)
		{
			ign_ORnode_deptab_tree(t);
		}
	}
}


/*
**	One of background funtion to adjust each sub-tree of deptab.
**	If a trunk node does not contain a ITEM_TYPE_OR_FAKE type child node, move it topper.
*/
static void far_trunk_ORnode(struct deptab_node* n)
{
	struct deptab_node* current = n->next;
	struct deptab_node* prev_or = n;
	struct deptab_node* first_or = NULL;
	struct deptab_node* prep_curr = n->next;
	struct deptab_node* tmp = NULL;
	

	while (current)
	{
		tmp = current->next;
		
		/* This trunk node contains a ITEM_TYPE_OR_FAKE type child node. */
		if (!current->cfg_name || have_child_OR_node(current))
		{
			if (!first_or)
			{
				first_or = current;
			}
			prep_curr = current;
		}
		else
		{
			if (first_or)
			{
				prep_curr->next  = current->next;
				current->next = first_or;
				prev_or->next = current;
			}
			prev_or = current;
		}
		
		current = tmp;
	}
}


/*
**	Adjust integrated kernel config item dependency tree of deptab.
**	#. Merge ITEM_TYPE_OR_FAKE type nodes to make deptab tree more clearly.
**	#. Put node of ITEM_TYPE_OR_FAKE type farthest among its brother nodes.
**	#. Extract common node from all ITEM_TYPE_OR_FAKE type brother nodes (to do).
**	#. Calc the value of ITEM_TYPE_OR_FAKE type brother nodes, if the value equal 0x1, just ignore them!
**	#. If a trunk node does not contain a ITEM_TYPE_OR_FAKE type child node, move it topper.
**	#. Make sure the more basic config item node, the earlier be found in LRD of deptab tree.
**	#. If a trunk node appears as child node of another trunk node, put former node ahead of later node.
*/
static void adjust_deptab_tree(struct deptab_node* node)
{
	struct deptab_node* t = NULL;
	struct deptab_node *t1 = NULL, *t2 = NULL;

	/* merge ITEM_TYPE_OR_FAKE type nodes to make deptab tree more clearly. */
	for (t = node->next; t; t = t->next)
	{
		merge_ORnode_deptab_tree(t);
	}

	/* 
	**	Attention!
	**	Do not change the sequence of following sentences without through thinking!
	*/
	/* Put node of ITEM_TYPE_OR_FAKE type farthest among its brother nodes */
	for (t = node->next; t; t = t->next)
	{
		far_ORnode_deptab_tree(t);
	}
	/* extract common node from all ITEM_TYPE_OR_FAKE type brother nodes! */
	/* DO IT LATER! */


	/* 	
	**	calc the value of ITEM_TYPE_OR_FAKE type brother nodes, 
	**	if the value equal 0x1, just ignore them 
	*/
	for (t = node->next; t; t = t->next)
	{
		if ((t->item_flag & ITEM_TYPE_OR_IGN) != ITEM_TYPE_OR_IGN)
		{
			ign_ORnode_deptab_tree(t);
		}
	}
	
#if	1
	/*
	**	If a trunk node does not contain a ITEM_TYPE_OR_FAKE type child node, move it topper!
	*/
	far_trunk_ORnode(node);
#endif

#if	1
	/* if a trunk node appears as child node of another trunk node, put former node ahead of later node. */
	for (t1 = node, t = node->next; t;)
	{
		t2 = t->next;
		/* This trunk node has been appeared somewhere. */
		if (!t->cfg_name && up_trunk_node_deptab_tree(t))
		{
			t1->next = t2;
		}
		else
		{
			t1 = t;
		}
		t = t2;
	}
#endif

#if	1
	/* make sure the more basic config item node, the earlier be found in LRD of deptab tree. */
	for (t = node->next; t; t = t->next)
	{
		up_bsc_node_deptab_tree(t);
	}
#endif
}


/*
**	Parse each line of ".deptables" to get indexs of cfg_name and cfg_value. 
*/
static int parse_deptab_args(char* str, unsigned char* t1, unsigned char* t2)
{
	char *tmp = NULL;

	if (!str)
	{
		return DEPTAB_WARN_ARGS;
	}

	tmp = strstr(str, DEPTAB_CFG_ITEM_PREFIX);
	if (!tmp || strcmp(str, tmp))
	{
		*t1 = 0x0;
	}
	else
	{
		*t1 = (unsigned char)strlen(DEPTAB_CFG_ITEM_PREFIX);
	}

	tmp = strchr(str, '=');
	if (!tmp || strlen(tmp) < 0x2)
	{
		*t2 = *t1;
	}
	else
	{
		*t2 = (unsigned char)(strlen(str) - strlen(tmp));
	}
	
	if (*t1 > *t2)
	{
		return DEPTAB_WARN_ARGS;
	}
	
	return DEPTAB_RET_VAL_OK;
}

/*
**	Check whether the value of config item of this node is just proper currently.
*/
static bool is_node_val_correct(struct deptab_node* node)
{
	const char *curr_val;

	if (!node || !node->sym || !node->cfg_value)
	{
		return false;
	}

	curr_val = sym_get_string_value(node->sym);
	
	if (!strcmp(curr_val, node->cfg_value))
	{
		return true;
	}
	
	if (node->val_ymn == RANGE_VAL_YM)
	{
		if ((*curr_val == 'm' &&  *(node->cfg_value) == 'y')
		|| (*curr_val == 'y' &&  *(node->cfg_value) == 'm'))
		{
			return true;
		}
	}

	return false;
}


/*
**	Set the value range of some kernel config item, so user cannot write illegal value to it.
*/
static void node_set_range(struct deptab_node* node)
{
	if (!node || node->range)
	{
		return;
	}

	node->range = (unsigned int*)malloc(sizeof(unsigned int));
	if (!node->range)
	{
		deptab_message(LV_WARN, DEPTAB_WARN_MALLOC, "No enough memory to alloc unsigned int.");
		return;
	}
	
#if	0
	*(unsigned int*)(node->range) = 0xff;

	if (!strcmp(node->cfg_value, deptab_val_n))
	{
		*(unsigned int*)(node->range) = (unsigned)no;
	}
	else if (!strcmp(node->cfg_value, deptab_val_y) || !strcmp(node->cfg_value, deptab_val_m))
	{
		*(unsigned int*)(node->range) = (unsigned)(yes | mod);
	}
#else
	switch (node->val_ymn)
	{
	case RANGE_VAL_N:
		*(unsigned int*)(node->range) = (unsigned)no;
		return;

	case RANGE_VAL_M:
		*(unsigned int*)(node->range) = (unsigned)mod;
		return;

	case RANGE_VAL_Y:
		*(unsigned int*)(node->range) = (unsigned)yes;
		return;

	case RANGE_VAL_YM:
		*(unsigned int*)(node->range) = (unsigned)(yes | mod);
		return;
		
	default:
		*(unsigned int*)(node->range) = 0xff;
	}
#endif
}


/*
**	Apply each entry of integrated kernel config item dependency tree of deptab.
**	write the value specified by deptab to config item also specified by deptab.
*/
static int node_set_value(struct deptab_node* node)
{
	bool ret = false;
	int ret_val = DEPTAB_RET_VAL_OK;
	struct deptab_node* actual_node = NULL;

	if (!node->sym)
	{
		ret_val = DEPTAB_ERR_CFG_SYM_NULL;
		deptab_message(LV_ERROR, ret_val, 
			"Just no symbol for config item %s.", node->cfg_name ? node->cfg_name : "none");
		goto end;
	}

	actual_node = node->sym->node;
	if (!actual_node)
	{
		ret_val = DEPTAB_ERR_SYM_NODE_NULL;
		deptab_message(LV_ERROR, ret_val, 
			"The symbol %s without one node for deptab.", node->sym->name ? node->sym->name : "none");
		goto end;
	}
	if (!actual_node->sym)
	{
		ret_val = DEPTAB_ERR_CFG_SYM_NULL;
		deptab_message(LV_ERROR, ret_val, 
			"Just no symbol for config item %s.", actual_node->cfg_name ? actual_node->cfg_name : "none");
		goto end;
	}

	if (!actual_node->cfg_name)
	{
		if (!actual_node->sym->name)
		{
			ret_val = DEPTAB_RET_VAL_OK;
			goto end;
		}
		deptab_message(LV_WARN, DEPTAB_WARN_OTHERS, 
			"Strange node! name:%s, pa_name %s.", 
			actual_node->sym->name, 
			(actual_node->pa && actual_node->pa->sym && actual_node->pa->sym->name) ? actual_node->pa->sym->name : "ukn");
		actual_node->cfg_name = actual_node->sym->name;
	}

	if (!actual_node->cfg_value || !node->cfg_value)
	{
		ret_val = DEPTAB_ERR_CFG_VAL_NULL;
		deptab_message(LV_ERROR, ret_val, 
			"The value of config item %s is null, perhaps this config item in E_UNEQUAL expression case.", 
			actual_node->cfg_name ? actual_node->cfg_name : "none");
		goto end;
	}
	
	if (strcmp(actual_node->cfg_value, node->cfg_value))
	{
		if (actual_node->sym->type == S_TRISTATE)
		{
			if ((*(actual_node->cfg_value) == 'm' &&  *(node->cfg_value) == 'y')
			|| (*(actual_node->cfg_value) == 'y' &&  *(node->cfg_value) == 'm'))
			{
				if (actual_node->val_ymn & node->val_ymn)
				{
					/* update actual_node->val_ymn now! */
					actual_node->val_ymn = actual_node->val_ymn & node->val_ymn;
					actual_node->cfg_value = 
						(actual_node->val_ymn == RANGE_VAL_M) ? deptab_val_m : deptab_val_y;
					goto set_val_step;
				}
			}
		}

		ret_val = DEPTAB_ERR_CFG_VAL_CLSN;
		deptab_message(LV_ERROR, ret_val, 
			"The value of config item %s are in collision.", actual_node->cfg_name ? actual_node->cfg_name : "none");
		goto end;
	}

set_val_step:

	if (actual_node->phase_flag == PH_SETED)
	{
		/* already set */
		ret_val = DEPTAB_RET_VAL_OK;
		goto end;
	}

	ret = sym_string_within_range(actual_node->sym, actual_node->cfg_value);
	if (ret != true)
	{
		if (actual_node->sym->visible == no)
		{
			if (/*(actual_node->sym->type == S_TRISTATE || actual_node->sym->type == S_BOOLEAN)
			&& */!strcmp(actual_node->cfg_value, deptab_val_n))
			{
				ret_val = DEPTAB_RET_VAL_OK;
				goto end;
			}
			/* for invisible config item, just return OK! */
			if (actual_node->item_flag & ITEM_FLAG_INVISIBLE)
			{
				goto wr_val_step;
			}
			
			deptab_message(LV_ERROR, DEPTAB_ERR_CFG_VISIBLE_NONE, 
				"It is not visible for config item %s.", actual_node->cfg_name ? actual_node->cfg_name : "none");
			ret_val = DEPTAB_ERR_CFG_VISIBLE_NONE;
			goto end;
		}

		if (!strcmp(actual_node->cfg_value, deptab_val_m) || !strcmp(actual_node->cfg_value, deptab_val_y))
		{
			if (actual_node->val_ymn == RANGE_VAL_YM &&
				node->val_ymn == RANGE_VAL_YM)
			{
				/* set its value to the other one and validate it again */
				if (!strcmp(actual_node->cfg_value, deptab_val_m))
				{
					actual_node->cfg_value = deptab_val_y;
				}
				else if (!strcmp(actual_node->cfg_value, deptab_val_y))
				{
					actual_node->cfg_value = deptab_val_m;
				}
				ret = sym_string_within_range(actual_node->sym, actual_node->cfg_value);
				if (ret == true)
				{
					goto wr_val_step;
				}
				deptab_message(LV_ERROR, DEPTAB_ERR_CFG_VAL_DISMATCH, 
					"both value 'm' and 'y' are invalid for config item %s.", 
					actual_node->cfg_name ? actual_node->cfg_name : "none");
			}
		}

		ret_val = DEPTAB_ERR_CFG_VAL_DISMATCH;
		deptab_message(LV_ERROR, ret_val, 
			"rev_dep.tri of %s is %d, sym->visible = %d", 
		actual_node->cfg_name, actual_node->sym->rev_dep.tri, actual_node->sym->visible);
		
		deptab_message(LV_ERROR, ret_val, 
			"Its value is invalid for config item %s.", actual_node->cfg_name ? actual_node->cfg_name : "none");
		goto end;
	}

wr_val_step:
	ret = sym_set_string_value(actual_node->sym, actual_node->cfg_value);
	if (ret != true)
	{
		deptab_message(LV_ERROR, DEPTAB_ERR_CFG_VAL_SET, 
			"It failed to set value of config item %s.", actual_node->cfg_name ? actual_node->cfg_name : "none");
		ret_val = DEPTAB_ERR_CFG_VAL_SET;
		goto end;
	}
	else
	{
		node_set_range(actual_node);
		ret_val = DEPTAB_RET_VAL_OK;
		deptab_message(LV_INFO, DEPTAB_COMMON_CODE, "Set value of kernel config item %s to %s successfully.",
						actual_node->sym->name, actual_node->cfg_value);
	}

end:
	if (node)
	{
		node->phase_flag = PH_SETED;
	}
	if (actual_node)
	{
		actual_node->phase_flag = PH_SETED;
	}
	
	return ret_val;
}


/*
**	Find and record the most top node for config item of ITEM_TYPE_OR_FAKE type.
*/
static bool record_top_node_orfake(struct deptab_node* node)
{
#define	DEPTAB_TOP_NODE_NUM_MAX		0x100

	static struct deptab_node *deptab_top_node[DEPTAB_TOP_NODE_NUM_MAX];
	static unsigned int deptab_top_node_count = 0x0;

	if (!node->pa || deptab_top_node_count >= DEPTAB_TOP_NODE_NUM_MAX)
	{
		/* Just return . */
		return false;
	}

	deptab_top_node[deptab_top_node_count] = node->pa;
	while (deptab_top_node[deptab_top_node_count]->pa)
	{
		deptab_top_node[deptab_top_node_count] = deptab_top_node[deptab_top_node_count]->pa;
	}

	if (node->pa->sym)
	{
		sym_calc_value(node->pa->sym);
	}
	if (is_node_val_correct(node->pa))
	{
		node->pa->phase_flag = PH_SETED;
		node_set_range(node->pa);
		deptab_top_node[deptab_top_node_count] = NULL;
		return false;
	}
#if	1
	else if (node_set_value(node->pa) == DEPTAB_RET_VAL_OK)
	{
		deptab_top_node[deptab_top_node_count] = NULL;
		return false;
	}
#endif

	deptab_message(LV_ERROR, DEPTAB_ERR_ITEM_TYPE_OR_FAKE, 
			"Config item %s couldn't be set value properly because it contains OR expression and is just confused to decide which one to depend on!", 
			(deptab_top_node[deptab_top_node_count]->cfg_name) ? deptab_top_node[deptab_top_node_count]->cfg_name : "unknown");
	deptab_top_node_count++;

	return true;
}


/*
**	Apply each sub-tree of integrated kernel config item dependency tree of deptab.
*/
static int tree_set_value(struct deptab_node* n)
{
	struct deptab_node* t = NULL;
	//struct property* prop = NULL;

	if (n->phase_flag == PH_SETED)
	{
		return DEPTAB_RET_VAL_OK;
	}

	/*
	**	Firstly, it's necerrary to check whether the value of  this node is just proper currently.
	*/
	if (is_node_val_correct(n))
	{
		n->phase_flag = PH_SETED;
		node_set_range(n);
		return DEPTAB_RET_VAL_OK;
	}

	if (n->item_flag & ITEM_TYPE_OR_FAKE)
	{
		if (!(n->item_flag & ITEM_FLAG_OR_IGN))
		{
			/* some important things to do! */
			if (record_top_node_orfake(n))
			{
				return DEPTAB_ERR_ITEM_TYPE_OR_FAKE;
			}
		}
		return DEPTAB_RET_VAL_OK;
	}

	if (n->sym && n->sym->type == S_TRISTATE)
	{
		if (!strcmp(n->cfg_value, deptab_val_n))
		{
			goto next;
		}
	}

	for (t = n->list; t; t = t->next)
	{
		tree_set_value(t);
	}

next:

	#if	0
	if (n->cfg_name && !strcmp(n->cfg_name, "MACH_JAZZ"))
	{
		if (n->sym)
		{
			for (prop = n->sym->prop; prop; prop = prop->next)
			{
				deptab_message(LV_WARN, DEPTAB_COMMON_CODE, 
					"prop type for config item MACH_JAZZ is %d, @%s:%d.", 
					prop->type, 
					prop->file ? (prop->file->name ? prop->file->name : "unknown") : "none",
					prop->lineno);
			}
		}
	}
	#endif
	
	if (n->sym)
	{
		sym_calc_value(n->sym);
	}
	
	return node_set_value(n);
}


/*
**	Apply integrated kernel config item dependency tree of deptab.
*/
static int deptab_set_value(struct deptab_node* node)
{
	int res, ret = DEPTAB_RET_VAL_OK;
	
	for (node = node->next; node; node = node->next)
	{
		res = tree_set_value(node);
		if (res > DEPTAB_RET_VAL_OK)
		{
			ret = DEPTAB_ERR_MAX;
		}
	}

	return ret;
}


/*
**	According to type of expression to expand config item dependency tree of deptab.
*/
static int expand_deptab_node(struct expr *e, struct deptab_node* n, bool op)
{
	char *str2 = NULL;
	struct deptab_node *t1 = NULL;
	IPT_DEP_MODE dep_mode = gset_dep_mode(OP_GET, 0);

	if (!e)
		return DEPTAB_RET_VAL_OK;

	
	switch (e->type)
	{
	case E_SYMBOL:
		if (e->left.sym && (e->left.sym)->name)
		{
			n->sym = e->left.sym;
			if (op == true)
			{
				if (n->sym->type == S_TRISTATE)
				{
					n->cfg_value = deptab_val_m;
					n->val_ymn = RANGE_VAL_YM;
				}
				else
				{
					n->cfg_value = deptab_val_y;
					n->val_ymn = RANGE_VAL_Y;
				}
			}
			else
			{
				n->cfg_value = deptab_val_n;
				n->val_ymn = RANGE_VAL_N;
			}
			if (!(e->left.sym)->node)
			{
				n->cfg_name = (e->left.sym)->name;
				(e->left.sym)->node = n;
			}
			else
			{
				deptab_message(LV_INFO, DEPTAB_COMMON_CODE, "Kernel config item %s appears in deptab again!",
					(e->left.sym)->name);
			}
		}
		else
		{
			if (e->left.sym)
			{
				n->sym = e->left.sym;
				(e->left.sym)->node = n;
				n->cfg_name = deptab_choice;
				n->cfg_value= deptab_val_y;
				n->item_flag |= ITEM_TYPE_CHOICE;
				/* due to choice, can not be set value, force to write its deps_times to PH_SETED(0x2).  */
				n->phase_flag = PH_SETED;
			}
			else
			{
				deptab_message(LV_WARN, DEPTAB_WARN_EXP_SYM_ENTITY_NONE,
					"The config item(type: E_SYMBOL) whose pa is %s, with sym none.", 
					n->pa ? (n->pa->cfg_name ? n->pa->cfg_name : "none") : "pa_none");
			}
		}
		break;
		
	case E_AND:
	case E_OR:
		t1 = malloc(sizeof(struct deptab_node));
		if (!t1)
		{
			deptab_message(LV_ERROR, DEPTAB_ERR_MALLOC,
				"No enough memory to alloc struct deptab_node in expand_deptab_node");
			return DEPTAB_ERR_MALLOC;
		}
		
		memset(t1, 0x0, sizeof(struct deptab_node));
		t1->next = n->next;
		t1->pa = n->pa;
		n->next = t1;
		
		if (dep_mode == MODE_TREE_ONLY)
		{
			t1->be_select = n->be_select;
		}

		if ((op && e->type == E_AND) || (!op && e->type == E_OR))
		{
			expand_deptab_node(e->left.expr, n, op);
			expand_deptab_node(e->right.expr, t1, op);
		}
		else
		{
			n->cfg_name =  t1->cfg_name = deptab_OR_item;
			n->cfg_value =  t1->cfg_value = op ? deptab_val_y : deptab_val_n;
			n->item_flag |= ITEM_TYPE_OR_FAKE;
			t1->item_flag |=  ITEM_TYPE_OR_FAKE;

			/* mark their parent with flag ITEM_FLAG_OR_PA  */
			if (n->pa)
			{
				n->pa->item_flag |= ITEM_FLAG_OR_PA;
			}
			
			n->list = (struct deptab_node*)malloc(sizeof(struct deptab_node));
			if (!n->list)
			{
				deptab_message(LV_ERROR, DEPTAB_ERR_MALLOC, 
					"No enough memory to alloc struct deptab_node in expand_deptab_node.");
				return DEPTAB_ERR_MALLOC;
			}
			memset(n->list, 0x0, sizeof(struct deptab_node));
			n->list->pa = n;
			n = n->list;

			t1->list = (struct deptab_node*)malloc(sizeof(struct deptab_node));
			if (!t1->list)
			{
				deptab_message(LV_ERROR, DEPTAB_ERR_MALLOC, 
					"No enough memory to alloc struct deptab_node in expand_deptab_node.");
				return DEPTAB_ERR_MALLOC;
			}
			memset(t1->list, 0x0, sizeof(struct deptab_node));
			t1->list->pa = t1;
			t1 = t1->list;

			expand_deptab_node(e->left.expr, n, op);
			expand_deptab_node(e->right.expr, t1, op);
		}
		break;
	
	case E_NOT:
		expand_deptab_node(e->left.expr, n, !op);
		break;

	case E_EQUAL:
	case E_UNEQUAL:
		if (e->left.sym && (e->left.sym)->name)
		{
			sym_calc_value(e->right.sym);
			str2 = (char *)sym_get_string_value(e->right.sym);

			n->sym = e->left.sym;
			if (!strcmp(str2, deptab_val_n))
			{
				if ((e->type == E_UNEQUAL && op)
				 || (e->type == E_EQUAL && !op))
				{
					if (n->sym->type == S_TRISTATE)
					{
						n->cfg_value = deptab_val_m;
						n->val_ymn = RANGE_VAL_YM;
					}
					else
					{
						n->cfg_value = deptab_val_y;
						n->val_ymn = RANGE_VAL_Y;
					}
				}
				else
				{
					n->cfg_value = deptab_val_n;
					n->val_ymn = RANGE_VAL_N;
				}
			}
			else if (!strcmp(str2, deptab_val_y) || !strcmp(str2, deptab_val_m))
			{
				if ((e->type == E_UNEQUAL && op)
				 || (e->type == E_EQUAL && !op))
				{
					n->cfg_value = deptab_val_n;
					/* Temporarily treat its val range as "n" only!!! */
					n->val_ymn = RANGE_VAL_N;
				}
				else
				{
					n->cfg_value = str2;
					if (!strcmp(str2, deptab_val_y))
					{
						n->val_ymn = RANGE_VAL_Y;
					}
					else if (!strcmp(str2, deptab_val_m))
					{
						n->val_ymn = RANGE_VAL_M;
					}
				}
			}
			else
			{
				if ((e->type == E_UNEQUAL && !op) 
				 ||(e->type == E_EQUAL && op))
				{
					n->cfg_value = str2;
				}
				else
				{
					deptab_message(LV_WARN, DEPTAB_WARN_EXP_SYM_UNEQUAL,
						"Kernel config item %s != %s, but its value is set to null!", 
						(e->left.sym)->name, str2 ? str2: "");
					n->cfg_value = NULL;
				}
			}
			if (!(e->left.sym)->node)
			{
				n->cfg_name = (e->left.sym)->name;
				(e->left.sym)->node = n;
			}
			else
			{
				deptab_message(LV_INFO, DEPTAB_COMMON_CODE, "Kernel config item %s appears in deptab again!",
					(e->left.sym)->name);
			}
		}
		else
		{
			if (e->left.sym)
			{
				n->sym = e->left.sym;
				(e->left.sym)->node = n;
				n->cfg_name = deptab_choice;
				n->cfg_value= deptab_val_y;
				n->item_flag |= ITEM_TYPE_CHOICE;
				/* due to choice, can not be set value, force to write its deps_times to 0x1.  */
				n->phase_flag = PH_SETED;
			}
			else
			{
				deptab_message(LV_WARN, DEPTAB_WARN_EXP_SYM_ENTITY_NONE,
					"The config item(type: E_(UN)EQUAL) whose pa is %s, with sym none.", 
					n->pa ? (n->pa->cfg_name ? n->pa->cfg_name : "none") : "pa_none");
			}
		}
		break;

	/* 
	**	We have searched the whole Kconfig tree, there are only E_SYMBOL, E_AND, E_OR, E_NOT, E_EQUAL, E_UNEQUAL, 
	**	together six types of expression for all symbols.
	**	So we treate other types as warning case.
	*/
	default:
		deptab_message(LV_WARN, DEPTAB_WARN_SYM_TYPE_UNKNOWN, 
				"Cofing item %s with unusual  type %d, how to deal with it?",
				e->left.sym ? ((e->left.sym)->name ? (e->left.sym)->name : "unknown") : "none", 
				e->type);
	}

	return DEPTAB_RET_VAL_OK;
}


/*
**	Get proper dependence-expression for some config item
*/
static struct expr* node_get_exp(struct symbol *sym)
{
	struct property *st = NULL;
	struct expr* exp = NULL;
	IPT_DEP_MODE dep_mode = gset_dep_mode(OP_GET, 0);

	if (!sym)
	{
		return exp;
	}

	for (st = sym->prop; st; st = st->next)
	{
		if (st->text)
		{
			break;
		}
	}

	if (st)
	{
	#if	0
		if (sym->dir_dep.expr != st->visible.expr)
		{
			deptab_message(LV_INFO, DEPTAB_COMMON_CODE,
				"For config item %s, its dir_dep.expr is different from visible.expr, 0x%08x vs 0x%08x.",
				sym->name ? sym->name : "unknown", sym->dir_dep.expr, st->visible.expr);
		}
		/*
		**	Use "st->visible.expr" rather than "sym->dir_dep.expr" because visible.expr is more downright expression.
		**	sometimes dir_dep.expr may be half-baked.
		*/
	#endif	/* #if	0 */	
		exp = st->visible.expr;
	}
	else
	{
		if (dep_mode != MODE_DEP_TAB)
		{
			return NULL;
		}

		/* Mark it! */
		if (sym->node)
		{
			sym->node->item_flag |= ITEM_FLAG_INVISIBLE;
		}
		exp = sym->rev_dep.expr;
		deptab_message(LV_INFO, DEPTAB_COMMON_CODE,
			"This config item %s without prompt text.",
			sym->name ? sym->name : "unknown");
	}

	return exp;
}


/*
**	Begin to create config item dependency tree of deptab.
*/
static int create_deptab_tree(struct symbol *sym, struct deptab_node* n)
{
	struct expr* dep_exp = node_get_exp(sym);

	bool first_exp_mark = false;
	struct property *st = NULL;
	struct deptab_node *t1 = NULL, *t2 = NULL;
	int ret = DEPTAB_RET_VAL_OK;

	IPT_DEP_MODE dep_mode = gset_dep_mode(OP_GET, 0);
		
	if (!sym)
	{
		deptab_message(LV_WARN, DEPTAB_WARN_SYM_NONE, 
			"Can not find corresponding symbol for config item %s.", 
			n->cfg_name ? n->cfg_name : (n->sym ? n->sym->name : "none"));
		return DEPTAB_RET_VAL_OK;
	}

	if (dep_exp)
	//if (sym->dir_dep.expr)/*rev_dep.expr*/
	{
		if (!(n->list))
		{
			n->list = (struct deptab_node*)malloc(sizeof(struct deptab_node));
			if (!n->list)
			{
				deptab_message(LV_ERROR, DEPTAB_ERR_MALLOC, 
					"No enough memory to alloc struct deptab_node in create_deptab_tree");
				return DEPTAB_ERR_MALLOC;
			}
		}
		else
		{
			deptab_message(LV_WARN, DEPTAB_WARN_DEPTAB_LIST_EXIST, 
				"why config item %s has its list(child) before accessing its dir_dep.expr?", 
				n->cfg_name ? n->cfg_name : (n->sym ? n->sym->name : "none"));
		}
		memset(n->list, 0x0, sizeof(struct deptab_node));
		n->list->pa = n;
		n = n->list;

		//expand_deptab_node(sym->dir_dep.expr, n, true);/* rev_dep.expr */
		ret = expand_deptab_node(dep_exp, n, true);
		if (ret > 0)
		{
			return ret;
		}
		
		if (dep_mode == MODE_TREE_ONLY)
		{
			first_exp_mark = true;
		}
	}
	else
	{
		deptab_message(LV_INFO, DEPTAB_COMMON_CODE, "Kernel config item %s do not depend on any other config item.",
					sym->name ? sym->name: "none");
	}

	if (dep_mode == MODE_DEP_TAB)
	{
		return DEPTAB_RET_VAL_OK;
	}

	for (st = sym->prop; st; st = st->next)
	{
		if (st->type == P_SELECT && st->expr)
		{
			if (first_exp_mark == false)
			{
				if (!(n->list))
				{
					n->list = (struct deptab_node*)malloc(sizeof(struct deptab_node));
					if (!n->list)
					{
						deptab_message(LV_ERROR, DEPTAB_ERR_MALLOC, 
							"No enough memory to alloc struct deptab_node in create_deptab_tree.");
						return DEPTAB_ERR_MALLOC;
					}
				}
				else
				{
					deptab_message(LV_WARN, DEPTAB_WARN_DEPTAB_LIST_EXIST, 
						"why config item %s has its list(child) before accessing its expr of select prop?", 
						n->cfg_name ? n->cfg_name : (n->sym ? n->sym->name : "none"));
				}
				memset(n->list, 0x0, sizeof(struct deptab_node));
				n->list->pa = n;
				n = n->list;
				n->be_select = true;
				first_exp_mark = true;
				expand_deptab_node(st->expr, n, true);
			}
			else
			{
				t1 = malloc(sizeof(struct deptab_node));
				if (!t1)
				{
					deptab_message(LV_ERROR, DEPTAB_ERR_MALLOC,
						"No enough memory to alloc struct deptab_node in create_deptab_tree.");
					return DEPTAB_ERR_MALLOC;
				}
				for (t2 = n; t2->next; t2 = t2->next);
			
				t2->next = t1;
				memset(t1, 0x0, sizeof(struct deptab_node));
				t1->pa = n->pa;
				t1->be_select = true;
				expand_deptab_node(st->expr, t1, true);
			}
		}
	}

	return DEPTAB_RET_VAL_OK;
}


/*
**	Build the whole integrated kernel config item dependency tree based ".deptables"
*/
static int build_deptab_tree(struct deptab_node* node)
{
	int ret = 0;
	struct symbol *sym = NULL;

	for (node = node->next; node;)
	{
		/* Just skip! */
		if (!node->cfg_name || node->item_flag > ITEM_FLAG_MAX)
		{
			goto next;
		}
		
		sym = sym_find(node->cfg_name);
		if (!sym || !sym->name || strcmp(sym->name, node->cfg_name))
		{
			deptab_message(LV_WARN, DEPTAB_WARN_SYM_NONE, 
				"Can not find corresponding symbol for config item %s.", node->cfg_name);
			goto next;
		}
		
		/* This is a trunk node! */
		if (!node->pa)
		{
			free(node->cfg_name);
			node->cfg_name = sym->name;
			if (sym->type == S_BOOLEAN || sym->type == S_TRISTATE)
			{
				if (!strcmp(node->cfg_value, deptab_val_n))
				{
					node->val_ymn = RANGE_VAL_N;
				}
				else if (!strcmp(node->cfg_value, deptab_val_m))
				{
					node->val_ymn = (sym->type == S_TRISTATE) ? RANGE_VAL_YM : RANGE_VAL_M;
				}
				else if (!strcmp(node->cfg_value, deptab_val_y))
				{
					node->val_ymn = (sym->type == S_TRISTATE) ? RANGE_VAL_YM : RANGE_VAL_Y;
				}
			}
		}

		if (!node->sym)
		{
			node->sym = sym;
		}
		else if (node->sym != sym)
		{
			deptab_message(LV_WARN, DEPTAB_WARN_SYM_ANOTHER, 
				"Multi-symbol found for config item %s, one in 0x%08x, another one in 0x%08x.", 
				sym->name, node->sym, sym);
		}
		if (!sym->node)
		{
			sym->node = node;
		}
		else if (sym->node != node)
		{
			/* This is a trunk node! */
			if (!node->pa)
			{
				node->cfg_name = NULL;
				sym->node->val_ymn &= node->val_ymn;
				goto next;
			}
			deptab_message(LV_INFO, DEPTAB_COMMON_CODE, 
				"Two different config items (%s && %s) attach with the same symbol(%s)",
				node->cfg_name ? node->cfg_name : "none", 
				sym->node->cfg_name ? sym->node->cfg_name : "none" , 
				sym->name ? sym->name : "unknown");
		}
		
		ret = create_deptab_tree(sym, node);
		if (ret > 0)
		{
			return ret;
		}

	next:
		if (node->list) 
		{
			node = node->list;
			continue;
		}
		if (node->next)
		{
			node = node->next;
		}
		else while ((node = node->pa)) 
		{
			if (node->next)
			{
				node = node->next;
				break;
			}
		}
	}

	return 0;
}


/*
**	Fill basic node based file of file_name.
*/
static int fill_basic_node(const char* file_name, struct deptab_node* node)
{
	FILE *deptab_fp = NULL;
	char *line = NULL;
	char content[DEP_TAB_LINE_LEN_MAX];
	unsigned char s1 = 0, s2 = 0;
	size_t len = 0;
	int ret = DEPTAB_RET_VAL_OK;
	IPT_DEP_MODE dep_mode = gset_dep_mode(OP_GET, 0);
	
	deptab_fp = fopen(file_name, "r");
	if (!deptab_fp)
	{
		if (dep_mode == MODE_DEP_TAB)
		{
			ret = DEPTAB_ERR_FILE_LOST;
			deptab_message(LV_ERROR, ret, "No deptable file: %s.", file_name);
		}
		else
		{
			/* Just return OK! */
			ret = DEPTAB_RET_VAL_OK;
		}
		return ret;
	}

	while (getline(&line, &len, deptab_fp) != -1)
	{
		memset(content, 0x0, sizeof(content));
		sscanf(line, "%[^# \t\r\n]", content);
		if (!(*content))
		{
			continue;
		}
		ret = parse_deptab_args(content, &s1, &s2);
		if (ret > 0)
		{
			deptab_message(LV_WARN, ret, "Unrecognised format: %s.", content);
			continue;
		}
		
		while (node->next)
		{
			node = node->next;
		}
		node->next = malloc(sizeof(struct deptab_node));
		node = node->next;
		if (!node)
		{
			ret = DEPTAB_ERR_MALLOC;
			deptab_message(LV_ERROR, ret, "No enough memory to alloc struct deptab_node");
			goto end;
		}

		memset(node, 0x0, sizeof(struct deptab_node));
		if (s2 > s1)
		{
			*(content + s2) = '\0';
		}
		node->cfg_name = strdup(content + s1);
		if (s2 > s1)
		{
			node->cfg_value = strdup(content + s2 + 1);
		}
		else if (s2 == s1)
		{
			node->cfg_value = deptab_val_y;
		}
		if (!node->cfg_name || !node->cfg_value)
		{
			ret = DEPTAB_ERR_MALLOC;
			deptab_message(LV_ERROR, ret, "No enough memory to alloc strings");
			goto end;
		}
	}
end:
	fclose(deptab_fp);
	return ret;
}


/*
**	Check the VALUE of config item for trunk node in deptab has been set properly? 
*/
static int check_trunk_node(struct deptab_node* node)
{
	int res = DEPTAB_RET_VAL_OK;
	struct deptab_node* trunk = NULL;
	FILE *fp = get_dbg_fp(MODE_DEP_TAB);

	if (!node)
	{
		return res;
	}

	fprintf(fp, "\n");
	fprintf(fp, "###############################################################\n");
	fprintf(fp, "#  Summary --- In checking phase to find missing config item.\n");
	fprintf(fp, "###############################################################\n");
	
	for (trunk = node->next; trunk; trunk = trunk->next)
	{
		if (!trunk->sym)
		{
			if (trunk->item_flag <= ITEM_FLAG_MAX)
			{
				/* Just no symbol for some config item(s) */
				deptab_summary(LV_SUM_HIGH, "Just no symbol for config item %s.", 
					trunk->cfg_name ? trunk->cfg_name : "unknown");
				res |= (1 << (DEPTAB_ERR_CFG_SYM_NULL % 0x1e));
			}
			continue;
		}
		if (trunk->cfg_value && strcmp(trunk->cfg_value, sym_get_string_value(trunk->sym)))
		{
			if (trunk->sym->type == S_TRISTATE && trunk->val_ymn == RANGE_VAL_YM)
			{
				if (strcmp(trunk->cfg_value, deptab_val_n) &&
					strcmp(sym_get_string_value(trunk->sym), deptab_val_n))
				{
					deptab_summary(LV_SUM_MID, 
						"Config item %s, current value <%s> VS target value <%s>.",
						(trunk->sym && trunk->sym->name) ? trunk->sym->name : "unknown", 
						sym_get_string_value(trunk->sym), trunk->cfg_value);
					continue;
				}
			}
			deptab_summary(LV_SUM_HIGH, "For config item %s, current value <%s> is different from target value <%s>.",
				(trunk->sym && trunk->sym->name) ? trunk->sym->name : "unknown", 
				sym_get_string_value(trunk->sym), trunk->cfg_value);
			
			res |= (1 << (DEPTAB_ERR_CFG_VAL_DISMATCH % 0x1e));
		}
	}

	return res;
}


/*
**	Background funtion to parse ".deptables" and apply it to rewrite default value of 
**	config item into .config file.
*/
static int _deptab_handler(IPT_DEP_MODE mode)
{
	int ret = DEPTAB_RET_VAL_OK;
	struct deptab_node* root_node = get_root_deptab_node();
	
	char* in_file = (mode == MODE_DEP_TAB) ? DEPTAB_FILE_NAME : TREE_ONLY_FILE_NAME;
	char* ass_file = DEPTAB_ASS_FILE;

	/* 
	**	parsing ass_file before in_file is recommended to make sure basic config items are in the front of deptree. 
	*/
	if (!access(ass_file, R_OK))
	{
		fill_basic_node(ass_file, root_node);
	}

	ret = fill_basic_node(in_file, root_node);
	if (ret > 0)
	{
		return ret;
	}

	ret = build_deptab_tree(root_node);
	if (ret > 0)
	{
		return ret;
	}

	adjust_deptab_tree(root_node);

	if (mode == MODE_DEP_TAB)
	{
		ret = deptab_set_value(root_node);
	}
	else if (mode == MODE_TREE_ONLY)
	{
		print_deptab_tree(root_node->next, true);
		print_ukn_dep_tree(root_node->next);
	}

	return ret;
}



/*
**	EXTERNALIZED FUNCTIONS
*/
/*
**	Check config item and its value in deptab has been written in ".config" file successfully? 
*/
void deptab_check_node(struct symbol *sym, const char* val, int type)
{
	struct deptab_node *node = sym->node;

	gset_dep_mode(OP_SET, MODE_DEP_TAB);

	if (node->sym != sym)
	{
		deptab_message(LV_WARN, DEPTAB_WARN_CHK_SYM_NONE,
			"When checking whether value of config item(%s) has been set, find the attached node points to another config item(%s)!", 
			sym->name, node->sym ? (node->sym->name ? node->sym->name : "unknown") : "none");
		node->phase_flag = PH_CHK_ERR;
		return;
	}

	if (!node->cfg_name || strcmp(sym->name, node->cfg_name))
	{
		deptab_message(LV_WARN, DEPTAB_WARN_CHK_NAME_DIFF,
			"When checking whether value of config item(%s) has been set, find the attached node has different config name(%s).",
			sym->name, node->cfg_name ? node->cfg_name : "none");
		node->phase_flag = PH_CHK_ERR;
		return;
	}

	if (!node->cfg_value)
	{
		deptab_message(LV_WARN, DEPTAB_WARN_CHK_VAL_NULL,
			"When checking whether value of config item(%s) has been set, find the attached node without value.",
			sym->name);
		node->phase_flag = PH_CHK_ERR;
		return;
	}

	if (!strcmp(val, node->cfg_value))
	{
		node->phase_flag = PH_CHK_OK;
	}
	else if (type == S_STRING)
	{
		deptab_message(LV_WARN, DEPTAB_WARN_CHK_STR_VAL_DIFF,
			"When checking whether value of config item(%s) has been set, find the attached node has different string val"\
			"{actual val(%s) vs value in deptab(%s)}.",
			sym->name, val, node->cfg_value);
		node->phase_flag = PH_CHK_UNSURE;
	}
	else
	{
		if (sym->type == S_TRISTATE)
		{
			if (((*(val) == 'm' &&  *(node->cfg_value) == 'y') || (*(val) == 'y' &&  *(node->cfg_value) == 'm'))
				&& node->val_ymn == RANGE_VAL_YM)
			{
				node->phase_flag = PH_CHK_UNSURE;
				deptab_message(LV_NOTICE, DEPTAB_COMMON_CODE, 
					"checking whether value of config item(%s) has been set, value in deptab(%s) has been modified to %s.",
					sym->name, node->cfg_value, val);
				return;
			}
		}
		deptab_message(LV_WARN, DEPTAB_WARN_CHK_VAL_DIFF,
			"When checking whether value of config item(%s) has been set, find the attached node has different values"\
			"{actual val(%s) vs value in deptab(%s)}.",
			sym->name, val, node->cfg_value);
		node->phase_flag = PH_CHK_ERR;
	}
}


/*
**	Find which config items and its values in deptab have not been written in ".config" file.
*/
int deptab_checker(int without_save)
{
	int res = DEPTAB_RET_VAL_OK;
	struct deptab_node *node, *root_node = get_root_deptab_node();
	FILE *fp = get_dbg_fp(MODE_DEP_TAB);

	gset_dep_mode(OP_SET, MODE_DEP_TAB);

	/* configuration changes were NOT saved */
	if (without_save)
	{
		res = check_trunk_node(root_node);
		goto print_tree;
	}
	
	node = root_node->next;
	if (node)
	{
		fprintf(fp, "\n");
		fprintf(fp, "#######################################################################\n");
		fprintf(fp, "#  Summary --- Find missing config item after .config has been written.\n");
		fprintf(fp, "#######################################################################\n");
	}
	while (node)
	{
		if (!node->sym)
		{
			/* For trunk node! */
			if (node->item_flag <= ITEM_FLAG_MAX && !node->pa && node->phase_flag != PH_CHK_OK)
			{
				/* Just no symbol for some config item(s) */
				deptab_summary(LV_SUM_HIGH, "Just no symbol for config item %s.", 
					node->cfg_name ? node->cfg_name : "unknown");
				res |= (1 << (DEPTAB_ERR_CFG_SYM_NULL % 0x1e));
			}
			goto next;
		}
		
		if (node->cfg_name && node->phase_flag != PH_CHK_OK
			&& node->item_flag <= ITEM_FLAG_MAX && node->cfg_value
			&& strcmp(node->cfg_value, deptab_val_n))
		{		
			if (!node->pa)
			{
				if (node->phase_flag == PH_CHK_UNSURE)
				{
					deptab_summary(LV_SUM_MID, 
						"Config item %s, current value <%s> VS target value <%s>.",
						(node->sym && node->sym->name) ? node->sym->name : "unknown", 
						sym_get_string_value(node->sym), node->cfg_value);
				}
				else
				{
					deptab_summary(LV_SUM_HIGH, "Config item(%s) with its value(%s) can not be set to .config, status code %d. ",
						node->cfg_name, node->cfg_value ? node->cfg_value : "none", node->phase_flag);

					res |= (1 << (DEPTAB_ERR_CHK_NAME_VAL_FAIL % 0x1e));
				}
			}
			else
			{
				deptab_summary(LV_SUM_LOW, 
					"Config item(%s) with its value(%s) can not be set to .config, status code %d. ",
					node->cfg_name, node->cfg_value ? node->cfg_value : "none", node->phase_flag);
			}
		}

	next:
		if (node->list && !(node->item_flag & ITEM_TYPE_OR_FAKE)) 
		{
			node = node->list;
			continue;
		}
		if (node->next)
		{
			node = node->next;
		}
		else while ((node = node->pa)) 
		{
			if (node->next)
			{
				node = node->next;
				break;
			}
		}
	}

print_tree:
	print_deptab_tree(root_node->next, true);

	show_set_err_event(OP_SHOW, 0);
	return res;
}


/*
**	External funtion to parse ".deptables" and apply it to rewrite default value of 
**	config item into .config file.
*/
int deptab_handler(IPT_DEP_MODE mode)
{
	int res = DEPTAB_RET_VAL_OK;
	
	gset_dep_mode(OP_SET, mode);
	
	 res = _deptab_handler(mode);

	return res;
}

