/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/tree_lazy_loader.h
 *
 * PURPOSE:
 *   Track request state for asynchronous/lazy child materialisation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_TREE_LAZY_LOADER_H
#define UMICOM_UI_ENTERPRISE_TREE_LAZY_LOADER_H

#include "umicom/ui/enterprise/types.h"
/**
 * List the named ui ent load state values accepted by this public contract.
 */
typedef enum UmiUiEntLoadState { UMI_UI_ENT_LOAD_IDLE=0,UMI_UI_ENT_LOAD_REQUESTED=1,UMI_UI_ENT_LOAD_LOADED=2,UMI_UI_ENT_LOAD_FAILED=3 } UmiUiEntLoadState;
/**
 * Represent the ui ent tree lazy loader data shared with callers of this public contract.
 */
typedef struct UmiUiEntTreeLazyLoader { char node_id[UMI_UI_ENT_ID_CAPACITY]; UmiUiEntLoadState state; uint64_t request_sequence; } UmiUiEntTreeLazyLoader;
/**
 * Provide the ui ent tree lazy loader request operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_tree_lazy_loader_request(UmiUiEntTreeLazyLoader *l,const char *node_id,uint64_t sequence);
/**
 * Provide the ui ent tree lazy loader complete operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_ent_tree_lazy_loader_complete(UmiUiEntTreeLazyLoader *l,uint64_t sequence,int success);

#endif
