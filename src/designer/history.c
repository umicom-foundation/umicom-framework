/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/history.c
 *
 * PURPOSE:
 *   Implement bounded undo/redo stacks around semantic operations so every authoring mutation is reversible.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The functions below modify semantic designer state so undo, preview and
 * generation behave the same in Studio, headless tests and future hosts.
 */

#include "umicom/designer/history.h"
#include <stdlib.h>
#include <string.h>
struct UmiDesignerHistory{UmiDesignerDocument *document;UmiDesignerOperation undo[UMI_DESIGNER_MAX_HISTORY];size_t undo_count;UmiDesignerOperation redo[UMI_DESIGNER_MAX_HISTORY];size_t redo_count;};
UmiStatus umi_designer_history_create(UmiDesignerDocument *doc,UmiDesignerHistory **out){UmiDesignerHistory *h;if(doc==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;h=calloc(1U,sizeof(*h));if(h==NULL)return UMI_STATUS_OUT_OF_MEMORY;h->document=doc;*out=h;return UMI_STATUS_OK;}
void umi_designer_history_destroy(UmiDesignerHistory *h){free(h);}
static void push(UmiDesignerOperation *items,size_t *count,const UmiDesignerOperation *op){if(*count==UMI_DESIGNER_MAX_HISTORY){(void)memmove(items,items+1,(UMI_DESIGNER_MAX_HISTORY-1U)*sizeof(items[0]));*count-=1U;}items[(*count)++]=*op;}
UmiStatus umi_designer_history_execute(UmiDesignerHistory *h,const UmiDesignerOperation *op){UmiStatus s;if(h==NULL||op==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_designer_operation_apply(h->document,op,0);if(s==UMI_STATUS_OK){push(h->undo,&h->undo_count,op);h->redo_count=0U;}return s;}
UmiStatus umi_designer_history_undo(UmiDesignerHistory *h){UmiDesignerOperation op;UmiStatus s;if(h==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(h->undo_count==0U)return UMI_STATUS_NOT_FOUND;op=h->undo[h->undo_count-1U];s=umi_designer_operation_apply(h->document,&op,1);if(s==UMI_STATUS_OK){h->undo_count-=1U;push(h->redo,&h->redo_count,&op);}return s;}
UmiStatus umi_designer_history_redo(UmiDesignerHistory *h){UmiDesignerOperation op;UmiStatus s;if(h==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(h->redo_count==0U)return UMI_STATUS_NOT_FOUND;op=h->redo[h->redo_count-1U];s=umi_designer_operation_apply(h->document,&op,0);if(s==UMI_STATUS_OK){h->redo_count-=1U;push(h->undo,&h->undo_count,&op);}return s;}
size_t umi_designer_history_undo_count(const UmiDesignerHistory *h){return h!=NULL?h->undo_count:0U;}size_t umi_designer_history_redo_count(const UmiDesignerHistory *h){return h!=NULL?h->redo_count:0U;}
