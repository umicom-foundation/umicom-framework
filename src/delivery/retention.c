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

/*
 * Retention makes cleanup predictable while preserving enough old generations for safe recovery.
 */

#include "umicom/delivery/retention.h"

/*
 * Provide the retention policy default operation used by this module and its client
 * applications.
 */
UmiRetentionPolicy umi_retention_policy_default(void)
{
    UmiRetentionPolicy policy = {3U, 5U, 30U};
    return policy;
}
/*
 * Provide the retention should keep generation operation used by this module and its
 * client applications.
 */
int umi_retention_should_keep_generation(const UmiRetentionPolicy *policy,
                                         size_t newest_index)
{
    return policy != NULL && newest_index < policy->keep_generations;
}
