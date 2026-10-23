/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/healthcare/access_worklist.c
 * PURPOSE: Project authoritative synthetic requests into stable-ID worklists with no side effects.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/healthcare/access_worklist.h"
#include <string.h>
static bool OwnerValid(const char *owner) {
    if (owner == NULL || owner[0] == '\0') return true;
    const char prefix[] = "STF-";
    for (size_t i = 0U; i < 4U; ++i) if (owner[i] == '\0' || owner[i] != prefix[i]) return false;
    for (size_t i = 4U; i < 10U; ++i) if (owner[i] < '0' || owner[i] > '9') return false;
    return owner[10] == '\0';
}
UmiStatus UmiHealthcareAccessWorklistBuild(const UmiHealthcareAccessBook *book,
    UmiHealthcareAccessFilter filter, const char *ownerId, uint64_t now,
    UmiHealthcareAccessWorklist *out) {
    if (book == NULL || out == NULL || now == 0U ||
        (unsigned)filter > (unsigned)UMI_HEALTHCARE_ACCESS_COLLECTED || !OwnerValid(ownerId))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiHealthcareAccessWorklist candidate = {0}; candidate.observedAt = now;
    UmiStatus status = UmiHealthcareAccessCount(book, &candidate.totalRequests);
    if (status != UMI_STATUS_OK) return status;
    if (candidate.totalRequests > UMI_HEALTHCARE_ACCESS_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < candidate.totalRequests; ++i) {
        UmiHealthcareAccessWorkItem item = {0};
        status = UmiHealthcareAccessAt(book, i, &item.record);
        if (status != UMI_STATUS_OK) return status;
        const UmiHealthcareAccessRecord *record = &item.record;
        if (record->collectedQuantity > record->requestedQuantity) return UMI_STATUS_INVALID_STATE;
        item.remainingQuantity = record->requestedQuantity - record->collectedQuantity;
        item.needsFollowup = record->unmetReason != UMI_HEALTHCARE_ACCESS_UNMET_NONE ||
            (record->collectedQuantity != 0U && item.remainingQuantity != 0U);
        bool reserved = record->reservationConfirmed && record->collectedQuantity == 0U &&
            record->unmetReason == UMI_HEALTHCARE_ACCESS_UNMET_NONE;
        item.overdueReservation = reserved && now >= record->expiresAt;
        bool unassigned = item.needsFollowup && record->followupOwnerId[0] == '\0';
        if (item.remainingQuantity != 0U) ++candidate.outstandingRequests;
        if (item.needsFollowup) ++candidate.followupRequests;
        if (unassigned) ++candidate.unassignedFollowups;
        if (item.overdueReservation) ++candidate.overdueReservations;
        bool selected = filter == UMI_HEALTHCARE_ACCESS_ALL ||
            (filter == UMI_HEALTHCARE_ACCESS_OUTSTANDING && item.remainingQuantity != 0U) ||
            (filter == UMI_HEALTHCARE_ACCESS_FOLLOWUP && item.needsFollowup) ||
            (filter == UMI_HEALTHCARE_ACCESS_UNASSIGNED && unassigned) ||
            (filter == UMI_HEALTHCARE_ACCESS_RESERVED && reserved) ||
            (filter == UMI_HEALTHCARE_ACCESS_OVERDUE && item.overdueReservation) ||
            (filter == UMI_HEALTHCARE_ACCESS_COLLECTED && record->collectedQuantity != 0U);
        if (selected && (ownerId == NULL || ownerId[0] == '\0' || !strcmp(ownerId, record->followupOwnerId)))
            candidate.items[candidate.count++] = item;
    }
    /* Only a complete projection escapes. The book is deliberately untouched:
     * reading a deadline must never silently expire a reservation. */
    *out = candidate;
    return UMI_STATUS_OK;
}
