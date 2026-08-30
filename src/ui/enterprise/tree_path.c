/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/tree_path.c
 *
 * PURPOSE:
 *   Implement bounded ancestor paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/tree_path.h"
#include <string.h>
void umi_ui_ent_tree_path_init(UmiUiEntTreePath *p){if(p)memset(p,0,sizeof *p);}
UmiStatus umi_ui_ent_tree_path_append(UmiUiEntTreePath *p,const char *id){if(!p||!umi_ui_ent_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;if(p->count>=UMI_UI_ENT_MAX_PATH)return UMI_STATUS_CAPACITY_EXCEEDED;if(umi_ui_ent_copy_text(p->ids[p->count],UMI_UI_ENT_ID_CAPACITY,id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;p->count++;return UMI_STATUS_OK;}
UmiStatus umi_ui_ent_tree_path_parent(UmiUiEntTreePath *p){if(!p)return UMI_STATUS_INVALID_ARGUMENT;if(p->count==0U)return UMI_STATUS_NOT_FOUND;p->count--;p->ids[p->count][0]='\0';return UMI_STATUS_OK;}
