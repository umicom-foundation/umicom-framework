/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/edit_history.c
 *
 * PURPOSE:
 *   Implement bounded linear edit history.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/edit_history.h"
#include <string.h>
void umi_ui_ent_edit_history_init(UmiUiEntEditHistory *h){if(h)memset(h,0,sizeof *h);}
UmiStatus umi_ui_ent_edit_history_push(UmiUiEntEditHistory *h,const UmiUiEntEditCommit *c){if(!h||!c||!c->committed)return UMI_STATUS_INVALID_ARGUMENT;if(h->cursor<h->count)h->count=h->cursor;if(h->count>=UMI_UI_ENT_MAX_RULES){memmove(&h->items[0],&h->items[1],(UMI_UI_ENT_MAX_RULES-1U)*sizeof h->items[0]);h->count--;if(h->cursor>0U)h->cursor--;}h->items[h->count++]=*c;h->cursor=h->count;return UMI_STATUS_OK;}
const UmiUiEntEditCommit *umi_ui_ent_edit_history_undo(UmiUiEntEditHistory *h){if(!h||h->cursor==0U)return NULL;h->cursor--;return &h->items[h->cursor];}
const UmiUiEntEditCommit *umi_ui_ent_edit_history_redo(UmiUiEntEditHistory *h){const UmiUiEntEditCommit *c;if(!h||h->cursor>=h->count)return NULL;c=&h->items[h->cursor];h->cursor++;return c;}
