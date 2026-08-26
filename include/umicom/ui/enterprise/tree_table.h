/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/tree_table.h
 *
 * PURPOSE:
 *   Compose a hierarchical tree column with ordinary enterprise columns.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_TREE_TABLE_H
#define UMICOM_UI_ENTERPRISE_TREE_TABLE_H

#include "umicom/ui/enterprise/column_catalogue.h"
typedef struct UmiUiEntTreeTable { char tree_column_id[UMI_UI_ENT_ID_CAPACITY]; UmiUiEntColumnCatalogue columns; int show_connectors; } UmiUiEntTreeTable;
UmiStatus umi_ui_ent_tree_table_init(UmiUiEntTreeTable *t,const char *tree_column_id);
int umi_ui_ent_tree_table_validate(const UmiUiEntTreeTable *t);

#endif
