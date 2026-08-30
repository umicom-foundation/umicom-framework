/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/target_catalogue.h
 *
 * PURPOSE:
 *   Maintain a bounded catalogue of named cross-target profiles for deterministic selection.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_TARGET_CATALOGUE_H
#define UMICOM_PLATFORM_CROSS_TARGET_TARGET_CATALOGUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCtTargetProfile UmiCtTargetProfile;
#include "umicom/platform/cross_target/target_profile.h"
typedef struct UmiCtTargetCatalogue { UmiCtTargetProfile items[UMI_CT_MAX_ITEMS]; size_t count; uint64_t revision; } UmiCtTargetCatalogue;
void umi_ct_target_catalogue_init(UmiCtTargetCatalogue *catalogue);
UmiStatus umi_ct_target_catalogue_add(UmiCtTargetCatalogue *catalogue,const UmiCtTargetProfile *profile);
const UmiCtTargetProfile *umi_ct_target_catalogue_find(const UmiCtTargetCatalogue *catalogue,const char *profile_id);

#ifdef __cplusplus
}
#endif

#endif
