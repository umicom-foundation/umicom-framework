/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/hardware_catalogue.c
 *
 * PURPOSE:
 *   Maintain bounded hardware inventory and detect overlapping MMIO regions before driver activation.
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

#include "umicom/platform/cross_target/hardware_catalogue.h"

#include <string.h>
static bool overlap(uint64_t a,uint64_t as,uint64_t b,uint64_t bs){return as!=0U&&bs!=0U&&a<b+bs&&b<a+as;}
void umi_ct_hardware_catalogue_init(UmiCtHardwareCatalogue*c){if(c!=NULL)memset(c,0,sizeof(*c));}
bool umi_ct_hardware_catalogue_mmio_conflict(const UmiCtHardwareCatalogue*c,const UmiCtHardwareDevice*d){size_t i;if(c==NULL||d==NULL)return true;for(i=0;i<c->count;++i)if(overlap(c->devices[i].mmio_base,c->devices[i].mmio_size,d->mmio_base,d->mmio_size))return true;return false;}
UmiStatus umi_ct_hardware_catalogue_add(UmiCtHardwareCatalogue*c,const UmiCtHardwareDevice*d){size_t i;if(c==NULL||umi_ct_hardware_device_validate(d)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<c->count;++i)if(strcmp(c->devices[i].device_id,d->device_id)==0)return UMI_STATUS_ALREADY_EXISTS;if(umi_ct_hardware_catalogue_mmio_conflict(c,d))return UMI_STATUS_BUSY;if(c->count>=UMI_CT_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;c->devices[c->count++]=*d;return UMI_STATUS_OK;}
const UmiCtHardwareDevice *umi_ct_hardware_catalogue_find(const UmiCtHardwareCatalogue*c,const char*id){size_t i;if(c==NULL||id==NULL)return NULL;for(i=0;i<c->count;++i)if(strcmp(c->devices[i].device_id,id)==0)return &c->devices[i];return NULL;}
