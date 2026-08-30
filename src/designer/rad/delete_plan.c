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
UmiStatus umi_rad_delete_plan_init(UmiRadDeletePlan *p){if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(p,0,sizeof *p);return UMI_STATUS_OK;}
int umi_rad_delete_plan_contains(const UmiRadDeletePlan *p,const char *id){size_t i;if(p==NULL||id==NULL)return 0;for(i=0U;i<p->count;i++)if(strcmp(p->ids[i],id)==0)return 1;return 0;}
UmiStatus umi_rad_delete_plan_add(UmiRadDeletePlan *p,const char *id){if(p==NULL||!umi_rad_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;if(umi_rad_delete_plan_contains(p,id))return UMI_STATUS_ALREADY_EXISTS;if(p->count>=UMI_RAD_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;if(umi_rad_copy_text(p->ids[p->count],sizeof p->ids[p->count],id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;p->count++;return UMI_STATUS_OK;}
