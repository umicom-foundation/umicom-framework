/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/os_service_catalogue.h
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_OS_SERVICE_CATALOGUE_H
#define UMICOM_PLATFORM_CROSS_TARGET_OS_SERVICE_CATALOGUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/platform/cross_target/os_service_descriptor.h"
typedef struct UmiCtOsServiceCatalogue { UmiCtOsServiceDescriptor items[UMI_CT_MAX_ITEMS]; size_t count; } UmiCtOsServiceCatalogue;
void umi_ct_os_service_catalogue_init(UmiCtOsServiceCatalogue *catalogue);
UmiStatus umi_ct_os_service_catalogue_add(UmiCtOsServiceCatalogue *catalogue,const UmiCtOsServiceDescriptor *service);
const UmiCtOsServiceDescriptor *umi_ct_os_service_catalogue_find(const UmiCtOsServiceCatalogue *catalogue,const char *service_id);

#ifdef __cplusplus
}
#endif

#endif
