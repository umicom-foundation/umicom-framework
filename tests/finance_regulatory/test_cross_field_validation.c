/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_cross_field_validation.c
 *
 * PURPOSE:
 *   Implement the test cross field validation behavior for
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

#include "umicom/finance/regulatory/cross_field_validation.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiCrossFieldValidation r; CHECK(umi_reg_cross_field_validation_init(&r,"balance",0.01)==UMI_STATUS_OK); CHECK(umi_reg_cross_field_validation_passes(&r,10.0,10.005)==1); return 0; }
