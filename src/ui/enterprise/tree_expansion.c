/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/tree_expansion.c
 *
 * PURPOSE:
 *   Implement independent tree expansion state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/tree_expansion.h"
#include <string.h>
/*
 * Initialise ui ent tree expansion from caller-provided values so later operations receive
 * a known state.
 */
void umi_ui_ent_tree_expansion_init(UmiUiEntTreeExpansion *e){/* Apply this branch only when its contract condition is satisfied. */ if(e)memset(e,0,sizeof *e);}
/*
 * Provide the ui ent tree expansion is expanded operation used by this module and its
 * client applications.
 */
int umi_ui_ent_tree_expansion_is_expanded(const UmiUiEntTreeExpansion *e,const char *id){size_t i;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!e||!id)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<e->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(e->ids[i],id)==0)return 1;return 0;}
/*
 * Copy ui ent tree expansion into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_ui_ent_tree_expansion_set(UmiUiEntTreeExpansion *e,const char *id,int expanded){size_t i;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!e||!umi_ui_ent_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<e->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(e->ids[i],id)==0){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(expanded)return UMI_STATUS_OK;memmove(&e->ids[i],&e->ids[i+1U],(e->count-i-1U)*sizeof e->ids[0]);e->count--;return UMI_STATUS_OK;}/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!expanded)return UMI_STATUS_OK;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(e->count>=UMI_UI_ENT_MAX_CACHE)return UMI_STATUS_CAPACITY_EXCEEDED;umi_ui_ent_copy_text(e->ids[e->count++],UMI_UI_ENT_ID_CAPACITY,id);return UMI_STATUS_OK;}
