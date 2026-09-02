/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/recently_closed_editor.c
 *
 * PURPOSE:
 *   Maintain a bounded LIFO history of recently closed resources.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/recently_closed_editor.h"

#include <string.h>
/*
 * Initialise editor wb recently closed editor from caller-provided values so later
 * operations receive a known state.
 */
void umi_editor_wb_recently_closed_editor_init(UmiEditorWbRecentlyClosedEditor *h){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(h!=NULL)memset(h,0,sizeof *h);}
/*
 * Provide the editor wb recently closed editor push operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_wb_recently_closed_editor_push(UmiEditorWbRecentlyClosedEditor *h,const char *path){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(h==NULL||path==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(h->count==UMI_EDITOR_WB_MAX_ITEMS){memmove(&h->paths[0],&h->paths[1],(UMI_EDITOR_WB_MAX_ITEMS-1U)*sizeof h->paths[0]);h->count--;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_copy_text(h->paths[h->count],sizeof h->paths[0],path)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;h->count++;return UMI_STATUS_OK;}
/*
 * Provide the editor wb recently closed editor pop operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_wb_recently_closed_editor_pop(UmiEditorWbRecentlyClosedEditor *h,char *out,size_t cap){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(h==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(h->count==0U)return UMI_STATUS_NOT_FOUND;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_copy_text(out,cap,h->paths[h->count-1U])!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;h->count--;return UMI_STATUS_OK;}
