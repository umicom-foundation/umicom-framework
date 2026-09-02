/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/journey/runtime.h
 *
 * PURPOSE:
 *   Execute acceptance journeys sequentially and block release-critical failures without invoking product logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_JOURNEY_RUNTIME_H
#define UMICOM_APPLICATION_JOURNEY_RUNTIME_H
#include "umicom/application/journey/catalogue.h"
#include "umicom/application/journey/evidence.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the application journey runtime data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationJourneyRuntime {
    const UmiApplicationJourney *journey;
    UmiApplicationJourneyEvidenceRegistry *evidence;
    size_t next_step_index;
    size_t passed_step_count;
    size_t failed_step_count;
    UmiApplicationJourneyResult result;
    int active;
    int complete;
} UmiApplicationJourneyRuntime;
/**
 * Provide the application journey runtime begin operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_journey_runtime_begin(
    UmiApplicationJourneyRuntime *runtime,
    const UmiApplicationJourney *journey,
    UmiApplicationJourneyEvidenceRegistry *evidence);
/**
 * Provide the application journey runtime current operation used by this module and its
 * client applications.
 */
const UmiApplicationJourneyStep *umi_application_journey_runtime_current(
    const UmiApplicationJourneyRuntime *runtime);
/**
 * Provide the application journey runtime record operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_journey_runtime_record(
    UmiApplicationJourneyRuntime *runtime,
    UmiApplicationJourneyResult result,
    const char *note,
    uint64_t timestamp_ms);
#ifdef __cplusplus
}
#endif
#endif

