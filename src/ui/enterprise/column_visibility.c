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
void umi_ui_ent_column_visibility_init(UmiUiEntColumnVisibility *v){if(v)memset(v,0,sizeof *v);}
int umi_ui_ent_column_visibility_is_visible(const UmiUiEntColumnVisibility *v,const char *id){size_t i;if(!v||!id)return 0;for(i=0;i<v->hidden_count;i++)if(strcmp(v->hidden[i],id)==0)return 0;return 1;}
UmiStatus umi_ui_ent_column_visibility_hide(UmiUiEntColumnVisibility *v,const char *id){if(!v||!umi_ui_ent_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;if(!umi_ui_ent_column_visibility_is_visible(v,id))return UMI_STATUS_ALREADY_EXISTS;if(v->hidden_count>=UMI_UI_ENT_MAX_COLUMNS)return UMI_STATUS_CAPACITY_EXCEEDED;return umi_ui_ent_copy_text(v->hidden[v->hidden_count++],UMI_UI_ENT_ID_CAPACITY,id);}
UmiStatus umi_ui_ent_column_visibility_show(UmiUiEntColumnVisibility *v,const char *id){size_t i;if(!v||!id)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<v->hidden_count;i++)if(strcmp(v->hidden[i],id)==0){memmove(&v->hidden[i],&v->hidden[i+1U],(v->hidden_count-i-1U)*sizeof v->hidden[0]);v->hidden_count--;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
