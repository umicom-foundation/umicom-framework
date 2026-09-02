/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/column_visibility.c
 *
 * PURPOSE:
 *   Implement column visibility overrides.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/column_visibility.h"
#include <string.h>
/*
 * Initialise ui ent column visibility from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_ent_column_visibility_init(UmiUiEntColumnVisibility *v){/* Apply this branch only when its contract condition is satisfied. */ if(v)memset(v,0,sizeof *v);}
/*
 * Provide the ui ent column visibility is visible operation used by this module and its
 * client applications.
 */
int umi_ui_ent_column_visibility_is_visible(const UmiUiEntColumnVisibility *v,const char *id){size_t i;/* Use the stable identifier comparison to choose the matching record or policy. */ if(!v||!id)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<v->hidden_count;i++)/* Use the stable identifier comparison to choose the matching record or policy. */ if(strcmp(v->hidden[i],id)==0)return 0;return 1;}
/*
 * Provide the ui ent column visibility hide operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_column_visibility_hide(UmiUiEntColumnVisibility *v,const char *id){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!v||!umi_ui_ent_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!umi_ui_ent_column_visibility_is_visible(v,id))return UMI_STATUS_ALREADY_EXISTS;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(v->hidden_count>=UMI_UI_ENT_MAX_COLUMNS)return UMI_STATUS_CAPACITY_EXCEEDED;return umi_ui_ent_copy_text(v->hidden[v->hidden_count++],UMI_UI_ENT_ID_CAPACITY,id);}
/*
 * Provide the ui ent column visibility show operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_column_visibility_show(UmiUiEntColumnVisibility *v,const char *id){size_t i;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!v||!id)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<v->hidden_count;i++)/* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(v->hidden[i],id)==0){memmove(&v->hidden[i],&v->hidden[i+1U],(v->hidden_count-i-1U)*sizeof v->hidden[0]);v->hidden_count--;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
