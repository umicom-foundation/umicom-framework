/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/delete_plan.c
 *
 * PURPOSE:
 *   Describe a bounded set of component identifiers scheduled for deletion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/delete_plan.h"
#include <string.h>
/*
 * Initialise rad delete plan from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_delete_plan_init(UmiRadDeletePlan *p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(p,0,sizeof *p);return UMI_STATUS_OK;}
/*
 * Provide the rad delete plan contains operation used by this module and its client
 * applications.
 */
int umi_rad_delete_plan_contains(const UmiRadDeletePlan *p,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||id==NULL)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<p->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(p->ids[i],id)==0)return 1;return 0;}
/* Add rad delete plan only after its inputs and available capacity have been checked. */
UmiStatus umi_rad_delete_plan_add(UmiRadDeletePlan *p,const char *id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||!umi_rad_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_rad_delete_plan_contains(p,id))return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->count>=UMI_RAD_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_rad_copy_text(p->ids[p->count],sizeof p->ids[p->count],id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;p->count++;return UMI_STATUS_OK;}
