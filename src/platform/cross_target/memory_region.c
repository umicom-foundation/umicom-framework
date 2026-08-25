/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/memory_region.c
 *
 * PURPOSE:
 *   Describe physical/virtual memory regions and detect address-space overlap before boot allocation.
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

#include "umicom/platform/cross_target/memory_region.h"

UmiStatus umi_ct_memory_region_validate(const UmiCtMemoryRegion*r){if(r==NULL||r->size==0U||r->base+r->size<r->base||r->type<UMI_CT_REGION_RAM||r->type>UMI_CT_REGION_FIRMWARE)return UMI_STATUS_INVALID_ARGUMENT;if(r->type==UMI_CT_REGION_ROM&&(r->permissions&UMI_CT_REGION_WRITE)!=0U)return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}
bool umi_ct_memory_region_overlaps(const UmiCtMemoryRegion*a,const UmiCtMemoryRegion*b){return a!=NULL&&b!=NULL&&a->base<b->base+b->size&&b->base<a->base+a->size;}
