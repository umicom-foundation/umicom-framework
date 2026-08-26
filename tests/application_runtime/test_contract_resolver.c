/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_contract_resolver.c
 *
 * PURPOSE:
 *   Exercise one focused application runtime or projection contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int test_contract_resolver(void){ UmiApplicationContract c; assert(umi_application_contract_resolve("org.umicom.trader",&c)==UMI_STATUS_OK); assert(c.definition!=NULL && c.experience!=NULL); return 0; }
