/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/journey/journey.h
 *
 * PURPOSE:
 *   Define executable, toolkit-neutral product acceptance journeys over canonical application experiences.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_JOURNEY_JOURNEY_H
#define UMICOM_APPLICATION_JOURNEY_JOURNEY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/application/experience.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_JOURNEY_API_VERSION 1U
#define UMI_APPLICATION_JOURNEY_CAPACITY 32U
#define UMI_APPLICATION_JOURNEY_EVIDENCE_CAPACITY 512U

/**
 * List the named application journey evidence kind values accepted by this public
 * contract.
 */
typedef enum UmiApplicationJourneyEvidenceKind {
    UMI_APPLICATION_JOURNEY_EVIDENCE_STATE = 0,
    UMI_APPLICATION_JOURNEY_EVIDENCE_COMMAND = 1,
    UMI_APPLICATION_JOURNEY_EVIDENCE_PANEL = 2,
    UMI_APPLICATION_JOURNEY_EVIDENCE_LAYOUT = 3,
    UMI_APPLICATION_JOURNEY_EVIDENCE_RECOVERY = 4,
    UMI_APPLICATION_JOURNEY_EVIDENCE_SAFETY = 5
} UmiApplicationJourneyEvidenceKind;

/**
 * List the named application journey result values accepted by this public contract.
 */
typedef enum UmiApplicationJourneyResult {
    UMI_APPLICATION_JOURNEY_PENDING = 0,
    UMI_APPLICATION_JOURNEY_PASSED = 1,
    UMI_APPLICATION_JOURNEY_FAILED = 2,
    UMI_APPLICATION_JOURNEY_BLOCKED = 3
} UmiApplicationJourneyResult;

/**
 * Represent the application journey step data shared with callers of this public contract.
 */
typedef struct UmiApplicationJourneyStep {
    uint32_t structure_size;
    const char *step_id;
    const char *title;
    const char *panel_id;
    const char *command_id;
    UmiApplicationJourneyEvidenceKind evidence_kind;
    int critical;
    int requires_confirmation;
} UmiApplicationJourneyStep;

/**
 * Represent the application journey data shared with callers of this public contract.
 */
typedef struct UmiApplicationJourney {
    uint32_t structure_size;
    uint32_t api_version;
    const char *application_id;
    const char *journey_id;
    const char *title;
    const char *layout_id;
    const UmiApplicationJourneyStep *steps;
    size_t step_count;
    int blocks_release;
} UmiApplicationJourney;

/**
 * Check that application journey satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_application_journey_validate(
    const UmiApplicationJourney *journey);
/**
 * Provide the application journey evidence kind text operation used by this module and its
 * client applications.
 */
const char *umi_application_journey_evidence_kind_text(
    UmiApplicationJourneyEvidenceKind kind);
/**
 * Provide the application journey result text operation used by this module and its client
 * applications.
 */
const char *umi_application_journey_result_text(
    UmiApplicationJourneyResult result);

#ifdef __cplusplus
}
#endif
#endif

