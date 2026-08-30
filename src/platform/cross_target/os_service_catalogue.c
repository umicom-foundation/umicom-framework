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
void umi_ct_os_service_catalogue_init(UmiCtOsServiceCatalogue*c){if(c!=NULL)memset(c,0,sizeof(*c));}
UmiStatus umi_ct_os_service_catalogue_add(UmiCtOsServiceCatalogue*c,const UmiCtOsServiceDescriptor*s){size_t i;if(c==NULL||s==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<c->count;++i)if(strcmp(c->items[i].service_id,s->service_id)==0)return UMI_STATUS_ALREADY_EXISTS;if(c->count>=UMI_CT_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;c->items[c->count++]=*s;return UMI_STATUS_OK;}
const UmiCtOsServiceDescriptor *umi_ct_os_service_catalogue_find(const UmiCtOsServiceCatalogue*c,const char*id){size_t i;if(c==NULL||id==NULL)return NULL;for(i=0U;i<c->count;++i)if(strcmp(c->items[i].service_id,id)==0)return &c->items[i];return NULL;}
