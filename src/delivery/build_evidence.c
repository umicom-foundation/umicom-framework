/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/build_evidence.c
 *
 * PURPOSE:
 *   Record reproducible build and test evidence used by release gates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Release decisions should use structured evidence instead of relying on a developer remembering whether a build looked successful.
 */

#include "umicom/delivery/build_evidence.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise build evidence from caller-provided values so later operations receive a
 * known state.
 */
void umi_build_evidence_init(UmiBuildEvidence *evidence)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (evidence != NULL) (void)memset(evidence, 0, sizeof(*evidence));
}

/*
 * Provide the build evidence passed operation used by this module and its client
 * applications.
 */
int umi_build_evidence_passed(const UmiBuildEvidence *evidence)
{
    return evidence != NULL && evidence->build_succeeded != 0 &&
           evidence->tests_failed == 0U &&
           evidence->tests_passed == evidence->tests_total;
}

/*
 * Provide the build evidence set source operation used by this module and its client
 * applications.
 */
UmiStatus umi_build_evidence_set_source(UmiBuildEvidence *evidence,
                                        const char *revision)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (evidence == NULL || revision == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_delivery_copy_text(evidence->source_revision,
                                  sizeof(evidence->source_revision), revision);
}
