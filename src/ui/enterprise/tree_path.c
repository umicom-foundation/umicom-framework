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
/*
 * Initialise ui ent tree path from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_ent_tree_path_init(UmiUiEntTreePath *p){/* Apply this branch only when its contract condition is satisfied. */ if(p)memset(p,0,sizeof *p);}
/* Add ui ent tree path only after its inputs and available capacity have been checked. */
UmiStatus umi_ui_ent_tree_path_append(UmiUiEntTreePath *p,const char *id){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!p||!umi_ui_ent_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->count>=UMI_UI_ENT_MAX_PATH)return UMI_STATUS_CAPACITY_EXCEEDED;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_ui_ent_copy_text(p->ids[p->count],UMI_UI_ENT_ID_CAPACITY,id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;p->count++;return UMI_STATUS_OK;}
/*
 * Provide the ui ent tree path parent operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_tree_path_parent(UmiUiEntTreePath *p){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!p)return UMI_STATUS_INVALID_ARGUMENT;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->count==0U)return UMI_STATUS_NOT_FOUND;p->count--;p->ids[p->count][0]='\0';return UMI_STATUS_OK;}
