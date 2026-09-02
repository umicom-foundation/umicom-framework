/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/journey/audit.h
 *
 * PURPOSE:
 *   Audit journey layout, panel and command references against canonical Framework experience definitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_JOURNEY_AUDIT_H
#define UMICOM_APPLICATION_JOURNEY_AUDIT_H
#include "umicom/application/journey/catalogue.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the application journey audit data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationJourneyAudit {
    size_t journey_count;
    size_t step_count;
    size_t blocking_journey_count;
    size_t critical_step_count;
    size_t confirmation_step_count;
    size_t missing_application_count;
    size_t missing_layout_count;
    size_t missing_panel_count;
    size_t invalid_command_count;
    size_t invalid_journey_count;
    int valid;
} UmiApplicationJourneyAudit;
/**
 * Provide the application journey audit catalogue operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_journey_audit_catalogue(
    const UmiApplicationJourneyCatalogue *catalogue,
    UmiApplicationJourneyAudit *out_audit);
#ifdef __cplusplus
}
#endif
#endif

