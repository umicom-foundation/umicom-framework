/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/healthcare/access_worklist.h
 * PURPOSE: Capture a bounded synthetic access worklist without changing clinical or reservation state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_HEALTHCARE_ACCESS_WORKLIST_H
#define UMICOM_HEALTHCARE_ACCESS_WORKLIST_H
#include <stdbool.h>
#include "umicom/healthcare/synthetic_access.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiHealthcareAccessFilter {
    UMI_HEALTHCARE_ACCESS_ALL = 0,
    UMI_HEALTHCARE_ACCESS_OUTSTANDING,
    UMI_HEALTHCARE_ACCESS_FOLLOWUP,
    UMI_HEALTHCARE_ACCESS_UNASSIGNED,
    UMI_HEALTHCARE_ACCESS_RESERVED,
    UMI_HEALTHCARE_ACCESS_OVERDUE,
    UMI_HEALTHCARE_ACCESS_COLLECTED
} UmiHealthcareAccessFilter;
typedef struct UmiHealthcareAccessWorkItem {
    UmiHealthcareAccessRecord record;
    uint32_t remainingQuantity;
    bool needsFollowup;
    bool overdueReservation;
} UmiHealthcareAccessWorkItem;
typedef struct UmiHealthcareAccessWorklist {
    UmiHealthcareAccessWorkItem items[UMI_HEALTHCARE_ACCESS_CAPACITY];
    size_t count, totalRequests, outstandingRequests, followupRequests;
    size_t unassignedFollowups, overdueReservations;
    uint64_t observedAt;
} UmiHealthcareAccessWorklist;
/* Capture insertion-ordered value copies on the book's owner thread. now is
 * positive UTC epoch seconds supplied by the caller. ownerId may be NULL/empty
 * for any coordinator, or an exact STF-###### ID; it is a filter, not authority.
 * Counts describe the complete book; items apply both the stage and owner filter.
 * Outstanding means requested quantity remains after collection. Follow-up is
 * an explicit unmet outcome or a partial collection, matching AssignFollowup.
 * An overdue reservation remains a reservation until Expire is explicitly called.
 * No clinical decision, clock service, persistence or automatic transition occurs.
 * Failure leaves *out unchanged. Copies remain valid after the book is destroyed;
 * use each record's ID and revision for checked edits, never its display index. */
UmiStatus UmiHealthcareAccessWorklistBuild(const UmiHealthcareAccessBook *book,
    UmiHealthcareAccessFilter filter, const char *ownerId, uint64_t now,
    UmiHealthcareAccessWorklist *out);
#ifdef __cplusplus
}
#endif
#endif
