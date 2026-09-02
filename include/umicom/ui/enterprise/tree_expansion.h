/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/tree_expansion.h
 *
 * PURPOSE:
 *   Track expanded tree-node identities separately from tree data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_TREE_EXPANSION_H
#define UMICOM_UI_ENTERPRISE_TREE_EXPANSION_H

#include "umicom/ui/enterprise/types.h"
/**
 * Represent the ui ent tree expansion data shared with callers of this public contract.
 */
typedef struct UmiUiEntTreeExpansion { char ids[UMI_UI_ENT_MAX_CACHE][UMI_UI_ENT_ID_CAPACITY]; size_t count; } UmiUiEntTreeExpansion;
/**
 * Initialise ui ent tree expansion from caller-provided values so later operations receive
 * a known state.
 */
void umi_ui_ent_tree_expansion_init(UmiUiEntTreeExpansion *e);
/**
 * Copy ui ent tree expansion into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_ui_ent_tree_expansion_set(UmiUiEntTreeExpansion *e,const char *id,int expanded);
/**
 * Provide the ui ent tree expansion is expanded operation used by this module and its
 * client applications.
 */
int umi_ui_ent_tree_expansion_is_expanded(const UmiUiEntTreeExpansion *e,const char *id);

#endif
