/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/release_candidate.c
 *
 * PURPOSE:
 *   Combine build evidence, artifacts, verification and policy into one release-candidate record.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A candidate is the object reviewed before publishing; it is separate from a release that has already been promoted.
 */

#include "umicom/delivery/release_candidate.h"
#include <string.h>
void umi_release_candidate_init(UmiReleaseCandidate *candidate)
{
    if (candidate == NULL) return;
    (void)memset(candidate, 0, sizeof(*candidate));
    umi_build_evidence_init(&candidate->build);
    umi_artifact_set_init(&candidate->artifacts);
    umi_verification_report_init(&candidate->verification);
}
int umi_release_candidate_ready(const UmiReleaseCandidate *candidate)
{
    return candidate != NULL &&
           umi_delivery_manifest_validate(&candidate->manifest) == UMI_STATUS_OK &&
           umi_build_evidence_passed(&candidate->build) &&
           umi_verification_report_passed(&candidate->verification);
}
