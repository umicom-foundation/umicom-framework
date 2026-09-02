/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_regulatory_service.c
 *
 * PURPOSE:
 *   Implement the test regulatory service behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/regulatory_service.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiRegulatoryService s;umi_reg_regulatory_service_init(&s);CHECK(umi_reg_regulatory_service_set_readiness(&s,1,1,1,1)==UMI_STATUS_OK);s.registered_authorities=1U;s.active_templates=1U;CHECK(umi_reg_regulatory_service_ready(&s)==1);return 0; }
