/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/transform_pipeline.c
 *
 * PURPOSE:
 *   Validate transform-plan completeness and calculate a deterministic pipeline fingerprint.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/transform_pipeline.h"
#include <string.h>
#include <limits.h>


UmiStatus umi_fabric_transform_pipeline_validate(const UmiFabricTransformPlan *plan,size_t maximum_steps,uint64_t *out_fingerprint){size_t i;uint64_t h=0U;if(plan==NULL||out_fingerprint==NULL||plan->count==0U||plan->count>maximum_steps)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<plan->count;++i){h=umi_fabric_hash64(plan->items[i].rule_id,strlen(plan->items[i].rule_id),h);h=umi_fabric_hash64(plan->items[i].operation,strlen(plan->items[i].operation),h);}*out_fingerprint=h;return h!=0U?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE;}
