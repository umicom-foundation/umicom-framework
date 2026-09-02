/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/os_architecture_decision.h
 *
 * PURPOSE:
 *   Record the accepted Umicom OS production, portability and kernel-research
 *   strategy as a versioned contract that tools and applications can inspect.
 *
 * ARCHITECTURE:
 *   The kernel and recovery environment never depend on Umicom Framework.
 *   Framework performs reusable product work in normal user space, while a
 *   small separately audited freestanding library may be shared later.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_CROSS_TARGET_OS_ARCHITECTURE_DECISION_H
#define UMICOM_PLATFORM_CROSS_TARGET_OS_ARCHITECTURE_DECISION_H

#include <stdbool.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Identify the base system selected for each Umicom OS development track. */
typedef enum UmiCtOsFoundation {
    UMI_CT_OS_FOUNDATION_UNKNOWN = 0,
    UMI_CT_OS_FOUNDATION_LINUX_LTS = 1,
    UMI_CT_OS_FOUNDATION_FREEBSD = 2,
    UMI_CT_OS_FOUNDATION_UMICOM_MICROKERNEL = 3
} UmiCtOsFoundation;

/* Keep the architectural promises in data so Studio, command-line tools and
 * the OS Control Centre can show the same decision without copying prose. */
typedef struct UmiCtOsArchitectureDecision {
    uint32_t structure_size;
    uint32_t api_version;
    UmiCtOsFoundation production_foundation;
    UmiCtOsFoundation portability_foundation;
    UmiCtOsFoundation research_foundation;
    bool kernel_uses_framework;
    bool recovery_uses_framework;
    bool normal_user_space_uses_framework;
    bool freestanding_subset_allowed;
    bool separate_kernel_repository;
    bool separate_distribution_repository;
    bool research_is_product_default;
} UmiCtOsArchitectureDecision;

/* Construct the accepted strategy. The production system uses a maintained
 * upstream kernel while the original microkernel remains a separate research
 * track until it earns product-level evidence. */
UmiStatus umi_ct_umicom_os_architecture_decision_default(
    UmiCtOsArchitectureDecision *out_decision);

/* Reject changes that would move Framework into privileged kernel or recovery
 * code, merge the independent repositories, or promote research implicitly. */
UmiStatus umi_ct_umicom_os_architecture_decision_validate(
    const UmiCtOsArchitectureDecision *decision);

/* Return stable display text for a foundation without transferring ownership
 * of the returned process-lifetime string to the caller. */
const char *umi_ct_os_foundation_text(UmiCtOsFoundation foundation);

#ifdef __cplusplus
}
#endif

#endif
