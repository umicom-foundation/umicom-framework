/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/bank_operations/work_queue.h
 * PURPOSE: Capture open banking requests and bind queue reviews to their retained history.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_OPERATIONS_WORK_QUEUE_H
#define UMICOM_BANK_OPERATIONS_WORK_QUEUE_H
#include "umicom/bank_operations/review.h"
#include "umicom/base/csv_document.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_BANK_WORK_QUEUE_CAPACITY (3U * UMI_BANK_RECORD_CAPACITY)
#define UMI_BANK_QUEUE_PENDING UINT32_C(1)
#define UMI_BANK_QUEUE_APPROVED UINT32_C(2)
#define UMI_BANK_QUEUE_ALL_STATES UINT32_C(3)
typedef enum UmiBankWorkKind {
    UMI_BANK_WORK_TRANSFER=1, UMI_BANK_WORK_INTEREST=2, UMI_BANK_WORK_CHARGE=4
} UmiBankWorkKind;
#define UMI_BANK_QUEUE_ALL_KINDS UINT32_C(7)
typedef enum UmiBankWorkDecision {
    UMI_BANK_WORK_APPROVE=1, UMI_BANK_WORK_REJECT=2,
    UMI_BANK_WORK_CANCEL=3, UMI_BANK_WORK_POST=4
} UmiBankWorkDecision;
/** Both masks must be nonzero subsets of the ALL constants. Empty accountId
 * means every account. Otherwise match the source or destination exactly;
 * capture rejects an unknown account instead of showing a misleading empty list. */
typedef struct UmiBankWorkQueueFilter {
    uint32_t kinds,states;
    UmiFinancialId accountId;
} UmiBankWorkQueueFilter;
typedef struct UmiBankWorkQueueSummary {
    UmiBankWorkQueueFilter filter;
    uint64_t revision;
    size_t totalOpen,count,pending,approved;
    bool durable;
} UmiBankWorkQueueSummary;
/** Rows own copied values. Kind plus ID identifies a request: different kinds
 * may legally use the same ID. Money remains in exact minor units; never sum
 * currencies or scales together. Interest also retains its fixed principal
 * and terms; charge reason and reference are the submitted values. */
typedef struct UmiBankWorkQueueRow {
    UmiBankWorkKind kind;
    UmiBankTransferState state;
    UmiFinancialId id,sourceAccountId,destinationAccountId,referenceId,makerId,checkerId;
    UmiMoney amount,principal;
    UmiBankInterestTerms interest;
    char reason[UMI_FINANCE_NAME_CAPACITY];
    uint64_t submittedRevision;
} UmiBankWorkQueueRow;
/** An immutable, owned view of pending and approved local practice requests.
 * Capture neither reloads storage nor makes a reservation or approval. The
 * queue remains readable after service destruction. Use the service's serial
 * owner thread, including during capture and review. */
typedef struct UmiBankWorkQueue UmiBankWorkQueue;
UmiBankWorkQueueFilter UmiBankWorkQueueFilterAll(void);
const char *UmiBankWorkKindName(UmiBankWorkKind kind);
UmiStatus UmiBankWorkQueueCapture(const UmiBankOperations *operations,
    const UmiBankWorkQueueFilter *filter,UmiBankWorkQueue **outQueue);
void UmiBankWorkQueueDestroy(UmiBankWorkQueue *queue);
UmiStatus UmiBankWorkQueueSummaryRead(const UmiBankWorkQueue *queue,UmiBankWorkQueueSummary *outSummary);
UmiStatus UmiBankWorkQueueRowAt(const UmiBankWorkQueue *queue,size_t index,UmiBankWorkQueueRow *outRow);
/** Resolve a lifecycle-appropriate action, not authority to perform it. Pending
 * rows allow approve/reject/cancel; approved rows allow post/execute/cancel.
 * Current actor, account state and funds are checked by the canonical review. */
UmiStatus UmiBankWorkQueueResolveAction(const UmiBankWorkQueue *queue,size_t index,
    UmiBankWorkDecision decision,UmiBankAction *outAction);
/** Review the exact row and command without executing it. Command ID, action,
 * revision and the complete captured history must agree. Another history with
 * the same revision is rejected. Failure clears *outReview. Equivalent reloaded
 * history is allowed; repository concurrency is rechecked on reviewed execute.
 * Read this review and call UmiBankOperationsExecuteReviewed only after a
 * separate explicit user decision. Capabilities are supplied by trusted code. */
UmiStatus UmiBankWorkQueueReview(const UmiBankOperations *operations,
    const UmiBankWorkQueue *queue,size_t index,const UmiBankActor *actor,
    const UmiBankCommand *command,UmiBankReview **outReview);
/** Copy the captured view, including filters and revision, as owned CSV text.
 * No recapture or storage write occurs. Destroy with UmiCsvDocumentDestroy. */
UmiStatus UmiBankWorkQueueExportCsv(const UmiBankWorkQueue *queue,UmiCsvDocument **outDocument);
#ifdef __cplusplus
}
#endif
#endif
