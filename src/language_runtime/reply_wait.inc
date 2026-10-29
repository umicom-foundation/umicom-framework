/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/reply_wait.inc
 *
 * PURPOSE:
 *   Await a source-query response with bounded correlation and elapsed-time checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "reply_deadline_private.h"

static UmiStatus QueryAwait(UmiLanguageRuntimeServer *server, uint64_t id, UmiClock *clock, uint64_t started,
                            uint32_t timeout, const UmiCancellationToken *cancel,
                            UmiLanguageRuntimeEnvelope *reply)
{
    unsigned messages = 0U;
    for (;;)
    {
        if (cancel != NULL && umi_cancellation_token_is_requested(cancel))
            return UMI_STATUS_CANCELLED;
        uint64_t elapsed = (clock->monotonic_nanoseconds(clock) - started) / 1000000U;
        /* A positive timeout belongs to the entire query, including time
         * already spent initializing. Do not renew it for a buffered reply. */
        if (UmiLanguageReplyBudgetReached(clock, started, timeout))
            return UMI_STATUS_TIMEOUT;
        uint32_t remaining = elapsed >= timeout ? 0U : timeout - (uint32_t)elapsed;
        UmiStatus status =
            umi_language_runtime_server_receive(server, remaining > 50U ? 50U : remaining, reply);
        if (status == UMI_STATUS_OK)
        {
            /* A transport may return a full frame after its requested slice.
             * Correlation must not bypass the enclosing positive deadline. */
            if (UmiLanguageReplyBudgetReached(clock, started, timeout))
                return UMI_STATUS_TIMEOUT;
            if (reply->kind == UMI_LANGUAGE_RUNTIME_MESSAGE_REQUEST)
                return UMI_STATUS_NOT_IMPLEMENTED;
            if ((reply->kind == UMI_LANGUAGE_RUNTIME_MESSAGE_RESPONSE ||
                 reply->kind == UMI_LANGUAGE_RUNTIME_MESSAGE_ERROR) &&
                reply->request_id == id)
                return UMI_STATUS_OK;
            if (++messages >= 4096U)
                return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        else if (status != UMI_STATUS_NOT_FOUND)
            return status;
        if (!umi_language_runtime_server_is_running(server))
            return UMI_STATUS_UNAVAILABLE;
        if ((clock->monotonic_nanoseconds(clock) - started) / 1000000U >= timeout)
            return UMI_STATUS_TIMEOUT;
        if (status == UMI_STATUS_NOT_FOUND)
            (void)clock->sleep_milliseconds(clock, 1U);
    }
}
