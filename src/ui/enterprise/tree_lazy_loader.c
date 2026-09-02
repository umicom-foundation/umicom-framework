/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/tree_lazy_loader.c
 *
 * PURPOSE:
 *   Implement ordered lazy-load request state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/tree_lazy_loader.h"
#include <string.h>
/*
 * Provide the ui ent tree lazy loader request operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_tree_lazy_loader_request(UmiUiEntTreeLazyLoader *l,const char *id,uint64_t seq){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!l||!umi_ui_ent_id_valid(id)||seq==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(l->state==UMI_UI_ENT_LOAD_REQUESTED)return UMI_STATUS_BUSY;memset(l,0,sizeof *l);umi_ui_ent_copy_text(l->node_id,sizeof l->node_id,id);l->state=UMI_UI_ENT_LOAD_REQUESTED;l->request_sequence=seq;return UMI_STATUS_OK;}
/*
 * Provide the ui ent tree lazy loader complete operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_ent_tree_lazy_loader_complete(UmiUiEntTreeLazyLoader *l,uint64_t seq,int success){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!l||l->state!=UMI_UI_ENT_LOAD_REQUESTED||seq!=l->request_sequence)return UMI_STATUS_INVALID_STATE;l->state=success?UMI_UI_ENT_LOAD_LOADED:UMI_UI_ENT_LOAD_FAILED;return UMI_STATUS_OK;}
