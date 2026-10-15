/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/healthcare/test_synthetic_access.c
 * PURPOSE: Exercise access state, quantity, ownership and expiry boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/healthcare/synthetic_access.h"
#include <stdio.h>
#include <string.h>
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    result = 1; goto cleanup; } } while (0)

/* Each invocation receives independent books. This deliberately avoids any
 * real provider, persistent patient data or wall-clock-dependent assertion. */
int main(int argc, char **argv)
{
    UmiHealthcareSyntheticRegistry *registry = NULL;
    UmiHealthcareAccessBook *book = NULL;
    UmiHealthcareSyntheticPatient patient;
    UmiHealthcareAccessRecord record = {0}, before = {0}, output = {0};
    size_t count = 0U, index;
    const char *testCase = argc == 2 ? argv[1] : "";
    int result = 0;
    CHECK(UmiHealthcareSyntheticRegistryCreate(&registry) == UMI_STATUS_OK);
    CHECK(UmiHealthcareSyntheticRegistryRegister(registry, "SYN-000001", &patient) == UMI_STATUS_OK);
    CHECK(UmiHealthcareAccessBookCreate(&book) == UMI_STATUS_OK);
    CHECK(UmiHealthcareAccessRequest(book, registry, "SYN-000001", "ITM-000001", 10U, &record) == UMI_STATUS_OK);
    CHECK(record.revision == 1U && record.requestedQuantity == 10U);
    CHECK(record.reply == UMI_HEALTHCARE_ACCESS_NO_REPLY && !record.reservationConfirmed);
    CHECK(UmiHealthcareAccessFind(book, record.requestId, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiHealthcareAccessSimulateFunding(book, record.requestId, record.revision, &output) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiHealthcareAccessAssignFollowup(book, record.requestId, record.revision, "STF-000001", &output) == UMI_STATUS_INVALID_STATE);

    if (strcmp(testCase, "capacity") == 0) {
        for (index = 1U; index < UMI_HEALTHCARE_ACCESS_CAPACITY; ++index) {
            CHECK(UmiHealthcareAccessRequest(book, registry, "SYN-000001", "ITM-000001", 10U, &record) == UMI_STATUS_OK);
            CHECK(UmiHealthcareAccessAt(book, index, &output) == UMI_STATUS_OK);
            CHECK(strcmp(output.requestId, record.requestId) == 0);
        }
        before = record;
        CHECK(UmiHealthcareAccessRequest(book, registry, "SYN-000001", "ITM-000001", 10U, &record) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(record.requestId, before.requestId) == 0 && record.revision == before.revision);
        CHECK(UmiHealthcareAccessCount(book, &count) == UMI_STATUS_OK && count == UMI_HEALTHCARE_ACCESS_CAPACITY);
        CHECK(UmiHealthcareAccessAt(book, count, &output) == UMI_STATUS_NOT_FOUND);
    } else if (strcmp(testCase, "invalid") == 0) {
        static const char *const invalidIds[] = {"", "I", "IT", "ITM", "ITM-", "ITM-12345", "ITM-1234567", "ITM-12345x", "medicine"};
        for (index = 0U; index < sizeof(invalidIds) / sizeof(invalidIds[0]); ++index) {
            CHECK(UmiHealthcareAccessRequest(book, registry, "SYN-000001", invalidIds[index], 10U, &output) == UMI_STATUS_INVALID_ARGUMENT);
        }
        CHECK(UmiHealthcareAccessRequest(book, registry, "SYN-999999", "ITM-000001", 10U, &output) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiHealthcareAccessRequest(book, registry, "SYN-000001", "ITM-000001", 0U, &output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiHealthcareAccessRequest(book, registry, "SYN-000001", "ITM-000001", UINT32_MAX, &output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiHealthcareAccessReplyRecord(book, record.requestId, record.revision, "PRV-000001", UMI_HEALTHCARE_ACCESS_FULL, 9U, &output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiHealthcareAccessReplyRecord(book, record.requestId, record.revision, "PRV-000001", UMI_HEALTHCARE_ACCESS_PARTIAL, 10U, &output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiHealthcareAccessReplyRecord(book, record.requestId, record.revision, "PRV-000001", UMI_HEALTHCARE_ACCESS_UNAVAILABLE, 1U, &output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiHealthcareAccessReplyRecord(book, record.requestId, record.revision, "PRV-000001", (UmiHealthcareAccessReply)99, 0U, &output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiHealthcareAccessAccept(book, record.requestId, record.revision, 10U, &output) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiHealthcareAccessReserve(book, record.requestId, record.revision, 100U, 200U, &output) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiHealthcareAccessFind(book, record.requestId, &output) == UMI_STATUS_OK && output.revision == 1U);
        CHECK(UmiHealthcareAccessCount(book, &count) == UMI_STATUS_OK && count == 1U);
    } else if (strcmp(testCase, "unavailable") == 0) {
        CHECK(UmiHealthcareAccessReplyRecord(book, record.requestId, record.revision, "PRV-000001", UMI_HEALTHCARE_ACCESS_CLARIFICATION, 0U, &record) == UMI_STATUS_OK);
        CHECK(UmiHealthcareAccessAccept(book, record.requestId, record.revision, 10U, &output) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiHealthcareAccessCloseUnmet(book, record.requestId, record.revision, UMI_HEALTHCARE_ACCESS_UNMET_UNAVAILABLE, &output) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiHealthcareAccessReplyRecord(book, record.requestId, record.revision, "PRV-000001", UMI_HEALTHCARE_ACCESS_UNAVAILABLE, 0U, &record) == UMI_STATUS_OK);
        CHECK(UmiHealthcareAccessCloseUnmet(book, record.requestId, record.revision, UMI_HEALTHCARE_ACCESS_UNMET_UNAVAILABLE, &record) == UMI_STATUS_OK);
        CHECK(UmiHealthcareAccessReplyRecord(book, record.requestId, record.revision, "PRV-000001", UMI_HEALTHCARE_ACCESS_FULL, 10U, &output) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiHealthcareAccessRequest(book, registry, "SYN-000001", "ITM-000002", 1U, &record) == UMI_STATUS_OK);
        CHECK(UmiHealthcareAccessReplyRecord(book, record.requestId, record.revision, "PRV-000001", UMI_HEALTHCARE_ACCESS_NO_RESPONSE, 0U, &record) == UMI_STATUS_OK);
        CHECK(UmiHealthcareAccessCloseUnmet(book, record.requestId, record.revision, UMI_HEALTHCARE_ACCESS_UNMET_NO_RESPONSE, &record) == UMI_STATUS_OK);
    } else if (strcmp(testCase, "lifecycle") == 0 || strcmp(testCase, "partial") == 0 ||
               strcmp(testCase, "expiry") == 0 || strcmp(testCase, "stale") == 0) {
        uint32_t offered = strcmp(testCase, "partial") == 0 ? 6U : 10U;
        UmiHealthcareAccessReply reply = offered == 10U ? UMI_HEALTHCARE_ACCESS_FULL : UMI_HEALTHCARE_ACCESS_PARTIAL;
        before = record;
        CHECK(UmiHealthcareAccessReplyRecord(book, record.requestId, record.revision, "PRV-000001", reply, offered, &record) == UMI_STATUS_OK);
        CHECK(!record.reservationConfirmed && !record.simulatedFundingReady && record.acceptedQuantity == 0U);
        if (strcmp(testCase, "stale") == 0) {
            output = before;
            CHECK(UmiHealthcareAccessAccept(book, record.requestId, before.revision, offered, &output) == UMI_STATUS_INVALID_STATE);
            CHECK(output.revision == before.revision && output.offeredQuantity == before.offeredQuantity);
            CHECK(UmiHealthcareAccessFind(book, record.requestId, &output) == UMI_STATUS_OK);
            CHECK(output.revision == record.revision && output.offeredQuantity == offered);
        }
        CHECK(UmiHealthcareAccessAccept(book, record.requestId, record.revision, offered + 1U, &output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiHealthcareAccessAccept(book, record.requestId, record.revision, offered, &record) == UMI_STATUS_OK);
        CHECK(UmiHealthcareAccessReplyRecord(book, record.requestId, record.revision, "PRV-000002", reply, offered, &output) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiHealthcareAccessReserve(book, record.requestId, record.revision, 100U, 200U, &output) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiHealthcareAccessSimulateFunding(book, record.requestId, record.revision, &record) == UMI_STATUS_OK);
        CHECK(UmiHealthcareAccessReserve(book, record.requestId, record.revision, 100U, 100U, &output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiHealthcareAccessReserve(book, record.requestId, record.revision, 100U, 200U, &record) == UMI_STATUS_OK);
        CHECK(UmiHealthcareAccessCollect(book, record.requestId, record.revision, "SYN-000002", 150U, &output) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(UmiHealthcareAccessCollect(book, record.requestId, record.revision, "SYN-000001", 99U, &output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiHealthcareAccessExpire(book, record.requestId, record.revision, 199U, &output) == UMI_STATUS_INVALID_STATE);
        if (strcmp(testCase, "expiry") == 0) {
            CHECK(UmiHealthcareAccessCollect(book, record.requestId, record.revision, "SYN-000001", 200U, &output) == UMI_STATUS_TIMEOUT);
            CHECK(UmiHealthcareAccessExpire(book, record.requestId, record.revision, 200U, &record) == UMI_STATUS_OK);
            CHECK(record.unmetReason == UMI_HEALTHCARE_ACCESS_UNMET_EXPIRED && record.collectedQuantity == 0U);
            CHECK(UmiHealthcareAccessAssignFollowup(book, record.requestId, record.revision, "STF-000001", &record) == UMI_STATUS_OK);
            CHECK(UmiHealthcareAccessCollect(book, record.requestId, record.revision, "SYN-000001", 150U, &output) == UMI_STATUS_INVALID_STATE);
        } else {
            CHECK(UmiHealthcareAccessCollect(book, record.requestId, record.revision, "SYN-000001", 199U, &record) == UMI_STATUS_OK);
            CHECK(record.collectedQuantity == offered && record.requestedQuantity == 10U);
            if (offered < record.requestedQuantity) {
                CHECK(UmiHealthcareAccessAssignFollowup(book, record.requestId, record.revision, "STF-000001", &record) == UMI_STATUS_OK);
                CHECK(strcmp(record.followupOwnerId, "STF-000001") == 0);
                CHECK(UmiHealthcareAccessAssignFollowup(book, record.requestId, record.revision, "STF-000001", &output) == UMI_STATUS_ALREADY_EXISTS);
            } else {
                CHECK(UmiHealthcareAccessAssignFollowup(book, record.requestId, record.revision, "STF-000001", &output) == UMI_STATUS_INVALID_STATE);
            }
            CHECK(UmiHealthcareAccessCollect(book, record.requestId, record.revision, "SYN-000001", 199U, &output) == UMI_STATUS_INVALID_STATE);
        }
    } else {
        CHECK(0 && "unknown test case");
    }
cleanup:
    UmiHealthcareAccessBookDestroy(book);
    UmiHealthcareSyntheticRegistryDestroy(registry);
    return result;
}
