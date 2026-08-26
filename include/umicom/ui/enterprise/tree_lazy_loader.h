/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/tree_lazy_loader.h
 *
 * PURPOSE:
 *   Track request state for asynchronous/lazy child materialisation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_TREE_LAZY_LOADER_H
#define UMICOM_UI_ENTERPRISE_TREE_LAZY_LOADER_H

#include "umicom/ui/enterprise/types.h"
typedef enum UmiUiEntLoadState { UMI_UI_ENT_LOAD_IDLE=0,UMI_UI_ENT_LOAD_REQUESTED=1,UMI_UI_ENT_LOAD_LOADED=2,UMI_UI_ENT_LOAD_FAILED=3 } UmiUiEntLoadState;
typedef struct UmiUiEntTreeLazyLoader { char node_id[UMI_UI_ENT_ID_CAPACITY]; UmiUiEntLoadState state; uint64_t request_sequence; } UmiUiEntTreeLazyLoader;
UmiStatus umi_ui_ent_tree_lazy_loader_request(UmiUiEntTreeLazyLoader *l,const char *node_id,uint64_t sequence);
UmiStatus umi_ui_ent_tree_lazy_loader_complete(UmiUiEntTreeLazyLoader *l,uint64_t sequence,int success);

#endif
