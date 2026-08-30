/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_submission_history.c
 *
 * PURPOSE:
 *   Implement the test submission history behavior for
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

#include "umicom/finance/regulatory/submission_history.h"

int main(void){ UmiSubmissionHistory h; umi_reg_submission_history_init(&h); CHECK(umi_reg_submission_history_append(&h,UMI_REG_STATE_DRAFT,10)==UMI_STATUS_OK); CHECK(umi_reg_submission_history_append(&h,UMI_REG_STATE_VALIDATED,9)==UMI_STATUS_INVALID_STATE); return 0; }
