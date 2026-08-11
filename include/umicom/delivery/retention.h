/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/retention.h
 *
 * PURPOSE:
 *   Define how many old generations and packages should be retained for rollback and audit.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Retention makes cleanup predictable while preserving enough old generations for safe recovery.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_RETENTION_H
#define INCLUDE_UMICOM_DELIVERY_RETENTION_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiRetentionPolicy {
    size_t keep_generations;
    size_t keep_packages;
    uint32_t keep_days;
} UmiRetentionPolicy;

UmiRetentionPolicy umi_retention_policy_default(void);
int umi_retention_should_keep_generation(const UmiRetentionPolicy *policy,
                                         size_t newest_index);

#ifdef __cplusplus
}
#endif

#endif
