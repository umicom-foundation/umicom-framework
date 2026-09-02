/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/release_evidence.c
 *
 * PURPOSE:
 *   Aggregate checksums, signatures, licences, SBOM and provenance evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/delivery/release_evidence.h"
#include <string.h>

/*
 * Initialise release evidence from caller-provided values so later operations receive a
 * known state.
 */
void umi_release_evidence_init(UmiReleaseEvidence *evidence)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (evidence != NULL) (void)memset(evidence, 0, sizeof(*evidence));
}

/*
 * Copy release evidence into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_release_evidence_set(UmiReleaseEvidence *evidence,
                                      const char *check_id,
                                      UmiEvidenceStatus status)
{
    UmiEvidenceStatus *destination = NULL;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (evidence == NULL || check_id == NULL ||
        status < UMI_EVIDENCE_UNKNOWN || status > UMI_EVIDENCE_SKIP) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(check_id, "tests") == 0) destination = &evidence->tests;
    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(check_id, "checksums") == 0) destination = &evidence->checksums;
    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(check_id, "signatures") == 0) destination = &evidence->signatures;
    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(check_id, "licences") == 0) destination = &evidence->licence_inventory;
    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(check_id, "sbom") == 0) destination = &evidence->sbom;
    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(check_id, "provenance") == 0) destination = &evidence->provenance;
    /* Use this fallback path when the earlier condition does not apply. */
    else return UMI_STATUS_NOT_FOUND;
    *destination = status;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by release evidence blocker without changing
 * their state.
 */
size_t umi_release_evidence_blocker_count(const UmiReleaseEvidence *evidence,
                                              UmiReleaseChannel channel)
{
    size_t blockers = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (evidence == NULL) return 6U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (evidence->tests != UMI_EVIDENCE_PASS) ++blockers;
    /* Apply this branch only when its contract condition is satisfied. */
    if (evidence->checksums != UMI_EVIDENCE_PASS) ++blockers;
    /* Apply this branch only when its contract condition is satisfied. */
    if (evidence->licence_inventory != UMI_EVIDENCE_PASS) ++blockers;
    /* Apply this branch only when its contract condition is satisfied. */
    if (evidence->sbom != UMI_EVIDENCE_PASS) ++blockers;
    /* Apply this branch only when its contract condition is satisfied. */
    if (evidence->provenance != UMI_EVIDENCE_PASS) ++blockers;
    /* Apply this branch only when its contract condition is satisfied. */
    if (channel == UMI_RELEASE_STABLE) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (evidence->signatures != UMI_EVIDENCE_PASS) ++blockers;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (evidence->signatures == UMI_EVIDENCE_FAIL) {
        ++blockers;
    }
    return blockers;
}

/*
 * Provide the release evidence ready operation used by this module and its client
 * applications.
 */
int umi_release_evidence_ready(const UmiReleaseEvidence *evidence,
                                  UmiReleaseChannel channel)
{
    return umi_release_evidence_blocker_count(evidence, channel) == 0U;
}
