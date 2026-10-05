/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/launch_receipts.h
 * PURPOSE: Keep bounded launch and exit evidence independently of native widgets.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_LAUNCH_RECEIPTS_H
#define UMICOM_APPLICATION_LAUNCH_RECEIPTS_H
#include "umicom/application/runtime_catalogue.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_APPLICATION_LAUNCH_RECEIPT_CAPACITY 64U

    typedef enum UmiApplicationLaunchReceiptState
    {
        UMI_APPLICATION_LAUNCH_RECEIPT_PENDING,
        UMI_APPLICATION_LAUNCH_RECEIPT_RUNNING,
        UMI_APPLICATION_LAUNCH_RECEIPT_REJECTED,
        UMI_APPLICATION_LAUNCH_RECEIPT_EXITED,
        UMI_APPLICATION_LAUNCH_RECEIPT_UNCERTAIN
    } UmiApplicationLaunchReceiptState;

    /* A process being alive is not proof that its window or services are ready.
 * Receipt identities are local to their owner and never reused. No arguments,
 * environment variables, credentials or process output are captured here. */
    typedef struct UmiApplicationLaunchReceipt
    {
        uint64_t id;
        char application_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
        char executable_path[UMI_APPLICATION_RUNTIME_PATH_CAPACITY];
        UmiApplicationLaunchReceiptState state;
        UmiStatus status;
        int exit_code;
    } UmiApplicationLaunchReceipt;
    typedef struct UmiApplicationLaunchReceipts UmiApplicationLaunchReceipts;

    /* All calls belong to one owning thread. Creation performs no I/O. */
    UmiStatus UmiApplicationLaunchReceiptsCreate(UmiApplicationLaunchReceipts **out);
    void UmiApplicationLaunchReceiptsDestroy(UmiApplicationLaunchReceipts *receipts);
    /* Reserve tracking space before any process starts. Reuse the oldest completed
 * slot only; pending, running and uncertain launches cannot be evicted.
 * Standard opens refuse another active receipt for the same application.
 * explicit_new_instance permits the caller's separate New window action.
 * Text must be terminated within the public field capacities. Failure leaves
 * both the collection and output unchanged. This is evidence, not path policy. */
    UmiStatus UmiApplicationLaunchReceiptsBegin(UmiApplicationLaunchReceipts *receipts,
                                                const char *application_id, const char *executable_path,
                                                bool explicit_new_instance, uint64_t *out_id);
    UmiStatus UmiApplicationLaunchReceiptsStarted(UmiApplicationLaunchReceipts *receipts, uint64_t id);
    UmiStatus UmiApplicationLaunchReceiptsRejected(UmiApplicationLaunchReceipts *receipts, uint64_t id,
                                                   UmiStatus reason);
    /* Only an observed process exit completes a running receipt. A failed wait
 * records uncertainty instead; it must not allow an automatic duplicate. */
    UmiStatus UmiApplicationLaunchReceiptsExited(UmiApplicationLaunchReceipts *receipts, uint64_t id,
                                                 int exit_code);
    UmiStatus UmiApplicationLaunchReceiptsUncertain(UmiApplicationLaunchReceipts *receipts, uint64_t id,
                                                    UmiStatus reason);
    size_t UmiApplicationLaunchReceiptsCount(const UmiApplicationLaunchReceipts *receipts);
    UmiStatus UmiApplicationLaunchReceiptsFind(const UmiApplicationLaunchReceipts *receipts, uint64_t id,
                                               UmiApplicationLaunchReceipt *out);
    /* Newest first; indexes are view positions, never persistent identifiers. */
    UmiStatus UmiApplicationLaunchReceiptsAt(const UmiApplicationLaunchReceipts *receipts, size_t index,
                                             UmiApplicationLaunchReceipt *out);
    UmiStatus UmiApplicationLaunchReceiptsLatest(const UmiApplicationLaunchReceipts *receipts,
                                                 const char *application_id,
                                                 UmiApplicationLaunchReceipt *out);
    /* Format a receipt for a human without promising successful application startup.
 * A too-small buffer is left unchanged; no text is silently truncated. */
    UmiStatus UmiApplicationLaunchReceiptDescribe(const UmiApplicationLaunchReceipt *receipt, char *text,
                                                  size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
