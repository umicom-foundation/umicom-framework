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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/memory_region.h"

/* Check that ct memory region satisfies its contract before another service relies on it. */
UmiStatus umi_ct_memory_region_validate(const UmiCtMemoryRegion*r){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||r->size==0U||r->base+r->size<r->base||r->type<UMI_CT_REGION_RAM||r->type>UMI_CT_REGION_FIRMWARE)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->type==UMI_CT_REGION_ROM&&(r->permissions&UMI_CT_REGION_WRITE)!=0U)return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}
/*
 * Provide the ct memory region overlaps operation used by this module and its client
 * applications.
 */
bool umi_ct_memory_region_overlaps(const UmiCtMemoryRegion*a,const UmiCtMemoryRegion*b){return a!=NULL&&b!=NULL&&a->base<b->base+b->size&&b->base<a->base+a->size;}
