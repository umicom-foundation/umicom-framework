/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/healthcare/test_synthetic_encounter.c
 * PURPOSE: Verify synthetic-only administrative encounter intake contracts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 * Copyright (c) 2026 Umicom Foundation
 *---------------------------------------------------------------------------*/
#include "umicom/healthcare/synthetic_encounter.h"

#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "Encounter check failed at line %d: %s\n", \
                __LINE__, #condition); \
        return 1; \
    } \
} while (0)

int main(void)
{
    UmiHealthcareSyntheticRegistry *patients = NULL;
    UmiHealthcareSyntheticEncounterBook *book = NULL;
    UmiHealthcareSyntheticPatient patient = {0};
    UmiHealthcareSyntheticEncounter first = {0};
    UmiHealthcareSyntheticEncounter next = {0};
    UmiHealthcareSyntheticEncounter marker = {0};
    size_t count = 0U;
    size_t index;

    CHECK(UmiHealthcareSyntheticEncounterBookCreate(NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiHealthcareSyntheticRegistryCreate(&patients) == UMI_STATUS_OK);
    CHECK(UmiHealthcareSyntheticEncounterBookCreate(&book) == UMI_STATUS_OK);
    CHECK(book != NULL);
    CHECK(UmiHealthcareSyntheticEncounterCount(book, &count) == UMI_STATUS_OK);
    CHECK(count == 0U);
    CHECK(UmiHealthcareSyntheticEncounterIdValidate("ENC-000001") == UMI_STATUS_OK);
    CHECK(UmiHealthcareSyntheticEncounterIdValidate("ENC-00001X") ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiHealthcareSyntheticEncounterIdValidate("SYN-000001") ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiHealthcareSyntheticEncounterIdValidate(NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);

    marker.revision = 707U;
    CHECK(UmiHealthcareSyntheticEncounterOpen(
              book, patients, "SYN-000001", &marker) == UMI_STATUS_NOT_FOUND);
    CHECK(marker.revision == 707U);
    CHECK(UmiHealthcareSyntheticRegistryRegister(
              patients, "SYN-000001", &patient) == UMI_STATUS_OK);
    CHECK(UmiHealthcareSyntheticEncounterOpen(
              book, patients, "SYN-000001", &first) == UMI_STATUS_OK);
    CHECK(strcmp(first.encounterId, "ENC-000001") == 0);
    CHECK(strcmp(first.patientId, "SYN-000001") == 0);
    CHECK(first.stage == UMI_HEALTHCARE_INTAKE_OPEN);
    CHECK(first.revision == 1U);

    CHECK(UmiHealthcareSyntheticEncounterMarkIntakeComplete(
              book, first.encounterId, 999U, &marker) == UMI_STATUS_INVALID_STATE);
    CHECK(marker.revision == 707U);
    CHECK(UmiHealthcareSyntheticEncounterMarkIntakeComplete(
              book, "ENC-999999", 1U, &marker) == UMI_STATUS_NOT_FOUND);
    CHECK(UmiHealthcareSyntheticEncounterMarkIntakeComplete(
              book, first.encounterId, first.revision, &next) == UMI_STATUS_OK);
    CHECK(next.stage == UMI_HEALTHCARE_INTAKE_COMPLETE);
    CHECK(next.revision == 2U);
    CHECK(UmiHealthcareSyntheticEncounterMarkIntakeComplete(
              book, first.encounterId, next.revision, &marker) == UMI_STATUS_INVALID_STATE);
    CHECK(marker.revision == 707U);
    CHECK(UmiHealthcareSyntheticEncounterFind(
              book, first.encounterId, &marker) == UMI_STATUS_OK);
    CHECK(marker.stage == UMI_HEALTHCARE_INTAKE_COMPLETE);
    marker.revision = 707U;
    CHECK(UmiHealthcareSyntheticEncounterFind(
              book, "ENC-999999", &marker) == UMI_STATUS_NOT_FOUND);
    CHECK(marker.revision == 707U);

    /* The same synthetic patient may have distinct administrative encounters,
     * each with its own stable ID and monotonically increasing book revision. */
    for (index = 2U; index <= UMI_HEALTHCARE_SYNTHETIC_MAX_ENCOUNTERS;
         ++index) {
        CHECK(UmiHealthcareSyntheticEncounterOpen(
                  book, patients, "SYN-000001", &next) == UMI_STATUS_OK);
        CHECK(next.revision == index + 1U);
    }
    CHECK(UmiHealthcareSyntheticEncounterCount(book, &count) == UMI_STATUS_OK);
    CHECK(count == UMI_HEALTHCARE_SYNTHETIC_MAX_ENCOUNTERS);
    marker.revision = 707U;
    CHECK(UmiHealthcareSyntheticEncounterOpen(
              book, patients, "SYN-000001", &marker) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(marker.revision == 707U);
    CHECK(UmiHealthcareSyntheticEncounterCount(NULL, &count) ==
          UMI_STATUS_INVALID_ARGUMENT);
    UmiHealthcareSyntheticEncounterBookDestroy(book);
    UmiHealthcareSyntheticRegistryDestroy(patients);
    UmiHealthcareSyntheticEncounterBookDestroy(NULL);
    puts("Synthetic encounter intake contracts passed");
    return 0;
}
