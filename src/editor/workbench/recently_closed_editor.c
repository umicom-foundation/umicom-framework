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
void umi_editor_wb_recently_closed_editor_init(UmiEditorWbRecentlyClosedEditor *h){if(h!=NULL)memset(h,0,sizeof *h);}
UmiStatus umi_editor_wb_recently_closed_editor_push(UmiEditorWbRecentlyClosedEditor *h,const char *path){if(h==NULL||path==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(h->count==UMI_EDITOR_WB_MAX_ITEMS){memmove(&h->paths[0],&h->paths[1],(UMI_EDITOR_WB_MAX_ITEMS-1U)*sizeof h->paths[0]);h->count--;}if(umi_editor_wb_copy_text(h->paths[h->count],sizeof h->paths[0],path)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;h->count++;return UMI_STATUS_OK;}
UmiStatus umi_editor_wb_recently_closed_editor_pop(UmiEditorWbRecentlyClosedEditor *h,char *out,size_t cap){if(h==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(h->count==0U)return UMI_STATUS_NOT_FOUND;if(umi_editor_wb_copy_text(out,cap,h->paths[h->count-1U])!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;h->count--;return UMI_STATUS_OK;}
