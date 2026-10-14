/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/healthcare/test_synthetic_registry.c
 * PURPOSE: Verify synthetic-only identity, revision and capacity contracts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 * Copyright (c) 2026 Umicom Foundation
 *---------------------------------------------------------------------------*/
#include "umicom/healthcare/synthetic_registry.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    UmiHealthcareSyntheticRegistry *registry = NULL;
    UmiHealthcareSyntheticPatient patient = {{0}, 0U};
    UmiHealthcareSyntheticPatient found = {{0}, 0U};
    UmiHealthcareSyntheticPatient unchanged = {{0}, 0U};
    size_t count = 0U;
    size_t index;
    char patientId[UMI_HEALTHCARE_SYNTHETIC_ID_CAPACITY];

    assert(UmiHealthcareSyntheticRegistryCreate(NULL) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiHealthcareSyntheticRegistryCreate(&registry) == UMI_STATUS_OK);
    assert(registry != NULL);
    assert(UmiHealthcareSyntheticRegistryCount(registry, &count) == UMI_STATUS_OK);
    assert(count == 0U);

    assert(UmiHealthcareSyntheticRegistryRegister(
               registry, "SYN-000001", &patient) == UMI_STATUS_OK);
    assert(strcmp(patient.patientId, "SYN-000001") == 0);
    assert(patient.revision == 1U);
    assert(UmiHealthcareSyntheticRegistryFind(
               registry, "SYN-000001", &found) == UMI_STATUS_OK);
    assert(found.revision == patient.revision);
    assert(UmiHealthcareSyntheticRegistryRegister(
               registry, "SYN-000001", &found) == UMI_STATUS_ALREADY_EXISTS);

    unchanged.revision = 707U;
    assert(UmiHealthcareSyntheticRegistryRegister(
               registry, "PAT-REAL00", &unchanged) == UMI_STATUS_INVALID_ARGUMENT);
    assert(unchanged.revision == 707U);
    assert(UmiHealthcareSyntheticRegistryRegister(
               registry, "SYN-00001A", &unchanged) == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiHealthcareSyntheticRegistryFind(
               registry, "SYN-999999", &unchanged) == UMI_STATUS_NOT_FOUND);
    assert(unchanged.revision == 707U);

    for (index = 2U; index <= UMI_HEALTHCARE_SYNTHETIC_MAX_PATIENTS;
         ++index) {
        assert(snprintf(patientId, sizeof(patientId), "SYN-%06zu", index) == 10);
        assert(UmiHealthcareSyntheticRegistryRegister(
                   registry, patientId, &patient) == UMI_STATUS_OK);
        assert(patient.revision == index);
    }
    assert(UmiHealthcareSyntheticRegistryCount(registry, &count) == UMI_STATUS_OK);
    assert(count == UMI_HEALTHCARE_SYNTHETIC_MAX_PATIENTS);
    assert(UmiHealthcareSyntheticRegistryRegister(
               registry, "SYN-999999", &unchanged) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(unchanged.revision == 707U);
    assert(UmiHealthcareSyntheticRegistryCount(NULL, &count) ==
           UMI_STATUS_INVALID_ARGUMENT);
    UmiHealthcareSyntheticRegistryDestroy(registry);
    UmiHealthcareSyntheticRegistryDestroy(NULL);
    puts("Synthetic patient registry contracts passed");
    return 0;
}
