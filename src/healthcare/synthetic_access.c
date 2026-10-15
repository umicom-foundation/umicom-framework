/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/healthcare/synthetic_access.c
 * PURPOSE: Keep synthetic access decisions consistent across native frontends.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/healthcare/synthetic_access.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiHealthcareAccessBook {
    UmiHealthcareAccessRecord records[UMI_HEALTHCARE_ACCESS_CAPACITY];
    size_t count;
};

/* Reject free text at the boundary. A short string is checked one byte at a
 * time so the parser never reads beyond its terminating null character. */
static int synthetic_id(const char *text, const char *prefix)
{
    size_t index;
    if (text == NULL) return 0;
    for (index = 0U; index < 4U; ++index) {
        if (text[index] == '\0' || text[index] != prefix[index]) return 0;
    }
    for (; index < 10U; ++index) {
        if (text[index] < '0' || text[index] > '9') return 0;
    }
    return text[10] == '\0';
}

/* Lookup never trusts a snapshot supplied by a frontend. Revisions below are
 * compared against this book's authoritative record before any change. */
static UmiStatus locate(const UmiHealthcareAccessBook *book,
    const char *requestId, size_t *outIndex)
{
    size_t index;
    if (book == NULL || !synthetic_id(requestId, "REQ-"))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < book->count; ++index) {
        if (strcmp(book->records[index].requestId, requestId) == 0) {
            *outIndex = index;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/* Terminal records and stale screens cannot be edited. Operations prepare a
 * value copy and publish only after all of their checks pass. */
static UmiStatus prepare(UmiHealthcareAccessBook *book, const char *requestId,
    uint64_t revision, UmiHealthcareAccessRecord *outRecord, size_t *index,
    UmiHealthcareAccessRecord *candidate)
{
    UmiStatus status;
    if (outRecord == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = locate(book, requestId, index);
    if (status != UMI_STATUS_OK) return status;
    *candidate = book->records[*index];
    if (revision != candidate->revision || revision == UINT64_MAX ||
        candidate->collectedQuantity != 0U ||
        candidate->unmetReason != UMI_HEALTHCARE_ACCESS_UNMET_NONE)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/* One revision identifies one accepted transition. A duplicate request with
 * the old revision is rejected; this is not durable idempotency or replay. */
static UmiStatus publish(UmiHealthcareAccessBook *book, size_t index,
    UmiHealthcareAccessRecord *candidate, UmiHealthcareAccessRecord *outRecord)
{
    ++candidate->revision;
    book->records[index] = *candidate;
    *outRecord = *candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiHealthcareAccessBookCreate(UmiHealthcareAccessBook **outBook)
{
    UmiHealthcareAccessBook *book;
    if (outBook == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    book = calloc(1U, sizeof(*book));
    if (book == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *outBook = book;
    return UMI_STATUS_OK;
}

void UmiHealthcareAccessBookDestroy(UmiHealthcareAccessBook *book)
{
    /* The book owns no provider handles or external resources. */
    free(book);
}

UmiStatus UmiHealthcareAccessRequest(UmiHealthcareAccessBook *book,
    const UmiHealthcareSyntheticRegistry *registry, const char *patientId,
    const char *itemId, uint32_t quantity, UmiHealthcareAccessRecord *outRecord)
{
    UmiHealthcareSyntheticPatient patient;
    UmiHealthcareAccessRecord record = {0};
    UmiStatus status;
    int written;
    if (book == NULL || registry == NULL || outRecord == NULL ||
        !synthetic_id(patientId, "SYN-") || !synthetic_id(itemId, "ITM-") ||
        quantity == 0U || quantity > UMI_HEALTHCARE_ACCESS_QUANTITY_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiHealthcareSyntheticRegistryFind(registry, patientId, &patient);
    if (status != UMI_STATUS_OK) return status;
    if (book->count >= UMI_HEALTHCARE_ACCESS_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    written = snprintf(record.requestId, sizeof(record.requestId),
        "REQ-%06u", (unsigned int)(book->count + 1U));
    if (written != 10) return UMI_STATUS_INTERNAL_ERROR;
    memcpy(record.patientId, patientId, sizeof(record.patientId));
    memcpy(record.itemId, itemId, sizeof(record.itemId));
    record.requestedQuantity = quantity;
    record.revision = 1U;
    /* Append only after validation, preserving allocation and output on error. */
    book->records[book->count++] = record;
    *outRecord = record;
    return UMI_STATUS_OK;
}

UmiStatus UmiHealthcareAccessFind(const UmiHealthcareAccessBook *book,
    const char *requestId, UmiHealthcareAccessRecord *outRecord)
{
    size_t index;
    UmiStatus status;
    if (outRecord == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = locate(book, requestId, &index);
    if (status == UMI_STATUS_OK) *outRecord = book->records[index];
    return status;
}

UmiStatus UmiHealthcareAccessAt(const UmiHealthcareAccessBook *book,
    size_t index, UmiHealthcareAccessRecord *outRecord)
{
    if (book == NULL || outRecord == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= book->count) return UMI_STATUS_NOT_FOUND;
    *outRecord = book->records[index];
    return UMI_STATUS_OK;
}

UmiStatus UmiHealthcareAccessCount(const UmiHealthcareAccessBook *book,
    size_t *outCount)
{
    if (book == NULL || outCount == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outCount = book->count;
    return UMI_STATUS_OK;
}

UmiStatus UmiHealthcareAccessReplyRecord(UmiHealthcareAccessBook *book,
    const char *requestId, uint64_t expectedRevision, const char *providerId,
    UmiHealthcareAccessReply reply, uint32_t quantity,
    UmiHealthcareAccessRecord *outRecord)
{
    UmiHealthcareAccessRecord candidate;
    size_t index;
    UmiStatus status = prepare(book, requestId, expectedRevision, outRecord,
        &index, &candidate);
    if (status != UMI_STATUS_OK) return status;
    if (!synthetic_id(providerId, "PRV-") ||
        reply < UMI_HEALTHCARE_ACCESS_FULL || reply > UMI_HEALTHCARE_ACCESS_NO_RESPONSE)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (candidate.acceptedQuantity != 0U) return UMI_STATUS_INVALID_STATE;
    /* Provider evidence must agree with the exact requested item and quantity.
     * Changing the item requires a separate request, never a substitution. */
    if ((reply == UMI_HEALTHCARE_ACCESS_FULL && quantity != candidate.requestedQuantity) ||
        (reply == UMI_HEALTHCARE_ACCESS_PARTIAL &&
         (quantity == 0U || quantity >= candidate.requestedQuantity)) ||
        (reply >= UMI_HEALTHCARE_ACCESS_UNAVAILABLE && quantity != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    memcpy(candidate.providerId, providerId, sizeof(candidate.providerId));
    candidate.reply = reply;
    candidate.offeredQuantity = quantity;
    return publish(book, index, &candidate, outRecord);
}

UmiStatus UmiHealthcareAccessAccept(UmiHealthcareAccessBook *book,
    const char *requestId, uint64_t expectedRevision, uint32_t quantity,
    UmiHealthcareAccessRecord *outRecord)
{
    UmiHealthcareAccessRecord candidate;
    size_t index;
    UmiStatus status = prepare(book, requestId, expectedRevision, outRecord,
        &index, &candidate);
    if (status != UMI_STATUS_OK) return status;
    if (candidate.acceptedQuantity != 0U || candidate.offeredQuantity == 0U)
        return UMI_STATUS_INVALID_STATE;
    if (quantity != candidate.offeredQuantity) return UMI_STATUS_INVALID_ARGUMENT;
    candidate.acceptedQuantity = quantity;
    return publish(book, index, &candidate, outRecord);
}

UmiStatus UmiHealthcareAccessSimulateFunding(UmiHealthcareAccessBook *book,
    const char *requestId, uint64_t expectedRevision,
    UmiHealthcareAccessRecord *outRecord)
{
    UmiHealthcareAccessRecord candidate;
    size_t index;
    UmiStatus status = prepare(book, requestId, expectedRevision, outRecord,
        &index, &candidate);
    if (status != UMI_STATUS_OK) return status;
    if (candidate.acceptedQuantity == 0U || candidate.simulatedFundingReady)
        return UMI_STATUS_INVALID_STATE;
    candidate.simulatedFundingReady = 1;
    return publish(book, index, &candidate, outRecord);
}

UmiStatus UmiHealthcareAccessReserve(UmiHealthcareAccessBook *book,
    const char *requestId, uint64_t expectedRevision, uint64_t now,
    uint64_t expiresAt, UmiHealthcareAccessRecord *outRecord)
{
    UmiHealthcareAccessRecord candidate;
    size_t index;
    UmiStatus status = prepare(book, requestId, expectedRevision, outRecord,
        &index, &candidate);
    if (status != UMI_STATUS_OK) return status;
    if (!candidate.simulatedFundingReady || candidate.reservationConfirmed)
        return UMI_STATUS_INVALID_STATE;
    if (now == 0U || expiresAt <= now) return UMI_STATUS_INVALID_ARGUMENT;
    candidate.reservationConfirmed = 1;
    candidate.reservedAt = now;
    candidate.expiresAt = expiresAt;
    return publish(book, index, &candidate, outRecord);
}

UmiStatus UmiHealthcareAccessCollect(UmiHealthcareAccessBook *book,
    const char *requestId, uint64_t expectedRevision, const char *patientId,
    uint64_t now, UmiHealthcareAccessRecord *outRecord)
{
    UmiHealthcareAccessRecord candidate;
    size_t index;
    UmiStatus status = prepare(book, requestId, expectedRevision, outRecord,
        &index, &candidate);
    if (status != UMI_STATUS_OK) return status;
    if (!synthetic_id(patientId, "SYN-")) return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(candidate.patientId, patientId) != 0) return UMI_STATUS_PERMISSION_DENIED;
    if (!candidate.reservationConfirmed) return UMI_STATUS_INVALID_STATE;
    if (now < candidate.reservedAt) return UMI_STATUS_INVALID_ARGUMENT;
    if (now >= candidate.expiresAt) return UMI_STATUS_TIMEOUT;
    candidate.collectedQuantity = candidate.acceptedQuantity;
    return publish(book, index, &candidate, outRecord);
}

UmiStatus UmiHealthcareAccessExpire(UmiHealthcareAccessBook *book,
    const char *requestId, uint64_t expectedRevision, uint64_t now,
    UmiHealthcareAccessRecord *outRecord)
{
    UmiHealthcareAccessRecord candidate;
    size_t index;
    UmiStatus status = prepare(book, requestId, expectedRevision, outRecord,
        &index, &candidate);
    if (status != UMI_STATUS_OK) return status;
    if (!candidate.reservationConfirmed || now < candidate.expiresAt)
        return UMI_STATUS_INVALID_STATE;
    candidate.unmetReason = UMI_HEALTHCARE_ACCESS_UNMET_EXPIRED;
    return publish(book, index, &candidate, outRecord);
}

UmiStatus UmiHealthcareAccessCloseUnmet(UmiHealthcareAccessBook *book,
    const char *requestId, uint64_t expectedRevision,
    UmiHealthcareAccessUnmetReason reason, UmiHealthcareAccessRecord *outRecord)
{
    UmiHealthcareAccessRecord candidate;
    size_t index;
    UmiStatus status = prepare(book, requestId, expectedRevision, outRecord,
        &index, &candidate);
    if (status != UMI_STATUS_OK) return status;
    if (reason < UMI_HEALTHCARE_ACCESS_UNMET_UNAVAILABLE ||
        reason > UMI_HEALTHCARE_ACCESS_UNMET_DECLINED)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* An accepted or reserved offer needs a future cancellation/refund workflow;
     * do not hide those obligations by relabelling it as an unanswered request. */
    if (candidate.acceptedQuantity != 0U ||
        (reason == UMI_HEALTHCARE_ACCESS_UNMET_UNAVAILABLE &&
         candidate.reply != UMI_HEALTHCARE_ACCESS_UNAVAILABLE) ||
        (reason == UMI_HEALTHCARE_ACCESS_UNMET_NO_RESPONSE &&
         candidate.reply != UMI_HEALTHCARE_ACCESS_NO_RESPONSE))
        return UMI_STATUS_INVALID_STATE;
    candidate.unmetReason = reason;
    return publish(book, index, &candidate, outRecord);
}

/* Administrative follow-up can be assigned after a terminal access outcome.
 * It has its own transition because reopening an offer would erase history. */
UmiStatus UmiHealthcareAccessAssignFollowup(UmiHealthcareAccessBook *book,
    const char *requestId, uint64_t expectedRevision, const char *ownerId,
    UmiHealthcareAccessRecord *outRecord)
{
    UmiHealthcareAccessRecord candidate;
    size_t index;
    UmiStatus status;
    if (outRecord == NULL || !synthetic_id(ownerId, "STF-"))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = locate(book, requestId, &index);
    if (status != UMI_STATUS_OK) return status;
    candidate = book->records[index];
    if (candidate.revision != expectedRevision || expectedRevision == UINT64_MAX)
        return UMI_STATUS_INVALID_STATE;
    if (candidate.unmetReason == UMI_HEALTHCARE_ACCESS_UNMET_NONE &&
        (candidate.collectedQuantity == 0U ||
         candidate.collectedQuantity >= candidate.requestedQuantity))
        return UMI_STATUS_INVALID_STATE;
    if (strcmp(candidate.followupOwnerId, ownerId) == 0)
        return UMI_STATUS_ALREADY_EXISTS;
    memcpy(candidate.followupOwnerId, ownerId, sizeof(candidate.followupOwnerId));
    return publish(book, index, &candidate, outRecord);
}
