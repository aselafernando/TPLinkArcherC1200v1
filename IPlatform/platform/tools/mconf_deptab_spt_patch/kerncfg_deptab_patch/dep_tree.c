/*
**	scripts/kconfig/dep_tree.c

--	*********************************************************************
||	source file to implement global/local variable, routines, which will used to find
||	dependency tree for some kernel config. e.g. dependency tree for config  
||	"IP_MULTICAST" is shown as followings:
||   `-- IP_MULTICAST (net/ipv4/Kconfig:10)
||        |-- NET_IPGRE_BROADCAST (net/ipv4/Kconfig:245)
||        |-- IP_MROUTE (net/ipv4/Kconfig:254)
||        |   |-- IP_MROUTE_MULTIPLE_TABLES (net/ipv4/Kconfig:267)
||        |   |-- IP_PIMSM_V1 (net/ipv4/Kconfig:281)
||        |   `-- IP_PIMSM_V2 (net/ipv4/Kconfig:294)
||        `-- QETH (drivers/s390/net/Kconfig:66)
||             |-- QETH_L2 (drivers/s390/net/Kconfig:80)
||             `-- QETH_L3 (drivers/s390/net/Kconfig:88)
--	*********************************************************************

@@	By TengFei, 13/07/01
*/


/*
**	DEFINITIONS AND DECLARATIONS
*/
#define	MCONF_CONFIG_RANGE_ALL		"range_allconfig"
#define	MCONF_CONFIG_RANGE_SEL		"range_selectedconfig"

#define	MCONF_DISP_TYPE_ONCE			"config_displayonce"
#define	MCONF_DISP_TYPE_ORIG			"config_displayorignal"

#define	MCONF_CFG_NAME_LEN_MAX		64
#define	MCONF_ROOT_KCFG_NAME		"INET"

#define	MCONF_OUTPUT_FILE_LEN_MAX	128
#define	MCONF_OUTPUT_FILE_DEFAULT	"/tmp/tree.log"

#define	MCONF_SCRIPT_FILE_LEN_MAX	136
#define	MCONF_SCRIPT_FILE_NAME		"cutcfgset.sh"


/*
**	define operation types for mconf_tree module.
*/
typedef	enum
{
	OP_GET = 0,
	OP_SET = 1,
	OP_GET_DIR = 2,

	OP_INV
}MCONF_OP;

/*
**	define tree_range types for mconf_tree module.
*/
typedef	enum
{
	RANGE_ALL = 0,			/* display config dependency tree through all configs */
	RANGE_SEL = 1,			/* display config dependency tree through seleted configs */

	RANGE_INV
}MCONF_TREE_RANGE;

/*
**	define tree_disp types for mconf_tree module.
*/
typedef	enum
{
	DISP_ONCE = 0,			/* for one specified config item, only display once in config dependency tree */
	DISP_ORIG = 1,			/* for one specified config item, keep orignal display-type in config dependency tree */

	DISP_INVA
}MCONF_TREE_DISP;



/*
**	define struct kcfg_node  to dispaly config item dependency tree
*/
struct kcfg_node
{
	struct kcfg_node *list;
	struct kcfg_node *next;
	struct kcfg_node *pa;

	struct menu* src_menu;
	struct symbol* src_sym;

	/*
	**	List of kcfg_nodes who are related with the same Kconfig file
	*/
	struct kcfg_node *same_file;
};


/*
**	define struct kcfg_file to dispaly groups each one made up of Kconfig file and its config items.
*/
struct kcfg_file
{
	struct file *fp;
	struct kcfg_node *node;

	struct kcfg_file *next;
};


/*
**	global and local variables
*/
extern struct menu rootmenu;
static struct kcfg_node	tree_rootnode;
static struct kcfg_file headfile;


/*
**	PRIVATE FUNTIONS
*/

/*
**	get/set config_range via arguments
*/
static MCONF_TREE_RANGE gset_config_range(MCONF_OP op, MCONF_TREE_RANGE range)
{
	static MCONF_TREE_RANGE config_range = RANGE_INV;

	switch (op)
	{
	case OP_GET:
		break;
	case OP_SET:
		if (range <= RANGE_INV && range >= RANGE_ALL)
		{
			config_range = range;
		}
		break;
	default:
		fprintf(stdout, "unsupported operation %d\n", op);
	}

	return config_range;
}


/*
**	get/set disp_type via arguments
*/
static MCONF_TREE_DISP gset_disp_type(MCONF_OP op, MCONF_TREE_DISP disp)
{
	static MCONF_TREE_DISP disp_type = DISP_INVA;

	switch (op)
	{
	case OP_GET:
		break;
	case OP_SET:
		if (disp <= DISP_INVA && disp >= DISP_ONCE)
		{
			disp_type = disp;
		}
		break;
	default:
		fprintf(stdout, "unsupported operation %d\n", op);
	}

	return disp_type;
}


/*
**	get/set root_config via arguments
*/
static char* gset_root_config(MCONF_OP op, char* name)
{
	static char root_config[MCONF_CFG_NAME_LEN_MAX] = MCONF_ROOT_KCFG_NAME;

	switch (op)
	{
	case OP_GET:
		break;

	case OP_SET:
		if (strlen(name) < MCONF_CFG_NAME_LEN_MAX)
		{
			memset(root_config, 0x0, sizeof(root_config));
			strcpy(root_config, name);
		}
		else
		{
			fprintf(stdout, "The root_config name specified is too long, using defalut %d as root_config.\n", MCONF_CFG_NAME_LEN_MAX);
		}
		break;

	default:
		fprintf(stdout, "unsupported operation %d\n", op);
	}

	return root_config;
}


/*
**	get/set output_file via arguments
*/
static char* gset_output_file(MCONF_OP op, char* flname)
{
	static char output_file[MCONF_OUTPUT_FILE_LEN_MAX] = MCONF_OUTPUT_FILE_DEFAULT;

	switch (op)
	{
	case OP_GET:
		return output_file;

	case OP_SET:
		if (strlen(flname) < MCONF_OUTPUT_FILE_LEN_MAX)
		{
			memset(output_file, 0x0, sizeof(output_file));
			strcpy(output_file, flname);
		}
		else
		{
			fprintf(stdout, "The output file name specified is too long, using defalut "MCONF_OUTPUT_FILE_DEFAULT" as output file.\n");
		}
		return NULL;

	default:
		fprintf(stdout, "unsupported operation %d\n", op);
	}

	return output_file;
}


/*
**	get/set script_file via arguments
*/
static char* gset_script_file(MCONF_OP op, char* dir)
{
	static char script_file[MCONF_SCRIPT_FILE_LEN_MAX] = "./"MCONF_SCRIPT_FILE_NAME;
	static char script_dir[MCONF_OUTPUT_FILE_LEN_MAX] = "/tmp";

	switch (op)
	{
	case OP_GET:
		return script_file;

	case OP_SET:
		if (strlen(dir)  + strlen(MCONF_SCRIPT_FILE_NAME) + 1 < MCONF_SCRIPT_FILE_LEN_MAX)
		{
			memset(script_file, 0x0, sizeof(script_file));
			sprintf(script_file, "%s/%s", dir, MCONF_SCRIPT_FILE_NAME);
			memset(script_dir, 0x0, sizeof(script_dir));
			strcpy(script_dir, dir);
		}
		else
		{
			fprintf(stdout, "The script file name specified is too long, using defalut "MCONF_SCRIPT_FILE_NAME" as script file.\n");
		}
		return NULL;

	case OP_GET_DIR:
		return script_dir;

	default:
		fprintf(stdout, "unsupported operation %d\n", op);
	}

	return script_file;
}


/*
**	get pointer of output_file
*/
static FILE* get_output_fp(void)
{
	static FILE *output_fp = NULL;
	char *outfile = NULL;

	if (!output_fp)
	{
		outfile = gset_output_file(OP_GET, NULL);
		if (!outfile)
		{
			fprintf(stdout, "Output file name is NULL!\n");
			output_fp = stdout;
		}
		output_fp = fopen(outfile, "w+");
		if (!output_fp)
		{
			fprintf(stdout, "open %s failed!\n", outfile);
			output_fp = stdout;
		}
	}

	return output_fp;
}


/*
**	get pointer of script_file
*/
static FILE* get_script_fp(void)
{
	static FILE *script_fp = NULL;
	char *scriptfile = NULL;
	char cmd_buf[MCONF_SCRIPT_FILE_LEN_MAX + 10];

	if (!script_fp)
	{
		scriptfile = gset_script_file(OP_GET, NULL);
		if (!scriptfile)
		{
			fprintf(stdout, "Script file name is NULL!\n");
			script_fp = stdout;
		}
		script_fp = fopen(scriptfile, "w+");
		if (!script_fp)
		{
			fprintf(stdout, "open %s failed!\n", scriptfile);
			script_fp = stdout;
		}
		else
		{
			sprintf(cmd_buf, "chmod +x %s %c", scriptfile, 0);
			system(cmd_buf);			/* add executable attribute to script_file */
		}
	}

	return script_fp;
}


/*
**	fill contents into tree_node
*/
static void fill_tree_node
(
	struct kcfg_node* node, 
	struct kcfg_node* pa_node, 
	struct menu *pmenu
)
{
	memset(node, 0x0, sizeof(struct kcfg_node));
	node->pa = pa_node;
	node->src_menu = pmenu;
	node->src_sym = (struct symbol*)(pmenu->sym);
	/* we use menu->times to mark the times this menu was hitted! */
	(node->src_menu->times)++;
}


/*
**	Just initialize first node of LOCAL VAR tree_rootnode
*/
static void init_tree_node(struct kcfg_node* node)
{
	struct menu	*menu;
	struct symbol *sym;
	FILE *fp = get_output_fp();
	char *root_kcfg = gset_root_config(OP_GET, NULL);
	
	memset(node, 0x0, sizeof(struct kcfg_node));
	node->list = NULL;
	node->next = NULL;
	node->pa = NULL;

	menu = rootmenu.list;
	while (menu) 
	{
		sym = menu->sym;
		if (sym && sym->name)
		{
			if (!strcmp(root_kcfg, sym->name))
			{
				fill_tree_node(node, NULL, menu);
				return;
			}
		}

		if (menu->list) 
		{
			menu = menu->list;
			continue;
		}
		if (menu->next)
		{
			menu = menu->next;
		}
		else while ((menu = menu->parent)) 
		{
			if (menu->next) 
			{
				menu = menu->next;
				break;
			}
		}
	}

	fprintf(fp, "Failed to find config %s\nBut we still try to create the tree.\n", root_kcfg);
	node->src_menu = malloc(sizeof(struct menu));
	node->src_sym = malloc(sizeof(struct symbol));
	if (!node->src_menu || !node->src_sym)
	{
		fprintf(fp, "tree_rootnode malloc .src_menu .src_sym failed!\n");
		return;
	}
	memset(node->src_menu, 0x0, sizeof(struct menu));
	memset(node->src_sym, 0x0, sizeof(struct symbol));
	node->src_menu->sym = node->src_sym;
	node->src_menu->times = 1;
	node->src_sym->name = gset_root_config(OP_GET, NULL);
	node->src_sym->curr.tri = no;
}


/*
**	Is specified file in kcfgfile list?
*/
static struct kcfg_file* in_kcfgfile_list(struct file* filp, struct kcfg_file* head)
{
	struct kcfg_file *curr = NULL;
	for (curr = head->next; curr; curr = curr->next)
	{
		if (curr->fp && curr->fp->name && filp->name
			&& !strcmp(curr->fp->name, filp->name))
		{
			break;
		}
	}

	return curr;
}


/*
**	insert a kcfg_node into kcfg_file list.
*/
static void insert_kcfg_file(struct kcfg_node* node, struct kcfg_file* file)
{
	struct kcfg_node *curr, *prev;
		
	if (!node || !file)
	{
		return;
	}

	curr = file->node;
	if (curr == NULL || node->src_menu->lineno < curr->src_menu->lineno)
	{
		node->same_file = curr;
		file->node = node;
		return;
	}

	prev = curr;
	curr = curr->same_file;
	while (curr)
	{
		if (node->src_menu->lineno < curr->src_menu->lineno)
			break;
		
		prev = curr;
		curr = curr->same_file;
	}

	node->same_file = curr;
	prev->same_file = node; 
}


/*
**	output result of kcg_tree to output_file
*/
static void print_kcfg_tree(void)
{
	struct kcfg_node *curnode = &tree_rootnode;
	int layer = 0, i = 0;
	FILE *fp = get_output_fp();
	MCONF_TREE_RANGE range = gset_config_range(OP_GET, RANGE_INV);
	MCONF_TREE_DISP   disp = gset_disp_type(OP_GET, DISP_INVA);
	unsigned	long showmap = 0;
	
	if (curnode->src_sym->name)
	{
		fprintf(fp, "#########################################################\n");
		fprintf(fp, "# kernel config \"%s\" dependency tree\n", curnode->src_sym->name);
		fprintf(fp, "# '*' indicates its config item has been set(selected).\n");
		fprintf(fp, "#########################################################\n");
	}

	while (curnode)
	{
		if (range != RANGE_SEL || curnode->src_sym->curr.tri)
		{
			if (disp != DISP_ONCE || curnode->src_menu->times < 0x80)
			{
				/*
				**	"curnode->src_menu->times" will continue to be used by "create_kcfg_file".
				**	make sure this routine is performed earlier than routine "create_kcfg_file"
				*/
				curnode->src_menu->times |= 0x80;
				for (i = 0; i < layer; i++)
				{
					if (i + 1 == layer)
					{
						if (curnode->next)
						{
							fprintf(fp, "|-- ");
							/* set bit_$(layer) of showmap to 0 */
							showmap &= (~(1 << i));
						}
						else
						{
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
				fprintf(fp, "%s (%s:%d)%s\n", curnode->src_sym->name ? curnode->src_sym->name : "none", 
					curnode->src_menu->file ? (curnode->src_menu->file->name ? curnode->src_menu->file->name: "None") : "none",
					curnode->src_menu->lineno, 
					curnode->src_sym->curr.tri ? "*" : " ");
			}
		}

		if (curnode->list) 
		{
			curnode = curnode->list;
			layer++;
			continue;
		}
		if (curnode->next)
		{
			curnode = curnode->next;
		}
		else while ((curnode = curnode->pa)) 
		{
			layer--;
			if (curnode->next)
			{
				curnode = curnode->next;
				break;
			}
		}
	}
}


/*
**	output result of kcg_tree to output_file
*/
static void print_kcfg_file(void)
{
	struct kcfg_file *curnode = &headfile;
	struct kcfg_node *node;
	FILE *fp = get_output_fp();
	FILE *script_fp = get_script_fp();
	int i = 0, j= 0;

	if (curnode->next)
	{
		fprintf(fp, "\n\n\n=========================================================\n\n\n");
		fprintf(fp, "#########################################################\n");
		fprintf(fp, "# The kernel config items are grouped by Kconfig files.\n");
		fprintf(fp, "# '*' indicates its config item has been set(selected).\n");
		fprintf(fp, "#########################################################\n");
	}

	fprintf(script_fp, "#!/bin/bash\n# Auto generated by `make cut_menuconfig`.\n# Do not modify it unless you understand it.\n\n");
	fprintf(script_fp, "export ENV_MK_SET=`find . -type f -name \"Makefile\"`\n\n");

	for (curnode = curnode->next; curnode; curnode = curnode->next)
	{
		fprintf(fp, "%d. %s (parentKconfig: %s)\n", ++i, curnode->fp->name ? curnode->fp->name : "none",
			curnode->fp->parent ? (curnode->fp->parent->name ? curnode->fp->parent->name : "None") : "none");
		for (node = curnode->node; node; node = node->same_file)
		{
			fprintf(fp, "\t%d: %s (No.%d)%s\n", node->src_menu->lineno, 
					node->src_sym->name ? node->src_sym->name : "none",
					++j, node->src_sym->curr.tri ? "*" : " ");
			fprintf(script_fp, "../cutdown_kernel/scripts/do_cutdown.sh %s %d %s %s\n",
				curnode->fp->name ? curnode->fp->name : "none",
				node->src_menu->lineno,
				node->src_sym->name ? node->src_sym->name : "none",
				gset_script_file(OP_GET_DIR, NULL));
		}
		fprintf(fp, "\n");
	}

	fprintf(script_fp, "\n../cutdown_kernel/scripts/cutdown_after.sh %s\n", gset_script_file(OP_GET_DIR, NULL));
}


/*
**	output result of kcfg_file to output_file
*/
static void create_kcfg_file(void)
{
	struct kcfg_file *headnode, *currnode, *tailnode;
	struct kcfg_node *curnode = &tree_rootnode;
	FILE *fp = get_output_fp();
	struct file *nodefile = NULL;

	/*
	**	1. initialization
	*/
	headnode = tailnode = &headfile;
	tailnode->fp = NULL;
	tailnode->node = NULL;
	tailnode->next = NULL;

	/*
	**	2. construct kcfg_file list
	*/
	while (curnode)
	{
		nodefile = curnode->src_menu->file;
		if (!nodefile || !curnode->src_menu->times)
		{
			goto next;
		}

		curnode->src_menu->times = 0;
		currnode = in_kcfgfile_list(nodefile, headnode);
		if (currnode)
		{
			insert_kcfg_file(curnode, currnode);
		}
		else
		{
			tailnode->next = malloc(sizeof(struct kcfg_file));
			if (!tailnode->next)
			{
				fprintf(fp, "malloc kcfg_file failed!\n");
				return;
			}
			tailnode = tailnode->next;
			memset(tailnode, 0x0, sizeof(struct kcfg_file));
			tailnode->fp = nodefile;
			tailnode->node = curnode;
		}

	next:
		if (curnode->list) 
		{
			curnode = curnode->list;
			continue;
		}
		if (curnode->next)
		{
			curnode = curnode->next;
		}
		else while ((curnode = curnode->pa)) 
		{
			if (curnode->next)
			{
				curnode = curnode->next;
				break;
			}
		}
	}

	/*
	**	3. output kcfg_file
	*/
	print_kcfg_file();
}


/*
**	get/set config_name via arguments
*/
static char* gset_config_name(MCONF_OP op, char* config)
{
	static char* config_name = MCONF_ROOT_KCFG_NAME;

	switch (op)
	{
	case OP_GET:
		return config_name;
	case OP_SET:
		config_name = config;
		break;
	default:
		fprintf(stdout, "unsupported operation %d\n", op);
	}

	return NULL;
}


/*
**	If current expression is hited to contain parent config?	
*/
static bool expr_is_hited(struct expr *e)
{
	tristate val1, val2;
	const char *str2;
	char *config = gset_config_name(OP_GET, NULL);

	if (!e)
		return false;

	switch (e->type)
	{
	case E_SYMBOL:
		if (e->left.sym && (e->left.sym)->name)
		{
			if (!strcmp(config, (e->left.sym)->name))
			{
				return true;
			}
		}
		return false;
	case E_AND:
		val1 = expr_is_hited(e->left.expr);
		val2 = expr_is_hited(e->right.expr);
		if (val1 || val2)
		{
			return true;
		}
		return false;
	case E_OR:
		val1 = expr_is_hited(e->left.expr);
		val2 = expr_is_hited(e->right.expr);
		if (val1 && val2)
		{
			return true;
		}
		return false;
	case E_NOT:
		return false;
	case E_EQUAL:
		if (e->left.sym && (e->left.sym)->name)
		{
			if (!strcmp(config, (e->left.sym)->name))
			{
				sym_calc_value(e->right.sym);
				str2 = sym_get_string_value(e->right.sym);
				if (!strcmp("y", str2) || !strcmp("m", str2))
				{
					return true;
				}
			}
		}
		return false;
	
	case E_UNEQUAL:
		if (e->left.sym && (e->left.sym)->name)
		{
			if (!strcmp(config, (e->left.sym)->name))
			{
				sym_calc_value(e->right.sym);
				str2 = sym_get_string_value(e->right.sym);
				if (!strcmp("n", str2))
				{
					return true;
				}
			}
		}
		return false;
	default:
		fprintf(stdout, "expr_is_hited: %d?\n", e->type);
		return false;
	}
}


/*
**	If current menu is hited as child config?	
*/
static bool menu_is_hited(struct menu *menu)
{
	return expr_is_hited(menu->dep);
}


/*
**	create dependency-tree for specified root_config.
*/
static void create_kcfg_tree(void)
{
	FILE *fp;
	struct symbol *sym;
	struct menu	*menu; 
	struct kcfg_node *rootnode, *curnode, *formernode;

	/*
	**	1. get pointer of output file first.
	*/
	fp = get_output_fp();

	/*
	**	2. validate config_range && disp_type
	*/
	if (gset_config_range(OP_GET, RANGE_INV) >= RANGE_INV ||
		gset_disp_type(OP_GET, DISP_INVA) >= DISP_INVA)
	{
		fprintf(stdout, "invalid arguments!\n");
		return;
	}

	/*
	**	3. init tree_rootnode
	*/
	rootnode = &tree_rootnode;
	init_tree_node(rootnode);

	/*
	**	4. create mconf_tree
	*/
	curnode = rootnode;
	while (curnode)
	{
		if (!curnode->src_sym->name)
		{
			goto next;
		}
		
		gset_config_name(OP_SET, curnode->src_sym->name);
		formernode = NULL;
		menu = rootmenu.list;
		while (menu) 
		{
			sym = menu->sym;
			if (sym && menu_is_hited(menu))
			{
				if (!formernode)
				{
					curnode->list = malloc(sizeof(struct kcfg_node));
					if (!curnode->list)
					{
						fprintf(fp, "curnode(%s) malloc curnode->list failed\n", curnode->src_sym->name);
						return;
					}

					formernode = curnode->list;
				}
				else
				{
					formernode->next  = malloc(sizeof(struct kcfg_node));
					if (!formernode->next)
					{
						fprintf(fp, "curnode(%s) malloc childnode failed\n", curnode->src_sym->name);
						return;
					}
		
					formernode = formernode->next;
				}
				fill_tree_node(formernode, curnode, menu);
			}
			
			if (menu->list) 
			{
				menu = menu->list;
				continue;
			}
			if (menu->next)
			{
				menu = menu->next;
			}
			else while ((menu = menu->parent))
			{
				if (menu->next) 
				{
					menu = menu->next;
					break;
				}
			}
		}

	next:
		if (curnode->list) 
		{
			curnode = curnode->list;
			continue;
		}
		if (curnode->next)
		{
			curnode = curnode->next;
		}
		else while ((curnode = curnode->pa)) 
		{
			if (curnode->next)
			{
				curnode = curnode->next;
				break;
			}
		}
	}

	/*
	**	5. output kcfg_tree
	*/
	print_kcfg_tree();
}


/*
**	EXTERNALIZED FUNCTIONS
*/

/*
**	create dependency-tree for specified root_config.
*/
void mconf_tree_maker(void)
{
	MCONF_TREE_DISP   disp = gset_disp_type(OP_GET, DISP_INVA);
	
	/*
	**	kcfg_tree creation must before than kcfg_file.
	*/
	create_kcfg_tree();
	
	/*
	**	check disp_type
	*/
	if (disp == DISP_ONCE)
	{
		create_kcfg_file();
	}
}


/*
**	parse VARs config_range, disp_type, root_config and output_file from arguments.
*/
void mconf_tree_getopt(int argc, char** argv)
{
	int i = 0;
	//FILE *fp = get_output_fp();
	char dir[MCONF_OUTPUT_FILE_LEN_MAX];
	char *end_dir;

	//fprintf(stdout, "argc == %d\n", argc);
	for (i = 0; i < argc; i++)
	{
		//fprintf(stdout, "argv[%d] == \"%s\"\n", i, argv[i]);
		switch(i)
		{
		case 0:
			if (!strcmp(argv[i], MCONF_CONFIG_RANGE_ALL))
			{
				gset_config_range(OP_SET, RANGE_ALL);
			}
			else if (!strcmp(argv[i], MCONF_CONFIG_RANGE_SEL))
			{
				gset_config_range(OP_SET, RANGE_SEL);
			}
			break;
		case 1:
			if (!strcmp(argv[i], MCONF_DISP_TYPE_ONCE))
			{
				gset_disp_type(OP_SET, DISP_ONCE);
			}
			else if (!strcmp(argv[i], MCONF_DISP_TYPE_ORIG))
			{
				gset_disp_type(OP_SET, DISP_ORIG);
			}
			break;
		case 2:
			gset_root_config(OP_SET, argv[i]);
			break;
		case 3:
			if (strlen(argv[i]) >= MCONF_OUTPUT_FILE_LEN_MAX)
			{
				fprintf(stdout, "The length of argv[%d](%d) must not exceed %d\n", i, strlen(argv[i]), MCONF_OUTPUT_FILE_LEN_MAX);
				fprintf(stdout, "Using default file "MCONF_OUTPUT_FILE_DEFAULT" as output.\n");
				break;
			}
			memset(dir, 0x0, sizeof(dir));
			strcpy(dir, argv[i]);
			/* find directory name */
			end_dir = strrchr(dir, '/');
			if (end_dir)
			{
				*end_dir = 0;
			}
			else
			{
				dir[0] = '.';
				dir[1] = 0;
			}
			//fprintf(stdout, "dir == %s\n", dir);
			if (access(dir, W_OK|X_OK|F_OK))
			{
				fprintf(stdout, "The dir of output_file(%s) is absence or permission missed.\n", dir);
				fprintf(stdout, "Using default file "MCONF_OUTPUT_FILE_DEFAULT" as output.\n");
				break;
			}
			gset_output_file(OP_SET, argv[i]);
			gset_script_file(OP_SET, dir);
			break;
		default:
			;
		}
	}
}

