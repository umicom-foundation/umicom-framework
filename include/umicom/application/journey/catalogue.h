/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/journey/catalogue.h
 *
 * PURPOSE:
 *   Publish Studio and Trader acceptance journeys without duplicating their canonical panel or layout definitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_JOURNEY_CATALOGUE_H
#define UMICOM_APPLICATION_JOURNEY_CATALOGUE_H
#include "umicom/application/journey/journey.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the application journey catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationJourneyCatalogue {
    const UmiApplicationJourney *entries[UMI_APPLICATION_JOURNEY_CAPACITY];
    size_t count;
} UmiApplicationJourneyCatalogue;
/**
 * Provide the application journey catalogue build operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_journey_catalogue_build(
    UmiApplicationJourneyCatalogue *out_catalogue);
/**
 * Find application journey catalogue while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiApplicationJourney *umi_application_journey_catalogue_find(
    const UmiApplicationJourneyCatalogue *catalogue,
    const char *journey_id);
/**
 * Provide the application journey catalogue count for operation used by this module and
 * its client applications.
 */
size_t umi_application_journey_catalogue_count_for(
    const UmiApplicationJourneyCatalogue *catalogue,
    const char *application_id);
#ifdef __cplusplus
}
#endif
#endif

