/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/resource_pack.c
 *
 * PURPOSE:
 *   bounded resource-pack entries and aggregate size accounting.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/resource_pack.h"

void umi_dr_resource_pack_init(UmiDrResourcePack*p){if(p!=NULL)*p=(UmiDrResourcePack){0};}UmiStatus umi_dr_resource_pack_add(UmiDrResourcePack*p,const UmiDrResourceManifest*i){if(p==NULL||i==NULL||!umi_dr_resource_manifest_valid(i))return UMI_STATUS_INVALID_ARGUMENT;if(p->count>=UMI_DR_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;p->items[p->count++]=*i;p->total_bytes+=i->size_bytes;return UMI_STATUS_OK;}
