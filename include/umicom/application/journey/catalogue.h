/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/journey/catalogue.h
 *
 * PURPOSE:
 *   Publish Studio and Trader acceptance journeys without duplicating their canonical panel or layout definitions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_JOURNEY_CATALOGUE_H
#define UMICOM_APPLICATION_JOURNEY_CATALOGUE_H
#include "umicom/application/journey/journey.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiApplicationJourneyCatalogue {
    const UmiApplicationJourney *entries[UMI_APPLICATION_JOURNEY_CAPACITY];
    size_t count;
} UmiApplicationJourneyCatalogue;
UmiStatus umi_application_journey_catalogue_build(
    UmiApplicationJourneyCatalogue *out_catalogue);
const UmiApplicationJourney *umi_application_journey_catalogue_find(
    const UmiApplicationJourneyCatalogue *catalogue,
    const char *journey_id);
size_t umi_application_journey_catalogue_count_for(
    const UmiApplicationJourneyCatalogue *catalogue,
    const char *application_id);
#ifdef __cplusplus
}
#endif
#endif

