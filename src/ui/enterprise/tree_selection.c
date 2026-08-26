/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/tree_selection.c
 *
 * PURPOSE:
 *   Implement stable tree selection by node identity.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/tree_selection.h"
#include <string.h>
void umi_ui_ent_tree_selection_init(UmiUiEntTreeSelection *s,UmiUiEntSelectionMode m){if(s){memset(s,0,sizeof *s);s->mode=m;}}
int umi_ui_ent_tree_selection_contains(const UmiUiEntTreeSelection *s,const char *id){size_t i;if(!s||!id)return 0;for(i=0;i<s->count;i++)if(strcmp(s->ids[i],id)==0)return 1;return 0;}
UmiStatus umi_ui_ent_tree_selection_add(UmiUiEntTreeSelection *s,const char *id){if(!s||!umi_ui_ent_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;if(umi_ui_ent_tree_selection_contains(s,id))return UMI_STATUS_ALREADY_EXISTS;if(s->mode==UMI_UI_ENT_SELECTION_SINGLE)s->count=0U;if(s->count>=UMI_UI_ENT_MAX_SELECTIONS)return UMI_STATUS_CAPACITY_EXCEEDED;umi_ui_ent_copy_text(s->ids[s->count++],UMI_UI_ENT_ID_CAPACITY,id);return UMI_STATUS_OK;}
