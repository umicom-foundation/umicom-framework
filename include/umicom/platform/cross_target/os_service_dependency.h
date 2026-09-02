/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/os_service_dependency.h
 *
 * PURPOSE:
 *   Represent service dependency edges and distinguish required versus optional boot dependencies.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_OS_SERVICE_DEPENDENCY_H
#define UMICOM_PLATFORM_CROSS_TARGET_OS_SERVICE_DEPENDENCY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ct os service dependency data shared with callers of this public contract.
 */
typedef struct UmiCtOsServiceDependency { char service_id[UMI_CT_ID_CAPACITY]; char dependency_id[UMI_CT_ID_CAPACITY]; bool required; } UmiCtOsServiceDependency;
/**
 * Initialise ct os service dependency from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ct_os_service_dependency_init(UmiCtOsServiceDependency *dependency,const char *service_id,const char *dependency_id,bool required);
/**
 * Provide the ct os service dependency self cycle operation used by this module and its
 * client applications.
 */
bool umi_ct_os_service_dependency_self_cycle(const UmiCtOsServiceDependency *dependency);

#ifdef __cplusplus
}
#endif

#endif
