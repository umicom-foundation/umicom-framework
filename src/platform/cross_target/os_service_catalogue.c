/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/os_service_catalogue.c
 *
 * PURPOSE:
 *   Maintain a bounded Umicom OS service catalogue used by boot and dependency planning.
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

#include "umicom/platform/cross_target/os_service_catalogue.h"

#include <string.h>
/*
 * Initialise ct os service catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_ct_os_service_catalogue_init(UmiCtOsServiceCatalogue*c){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c!=NULL)memset(c,0,sizeof(*c));}
/*
 * Add ct os service catalogue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ct_os_service_catalogue_add(UmiCtOsServiceCatalogue*c,const UmiCtOsServiceDescriptor*s){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||s==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<c->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(c->items[i].service_id,s->service_id)==0)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c->count>=UMI_CT_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;c->items[c->count++]=*s;return UMI_STATUS_OK;}
/*
 * Find ct os service catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiCtOsServiceDescriptor *umi_ct_os_service_catalogue_find(const UmiCtOsServiceCatalogue*c,const char*id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||id==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<c->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(c->items[i].service_id,id)==0)return &c->items[i];return NULL;}
