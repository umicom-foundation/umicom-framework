/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_submission_status.c
 *
 * PURPOSE:
 *   Implement the test submission status behavior for
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

#include "umicom/finance/regulatory/submission_status.h"

int main(void){ CHECK(umi_reg_submission_status_transition_allowed(UMI_REG_STATE_APPROVED,UMI_REG_STATE_SUBMITTED)==1); CHECK(umi_reg_submission_status_transition_allowed(UMI_REG_STATE_ACCEPTED,UMI_REG_STATE_DRAFT)==0); return 0; }
