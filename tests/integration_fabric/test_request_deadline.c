/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_request_deadline.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the request deadline Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/request_deadline.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFabricRequestDeadline d; CHECK(umi_fabric_request_deadline_init(&d,100U,50U)==UMI_STATUS_OK); CHECK(umi_fabric_request_deadline_remaining(&d,120U)==30U); CHECK(umi_fabric_request_deadline_expired(&d,150U));
    return 0;
}
