/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/healthcare/test_access_worklist.c
 * PURPOSE: Check worklist filters, revision identity, immutable snapshots and deadline boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/healthcare/access_worklist.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c); result = 1; goto cleanup; } } while (0)
#define OK(c) CHECK((c) == UMI_STATUS_OK)

/* Advance through the same checked transitions as an application. No mutable
 * record fixture can bypass domain invariants to manufacture a worklist row. */
static UmiStatus reserve(UmiHealthcareAccessBook *book, UmiHealthcareAccessRecord *r, uint32_t offer)
{
    UmiStatus s = UmiHealthcareAccessReplyRecord(book, r->requestId, r->revision, "PRV-000001",
        offer == r->requestedQuantity ? UMI_HEALTHCARE_ACCESS_FULL : UMI_HEALTHCARE_ACCESS_PARTIAL, offer, r);
    if (s == UMI_STATUS_OK) s = UmiHealthcareAccessAccept(book, r->requestId, r->revision, offer, r);
    if (s == UMI_STATUS_OK) s = UmiHealthcareAccessSimulateFunding(book, r->requestId, r->revision, r);
    if (s == UMI_STATUS_OK) s = UmiHealthcareAccessReserve(book, r->requestId, r->revision, 100U, 200U, r);
    return s;
}
int main(int argc, char **argv)
{
    const char *name = argc == 2 ? argv[1] : "";
    UmiHealthcareSyntheticRegistry *registry = NULL;
    UmiHealthcareAccessBook *book = NULL;
    UmiHealthcareSyntheticPatient patient;
    UmiHealthcareAccessRecord records[5] = {0}, observed = {0};
    UmiHealthcareAccessWorklist list = {0}, before = {0};
    int result = 0;
    OK(UmiHealthcareSyntheticRegistryCreate(&registry));
    OK(UmiHealthcareSyntheticRegistryRegister(registry, "SYN-000001", &patient));
    OK(UmiHealthcareAccessBookCreate(&book));
    if (strcmp(name, "empty") == 0) {
        OK(UmiHealthcareAccessWorklistBuild(book, UMI_HEALTHCARE_ACCESS_ALL, NULL, 150U, &list));
        CHECK(list.count == 0U && list.totalRequests == 0U && list.observedAt == 150U);
        goto cleanup;
    }
    for (size_t i = 0U; i < 5U; ++i)
        OK(UmiHealthcareAccessRequest(book, registry, "SYN-000001", "ITM-000001", 10U, &records[i]));
    OK(UmiHealthcareAccessReplyRecord(book, records[1].requestId, records[1].revision,
        "PRV-000001", UMI_HEALTHCARE_ACCESS_NO_RESPONSE, 0U, &records[1]));
    OK(UmiHealthcareAccessCloseUnmet(book, records[1].requestId, records[1].revision,
        UMI_HEALTHCARE_ACCESS_UNMET_NO_RESPONSE, &records[1]));
    OK(reserve(book, &records[2], 6U));
    OK(UmiHealthcareAccessCollect(book, records[2].requestId, records[2].revision, "SYN-000001", 150U, &records[2]));
    OK(UmiHealthcareAccessAssignFollowup(book, records[2].requestId, records[2].revision, "STF-000002", &records[2]));
    OK(reserve(book, &records[3], 10U));
    OK(UmiHealthcareAccessCollect(book, records[3].requestId, records[3].revision, "SYN-000001", 150U, &records[3]));
    OK(reserve(book, &records[4], 10U));
    OK(UmiHealthcareAccessWorklistBuild(book, UMI_HEALTHCARE_ACCESS_ALL, "", 150U, &list));

    if (strcmp(name, "filters") == 0) {
        const size_t expected[] = {5U, 4U, 2U, 1U, 1U, 0U, 2U};
        for (unsigned f = 0U; f <= (unsigned)UMI_HEALTHCARE_ACCESS_COLLECTED; ++f) {
            OK(UmiHealthcareAccessWorklistBuild(book, (UmiHealthcareAccessFilter)f, NULL, 150U, &list));
            CHECK(list.count == expected[f]);
            CHECK(list.totalRequests == 5U && list.outstandingRequests == 4U);
            CHECK(list.followupRequests == 2U && list.unassignedFollowups == 1U);
            CHECK(list.overdueReservations == 0U);
        }
    } else if (strcmp(name, "identity") == 0) {
        OK(UmiHealthcareAccessWorklistBuild(book, UMI_HEALTHCARE_ACCESS_FOLLOWUP, NULL, 150U, &list));
        CHECK(!strcmp(list.items[0].record.requestId, records[1].requestId));
        CHECK(!strcmp(list.items[1].record.requestId, records[2].requestId));
        CHECK(list.items[1].remainingQuantity == 4U);
        CHECK(list.items[0].record.revision == records[1].revision);
        OK(UmiHealthcareAccessAssignFollowup(book, list.items[0].record.requestId,
            list.items[0].record.revision, "STF-000001", &observed));
        CHECK(UmiHealthcareAccessAssignFollowup(book, list.items[0].record.requestId,
            list.items[0].record.revision, "STF-000002", &observed) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name, "owner") == 0) {
        OK(UmiHealthcareAccessWorklistBuild(book, UMI_HEALTHCARE_ACCESS_FOLLOWUP, "STF-000002", 150U, &list));
        CHECK(list.count == 1U && !strcmp(list.items[0].record.requestId, records[2].requestId));
        CHECK(list.totalRequests == 5U && list.followupRequests == 2U);
        OK(UmiHealthcareAccessWorklistBuild(book, UMI_HEALTHCARE_ACCESS_UNASSIGNED, "STF-000002", 150U, &list));
        CHECK(list.count == 0U);
        OK(UmiHealthcareAccessWorklistBuild(book, UMI_HEALTHCARE_ACCESS_ALL, "STF-999999", 150U, &list));
        CHECK(list.count == 0U);
    } else if (strcmp(name, "deadline") == 0) {
        OK(UmiHealthcareAccessWorklistBuild(book, UMI_HEALTHCARE_ACCESS_OVERDUE, NULL, 199U, &list));
        CHECK(list.count == 0U);
        OK(UmiHealthcareAccessWorklistBuild(book, UMI_HEALTHCARE_ACCESS_OVERDUE, NULL, 200U, &list));
        CHECK(list.count == 1U && list.items[0].overdueReservation && list.overdueReservations == 1U);
        OK(UmiHealthcareAccessFind(book, records[4].requestId, &observed));
        CHECK(observed.revision == records[4].revision && observed.unmetReason == UMI_HEALTHCARE_ACCESS_UNMET_NONE);
        OK(UmiHealthcareAccessExpire(book, observed.requestId, observed.revision, 200U, &observed));
        OK(UmiHealthcareAccessWorklistBuild(book, UMI_HEALTHCARE_ACCESS_UNASSIGNED, NULL, 200U, &list));
        CHECK(list.count == 2U && list.overdueReservations == 0U && list.followupRequests == 3U);
    } else if (strcmp(name, "snapshot") == 0) {
        before = list;
        UmiHealthcareAccessBookDestroy(book); book = NULL;
        CHECK(list.count == 5U && !strcmp(list.items[4].record.requestId, records[4].requestId));
        CHECK(list.items[2].remainingQuantity == 4U && list.observedAt == before.observedAt);
    } else if (strcmp(name, "invalid") == 0) {
        static const char *const owners[] = {"S", "STF-", "STF-12345", "STF-1234567", "STF-12345x", "real name"};
        before = list;
        for (size_t i = 0U; i < sizeof(owners) / sizeof(owners[0]); ++i) {
            CHECK(UmiHealthcareAccessWorklistBuild(book, UMI_HEALTHCARE_ACCESS_ALL, owners[i], 150U, &list) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(!memcmp(&list, &before, sizeof(list)));
        }
        CHECK(UmiHealthcareAccessWorklistBuild(book, (UmiHealthcareAccessFilter)-1, NULL, 150U, &list) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiHealthcareAccessWorklistBuild(book, UMI_HEALTHCARE_ACCESS_ALL, NULL, 0U, &list) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiHealthcareAccessWorklistBuild(NULL, UMI_HEALTHCARE_ACCESS_ALL, NULL, 150U, &list) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiHealthcareAccessWorklistBuild(book, UMI_HEALTHCARE_ACCESS_ALL, NULL, 150U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(&list, &before, sizeof(list)));
    } else if (strcmp(name, "capacity") == 0) {
        for (size_t i = 5U; i < UMI_HEALTHCARE_ACCESS_CAPACITY; ++i)
            OK(UmiHealthcareAccessRequest(book, registry, "SYN-000001", "ITM-000001", 10U, &observed));
        OK(UmiHealthcareAccessWorklistBuild(book, UMI_HEALTHCARE_ACCESS_ALL, NULL, 150U, &list));
        CHECK(list.count == UMI_HEALTHCARE_ACCESS_CAPACITY);
        CHECK(!strcmp(list.items[list.count - 1U].record.requestId, observed.requestId));
    } else CHECK(0 && "unknown case");
cleanup:
    UmiHealthcareAccessBookDestroy(book);
    UmiHealthcareSyntheticRegistryDestroy(registry);
    return result;
}
