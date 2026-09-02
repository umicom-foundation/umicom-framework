/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_selection.c
 *
 * PURPOSE:
 *   Verify the reusable AI coding runtime selection contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/ai_coding_runtime/selection.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{

    char selected[128];
    size_t length = 0U;
    assert(umi_ai_coding_extract_selection(
        "one\ntwo\nthree\nfour\n", 2U, 3U,
        selected, sizeof(selected), &length) == UMI_STATUS_OK);
    assert(strcmp(selected, "two\nthree\n") == 0);
    assert(length == strlen(selected));

    return 0;
}
