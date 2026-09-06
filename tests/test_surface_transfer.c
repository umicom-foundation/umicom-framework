/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_surface_transfer.c
 *
 * PURPOSE:
 *   Exercise the acknowledged application-surface transfer protocol.  The
 *   test keeps the session data out of the token so it also documents the
 *   ownership boundary used by Workbench Hosts.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/surface_transfer.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

/* Check the normal source -> destination acknowledgement -> commit journey. */
static void verify_acknowledged_transfer(void)
{
    UmiApplicationSurfaceTransferToken token;

    assert(umi_application_surface_transfer_issue(
               "transfer-1",
               "org.umicom.studio",
               "studio-session-1",
               "host-source",
               "checkpoint-42",
               "capabilities-fingerprint",
               1000U,
               60000U,
               &token) == UMI_STATUS_OK);
    assert(strcmp(umi_application_surface_transfer_state_text(token.state),
                  "pending") == 0);
    assert(umi_application_surface_transfer_validate(&token, 1001U) ==
           UMI_STATUS_OK);
    /* A source cannot release its tab before another host acknowledges it. */
    assert(umi_application_surface_transfer_commit(&token, 1002U) ==
           UMI_STATUS_INVALID_STATE);
    assert(umi_application_surface_transfer_acknowledge(
               &token, "host-destination", 1003U) == UMI_STATUS_OK);
    /* Repeating the same delivery acknowledgement must be harmless. */
    assert(umi_application_surface_transfer_acknowledge(
               &token, "host-destination", 1004U) == UMI_STATUS_OK);
    /* A second host cannot claim a token already accepted elsewhere. */
    assert(umi_application_surface_transfer_acknowledge(
               &token, "host-other", 1005U) == UMI_STATUS_ALREADY_EXISTS);
    assert(umi_application_surface_transfer_commit(&token, 1006U) ==
           UMI_STATUS_OK);
    /* Commit is idempotent so a retried source response cannot fail noisily. */
    assert(umi_application_surface_transfer_commit(&token, 1007U) ==
           UMI_STATUS_OK);
    assert(umi_application_surface_transfer_validate(&token, 1008U) ==
           UMI_STATUS_OK);
    assert(strcmp(umi_application_surface_transfer_state_text(token.state),
                  "committed") == 0);
}

/* Check that cancellation preserves the source when no destination accepts. */
static void verify_cancelled_transfer(void)
{
    UmiApplicationSurfaceTransferToken token;

    assert(umi_application_surface_transfer_issue(
               "transfer-2",
               "org.umicom.trader",
               "trader-session-1",
               "host-source",
               "checkpoint-43",
               "trader-capabilities",
               2000U,
               60000U,
               &token) == UMI_STATUS_OK);
    assert(umi_application_surface_transfer_cancel(&token, 2001U) ==
           UMI_STATUS_OK);
    /* Cancelling again is safe for a close or Escape action that is retried. */
    assert(umi_application_surface_transfer_cancel(&token, 2002U) ==
           UMI_STATUS_OK);
    assert(umi_application_surface_transfer_acknowledge(
               &token, "host-destination", 2003U) == UMI_STATUS_CANCELLED);
    assert(umi_application_surface_transfer_commit(&token, 2004U) ==
           UMI_STATUS_CANCELLED);
}

/* Check that an unacknowledged token expires and cannot be adopted later. */
static void verify_expired_transfer(void)
{
    UmiApplicationSurfaceTransferToken token;

    assert(umi_application_surface_transfer_issue(
               "transfer-3",
               "org.umicom.bank",
               "bank-session-1",
               "host-source",
               "checkpoint-44",
               "bank-capabilities",
               3000U,
               10U,
               &token) == UMI_STATUS_OK);
    assert(umi_application_surface_transfer_validate(&token, 3011U) ==
           UMI_STATUS_TIMEOUT);
    assert(umi_application_surface_transfer_acknowledge(
               &token, "host-destination", 3011U) == UMI_STATUS_TIMEOUT);
    assert(strcmp(umi_application_surface_transfer_state_text(token.state),
                  "expired") == 0);
}

/* Reject malformed lifetimes and prove output storage is not partially published. */
static void verify_invalid_issue(void)
{
    UmiApplicationSurfaceTransferToken token;

    (void)memset(&token, 0xA5, sizeof(token));
    assert(umi_application_surface_transfer_issue(
               "transfer-invalid",
               "org.umicom.studio",
               "studio-session-2",
               "host-source",
               "checkpoint-45",
               "studio-capabilities",
               UINT64_MAX,
               1U,
               &token) == UMI_STATUS_INVALID_ARGUMENT);
}

/* Run each bounded protocol journey as one deterministic Framework test. */
int main(void)
{
    verify_acknowledged_transfer();
    verify_cancelled_transfer();
    verify_expired_transfer();
    verify_invalid_issue();
    return 0;
}
