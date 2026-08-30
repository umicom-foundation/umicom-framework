/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/inspection_policy.c
 *
 * PURPOSE:
 *   Implement define additive inspection checks layered on top of the existing repository doctor policy.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/repository/inspection_policy.h"

#include <string.h>

/* Initialise only structural checks that extend the existing DoctorPolicy. */
void umi_repository_inspection_policy_init(
    UmiRepositoryInspectionPolicy *policy)
{
    /* A NULL output is a no-op so callers can initialise optional policy state. */
    if (policy == NULL) {
        return;
    }

    /* Start from zero so future appended structural flags default safely. */
    (void)memset(policy, 0, sizeof(*policy));
    policy->require_fetch_capability = 1;
    policy->require_gitlink_alignment = 1;
    policy->require_gitignore = 1;
    policy->require_gitattributes = 1;
}
