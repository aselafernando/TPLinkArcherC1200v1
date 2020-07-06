/*
**	scripts/xconfig/dep_util.c

--	*********************************************************************
||	source file to implement global/local variable, routines, which will used to find
||	all config items in whole menu.
--	*********************************************************************

@@	By TengFei, 14/08/08
*/

#define	DEP_CFG_ITEM_LIST_FILE_NAME		"config.list"


/*
**	Print visiable config item infomation
*/
static void print_sym_info(FILE *fp, struct symbol* sym)
{
	int i, j;
	struct menu *submenu[8], *menu;
	struct property *prop;

	for (prop = sym->prop; prop; prop = prop->next)
	{
		if (prop->text)
		{
			/* This config item is visiable */
			goto print_info;
		}
	}
	return;

print_info:
	
	
	fprintf(fp, "# Prompt: %s\n", prop->text);
	fprintf(fp, "# Defined at %s:%d\n", 
		(prop->menu && prop->menu->file && prop->menu->file->name) ? prop->menu->file->name : "none",
		prop->menu ? prop->menu->lineno : 0x0);
	
	menu = prop->menu->parent;
	for (i = 0; menu != &rootmenu && i < 8; menu = menu->parent)
		submenu[i++] = menu;
	if (i > 0) 
	{
		fprintf(fp, "# Location:\n");
		for (j = 4; --i >= 0; j += 2)
		{
			menu = submenu[i];
			fprintf(fp, "# %*c-> %s", j, ' ', menu_get_prompt(menu));
			if (menu->sym)
			{
				fprintf(fp, "(%s [=%s])",
					menu->sym->name ? menu->sym->name : "<choice>",
					sym_get_string_value(menu->sym));
			}
			fprintf(fp, "\n");
		}
	}

	fprintf(fp, "%s\n\n", sym->name);
}


/*
**	Seek through the whole menu to find all the config items.
*/
void dep_find_all_configs(void)
{
	struct symbol* sym = NULL;
	struct menu* menu = NULL;
	FILE *output_fp = NULL;
	
	if (access(DEP_CFG_ITEM_LIST_FILE_NAME, R_OK | W_OK))
	{
		/* Just return without complaining */
		return;
	}

	output_fp = fopen(DEP_CFG_ITEM_LIST_FILE_NAME, "w+");
	if (!output_fp)
	{
		fprintf(stdout, "open "DEP_CFG_ITEM_LIST_FILE_NAME" failed!\n");
		output_fp = stdout;
	}

	menu = rootmenu.list;
	while (menu) 
	{
		sym = menu->sym;
		if (sym && sym->name)
		{
			print_sym_info(output_fp, sym);
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

	fclose(output_fp);
}

