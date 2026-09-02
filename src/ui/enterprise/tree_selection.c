/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/tree_selection.c
 *
 * PURPOSE:
 *   Implement stable tree selection by node identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/tree_selection.h"
#include <string.h>
/*
 * Initialise ui ent tree selection from caller-provided values so later operations receive
 * a known state.
 */
void umi_ui_ent_tree_selection_init(UmiUiEntTreeSelection *s,UmiUiEntSelectionMode m){/* Apply this branch only when its contract condition is satisfied. */ if(s){memset(s,0,sizeof *s);s->mode=m;}}
/*
 * Provide the ui ent tree selection contains operation used by this module and its client
 * applications.
 */
int umi_ui_ent_tree_selection_contains(const UmiUiEntTreeSelection *s,const char *id){size_t i;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!s||!id)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<s->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(s->ids[i],id)==0)return 1;return 0;}
/*
 * Add ui ent tree selection only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ui_ent_tree_selection_add(UmiUiEntTreeSelection *s,const char *id){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!s||!umi_ui_ent_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_ui_ent_tree_selection_contains(s,id))return UMI_STATUS_ALREADY_EXISTS;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s->mode==UMI_UI_ENT_SELECTION_SINGLE)s->count=0U;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s->count>=UMI_UI_ENT_MAX_SELECTIONS)return UMI_STATUS_CAPACITY_EXCEEDED;umi_ui_ent_copy_text(s->ids[s->count++],UMI_UI_ENT_ID_CAPACITY,id);return UMI_STATUS_OK;}
