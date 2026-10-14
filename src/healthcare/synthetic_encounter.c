/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/healthcare/synthetic_encounter.c
 *
 * PURPOSE:
 *   Implement the bounded synthetic encounter intake book, using a patient
 *   lookup from the existing Framework synthetic registry. The book is an
 *   ephemeral administrative test object, not a medical record store.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *   Copyright (c) 2026 Umicom Foundation
 *---------------------------------------------------------------------------*/
#include "umicom/healthcare/synthetic_encounter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiHealthcareSyntheticEncounterBook {
    UmiHealthcareSyntheticEncounter records[UMI_HEALTHCARE_SYNTHETIC_MAX_ENCOUNTERS];
    size_t count;
    uint64_t latestRevision;
};

UmiStatus UmiHealthcareSyntheticEncounterIdValidate(const char *encounterId)
{
    size_t index;

    if (encounterId == NULL ||
        strlen(encounterId) != UMI_HEALTHCARE_SYNTHETIC_ENCOUNTER_ID_CAPACITY - 1U ||
        memcmp(encounterId, "ENC-", 4U) != 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 4U; index < UMI_HEALTHCARE_SYNTHETIC_ENCOUNTER_ID_CAPACITY - 1U;
         ++index) {
        if (encounterId[index] < '0' || encounterId[index] > '9') {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiHealthcareSyntheticEncounterBookCreate(
    UmiHealthcareSyntheticEncounterBook **outBook)
{
    if (outBook == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outBook = (UmiHealthcareSyntheticEncounterBook *)calloc(1U, sizeof(**outBook));
    return *outBook != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}

void UmiHealthcareSyntheticEncounterBookDestroy(
    UmiHealthcareSyntheticEncounterBook *book)
{
    free(book);
}

UmiStatus UmiHealthcareSyntheticEncounterOpen(
    UmiHealthcareSyntheticEncounterBook *book,
    const UmiHealthcareSyntheticRegistry *registry,
    const char *syntheticPatientId,
    UmiHealthcareSyntheticEncounter *outEncounter)
{
    UmiHealthcareSyntheticPatient patient = {0};
    UmiHealthcareSyntheticEncounter candidate = {0};
    UmiStatus status;
    int written;

    if (book == NULL || registry == NULL || outEncounter == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Identity authority remains the existing Framework registry. Do not
     * invent a second patient-ID validator or patient store here. */
    status = UmiHealthcareSyntheticRegistryFind(
        registry, syntheticPatientId, &patient);
    if (status != UMI_STATUS_OK) return status;
    if (book->count >= UMI_HEALTHCARE_SYNTHETIC_MAX_ENCOUNTERS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    written = snprintf(candidate.encounterId, sizeof(candidate.encounterId),
                       "ENC-%06zu", book->count + 1U);
    if (written != (int)(UMI_HEALTHCARE_SYNTHETIC_ENCOUNTER_ID_CAPACITY - 1U)) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    (void)memcpy(candidate.patientId, patient.patientId, sizeof(candidate.patientId));
    candidate.stage = UMI_HEALTHCARE_INTAKE_OPEN;
    candidate.revision = book->latestRevision + 1U;
    /* Copy to the book only when all validation has completed. */
    book->records[book->count] = candidate;
    ++book->count;
    book->latestRevision = candidate.revision;
    *outEncounter = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiHealthcareSyntheticEncounterMarkIntakeComplete(
    UmiHealthcareSyntheticEncounterBook *book,
    const char *encounterId,
    uint64_t expectedRevision,
    UmiHealthcareSyntheticEncounter *outEncounter)
{
    size_t index;
    UmiHealthcareSyntheticEncounter candidate;

    if (book == NULL || outEncounter == NULL ||
        UmiHealthcareSyntheticEncounterIdValidate(encounterId) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < book->count; ++index) {
        if (strcmp(book->records[index].encounterId, encounterId) == 0) {
            if (book->records[index].revision != expectedRevision ||
                book->records[index].stage != UMI_HEALTHCARE_INTAKE_OPEN) {
                return UMI_STATUS_INVALID_STATE;
            }
            candidate = book->records[index];
            candidate.stage = UMI_HEALTHCARE_INTAKE_COMPLETE;
            candidate.revision = book->latestRevision + 1U;
            book->records[index] = candidate;
            book->latestRevision = candidate.revision;
            *outEncounter = candidate;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus UmiHealthcareSyntheticEncounterFind(
    const UmiHealthcareSyntheticEncounterBook *book,
    const char *encounterId,
    UmiHealthcareSyntheticEncounter *outEncounter)
{
    size_t index;

    if (book == NULL || outEncounter == NULL ||
        UmiHealthcareSyntheticEncounterIdValidate(encounterId) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < book->count; ++index) {
        if (strcmp(book->records[index].encounterId, encounterId) == 0) {
            *outEncounter = book->records[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus UmiHealthcareSyntheticEncounterCount(
    const UmiHealthcareSyntheticEncounterBook *book,
    size_t *outCount)
{
    if (book == NULL || outCount == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outCount = book->count;
    return UMI_STATUS_OK;
}
