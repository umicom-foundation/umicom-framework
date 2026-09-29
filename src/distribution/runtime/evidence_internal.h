/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/evidence_internal.h
 *
 * PURPOSE:
 *   Private immutable release-evidence storage, shared only by the codec and evaluator.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RELEASE_EVIDENCE_INTERNAL_H
#define UMICOM_RELEASE_EVIDENCE_INTERNAL_H
#include "umicom/distribution/runtime/evidence.h"
struct UmiReleaseContract {
    char candidate[64];
    char source[41];
    size_t count;
    UmiReleaseRequirement rows[UMI_RELEASE_EVIDENCE_LIMIT];
};
typedef struct UmiReleaseObservation {
    char id[64], candidate[64], source[41], profile[64], configuration[64], kind[64];
    char outcome[16], digest[65], reference[256];
    uint64_t total, passed, failed, skipped, notRun;
} UmiReleaseObservation;
struct UmiReleaseEvidence {
    size_t count;
    UmiReleaseObservation rows[UMI_RELEASE_EVIDENCE_LIMIT];
};
#endif
