/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/tree_table.h
 *
 * PURPOSE:
 *   Compose a hierarchical tree column with ordinary enterprise columns.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_TREE_TABLE_H
#define UMICOM_UI_ENTERPRISE_TREE_TABLE_H

#include "umicom/ui/enterprise/column_catalogue.h"
/**
 * Represent the ui ent tree table data shared with callers of this public contract.
 */
typedef struct UmiUiEntTreeTable { char tree_column_id[UMI_UI_ENT_ID_CAPACITY]; UmiUiEntColumnCatalogue columns; int show_connectors; } UmiUiEntTreeTable;
/**
 * Initialise ui ent tree table from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_ent_tree_table_init(UmiUiEntTreeTable *t,const char *tree_column_id);
/**
 * Check that ui ent tree table satisfies its contract before another service relies on it.
 */
int umi_ui_ent_tree_table_validate(const UmiUiEntTreeTable *t);

#endif
