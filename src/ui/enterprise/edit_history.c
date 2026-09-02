/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/edit_history.c
 *
 * PURPOSE:
 *   Implement bounded linear edit history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/edit_history.h"
#include <string.h>
/*
 * Initialise ui ent edit history from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_ent_edit_history_init(UmiUiEntEditHistory *h){/* Apply this branch only when its contract condition is satisfied. */ if(h)memset(h,0,sizeof *h);}
/*
 * Provide the ui ent edit history push operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_edit_history_push(UmiUiEntEditHistory *h,const UmiUiEntEditCommit *c){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!h||!c||!c->committed)return UMI_STATUS_INVALID_ARGUMENT;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(h->cursor<h->count)h->count=h->cursor;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(h->count>=UMI_UI_ENT_MAX_RULES){memmove(&h->items[0],&h->items[1],(UMI_UI_ENT_MAX_RULES-1U)*sizeof h->items[0]);h->count--;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(h->cursor>0U)h->cursor--;}h->items[h->count++]=*c;h->cursor=h->count;return UMI_STATUS_OK;}
/*
 * Provide the ui ent edit history undo operation used by this module and its client
 * applications.
 */
const UmiUiEntEditCommit *umi_ui_ent_edit_history_undo(UmiUiEntEditHistory *h){/* Apply this branch only when its contract condition is satisfied. */ if(!h||h->cursor==0U)return NULL;h->cursor--;return &h->items[h->cursor];}
/*
 * Provide the ui ent edit history redo operation used by this module and its client
 * applications.
 */
const UmiUiEntEditCommit *umi_ui_ent_edit_history_redo(UmiUiEntEditHistory *h){const UmiUiEntEditCommit *c;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!h||h->cursor>=h->count)return NULL;c=&h->items[h->cursor];h->cursor++;return c;}
