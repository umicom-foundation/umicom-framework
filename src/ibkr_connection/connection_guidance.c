/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/connection_guidance.c
 *
 * PURPOSE:
 *   Share actionable connection and account-report guidance across broker clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/connection_guidance.h"

UmiStatus UmiIbkrConnectionExplain(const UmiIbkrConnectionSnapshot *s,
    UmiIbkrConnectionGuidance *out)
{
    if (s == NULL || out == NULL || s->state < UMI_IBKR_IDLE ||
        s->state > UMI_IBKR_FAILED) return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrConnectionGuidance result = {UMI_IBKR_STEP_OPEN,
        "Open a local broker session",
        "Log in to TWS or IB Gateway on this computer, finish authentication there, "
        "enable the socket API and match its port before choosing Connect."};
    if (s->state == UMI_IBKR_FAILED) {
        /* Recognised provider messages refine advice only after failure. Farm
         * notifications and an old provider code must not replace a healthy
         * session's next step. Preserve the original provider text separately. */
        if (s->providerCode == 326) {
            result = (UmiIbkrConnectionGuidance){UMI_IBKR_STEP_CHANGE_CLIENT,
                "Choose an unused client ID",
                "Another API client is using this ID. Disconnect, choose a different "
                "positive client ID, then reconnect. Do not disconnect another application automatically."};
        } else if (s->providerCode == 502) {
            result = (UmiIbkrConnectionGuidance){UMI_IBKR_STEP_CHECK_ENDPOINT,
                "Check the local API endpoint",
                "Confirm TWS or IB Gateway is running, its socket API is enabled, and "
                "the port matches this profile. Review local connection permissions if those settings agree."};
        } else if (s->providerCode != 0) {
            result = (UmiIbkrConnectionGuidance){UMI_IBKR_STEP_REVIEW_PROVIDER,
                "Review the broker diagnostic",
                "Read the provider code and message above. Resolve the reported condition in "
                "the broker application before reconnecting; retained observations remain stale."};
        } else if (s->lastStatus == UMI_STATUS_PARSE_ERROR ||
                   s->lastStatus == UMI_STATUS_CAPACITY_EXCEEDED) {
            result = (UmiIbkrConnectionGuidance){UMI_IBKR_STEP_REVIEW_PROTOCOL,
                "Review the API response",
                "The response could not be accepted within this adapter's protocol or size limits. "
                "Record the broker/API version and diagnostic for investigation; do not treat partial data as complete."};
        } else if (s->protocolVersion == 0 && s->framesReceived == 0U) {
            result = (UmiIbkrConnectionGuidance){UMI_IBKR_STEP_CHECK_ENDPOINT,
                "Check the local API endpoint",
                "Confirm the broker application is logged in, the socket API is enabled and "
                "the selected port matches. A local connection failure does not by itself identify the cause."};
        } else {
            result = (UmiIbkrConnectionGuidance){UMI_IBKR_STEP_RECONNECT,
                "Review and reconnect",
                "Check the diagnostic and the broker application's connection status. Reconnect "
                "after resolving the cause, then explicitly request a new account report."};
        }
    } else if (s->state == UMI_IBKR_DISCONNECTED ||
               (s->state == UMI_IBKR_READY && s->stale)) {
        result = (UmiIbkrConnectionGuidance){UMI_IBKR_STEP_RECONNECT,
            "Reconnect for a fresh report",
            "These observations belong to the previous connection. Reconnect and read the selected "
            "account again before using them as a new report."};
    } else if (s->state == UMI_IBKR_CONNECTING || s->state == UMI_IBKR_HANDSHAKE ||
               s->state == UMI_IBKR_WAITING) {
        result = (UmiIbkrConnectionGuidance){UMI_IBKR_STEP_WAIT,
            "Wait for API readiness",
            "A socket alone is not a ready broker session. Wait for protocol negotiation, "
            "managed accounts, API readiness and the clock response."};
    } else if (s->state == UMI_IBKR_READY && !s->requestIssued) {
        result = (UmiIbkrConnectionGuidance){UMI_IBKR_STEP_READ_ACCOUNT,
            "Select and read an account",
            "Choose an account reported by this connection and select Read account. "
            "No account report is requested automatically."};
    } else if (s->state == UMI_IBKR_READY && (!s->summaryComplete || !s->positionsComplete)) {
        result = (UmiIbkrConnectionGuidance){UMI_IBKR_STEP_WAIT_REPORT,
            "Wait for both report streams",
            "Account values and positions complete independently. Rows may be partial until "
            "both end markers arrive; an empty unfinished table is not evidence of a zero balance or position."};
    } else if (s->state == UMI_IBKR_READY) {
        result = (UmiIbkrConnectionGuidance){UMI_IBKR_STEP_REVIEW_REPORT,
            "Review the completed observation",
            "Both report streams completed. This is a point-in-time observation, not a continuously "
            /* Account refresh now owns request generations on this connection.
             * Retain the former reconnect-only advice for compatibility review. */
#if 0
            "updated balance. Reconnect to request another report and verify the account mode in the broker application."
#else
            "updated balance. Choose Refresh account to request another report on this connection, "
            "and verify the account mode in the broker application."
#endif
        };
    }
    *out = result;
    return UMI_STATUS_OK;
}
