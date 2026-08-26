/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_operation.c
 *
 * PURPOSE:
 *   Exercise one focused application runtime or projection contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int test_operation(void){ UmiApplicationOperationLog l; umi_application_operation_log_init(&l); assert(umi_application_operation_log_record(&l,UMI_APPLICATION_OPERATION_SESSION_START,"org.umicom.trader",UMI_STATUS_OK)==UMI_STATUS_OK); assert(umi_application_operation_log_last(&l)!=NULL); return 0; }
