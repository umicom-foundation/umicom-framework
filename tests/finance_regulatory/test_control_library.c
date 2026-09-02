/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_control_library.c
 *
 * PURPOSE:
 *   Implement the test control library behavior for
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

#include "umicom/finance/regulatory/control_library.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiControlLibrary l; umi_reg_control_library_init(&l); CHECK(umi_reg_control_library_add(&l,"CTRL-1")==UMI_STATUS_OK); CHECK(umi_reg_control_library_add(&l,"CTRL-1")==UMI_STATUS_ALREADY_EXISTS); return 0; }
