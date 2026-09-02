/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_regulatory_snapshot.c
 *
 * PURPOSE:
 *   Implement the test regulatory snapshot behavior for
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

#include "umicom/finance/regulatory/regulatory_snapshot.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiRegulatorySnapshot s;CHECK(umi_reg_regulatory_snapshot_init(&s,4U,4U,0U,0.14,1.2)==UMI_STATUS_OK);CHECK(umi_reg_regulatory_snapshot_ready(&s,0.10,1.0)==1);return 0; }
