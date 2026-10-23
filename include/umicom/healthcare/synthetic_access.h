/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/healthcare/synthetic_access.h
 * PURPOSE: Own a bounded, synthetic medication-access coordination exercise.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_HEALTHCARE_SYNTHETIC_ACCESS_H
#define UMICOM_HEALTHCARE_SYNTHETIC_ACCESS_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/healthcare/synthetic_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

/* These identifiers are deliberately fictional. No names, medicines, clinical
 * decisions, credentials or payment details belong in this demonstration.
 * The caller owns one book on one thread. Destroying it loses its records;
 * durable storage and authenticated provider adapters are separate work. */
#define UMI_HEALTHCARE_ACCESS_CAPACITY 32U
#define UMI_HEALTHCARE_ACCESS_ID_CAPACITY 11U
#define UMI_HEALTHCARE_ACCESS_QUANTITY_LIMIT 1000000U

typedef struct UmiHealthcareAccessBook UmiHealthcareAccessBook;

/** Distinguish availability evidence from a reservation or collection. */
typedef enum UmiHealthcareAccessReply {
    UMI_HEALTHCARE_ACCESS_NO_REPLY = 0,
    UMI_HEALTHCARE_ACCESS_FULL = 1,
    UMI_HEALTHCARE_ACCESS_PARTIAL = 2,
    UMI_HEALTHCARE_ACCESS_UNAVAILABLE = 3,
    UMI_HEALTHCARE_ACCESS_CLARIFICATION = 4,
    UMI_HEALTHCARE_ACCESS_NO_RESPONSE = 5
} UmiHealthcareAccessReply;

/** A terminal unmet request retains its reason for an owned follow-up. */
typedef enum UmiHealthcareAccessUnmetReason {
    UMI_HEALTHCARE_ACCESS_UNMET_NONE = 0,
    UMI_HEALTHCARE_ACCESS_UNMET_UNAVAILABLE = 1,
    UMI_HEALTHCARE_ACCESS_UNMET_NO_RESPONSE = 2,
    UMI_HEALTHCARE_ACCESS_UNMET_DECLINED = 3,
    UMI_HEALTHCARE_ACCESS_UNMET_EXPIRED = 4
} UmiHealthcareAccessUnmetReason;

/** Independent facts prevent an availability reply from implying fulfilment. */
typedef struct UmiHealthcareAccessRecord {
    char requestId[UMI_HEALTHCARE_ACCESS_ID_CAPACITY];
    char patientId[UMI_HEALTHCARE_ACCESS_ID_CAPACITY];
    char itemId[UMI_HEALTHCARE_ACCESS_ID_CAPACITY];
    char providerId[UMI_HEALTHCARE_ACCESS_ID_CAPACITY];
    char followupOwnerId[UMI_HEALTHCARE_ACCESS_ID_CAPACITY];
    uint64_t revision;
    uint32_t requestedQuantity;
    uint32_t offeredQuantity;
    uint32_t acceptedQuantity;
    uint32_t collectedQuantity;
    UmiHealthcareAccessReply reply;
    int simulatedFundingReady;
    int reservationConfirmed;
    uint64_t reservedAt;
    uint64_t expiresAt;
    UmiHealthcareAccessUnmetReason unmetReason;
} UmiHealthcareAccessRecord;

/** Allocate an empty book; the caller releases it with Destroy. */
UmiStatus UmiHealthcareAccessBookCreate(UmiHealthcareAccessBook **outBook);
/** Restore a complete ordered snapshot into a new independently owned book.
 * The caller supplies at most CAPACITY records and a matching registry. Each
 * state must be reachable through the existing transition rules, and each
 * stored revision must be consistent with that state. IDs and revisions are
 * retained; the snapshot is not an authenticated event history. No time-based
 * state is changed automatically. Outputs are cleared on failure; existing
 * books are never mutated. NULL records are accepted only with count zero. */
UmiStatus UmiHealthcareAccessBookRestore(
    const UmiHealthcareSyntheticRegistry *registry,
    const UmiHealthcareAccessRecord *records, size_t count,
    UmiHealthcareAccessBook **outBook);

/** Release the book after all callers and widgets stop using it. */
void UmiHealthcareAccessBookDestroy(UmiHealthcareAccessBook *book);
/** Open a request for a registered SYN-###### patient and ITM-###### item.
 * Creation assigns a REQ-###### ID. Outputs remain unchanged on failure. */
UmiStatus UmiHealthcareAccessRequest(UmiHealthcareAccessBook *book,
    const UmiHealthcareSyntheticRegistry *registry, const char *patientId,
    const char *itemId, uint32_t quantity, UmiHealthcareAccessRecord *outRecord);
/** Copy an authoritative record without transferring ownership. */
UmiStatus UmiHealthcareAccessFind(const UmiHealthcareAccessBook *book,
    const char *requestId, UmiHealthcareAccessRecord *outRecord);
/** Copy a record by insertion index for bounded worklist presentation. */
UmiStatus UmiHealthcareAccessAt(const UmiHealthcareAccessBook *book,
    size_t index, UmiHealthcareAccessRecord *outRecord);
/** Count records without exposing their storage. */
UmiStatus UmiHealthcareAccessCount(const UmiHealthcareAccessBook *book,
    size_t *outCount);
/** Record a PRV-###### provider reply before acceptance. Full means the exact
 * requested quantity; partial must be positive and smaller. Other replies
 * carry zero quantity. A reply is neither funding nor a stock reservation. */
UmiStatus UmiHealthcareAccessReplyRecord(UmiHealthcareAccessBook *book,
    const char *requestId, uint64_t expectedRevision, const char *providerId,
    UmiHealthcareAccessReply reply, uint32_t quantity,
    UmiHealthcareAccessRecord *outRecord);
/** Accept the exact displayed offer. No silent substitution or quantity change. */
UmiStatus UmiHealthcareAccessAccept(UmiHealthcareAccessBook *book,
    const char *requestId, uint64_t expectedRevision, uint32_t quantity,
    UmiHealthcareAccessRecord *outRecord);
/** Mark fictional funding after acceptance; this never moves money. */
UmiStatus UmiHealthcareAccessSimulateFunding(UmiHealthcareAccessBook *book,
    const char *requestId, uint64_t expectedRevision,
    UmiHealthcareAccessRecord *outRecord);
/** Explicitly confirm a fictional reservation using caller-supplied UTC seconds.
 * Expiry must be later than now. Availability alone never reserves stock. */
UmiStatus UmiHealthcareAccessReserve(UmiHealthcareAccessBook *book,
    const char *requestId, uint64_t expectedRevision, uint64_t now,
    uint64_t expiresAt, UmiHealthcareAccessRecord *outRecord);
/** Record simulated collection before expiry for the matching synthetic patient.
 * Matching an ID is not authentication; a future authenticated host must enforce
 * identity and delegated collection before calling a real provider adapter. */
UmiStatus UmiHealthcareAccessCollect(UmiHealthcareAccessBook *book,
    const char *requestId, uint64_t expectedRevision, const char *patientId,
    uint64_t now, UmiHealthcareAccessRecord *outRecord);
/** Expire a confirmed, uncollected reservation when now reaches its deadline. */
UmiStatus UmiHealthcareAccessExpire(UmiHealthcareAccessBook *book,
    const char *requestId, uint64_t expectedRevision, uint64_t now,
    UmiHealthcareAccessRecord *outRecord);
/** Close a pending request as unavailable, unanswered or explicitly declined.
 * Reservation expiry has its own operation so callers cannot invent a timeout. */
UmiStatus UmiHealthcareAccessCloseUnmet(UmiHealthcareAccessBook *book,
    const char *requestId, uint64_t expectedRevision,
    UmiHealthcareAccessUnmetReason reason, UmiHealthcareAccessRecord *outRecord);

/** Assign a fictional STF-###### coordinator to an unmet request or the remaining
 * need after partial collection. This records responsibility, not authentication,
 * completion of care or a recommendation about treatment. */
UmiStatus UmiHealthcareAccessAssignFollowup(UmiHealthcareAccessBook *book,
    const char *requestId, uint64_t expectedRevision, const char *ownerId,
    UmiHealthcareAccessRecord *outRecord);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_HEALTHCARE_SYNTHETIC_ACCESS_H */
