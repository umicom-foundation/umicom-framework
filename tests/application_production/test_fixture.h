/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/application_production/test_fixture.h
 *
 * PURPOSE:
 *   Share immutable Studio adoption and capability fixtures across focused
 *   application production tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_TEST_FIXTURE_H
#define UMICOM_APPLICATION_PRODUCTION_TEST_FIXTURE_H

#include "umicom/application/production/production.h"

/**
 * Exercise test application production studio adoption and return a clear result when the
 * behaviour no longer matches its contract.
 */
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

/**
 * Exercise test application production all available and return a clear result when the
 * behaviour no longer matches its contract.
 */
static inline int umi_test_application_production_all_available(
    const char *capability_id, void *context)
{
    (void)context;
    return capability_id != NULL && capability_id[0] != '\0';
}

/**
 * Exercise test application production none available and return a clear result when the
 * behaviour no longer matches its contract.
 */
static inline int umi_test_application_production_none_available(
    const char *capability_id, void *context)
{
    (void)capability_id;
    (void)context;
    return 0;
}

#endif
