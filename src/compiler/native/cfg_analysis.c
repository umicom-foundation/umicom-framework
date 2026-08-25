/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/cfg_analysis.c
 *
 * PURPOSE:
 *   Compute reachable basic blocks and edge counts from the native IR control-flow graph.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/cfg_analysis.h"
#include <string.h>
bool umi_nc_cfg_is_reachable(const UmiNativeCfgAnalysis *a,uint32_t id){if(a==NULL||id==0U)return false;for(size_t i=0U;i<a->reachable_count;i++)if(a->reachable_ids[i]==id)return true;return false;}
UmiStatus umi_nc_cfg_analyze(const UmiNativeIrFunction *f,UmiNativeCfgAnalysis *a){if(f==NULL||a==NULL||f->entry_block_id==0U)return UMI_STATUS_INVALID_ARGUMENT;memset(a,0,sizeof(*a));uint32_t queue[UMI_NC_MAX_IR_BLOCKS];size_t head=0U,tail=0U;queue[tail++]=f->entry_block_id;while(head<tail){uint32_t id=queue[head++];if(umi_nc_cfg_is_reachable(a,id))continue;if(a->reachable_count>=UMI_NC_MAX_IR_BLOCKS)return UMI_STATUS_CAPACITY_EXCEEDED;a->reachable_ids[a->reachable_count++]=id;const UmiNativeIrBlock *b=umi_nc_ir_function_block_const(f,id);if(b==NULL)continue;a->edge_count+=b->successor_count;for(size_t i=0U;i<b->successor_count;i++)if(!umi_nc_cfg_is_reachable(a,b->successors[i])&&tail<UMI_NC_MAX_IR_BLOCKS)queue[tail++]=b->successors[i];}a->unreachable_count=f->block_count-a->reachable_count;return UMI_STATUS_OK;}
