/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/retention.c
 *
 * PURPOSE:
 *   Implement how many old generations and packages should be retained for rollback and audit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Retention makes cleanup predictable while preserving enough old generations for safe recovery.
 */

#include "umicom/delivery/retention.h"

UmiRetentionPolicy umi_retention_policy_default(void)
{
    UmiRetentionPolicy policy = {3U, 5U, 30U};
    return policy;
}
int umi_retention_should_keep_generation(const UmiRetentionPolicy *policy,
                                         size_t newest_index)
{
    return policy != NULL && newest_index < policy->keep_generations;
}
