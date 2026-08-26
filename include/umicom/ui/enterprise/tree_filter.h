/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/tree_filter.h
 *
 * PURPOSE:
 *   Apply text filtering while optionally retaining matching ancestors.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_TREE_FILTER_H
#define UMICOM_UI_ENTERPRISE_TREE_FILTER_H

#include "umicom/ui/enterprise/tree_node.h"
typedef struct UmiUiEntTreeFilter { char query[UMI_UI_ENT_TEXT_CAPACITY]; int case_sensitive; int retain_ancestors; } UmiUiEntTreeFilter;
UmiStatus umi_ui_ent_tree_filter_set(UmiUiEntTreeFilter *f,const char *query,int case_sensitive,int retain_ancestors);
int umi_ui_ent_tree_filter_matches(const UmiUiEntTreeFilter *f,const UmiUiEntTreeNode *node);

#endif
