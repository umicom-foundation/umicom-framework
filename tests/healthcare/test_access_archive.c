/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/healthcare/test_access_archive.c
 * PURPOSE: Cover strict archive input, state reachability and revision-preserving reopen.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/healthcare/access_archive.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c); result = 1; goto cleanup; } } while (0)
#define OK(c) CHECK((c) == UMI_STATUS_OK)

static char *replace_text(const char *text, const char *before, const char *after)
{
    const char *found = strstr(text, before);
    if (found == NULL) return NULL;
    size_t offset = (size_t)(found - text), a = strlen(after), b = strlen(before);
    char *copy = malloc(strlen(text) - b + a + 1U);
    if (copy == NULL) return NULL;
    memcpy(copy, text, offset);
    memcpy(copy + offset, after, a);
    strcpy(copy + offset + a, found + b);
    return copy;
}

/* Each invalid last row must withhold both handles, not expose a usable prefix. */
static int rejects(const char *text, size_t length)
{
    UmiHealthcareSyntheticRegistry *registry = NULL;
    UmiHealthcareAccessBook *book = NULL;
    UmiStatus status = UmiHealthcareAccessArchiveDecode(text, length, &registry, &book);
    int rejected = status != UMI_STATUS_OK && registry == NULL && book == NULL;
    UmiHealthcareAccessBookDestroy(book);
    UmiHealthcareSyntheticRegistryDestroy(registry);
    return rejected;
}

static int roundtrip(const UmiHealthcareSyntheticRegistry *registry,
    const UmiHealthcareAccessBook *book)
{
    UmiHealthcareSyntheticRegistry *restoredRegistry = NULL;
    UmiHealthcareAccessBook *restoredBook = NULL;
    char *before = NULL, *after = NULL;
    size_t beforeLength = 0U, afterLength = 0U;
    int result = 0;
    OK(UmiHealthcareAccessArchiveEncode(registry, book, &before, &beforeLength));
    CHECK(before[beforeLength] == '\0');
    OK(UmiHealthcareAccessArchiveDecode(before, beforeLength, &restoredRegistry, &restoredBook));
    OK(UmiHealthcareAccessArchiveEncode(restoredRegistry, restoredBook, &after, &afterLength));
    CHECK(beforeLength == afterLength && memcmp(before, after, beforeLength) == 0);
cleanup:
    UmiHealthcareAccessArchiveFree(before); UmiHealthcareAccessArchiveFree(after);
    UmiHealthcareAccessBookDestroy(restoredBook);
    UmiHealthcareSyntheticRegistryDestroy(restoredRegistry);
    return result;
}

int main(int argc, char **argv)
{
    const char *name = argc == 2 ? argv[1] : "";
    UmiHealthcareSyntheticRegistry *registry = NULL, *decoded = NULL;
    UmiHealthcareAccessBook *book = NULL, *restored = NULL;
    UmiHealthcareAccessRecord record = {0}, records[2] = {0}, output = {0};
    UmiHealthcareSyntheticPatient patient = {0};
    char *bytes = NULL, *changed = NULL;
    size_t length = 0U, count = 0U;
    int result = 0;
    OK(UmiHealthcareSyntheticRegistryCreate(&registry));
    OK(UmiHealthcareAccessBookCreate(&book));
    if (!strcmp(name, "empty")) {
        CHECK(roundtrip(registry, book) == 0);
        OK(UmiHealthcareAccessBookRestore(registry, NULL, 0U, &restored));
        OK(UmiHealthcareAccessCount(restored, &count)); CHECK(count == 0U);
        UmiHealthcareAccessBookDestroy(restored); restored = NULL;
        CHECK(UmiHealthcareAccessBookRestore(registry, NULL, 1U, &restored) != UMI_STATUS_OK);
        CHECK(restored == NULL);
    } else if (!strcmp(name, "identities")) {
        OK(UmiHealthcareSyntheticRegistryRegister(registry, "SYN-000007", &patient));
        OK(UmiHealthcareSyntheticRegistryRegister(registry, "SYN-000001", &patient));
        OK(UmiHealthcareAccessArchiveEncode(registry, book, &bytes, &length));
        OK(UmiHealthcareAccessArchiveDecode(bytes, length, &decoded, &restored));
        OK(UmiHealthcareSyntheticRegistryCount(decoded, &count)); CHECK(count == 2U);
        OK(UmiHealthcareSyntheticRegistryAt(decoded, 0U, &patient));
        CHECK(!strcmp(patient.patientId, "SYN-000007") && patient.revision == 1U);
        OK(UmiHealthcareSyntheticRegistryRegister(decoded, "SYN-000002", &patient)); CHECK(patient.revision == 3U);
        patient.revision = 123U;
        CHECK(UmiHealthcareSyntheticRegistryAt(decoded, 99U, &patient) == UMI_STATUS_NOT_FOUND);
        CHECK(patient.revision == 123U);
    } else {
        OK(UmiHealthcareSyntheticRegistryRegister(registry, "SYN-000001", &patient));
        OK(UmiHealthcareAccessRequest(book, registry, patient.patientId, "ITM-000001", 10U, &record));
        if (!strcmp(name, "lifecycle") || !strcmp(name, "partial") || !strcmp(name, "expiry")) {
            CHECK(roundtrip(registry, book) == 0);
            bool partial = !strcmp(name, "partial");
            OK(UmiHealthcareAccessReplyRecord(book, record.requestId, record.revision, "PRV-000002",
                partial ? UMI_HEALTHCARE_ACCESS_PARTIAL : UMI_HEALTHCARE_ACCESS_FULL, partial ? 4U : 10U, &record));
            CHECK(roundtrip(registry, book) == 0);
            OK(UmiHealthcareAccessAccept(book, record.requestId, record.revision, record.offeredQuantity, &record));
            CHECK(roundtrip(registry, book) == 0);
            OK(UmiHealthcareAccessSimulateFunding(book, record.requestId, record.revision, &record));
            CHECK(roundtrip(registry, book) == 0);
            OK(UmiHealthcareAccessReserve(book, record.requestId, record.revision, 100U, 200U, &record));
            CHECK(roundtrip(registry, book) == 0);
            if (!strcmp(name, "expiry"))
                OK(UmiHealthcareAccessExpire(book, record.requestId, record.revision, 200U, &record));
            else OK(UmiHealthcareAccessCollect(book, record.requestId, record.revision, record.patientId, 150U, &record));
            if (partial || !strcmp(name, "expiry"))
                OK(UmiHealthcareAccessAssignFollowup(book, record.requestId, record.revision, "STF-000008", &record));
            CHECK(roundtrip(registry, book) == 0);
            if (!strcmp(name, "expiry")) {
                /* No float or signed coercion may truncate large UTC values. */
                record.reservedAt = UINT64_MAX - 1U;
                record.expiresAt = UINT64_MAX;
                OK(UmiHealthcareAccessBookRestore(registry, &record, 1U, &restored));
                CHECK(roundtrip(registry, restored) == 0);
            }
        } else if (!strcmp(name, "followup")) {
            OK(UmiHealthcareAccessCloseUnmet(book, record.requestId, record.revision,
                UMI_HEALTHCARE_ACCESS_UNMET_DECLINED, &record));
            CHECK(roundtrip(registry, book) == 0);
            OK(UmiHealthcareAccessAssignFollowup(book, record.requestId, record.revision, "STF-000002", &record));
            OK(UmiHealthcareAccessAssignFollowup(book, record.requestId, record.revision, "STF-000003", &record));
            CHECK(roundtrip(registry, book) == 0);
        } else if (!strcmp(name, "revision")) {
            OK(UmiHealthcareAccessReplyRecord(book, record.requestId, record.revision,
                "PRV-000001", UMI_HEALTHCARE_ACCESS_FULL, 10U, &record));
            OK(UmiHealthcareAccessReplyRecord(book, record.requestId, record.revision,
                "PRV-000002", UMI_HEALTHCARE_ACCESS_FULL, 10U, &record));
            OK(UmiHealthcareAccessArchiveEncode(registry, book, &bytes, &length));
            OK(UmiHealthcareAccessArchiveDecode(bytes, length, &decoded, &restored));
            CHECK(UmiHealthcareAccessAccept(restored, record.requestId, record.revision - 1U, 10U, &output) == UMI_STATUS_INVALID_STATE);
            OK(UmiHealthcareAccessAccept(restored, record.requestId, record.revision, 10U, &output));
            CHECK(output.revision == record.revision + 1U);
            UmiHealthcareAccessBookDestroy(restored); restored = NULL;
            record.revision = UINT64_MAX;
            OK(UmiHealthcareAccessBookRestore(registry, &record, 1U, &restored));
            CHECK(roundtrip(registry, restored) == 0);
            CHECK(UmiHealthcareAccessAccept(restored, record.requestId, UINT64_MAX, 10U, &output) == UMI_STATUS_INVALID_STATE);
        } else if (!strcmp(name, "schema") || !strcmp(name, "integers")) {
            OK(UmiHealthcareAccessArchiveEncode(registry, book, &bytes, &length));
            const char *schema[][2] = {
                {"\"version\":1", "\"version\":2"},
                {"\"format\":\"umicom.synthetic-access\"", "\"format\":\"other\""},
                {"\"requests\":[", "\"unknown\":[],\"requests\":["},
                {"\"item\":\"ITM-000001\"", "\"patient\":\"SYN-000001\""},
                {"\"item\":\"ITM-000001\"", "\"pat\\u0069ent\":\"SYN-000001\""},
                {"\"owner\":\"\"", "\"owner\":\"a person\""},
                {"\"funded\":false", "\"funded\":0"},
                {"\"requests\":[", "\"requests\":null,\"spare\":["},
                {"\"revision\":1", "\"revision\":2"}
            };
            const char *numbers[] = {"-1", "1.0", "1e0", "\"1\"", "true", "null", "18446744073709551616", "0"};
            size_t cases = !strcmp(name, "schema") ? sizeof(schema)/sizeof(schema[0]) : sizeof(numbers)/sizeof(numbers[0]);
            for (size_t i = 0U; i < cases; ++i) {
                char replacement[100];
                (void)snprintf(replacement, sizeof(replacement), "\"requested\":%s", !strcmp(name, "integers") ? numbers[i] : "0");
                changed = replace_text(bytes, !strcmp(name, "schema") ? schema[i][0] : "\"requested\":10",
                    !strcmp(name, "schema") ? schema[i][1] : replacement);
                CHECK(changed != NULL); CHECK(rejects(changed, strlen(changed)));
                free(changed); changed = NULL;
            }
            CHECK(rejects(bytes, length - 2U));
            changed = malloc(UMI_HEALTHCARE_ACCESS_ARCHIVE_MAX_BYTES + 1U); CHECK(changed != NULL);
            memset(changed, ' ', UMI_HEALTHCARE_ACCESS_ARCHIVE_MAX_BYTES + 1U);
            CHECK(rejects(changed, UMI_HEALTHCARE_ACCESS_ARCHIVE_MAX_BYTES + 1U));
        } else if (!strcmp(name, "invalid")) {
            for (unsigned int i = 0U; i < 9U; ++i) {
                UmiHealthcareAccessRecord bad = record;
                switch (i) {
                case 0: bad.acceptedQuantity = 10U; break;
                case 1: bad.simulatedFundingReady = 2; break;
                case 2: bad.reservedAt = 12U; break;
                case 3: bad.collectedQuantity = 10U; break;
                case 4: strcpy(bad.providerId, "PRV-000001"); break;
                case 5: bad.revision = 999U; break;
                case 6: strcpy(bad.followupOwnerId, "STF-000001"); break;
                case 7: memset(bad.patientId, 'S', sizeof(bad.patientId)); break;
                default: bad.unmetReason = UMI_HEALTHCARE_ACCESS_UNMET_EXPIRED; break;
                }
                CHECK(UmiHealthcareAccessBookRestore(registry, &bad, 1U, &restored) != UMI_STATUS_OK);
                CHECK(restored == NULL);
            }
            OK(UmiHealthcareAccessCount(book, &count)); CHECK(count == 1U);
        } else if (!strcmp(name, "late-failure")) {
            records[0] = record;
            OK(UmiHealthcareAccessRequest(book, registry, patient.patientId, "ITM-000002", 2U, &records[1]));
            OK(UmiHealthcareAccessArchiveEncode(registry, book, &bytes, &length));
            changed = replace_text(bytes, "ITM-000002", "BAD-000002"); CHECK(changed != NULL);
            CHECK(rejects(changed, strlen(changed)));
            records[1].requestId[9] = '1';
            CHECK(UmiHealthcareAccessBookRestore(registry, records, 2U, &restored) != UMI_STATUS_OK);
            CHECK(restored == NULL);
            OK(UmiHealthcareAccessCount(book, &count)); CHECK(count == 2U);
        } else if (!strcmp(name, "capacity")) {
            for (unsigned int i = 1U; i < UMI_HEALTHCARE_ACCESS_CAPACITY; ++i) {
                char id[11]; (void)snprintf(id, sizeof(id), "SYN-%06u", i + 1U);
                OK(UmiHealthcareSyntheticRegistryRegister(registry, id, &patient));
                OK(UmiHealthcareAccessRequest(book, registry, id, "ITM-000001", 1U, &record));
            }
            CHECK(roundtrip(registry, book) == 0);
            OK(UmiHealthcareAccessArchiveEncode(registry, book, &bytes, &length));
            OK(UmiHealthcareAccessArchiveDecode(bytes, length, &decoded, &restored));
            CHECK(UmiHealthcareAccessRequest(restored, decoded, patient.patientId, "ITM-000001", 1U, &output) == UMI_STATUS_CAPACITY_EXCEEDED);
        } else CHECK(0 && "unknown case");
    }
cleanup:
    UmiHealthcareAccessArchiveFree(bytes); free(changed);
    UmiHealthcareAccessBookDestroy(restored); UmiHealthcareSyntheticRegistryDestroy(decoded);
    UmiHealthcareAccessBookDestroy(book); UmiHealthcareSyntheticRegistryDestroy(registry);
    return result;
}
