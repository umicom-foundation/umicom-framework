/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/policy.h
 *
 * PURPOSE:
 *   Define autonomy limits and mandatory approval gates for Helix.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The default policy keeps release authority with a human and makes autonomy an explicit configuration value rather than an informal promise.
 */

#ifndef INCLUDE_UMICOM_HELIX_POLICY_H
#define INCLUDE_UMICOM_HELIX_POLICY_H

#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the helix policy data shared with callers of this public contract.
 */
typedef struct UmiHelixPolicy {
    uint32_t autonomy_level;
    int allow_source_changes;
    int allow_dependency_changes;
    int allow_release;
    int require_human_approval;
    double minimum_fitness;
} UmiHelixPolicy;

/**
 * Provide the helix policy default operation used by this module and its client
 * applications.
 */
UmiHelixPolicy umi_helix_policy_default(void);
/**
 * Provide the helix policy can modify operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_policy_can_modify(const UmiHelixPolicy *policy,
                                      int dependency_change,
                                      int human_approved);
/**
 * Provide the helix policy can promote operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_policy_can_promote(const UmiHelixPolicy *policy,
                                       double fitness,
                                       int human_approved);

#ifdef __cplusplus
}
#endif

#endif
