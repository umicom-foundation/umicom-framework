/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/selection.c
 *
 * PURPOSE:
 *   Implement semantic designer selection so multiple frontend adapters can share one authoring state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The functions below modify semantic designer state so undo, preview and
 * generation behave the same in Studio, headless tests and future hosts.
 */

#include "umicom/designer/selection.h"
#include <string.h>
/*
 * Release or reset state held by designer selection so the same storage can be reused
 * safely.
 */
void umi_designer_selection_clear(UmiDesignerSelection *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL)(void)memset(s,0,sizeof(*s));}
/*
 * Provide the designer selection contains operation used by this module and its client
 * applications.
 */
int umi_designer_selection_contains(const UmiDesignerSelection *s,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||id==NULL)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<s->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(s->node_ids[i],id)==0)return 1;return 0;}
/* Add designer selection only after its inputs and available capacity have been checked. */
UmiStatus umi_designer_selection_add(UmiDesignerSelection *s,const char *id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||!umi_decl_id_is_valid(id))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_designer_selection_contains(s,id))return UMI_STATUS_OK;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count>=UMI_DESIGNER_MAX_SELECTION)return UMI_STATUS_CAPACITY_EXCEEDED;(void)umi_decl_copy_text(s->node_ids[s->count++],UMI_DECL_ID_CAPACITY,id);return UMI_STATUS_OK;}
/*
 * Provide the designer selection set primary operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_selection_set_primary(UmiDesignerSelection *s,const char *id){UmiStatus status;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||!umi_decl_id_is_valid(id))return UMI_STATUS_INVALID_ARGUMENT;status=umi_designer_selection_add(s,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(status==UMI_STATUS_OK)status=umi_decl_copy_text(s->primary,sizeof(s->primary),id);return status;}
