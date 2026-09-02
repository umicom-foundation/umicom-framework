/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/build_evidence.h
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

#ifndef INCLUDE_UMICOM_DELIVERY_BUILD_EVIDENCE_H
#define INCLUDE_UMICOM_DELIVERY_BUILD_EVIDENCE_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the build evidence data shared with callers of this public contract.
 */
typedef struct UmiBuildEvidence {
    char source_revision[UMI_DELIVERY_ID_CAPACITY];
    char compiler[UMI_DELIVERY_ID_CAPACITY];
    char preset[UMI_DELIVERY_ID_CAPACITY];
    uint32_t warnings;
    uint32_t tests_total;
    uint32_t tests_passed;
    uint32_t tests_failed;
    int build_succeeded;
} UmiBuildEvidence;

/**
 * Initialise build evidence from caller-provided values so later operations receive a
 * known state.
 */
void umi_build_evidence_init(UmiBuildEvidence *evidence);
/**
 * Provide the build evidence passed operation used by this module and its client
 * applications.
 */
int umi_build_evidence_passed(const UmiBuildEvidence *evidence);
/**
 * Provide the build evidence set source operation used by this module and its client
 * applications.
 */
UmiStatus umi_build_evidence_set_source(UmiBuildEvidence *evidence,
                                        const char *revision);

#ifdef __cplusplus
}
#endif

#endif
