/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_integration_session.c
 *
 * PURPOSE:
 *   Exercise one Suite and Inter-Application Runtime Foundation behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/session.h"
#include <string.h>

#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s\n", #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiIntegrationSession s; char id[128];
    CHECK(umi_integration_session_init(&s,"session","user")==UMI_STATUS_OK);
    CHECK(umi_integration_session_next_correlation(&s,id,sizeof(id))==UMI_STATUS_OK);
    CHECK(strcmp(id,"session-1")==0);
    return 0;
}
