/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_runtime_catalogue.c
 *
 * PURPOSE:
 *   Exercise one focused application runtime or projection contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int test_runtime_catalogue(void){ UmiApplicationRuntimeCatalogueStatus s; assert(umi_application_runtime_catalogue_status(&s)==UMI_STATUS_OK); assert(s.aligned_count>0U); return 0; }
