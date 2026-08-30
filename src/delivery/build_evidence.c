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

/* BEGINNER NOTE:
 * Release decisions should use structured evidence instead of relying on a developer remembering whether a build looked successful.
 */

#include "umicom/delivery/build_evidence.h"
#include "delivery_internal.h"
#include <string.h>

void umi_build_evidence_init(UmiBuildEvidence *evidence)
{
    if (evidence != NULL) (void)memset(evidence, 0, sizeof(*evidence));
}

int umi_build_evidence_passed(const UmiBuildEvidence *evidence)
{
    return evidence != NULL && evidence->build_succeeded != 0 &&
           evidence->tests_failed == 0U &&
           evidence->tests_passed == evidence->tests_total;
}

UmiStatus umi_build_evidence_set_source(UmiBuildEvidence *evidence,
                                        const char *revision)
{
    if (evidence == NULL || revision == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_delivery_copy_text(evidence->source_revision,
                                  sizeof(evidence->source_revision), revision);
}
