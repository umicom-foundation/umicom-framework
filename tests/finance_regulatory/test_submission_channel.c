/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_submission_channel.c
 *
 * PURPOSE:
 *   Implement the test submission channel behavior for
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

#include "umicom/finance/regulatory/submission_channel.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSubmissionChannel record;
    CHECK(umi_reg_submission_channel_init(&record, "channel_id-1", "authority_id-1", "Protocol", 1) == UMI_STATUS_OK);
    CHECK(record.channel_id[0] != '\0');
    CHECK(record.authority_id[0] != '\0');
    CHECK(record.protocol[0] != '\0');
    return 0;
}
