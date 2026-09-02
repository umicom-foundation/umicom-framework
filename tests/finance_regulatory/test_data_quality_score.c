/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_data_quality_score.c
 *
 * PURPOSE:
 *   Implement the test data quality score behavior for
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

#include "umicom/finance/regulatory/data_quality_score.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ double s[2]={1.0,0.5},w[2]={1.0,1.0},o=0.0; CHECK(umi_reg_data_quality_score_compute(s,w,2U,&o)==UMI_STATUS_OK); CHECK(o>0.74&&o<0.76); return 0; }
