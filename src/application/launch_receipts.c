/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/launch_receipts.c
 * PURPOSE: Reserve tracking before launch and retain honest process outcomes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/launch_receipts.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiApplicationLaunchReceipts
{
    UmiApplicationLaunchReceipt rows[UMI_APPLICATION_LAUNCH_RECEIPT_CAPACITY];
    size_t count;
    uint64_t next_id;
};

/* Read at most the destination bound, so malformed fixed arrays cannot turn
 * a diagnostic record into an unbounded string read. */
static bool bounded_text(const char *text, size_t capacity)
{
    if (text == NULL || text[0] == '\0')
        return false;
    for (size_t index = 0U; index < capacity; ++index)
        if (text[index] == '\0')
            return true;
    return false;
}

static bool active(UmiApplicationLaunchReceiptState state)
{
    return state == UMI_APPLICATION_LAUNCH_RECEIPT_PENDING ||
           state == UMI_APPLICATION_LAUNCH_RECEIPT_RUNNING ||
           state == UMI_APPLICATION_LAUNCH_RECEIPT_UNCERTAIN;
}

UmiStatus UmiApplicationLaunchReceiptsCreate(UmiApplicationLaunchReceipts **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = calloc(1U, sizeof(**out));
    if (*out == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    (*out)->next_id = 1U;
    return UMI_STATUS_OK;
}

void UmiApplicationLaunchReceiptsDestroy(UmiApplicationLaunchReceipts *receipts) { free(receipts); }

UmiStatus UmiApplicationLaunchReceiptsBegin(UmiApplicationLaunchReceipts *receipts,
                                            const char *application_id, const char *executable_path,
                                            bool explicit_new_instance, uint64_t *out_id)
{
    UmiApplicationLaunchReceipt row = {0};
    size_t slot;
    if (receipts == NULL || out_id == NULL || !bounded_text(application_id, sizeof(row.application_id)) ||
        !bounded_text(executable_path, sizeof(row.executable_path)))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (receipts->next_id == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Complete all fallible checks before changing an old receipt. Keeping
     * active rows protects child identity even when the history is full. */
    slot = receipts->count;
    for (size_t index = 0U; index < receipts->count; ++index)
    {
        const UmiApplicationLaunchReceipt *existing = &receipts->rows[index];
        if (!explicit_new_instance && active(existing->state) &&
            strcmp(existing->application_id, application_id) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
        if (!active(existing->state) && (slot == receipts->count || existing->id < receipts->rows[slot].id))
            slot = index;
    }
    if (receipts->count < UMI_APPLICATION_LAUNCH_RECEIPT_CAPACITY)
        slot = receipts->count;
    if (slot == UMI_APPLICATION_LAUNCH_RECEIPT_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    row.id = receipts->next_id++;
    memcpy(row.application_id, application_id, strlen(application_id) + 1U);
    memcpy(row.executable_path, executable_path, strlen(executable_path) + 1U);
    row.state = UMI_APPLICATION_LAUNCH_RECEIPT_PENDING;
    receipts->rows[slot] = row;
    if (slot == receipts->count)
        ++receipts->count;
    *out_id = row.id;
    return UMI_STATUS_OK;
}

/* State transitions require the exact receipt, so a late exit cannot alter a
 * newer launch for the same application. No callback or allocation runs here. */
static UmiStatus transition(UmiApplicationLaunchReceipts *receipts, uint64_t id,
                            UmiApplicationLaunchReceiptState expected, UmiApplicationLaunchReceiptState next,
                            UmiStatus status, int exit_code)
{
    if (receipts == NULL || id == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < receipts->count; ++index)
    {
        UmiApplicationLaunchReceipt *row = &receipts->rows[index];
        if (row->id != id)
            continue;
        if (row->state != expected)
            return UMI_STATUS_INVALID_STATE;
        row->state = next;
        row->status = status;
        row->exit_code = exit_code;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus UmiApplicationLaunchReceiptsStarted(UmiApplicationLaunchReceipts *receipts, uint64_t id)
{
    return transition(receipts, id, UMI_APPLICATION_LAUNCH_RECEIPT_PENDING,
                      UMI_APPLICATION_LAUNCH_RECEIPT_RUNNING, UMI_STATUS_OK, 0);
}

UmiStatus UmiApplicationLaunchReceiptsRejected(UmiApplicationLaunchReceipts *receipts, uint64_t id,
                                               UmiStatus reason)
{
    if (reason <= UMI_STATUS_OK || reason > UMI_STATUS_BUSY)
        return UMI_STATUS_INVALID_ARGUMENT;
    return transition(receipts, id, UMI_APPLICATION_LAUNCH_RECEIPT_PENDING,
                      UMI_APPLICATION_LAUNCH_RECEIPT_REJECTED, reason, 0);
}

UmiStatus UmiApplicationLaunchReceiptsExited(UmiApplicationLaunchReceipts *receipts, uint64_t id,
                                             int exit_code)
{
    return transition(receipts, id, UMI_APPLICATION_LAUNCH_RECEIPT_RUNNING,
                      UMI_APPLICATION_LAUNCH_RECEIPT_EXITED, UMI_STATUS_OK, exit_code);
}

UmiStatus UmiApplicationLaunchReceiptsUncertain(UmiApplicationLaunchReceipts *receipts, uint64_t id,
                                                UmiStatus reason)
{
    if (reason <= UMI_STATUS_OK || reason > UMI_STATUS_BUSY)
        return UMI_STATUS_INVALID_ARGUMENT;
    return transition(receipts, id, UMI_APPLICATION_LAUNCH_RECEIPT_RUNNING,
                      UMI_APPLICATION_LAUNCH_RECEIPT_UNCERTAIN, reason, 0);
}

size_t UmiApplicationLaunchReceiptsCount(const UmiApplicationLaunchReceipts *receipts)
{
    return receipts != NULL ? receipts->count : 0U;
}

UmiStatus UmiApplicationLaunchReceiptsFind(const UmiApplicationLaunchReceipts *receipts, uint64_t id,
                                           UmiApplicationLaunchReceipt *out)
{
    if (receipts == NULL || out == NULL || id == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < receipts->count; ++index)
        if (receipts->rows[index].id == id)
        {
            *out = receipts->rows[index];
            return UMI_STATUS_OK;
        }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus UmiApplicationLaunchReceiptsAt(const UmiApplicationLaunchReceipts *receipts, size_t index,
                                         UmiApplicationLaunchReceipt *out)
{
    uint64_t below = UINT64_MAX;
    if (receipts == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= receipts->count)
        return UMI_STATUS_NOT_FOUND;
    /* At most 64 rows: bounded selection avoids publishing mutable sort order
     * or moving active rows while a native completion refers to their IDs. */
    for (size_t rank = 0U; rank <= index; ++rank)
    {
        const UmiApplicationLaunchReceipt *best = NULL;
        for (size_t slot = 0U; slot < receipts->count; ++slot)
        {
            const UmiApplicationLaunchReceipt *row = &receipts->rows[slot];
            if (row->id < below && (best == NULL || row->id > best->id))
                best = row;
        }
        if (best == NULL)
            return UMI_STATUS_INTERNAL_ERROR;
        below = best->id;
        if (rank == index)
            *out = *best;
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiApplicationLaunchReceiptsLatest(const UmiApplicationLaunchReceipts *receipts,
                                             const char *application_id, UmiApplicationLaunchReceipt *out)
{
    const UmiApplicationLaunchReceipt *best = NULL;
    if (receipts == NULL || out == NULL || !bounded_text(application_id, UMI_APPLICATION_RUNTIME_ID_CAPACITY))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t slot = 0U; slot < receipts->count; ++slot)
    {
        const UmiApplicationLaunchReceipt *row = &receipts->rows[slot];
        if (strcmp(row->application_id, application_id) == 0 && (best == NULL || row->id > best->id))
            best = row;
    }
    if (best == NULL)
        return UMI_STATUS_NOT_FOUND;
    *out = *best;
    return UMI_STATUS_OK;
}

UmiStatus UmiApplicationLaunchReceiptDescribe(const UmiApplicationLaunchReceipt *receipt, char *text,
                                              size_t capacity)
{
    char value[256];
    int count;
    if (receipt == NULL || text == NULL || receipt->id == 0U || receipt->status < UMI_STATUS_OK ||
        receipt->status > UMI_STATUS_BUSY)
        return UMI_STATUS_INVALID_ARGUMENT;
    switch (receipt->state)
    {
    case UMI_APPLICATION_LAUNCH_RECEIPT_PENDING:
        count = snprintf(value, sizeof(value), "Launch reserved; the process has not started.");
        break;
    case UMI_APPLICATION_LAUNCH_RECEIPT_RUNNING:
        count =
            snprintf(value, sizeof(value), "Process started; window and service readiness are unverified.");
        break;
    case UMI_APPLICATION_LAUNCH_RECEIPT_REJECTED:
        count =
            snprintf(value, sizeof(value), "Process could not start: %s.", umi_status_text(receipt->status));
        break;
    case UMI_APPLICATION_LAUNCH_RECEIPT_EXITED:
        count = snprintf(value, sizeof(value),
                         "Process exited with code %d. This does not establish feature readiness.",
                         receipt->exit_code);
        break;
    case UMI_APPLICATION_LAUNCH_RECEIPT_UNCERTAIN:
        count = snprintf(value, sizeof(value),
                         "Process status could not be confirmed: %s. Check before opening another instance.",
                         umi_status_text(receipt->status));
        break;
    default:
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (count < 0 || (size_t)count >= sizeof(value))
        return UMI_STATUS_INTERNAL_ERROR;
    if ((size_t)count >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(text, value, (size_t)count + 1U);
    return UMI_STATUS_OK;
}
