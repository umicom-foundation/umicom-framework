/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_context_binding.c
 *
 * PURPOSE:
 *   Exercise one focused application runtime or projection contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int test_context_binding(void){ UmiApplicationContextBindingStore s; umi_application_context_binding_store_init(&s); assert(umi_application_context_binding_set(&s,"trading.red","EURUSD")==UMI_STATUS_OK); assert(strcmp(umi_application_context_binding_get(&s,"trading.red"),"EURUSD")==0); return 0; }
