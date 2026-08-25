/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/os_service_descriptor.h
 *
 * PURPOSE:
 *   Define Framework-owned operating-system service descriptors and declared service dependencies.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_OS_SERVICE_DESCRIPTOR_H
#define UMICOM_PLATFORM_CROSS_TARGET_OS_SERVICE_DESCRIPTOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiCtOsServiceKind { UMI_CT_OS_SERVICE_KERNEL=1, UMI_CT_OS_SERVICE_DRIVER=2, UMI_CT_OS_SERVICE_FILESYSTEM=3, UMI_CT_OS_SERVICE_NETWORK=4, UMI_CT_OS_SERVICE_SECURITY=5, UMI_CT_OS_SERVICE_USER=6 } UmiCtOsServiceKind;
typedef struct UmiCtOsServiceDescriptor { char service_id[UMI_CT_ID_CAPACITY]; UmiCtOsServiceKind kind; char dependencies[UMI_CT_MAX_DEPENDENCIES][UMI_CT_ID_CAPACITY]; size_t dependency_count; bool essential; } UmiCtOsServiceDescriptor;
UmiStatus umi_ct_os_service_descriptor_init(UmiCtOsServiceDescriptor *service,const char *service_id,UmiCtOsServiceKind kind,bool essential);
UmiStatus umi_ct_os_service_descriptor_add_dependency(UmiCtOsServiceDescriptor *service,const char *dependency_id);

#ifdef __cplusplus
}
#endif

#endif
