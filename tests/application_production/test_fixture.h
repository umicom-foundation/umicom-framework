/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/application_production/test_fixture.h
 *
 * PURPOSE:
 *   Share immutable Studio adoption and capability fixtures across focused
 *   application production tests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_TEST_FIXTURE_H
#define UMICOM_APPLICATION_PRODUCTION_TEST_FIXTURE_H

#include "umicom/application/production/production.h"

static inline UmiProductApplicationAdoption
umi_test_application_production_studio_adoption(void)
{
    const UmiProductApplicationAdoption adoption = {
        sizeof(UmiProductApplicationAdoption),
        "studio",
        "org.umicom.studio",
        "Umicom Studio IDE",
        "umicom-studio-ide",
        UMI_PRODUCT_FRONTEND_FLAG_CONSOLE | UMI_PRODUCT_FRONTEND_FLAG_GTK4,
        1, 1, 1, 1
    };
    return adoption;
}

static inline int umi_test_application_production_all_available(
    const char *capability_id, void *context)
{
    (void)context;
    return capability_id != NULL && capability_id[0] != '\0';
}

static inline int umi_test_application_production_none_available(
    const char *capability_id, void *context)
{
    (void)capability_id;
    (void)context;
    return 0;
}

#endif
