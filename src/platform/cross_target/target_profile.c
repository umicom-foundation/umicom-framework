/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/target_profile.c
 *
 * PURPOSE:
 *   Define named deployment/runtime profiles that bind a target identity to ABI and minimum CPU capability requirements.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/target_profile.h"

#include <string.h>
UmiStatus umi_ct_target_profile_init(UmiCtTargetProfile*p,const char*id,const UmiCtTarget*t,const char*abi){if(p==NULL||!umi_ct_id_valid(id)||t==NULL||abi==NULL||abi[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;memset(p,0,sizeof(*p));if(umi_ct_copy(p->profile_id,sizeof(p->profile_id),id)!=UMI_STATUS_OK||umi_ct_copy(p->abi,sizeof(p->abi),abi)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;p->target=*t;p->minimum_page_size=4096U;return UMI_STATUS_OK;}
bool umi_ct_target_profile_satisfied(const UmiCtTargetProfile*p,uint64_t f,uint32_t page){if(p==NULL)return false;return (f&p->required_cpu_features)==p->required_cpu_features&&page>=p->minimum_page_size;}
