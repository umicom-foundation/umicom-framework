/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/broker_connectivity/test_order_observation.c
 * PURPOSE: Keep partial-fill evidence and pending cancellation distinct from terminal outcomes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/order_observation.h"
#include "umicom/broker_connectivity/ibkr_adapter.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                                  \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
static int Run(const char *mode)
{
    UmiIbkrOrderObservation out = {0}, saved;
    strcpy(out.providerStatus, "retained");
    saved = out;
    UmiDecimal filled = {0, 0U}, remaining = {7000, 0U};
    const char *text = "Submitted";
    char malformed[32];
    memset(malformed, 'x', sizeof malformed);
    UmiStatus expected = UMI_STATUS_OK;
    if (!strncmp(mode, "legacy-", 7U))
    {
        UmiOrderStatus state = UMI_ORDER_NEW;
        if (!strcmp(mode, "legacy-inactive"))
        {
            CHECK(umi_ibkr_adapter_map_status("Inactive", &state) == UMI_STATUS_UNAVAILABLE);
            CHECK(state == UMI_ORDER_NEW);
        }
        else if (!strcmp(mode, "legacy-pending"))
        {
            CHECK(umi_ibkr_adapter_map_status("PendingCancel", &state) == UMI_STATUS_UNAVAILABLE);
            CHECK(state == UMI_ORDER_NEW);
        }
        else if (!strcmp(mode, "legacy-working"))
        {
            CHECK(umi_ibkr_adapter_map_status("Submitted", &state) == UMI_STATUS_OK);
            CHECK(state == UMI_ORDER_ACCEPTED);
        }
        else if (!strcmp(mode, "legacy-unknown"))
        {
            CHECK(umi_ibkr_adapter_map_status("FutureState", &state) == UMI_STATUS_NOT_FOUND);
            CHECK(state == UMI_ORDER_NEW);
        }
        else
            return 2;
        return 0;
    }
    bool canonical = true, terminal = false, pending = false, conflict = false;
    UmiOrderStatus state = UMI_ORDER_ACCEPTED;
    UmiIbkrOrderPhase phase = UMI_IBKR_ORDER_WORKING;
    if (!strcmp(mode, "working"))
    {
    }
    else if (!strcmp(mode, "partial"))
    {
        filled = (UmiDecimal){100, 1U};
        remaining = (UmiDecimal){69900, 1U};
        state = UMI_ORDER_PARTIALLY_FILLED;
    }
    else if (!strcmp(mode, "filled") || !strcmp(mode, "filled-empty") || !strcmp(mode, "filled-remainder"))
    {
        text = "Filled";
        filled.coefficient = 7000;
        remaining.coefficient = 0;
        phase = UMI_IBKR_ORDER_FILLED;
        state = UMI_ORDER_FILLED;
        terminal = true;
        if (!strcmp(mode, "filled-empty"))
            filled.coefficient = 0;
        if (!strcmp(mode, "filled-remainder"))
            remaining.coefficient = 1;
        if (strcmp(mode, "filled"))
        {
            conflict = true;
            terminal = false;
            canonical = false;
        }
    }
    else if (!strcmp(mode, "cancelled-empty") || !strcmp(mode, "cancelled-partial") ||
             !strcmp(mode, "api-cancelled"))
    {
        text = !strcmp(mode, "api-cancelled") ? "ApiCancelled" : "Cancelled";
        phase = UMI_IBKR_ORDER_CANCELLED;
        terminal = true;
        state = UMI_ORDER_CANCELLED;
        if (!strcmp(mode, "cancelled-partial"))
        {
            filled.coefficient = 10;
            remaining.coefficient = 6990;
        }
    }
    else if (!strcmp(mode, "pending-cancel") || !strcmp(mode, "pre-cancelled"))
    {
        text = !strcmp(mode, "pending-cancel") ? "PendingCancel" : "PreCancelled";
        phase = UMI_IBKR_ORDER_PENDING_CANCEL;
        filled.coefficient = 10;
        remaining.coefficient = 6990;
        pending = true;
        canonical = false;
    }
    else if (!strcmp(mode, "inactive") || !strcmp(mode, "warning") || !strcmp(mode, "unknown"))
    {
        text =
            !strcmp(mode, "inactive") ? "Inactive" : (!strcmp(mode, "warning") ? "WarnState" : "FutureState");
        phase = !strcmp(mode, "inactive")
                    ? UMI_IBKR_ORDER_INACTIVE
                    : (!strcmp(mode, "warning") ? UMI_IBKR_ORDER_WARNING : UMI_IBKR_ORDER_UNRECOGNIZED);
        canonical = false;
    }
    else if (!strcmp(mode, "pending-submit") || !strcmp(mode, "pre-submitted"))
    {
        text = !strcmp(mode, "pending-submit") ? "PendingSubmit" : "PreSubmitted";
        phase =
            !strcmp(mode, "pending-submit") ? UMI_IBKR_ORDER_PENDING_SUBMIT : UMI_IBKR_ORDER_PRE_SUBMITTED;
        state = UMI_ORDER_VALIDATED;
    }
    else if (!strcmp(mode, "working-empty"))
    {
        remaining.coefficient = 0;
        conflict = true;
        canonical = false;
    }
    else if (!strcmp(mode, "invalid-scale"))
    {
        filled.scale = 10U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (!strcmp(mode, "negative"))
    {
        remaining.coefficient = -1;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (!strcmp(mode, "unterminated"))
    {
        text = malformed;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (!strcmp(mode, "empty"))
    {
        text = "";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (!strcmp(mode, "control"))
    {
        text = "Filled\n";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (!strcmp(mode, "alias"))
    {
        strcpy(out.providerStatus, "Submitted");
        text = out.providerStatus;
    }
    else
        return 2;
    CHECK(UmiIbkrReviewOrderObservation(text, filled, remaining, &out) == expected);
    if (expected != UMI_STATUS_OK)
    {
        CHECK(!memcmp(&out, &saved, sizeof out));
        return 0;
    }
    CHECK(out.phase == phase && out.hasCanonicalStatus == canonical && out.terminal == terminal &&
          out.cancellationPending == pending && out.conflictingQuantities == conflict);
    CHECK(out.hasFills == (filled.coefficient != 0) && out.hasRemaining == (remaining.coefficient != 0));
    CHECK(out.filled.coefficient == filled.coefficient && out.filled.scale == filled.scale &&
          out.remaining.coefficient == remaining.coefficient && out.remaining.scale == remaining.scale);
    if (canonical)
        CHECK(out.canonicalStatus == state);
    CHECK(!strcmp(out.providerStatus, text));
    return 0;
}
int main(int argc, char **argv) { return argc == 2 ? Run(argv[1]) : 2; }
