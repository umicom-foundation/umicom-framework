/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/tree_filter.h
 *
 * PURPOSE:
 *   Apply text filtering while optionally retaining matching ancestors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_TREE_FILTER_H
#define UMICOM_UI_ENTERPRISE_TREE_FILTER_H

#include "umicom/ui/enterprise/tree_node.h"
/**
 * Represent the ui ent tree filter data shared with callers of this public contract.
 */
typedef struct UmiUiEntTreeFilter { char query[UMI_UI_ENT_TEXT_CAPACITY]; int case_sensitive; int retain_ancestors; } UmiUiEntTreeFilter;
/**
 * Copy ui ent tree filter into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_ui_ent_tree_filter_set(UmiUiEntTreeFilter *f,const char *query,int case_sensitive,int retain_ancestors);
/**
 * Provide the ui ent tree filter matches operation used by this module and its client
 * applications.
 */
int umi_ui_ent_tree_filter_matches(const UmiUiEntTreeFilter *f,const UmiUiEntTreeNode *node);

#endif
