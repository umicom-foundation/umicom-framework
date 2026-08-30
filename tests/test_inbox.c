/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_inbox.c
 *
 * PURPOSE:
 *   Verify duplicate detection and remembered consumer outcomes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>

int main(void)
{
    UmiInbox *inbox = NULL;
    UmiStatus outcome = UMI_STATUS_INTERNAL_ERROR;
    assert(umi_inbox_create(8U, &inbox) == UMI_STATUS_OK);
    assert(umi_inbox_mark_processed(inbox, 42U, UMI_STATUS_OK) == UMI_STATUS_OK);
    assert(umi_inbox_contains(inbox, 42U));
    assert(umi_inbox_outcome(inbox, 42U, &outcome) == UMI_STATUS_OK);
    assert(outcome == UMI_STATUS_OK);
    assert(umi_inbox_mark_processed(inbox, 42U, UMI_STATUS_OK) == UMI_STATUS_ALREADY_EXISTS);
    assert(umi_inbox_stats(inbox).duplicates == 1U);
    umi_inbox_destroy(inbox);
    return 0;
}
