/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/healthcare/synthetic_registry.c
 *
 * PURPOSE:
 *   Implement a bounded, deterministic synthetic identity registry without
 *   accepting identifiable clinical data or bypassing the Data Server.
 *
 * ARCHITECTURE:
 *   This module demonstrates the Framework's ownership of common healthcare
 *   contracts. It is deliberately ephemeral, and is not a replacement for
 *   the future security-governed, Data Server-owned patient service.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *   Copyright (c) 2026 Umicom Foundation
 *---------------------------------------------------------------------------*/
#include "umicom/healthcare/synthetic_registry.h"

#include <stdlib.h>
#include <string.h>

struct UmiHealthcareSyntheticRegistry {
    UmiHealthcareSyntheticPatient records[UMI_HEALTHCARE_SYNTHETIC_MAX_PATIENTS];
    size_t count;
    uint64_t latestRevision;
};

/* The narrow token grammar prevents names and other free text from entering
 * this demonstration registry. Caller input must be a terminated C string. */
static int UmiHealthcareIsSyntheticId(const char *patientId)
{
    size_t index;

    if (patientId == NULL || strlen(patientId) !=
            UMI_HEALTHCARE_SYNTHETIC_ID_CAPACITY - 1U ||
        memcmp(patientId, "SYN-", 4U) != 0) {
        return 0;
    }
    for (index = 4U; index < UMI_HEALTHCARE_SYNTHETIC_ID_CAPACITY - 1U;
         ++index) {
        if (patientId[index] < '0' || patientId[index] > '9') {
            return 0;
        }
    }
    return 1;
}

UmiStatus UmiHealthcareSyntheticRegistryCreate(
    UmiHealthcareSyntheticRegistry **outRegistry)
{
    UmiHealthcareSyntheticRegistry *registry;

    if (outRegistry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outRegistry = NULL;
    registry = (UmiHealthcareSyntheticRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *outRegistry = registry;
    return UMI_STATUS_OK;
}

void UmiHealthcareSyntheticRegistryDestroy(
    UmiHealthcareSyntheticRegistry *registry)
{
    free(registry);
}

UmiStatus UmiHealthcareSyntheticRegistryRegister(
    UmiHealthcareSyntheticRegistry *registry,
    const char *syntheticPatientId,
    UmiHealthcareSyntheticPatient *outPatient)
{
    UmiHealthcareSyntheticPatient patient = {0};
    size_t index;

    if (registry == NULL || outPatient == NULL ||
        !UmiHealthcareIsSyntheticId(syntheticPatientId)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->records[index].patientId,
                   syntheticPatientId) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (registry->count >= UMI_HEALTHCARE_SYNTHETIC_MAX_PATIENTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Revision belongs to this in-memory registry only. It is not a durable
     * clinical record version, audit ID or cross-process sequence number. */
    (void)memcpy(patient.patientId, syntheticPatientId,
                 UMI_HEALTHCARE_SYNTHETIC_ID_CAPACITY);
    patient.revision = ++registry->latestRevision;
    registry->records[registry->count++] = patient;
    *outPatient = patient;
    return UMI_STATUS_OK;
}

UmiStatus UmiHealthcareSyntheticRegistryFind(
    const UmiHealthcareSyntheticRegistry *registry,
    const char *syntheticPatientId,
    UmiHealthcareSyntheticPatient *outPatient)
{
    size_t index;

    if (registry == NULL || outPatient == NULL ||
        !UmiHealthcareIsSyntheticId(syntheticPatientId)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->records[index].patientId,
                   syntheticPatientId) == 0) {
            *outPatient = registry->records[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus UmiHealthcareSyntheticRegistryCount(
    const UmiHealthcareSyntheticRegistry *registry,
    size_t *outCount)
{
    if (registry == NULL || outCount == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *outCount = registry->count;
    return UMI_STATUS_OK;
}
