/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/kernel_boundary.h
 *
 * PURPOSE:
 *   Define explicit user/kernel/hypervisor ownership boundaries for Umicom OS reusable services.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_KERNEL_BOUNDARY_H
#define UMICOM_PLATFORM_CROSS_TARGET_KERNEL_BOUNDARY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiCtBoundaryDomain { UMI_CT_DOMAIN_USER=1, UMI_CT_DOMAIN_KERNEL=2, UMI_CT_DOMAIN_HYPERVISOR=3, UMI_CT_DOMAIN_FIRMWARE=4 } UmiCtBoundaryDomain;
typedef struct UmiCtKernelBoundary { char boundary_id[UMI_CT_ID_CAPACITY]; UmiCtBoundaryDomain caller; UmiCtBoundaryDomain callee; bool copy_in; bool copy_out; bool privileged; } UmiCtKernelBoundary;
UmiStatus umi_ct_kernel_boundary_validate(const UmiCtKernelBoundary *boundary);

#ifdef __cplusplus
}
#endif

#endif
