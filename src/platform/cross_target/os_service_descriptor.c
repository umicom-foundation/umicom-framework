/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/os_service_descriptor.c
 *
 * PURPOSE:
 *   Define Framework-owned operating-system service descriptors and declared service dependencies.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/os_service_descriptor.h"

#include <string.h>
UmiStatus umi_ct_os_service_descriptor_init(UmiCtOsServiceDescriptor*s,const char*id,UmiCtOsServiceKind k,bool e){if(s==NULL||!umi_ct_id_valid(id)||k<UMI_CT_OS_SERVICE_KERNEL||k>UMI_CT_OS_SERVICE_USER)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof(*s));if(umi_ct_copy(s->service_id,sizeof(s->service_id),id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->kind=k;s->essential=e;return UMI_STATUS_OK;}
UmiStatus umi_ct_os_service_descriptor_add_dependency(UmiCtOsServiceDescriptor*s,const char*id){size_t i;if(s==NULL||!umi_ct_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<s->dependency_count;++i)if(strcmp(s->dependencies[i],id)==0)return UMI_STATUS_ALREADY_EXISTS;if(s->dependency_count>=UMI_CT_MAX_DEPENDENCIES)return UMI_STATUS_CAPACITY_EXCEEDED;return umi_ct_copy(s->dependencies[s->dependency_count++],UMI_CT_ID_CAPACITY,id);}
