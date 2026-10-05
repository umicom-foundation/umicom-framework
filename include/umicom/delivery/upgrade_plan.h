/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/upgrade_plan.h
 *
 * PURPOSE:
 *   Plan product upgrades with explicit compatibility, backup and rollback.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DELIVERY_UPGRADE_PLAN_H
#define UMICOM_DELIVERY_UPGRADE_PLAN_H

#include <stdint.h>
#include "umicom/base/value_archive.h"
#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the upgrade plan data shared with callers of this public contract.
 */
typedef struct UmiUpgradePlan {
    char current_version[UMI_DELIVERY_VERSION_CAPACITY];
    char target_version[UMI_DELIVERY_VERSION_CAPACITY];
    uint64_t current_generation;
    uint64_t target_generation;
    int compatible;
    int backup_required;
    int rollback_supported;
    int authorised;
} UmiUpgradePlan;

/**
 * Initialise upgrade plan from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_upgrade_plan_init(UmiUpgradePlan *plan,
                                    const char *current_version,
                                    const char *target_version,
                                    uint64_t current_generation,
                                    uint64_t target_generation,
                                    int compatible);
/**
 * Provide the upgrade plan authorise operation used by this module and its client
 * applications.
 */
UmiStatus umi_upgrade_plan_authorise(UmiUpgradePlan *plan,
                                         int backup_available);
/**
 * Check that upgrade plan satisfies its contract before another service relies on it.
 */
UmiStatus umi_upgrade_plan_validate(const UmiUpgradePlan *plan);
/**
 * Provide the upgrade plan rollback generation operation used by this module and its
 * client applications.
 */
uint64_t umi_upgrade_plan_rollback_generation(const UmiUpgradePlan *plan);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_upgrade_plan_archive_encode(const UmiUpgradePlan *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_upgrade_plan_archive_decode(const void *bytes, size_t byte_count,
    UmiUpgradePlan *value);

#ifdef __cplusplus
}
#endif
#endif
