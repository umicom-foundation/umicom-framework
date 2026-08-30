/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/journey/report.h
 *
 * PURPOSE:
 *   Summarise executable product journey coverage and evidence-backed release readiness.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_JOURNEY_REPORT_H
#define UMICOM_APPLICATION_JOURNEY_REPORT_H
#include "umicom/application/journey/catalogue.h"
#include "umicom/application/journey/evidence.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiApplicationJourneyReport {
    char application_id[96];
    size_t journey_count;
    size_t blocking_journey_count;
    size_t step_count;
    size_t passed_step_count;
    size_t failed_step_count;
    size_t pending_step_count;
    unsigned completion_percent;
    int release_ready;
} UmiApplicationJourneyReport;
UmiStatus umi_application_journey_report_build(
    const UmiApplicationJourneyCatalogue *catalogue,
    const UmiApplicationJourneyEvidenceRegistry *evidence,
    const char *application_id,
    UmiApplicationJourneyReport *out_report);
#ifdef __cplusplus
}
#endif
#endif

