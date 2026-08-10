/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/selection.c
 *
 * PURPOSE:
 *   Implement semantic designer selection so multiple frontend adapters can share one authoring state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The functions below modify semantic designer state so undo, preview and
 * generation behave the same in Studio, headless tests and future hosts.
 */

#include "umicom/designer/selection.h"
#include <string.h>
void umi_designer_selection_clear(UmiDesignerSelection *s){if(s!=NULL)(void)memset(s,0,sizeof(*s));}
int umi_designer_selection_contains(const UmiDesignerSelection *s,const char *id){size_t i;if(s==NULL||id==NULL)return 0;for(i=0U;i<s->count;++i)if(strcmp(s->node_ids[i],id)==0)return 1;return 0;}
UmiStatus umi_designer_selection_add(UmiDesignerSelection *s,const char *id){if(s==NULL||!umi_decl_id_is_valid(id))return UMI_STATUS_INVALID_ARGUMENT;if(umi_designer_selection_contains(s,id))return UMI_STATUS_OK;if(s->count>=UMI_DESIGNER_MAX_SELECTION)return UMI_STATUS_CAPACITY_EXCEEDED;(void)umi_decl_copy_text(s->node_ids[s->count++],UMI_DECL_ID_CAPACITY,id);return UMI_STATUS_OK;}
UmiStatus umi_designer_selection_set_primary(UmiDesignerSelection *s,const char *id){UmiStatus status;if(s==NULL||!umi_decl_id_is_valid(id))return UMI_STATUS_INVALID_ARGUMENT;status=umi_designer_selection_add(s,id);if(status==UMI_STATUS_OK)status=umi_decl_copy_text(s->primary,sizeof(s->primary),id);return status;}
