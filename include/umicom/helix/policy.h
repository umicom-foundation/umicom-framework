/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/policy.h
 *
 * PURPOSE:
 *   Define autonomy limits and mandatory approval gates for Helix.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The default policy keeps release authority with a human and makes autonomy an explicit configuration value rather than an informal promise.
 */

#ifndef INCLUDE_UMICOM_HELIX_POLICY_H
#define INCLUDE_UMICOM_HELIX_POLICY_H

#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiHelixPolicy {
    uint32_t autonomy_level;
    int allow_source_changes;
    int allow_dependency_changes;
    int allow_release;
    int require_human_approval;
    double minimum_fitness;
} UmiHelixPolicy;

UmiHelixPolicy umi_helix_policy_default(void);
UmiStatus umi_helix_policy_can_modify(const UmiHelixPolicy *policy,
                                      int dependency_change,
                                      int human_approved);
UmiStatus umi_helix_policy_can_promote(const UmiHelixPolicy *policy,
                                       double fitness,
                                       int human_approved);

#ifdef __cplusplus
}
#endif

#endif
