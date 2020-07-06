/*
**	scripts/xconfig/dept_tab.c

--	*********************************************************************
||	source file to implement global/local variable, routines, which will used to find
||	depent tree for some kernel config. e.g. depent tree for config  
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

@@	By TengFei, 14/08/07
*/


/*
**	DEFINITIONS AND DECLARATIONS
*/
#define	DEPT_SEL_FALSE	(-2)
#define	DEPT_DEP_FALSE	(-1)
#define	DEPT_DEP_TRUE		1
#define	DEPT_SEL_TRUE		2


#define	dept_tab_message(lv, code, fmt, args...)		\
	do {		\
		show_message(MODE_DEPT_TAB, lv, code, fmt, ##args);	\
	} while (0)



/*
**	PRIVATE FUNTIONS
*/
/*
**	Just create && initialize first node of deptab.
*/
static struct deptab_node* get_root_dept_tab_node(void)
{
	static struct deptab_node *root_dept_tab_node = NULL;

	if (!root_dept_tab_node)
	{
		root_dept_tab_node = malloc(sizeof(struct deptab_node));
		if (!root_dept_tab_node)
		{
			dept_tab_message(LV_ERROR, DEPTAB_ERR_MALLOC, 
				"No enough memory to alloc root_dept_tab_node, just exit running.");
			exit(DEPTAB_ERR_MALLOC);
		}
		memset(root_dept_tab_node, 0x0, sizeof(struct deptab_node));
	}
	return root_dept_tab_node;
}


/*
**	fill contents into tree_node
*/
static void fill_dept_node
(
	struct deptab_node* node, 
	struct deptab_node* pa_node, 
	struct symbol* psym
)
{
	struct property *prop;
	
	memset(node, 0x0, sizeof(struct deptab_node));
	node->pa = pa_node;
	node->sym = psym;

	for (prop = psym->prop; prop; prop = prop->next)
	{
		if (prop->text && prop->menu)
		{
			node->src_menu = prop->menu;
			break;
		}
	}
}


/*
**	get/set config_name via arguments
*/
static char* gset_config_name(DEP_TAB_OP op, char* config)
{
	static char* config_name = NULL;

	switch (op)
	{
	case OP_GET:
		return config_name;
	case OP_SET:
		config_name = config;
		break;
	default:
		dept_tab_message(LV_ERROR, DEPTAB_ERR_OTHERS, "Unsupported operation %d\n", op);
	}

	return NULL;
}


/*
**	If current expression is hited to contain parent config?	
*/
static int expr_is_hited(struct expr *e, int op)
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
				return op;
			}
		}
		return false;
		
	case E_AND:
	case E_OR:
		val1 = expr_is_hited(e->left.expr, op);
		val2 = expr_is_hited(e->right.expr, op);
		if (val1 || val2)
		{
			if ((val1 * val2) < 0)
			{
				dept_tab_message(LV_WARN, DEPTAB_WARN_OTHERS,
					"Expression conflicted(%d vs %d) in expr_is_hited for config %s.", 
					val1, val2, config ? config : "unknown");
				return DEPT_SEL_FALSE;	/* Temp! */
			}
			else if (!(val1 * val2))
			{
				return (val1 + val2);
			}	
			if (val1 > 0)
			{
				return (val1 < val2) ? val1 : val2 ;
			}
			return (val1 < val2) ? val2 : val1 ;
		}
		return false;
		
	case E_NOT:
		return expr_is_hited(e->left.expr, -1 * op);
		
	case E_EQUAL:
		if (e->left.sym && (e->left.sym)->name)
		{
			if (!strcmp(config, (e->left.sym)->name))
			{
				sym_calc_value(e->right.sym);
				str2 = sym_get_string_value(e->right.sym);
				if (!strcmp("n", str2) || !strcmp("N", str2))
				{
					return (-1 * op);
				}
				return op;
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
				if (!strcmp("y", str2) || !strcmp("y", str2))
				{
					return (-1 * op);
				}
				return op;
			}
		}
		return false;
		
	default:
		dept_tab_message(LV_ERROR, DEPTAB_ERR_OTHERS, "expr_is_hited: %d?\n", e->type);
		return false;
	}
}


/*
**	If current menu is hited as child config?	
*/
static int menu_is_hited(struct menu *menu)
{
	struct property *st = NULL;
	struct expr* exp = node_get_exp(menu->sym);
	int ret = false;

	if (!menu->sym)
	{
		return false;
	}
	
	if (exp)
	{
		ret = expr_is_hited(exp, DEPT_DEP_TRUE);
		if (ret)
		{
			return ret;
		}
	}

	for (st = menu->sym->prop; st; st = st->next)
	{
		if (st->type == P_SELECT && st->expr)
		{
			ret = expr_is_hited(st->expr, DEPT_SEL_TRUE);
			if (ret)
			{
				return ret;
			}
		}
	}

	return ret;
}


/*
**	Begin to create config item depent tree of dept_tab.
*/
static int create_dept_tab_tree(struct symbol *symb, struct deptab_node* n)
{
	struct menu* menu = NULL;
	struct symbol* sym = NULL;
	struct deptab_node* formernode = NULL;
	struct deptab_node* curnode = n;
	int ret_val = 0;

	if (!curnode || !curnode->cfg_name || !symb)
	{
		return DEPTAB_RET_VAL_IGN;
	}
	
	gset_config_name(OP_SET, curnode->cfg_name);
	menu = rootmenu.list;
	while (menu)
	{
		sym = menu->sym;
		if (!sym)
		{
			goto next;
		}
		ret_val = menu_is_hited(menu);
		if (ret_val)
		{
			if (!formernode)
			{
				curnode->list = malloc(sizeof(struct deptab_node));
				if (!curnode->list)
				{
					dept_tab_message(LV_ERROR, DEPTAB_ERR_MALLOC, 
							"No enough memory to alloc struct deptab_node in create_dept_tab_tree.");
					return DEPTAB_ERR_MALLOC;
				}
				formernode = curnode->list;
			}
			else
			{
				formernode->next  = malloc(sizeof(struct deptab_node));
				if (!formernode->next)
				{
					dept_tab_message(LV_ERROR, DEPTAB_ERR_MALLOC, 
						"No enough memory to alloc curnode(%s) in create_dept_tab_tree.\n", curnode->sym->name);
					return DEPTAB_ERR_MALLOC;
				}
	
				formernode = formernode->next;
			}
			fill_dept_node(formernode, curnode, sym);
			if (!sym->dept_tab_node)
			{
				sym->dept_tab_node = formernode;
				if (sym->name)
				{
					formernode->cfg_name = sym->name;
				}
				else
				{
					formernode->cfg_name = deptab_choice;
				}
			}
			formernode->be_select = ret_val;
		}

	next:
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

	return DEPTAB_RET_VAL_OK;
}


/*
**	Build the whole integrated config item depent tree based ".dept_tab"
*/
static int build_dept_tab_tree(struct deptab_node* node)
{
	int ret = 0;
	struct symbol *sym = NULL;

	for (node = node->next; node;)
	{
		/* Just skip! */
		if (!node->cfg_name || !strcmp(node->cfg_name, deptab_choice))
		{
			goto next;
		}
		
		sym = sym_find(node->cfg_name);
		if (!sym || !sym->name || strcmp(sym->name, node->cfg_name))
		{
			dept_tab_message(LV_WARN, DEPTAB_WARN_SYM_NONE, 
				"Can not find corresponding symbol for config item %s.", node->cfg_name);
			goto next;
		}
		/* This is a trunk node! */
		if (!node->pa)
		{
			free(node->cfg_name);
			node->cfg_name = sym->name;
		}

		if (!node->sym)
		{
			node->sym = sym;
		}
		else if (node->sym != sym)
		{
			dept_tab_message(LV_WARN, DEPTAB_WARN_SYM_ANOTHER, 
				"Multi-symbol found for config item %s, one in 0x%08x, another one in 0x%08x.", 
				sym->name, node->sym, sym);
		}
		if (!sym->dept_tab_node)
		{
			sym->dept_tab_node = node;
		}
		else if (sym->dept_tab_node != node)
		{
			/* This is a trunk node! */
			if (!node->pa)
			{
				node->cfg_name = NULL;
				goto next;
			}
			dept_tab_message(LV_INFO, DEPTAB_COMMON_CODE, 
				"Two different config items (%s && %s) attach with the same symbol(%s)",
				node->cfg_name ? node->cfg_name : "none", 
				sym->dept_tab_node->cfg_name ? sym->dept_tab_node->cfg_name : "none" , 
				sym->name ? sym->name : "unknown");
		}
		
		ret = create_dept_tab_tree(sym, node);
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
**	Background funtion to parse ".dept_tab" and apply it to rewrite default value of 
**	config item into .config file.
*/
static int  _dept_tab_hdlr(void)
{
	int ret = DEPTAB_RET_VAL_OK;
	struct deptab_node* root_node = get_root_dept_tab_node();

	ret = fill_basic_node(DEPT_TAB_FILE_NAME, root_node);
	if (ret > 0)
	{
		return ret;
	}

	ret = build_dept_tab_tree(root_node);
	if (ret > 0)
	{
		return ret;
	}
	
	print_deptab_tree(root_node->next, true);

	return ret;
}



/*
**	EXTERNALIZED FUNCTIONS
*/

/*
**	create dependency-tree for config items in ".dept_tab".
*/
int dept_tab_hdlr(IPT_DEP_MODE mode)
{
	int res = 0;

	gset_dep_mode(OP_SET, mode);
	
	res = _dept_tab_hdlr();

	return res;
}

