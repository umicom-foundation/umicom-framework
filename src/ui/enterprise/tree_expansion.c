/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/tree_expansion.c
 *
 * PURPOSE:
 *   Implement independent tree expansion state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/tree_expansion.h"
#include <string.h>
void umi_ui_ent_tree_expansion_init(UmiUiEntTreeExpansion *e){if(e)memset(e,0,sizeof *e);}
int umi_ui_ent_tree_expansion_is_expanded(const UmiUiEntTreeExpansion *e,const char *id){size_t i;if(!e||!id)return 0;for(i=0;i<e->count;i++)if(strcmp(e->ids[i],id)==0)return 1;return 0;}
UmiStatus umi_ui_ent_tree_expansion_set(UmiUiEntTreeExpansion *e,const char *id,int expanded){size_t i;if(!e||!umi_ui_ent_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<e->count;i++)if(strcmp(e->ids[i],id)==0){if(expanded)return UMI_STATUS_OK;memmove(&e->ids[i],&e->ids[i+1U],(e->count-i-1U)*sizeof e->ids[0]);e->count--;return UMI_STATUS_OK;}if(!expanded)return UMI_STATUS_OK;if(e->count>=UMI_UI_ENT_MAX_CACHE)return UMI_STATUS_CAPACITY_EXCEEDED;umi_ui_ent_copy_text(e->ids[e->count++],UMI_UI_ENT_ID_CAPACITY,id);return UMI_STATUS_OK;}
