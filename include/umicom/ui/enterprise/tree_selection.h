/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/tree_selection.h
 *
 * PURPOSE:
 *   Maintain selected tree-node identities independently of visible projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_TREE_SELECTION_H
#define UMICOM_UI_ENTERPRISE_TREE_SELECTION_H

#include "umicom/ui/enterprise/types.h"
/**
 * Represent the ui ent tree selection data shared with callers of this public contract.
 */
typedef struct UmiUiEntTreeSelection { UmiUiEntSelectionMode mode; char ids[UMI_UI_ENT_MAX_SELECTIONS][UMI_UI_ENT_ID_CAPACITY]; size_t count; } UmiUiEntTreeSelection;
/**
 * Initialise ui ent tree selection from caller-provided values so later operations receive
 * a known state.
 */
void umi_ui_ent_tree_selection_init(UmiUiEntTreeSelection *s,UmiUiEntSelectionMode mode);
/**
 * Add ui ent tree selection only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ui_ent_tree_selection_add(UmiUiEntTreeSelection *s,const char *id);
/**
 * Provide the ui ent tree selection contains operation used by this module and its client
 * applications.
 */
int umi_ui_ent_tree_selection_contains(const UmiUiEntTreeSelection *s,const char *id);

#endif
