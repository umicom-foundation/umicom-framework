/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/healthcare/synthetic_registry.h
 *
 * PURPOSE:
 *   Provide an intentionally restricted, in-memory synthetic patient registry
 *   for development and deterministic integration tests. This is the first
 *   healthcare domain contract shared by standalone and suite applications.
 *
 * SAFETY AND OWNERSHIP:
 *   Identifiers must be SYN- followed by six decimal digits. The registry does
 *   not accept names, demographics, health details or other patient information.
 *   It performs no persistence, clinical routing, diagnosis, treatment,
 *   prescribing, consent processing or clinical audit. Never use it for care.
 *   The caller owns the opaque registry handle and must destroy it. This first
 *   teaching implementation is not thread-safe.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *   Copyright (c) 2026 Umicom Foundation
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_HEALTHCARE_SYNTHETIC_REGISTRY_H
#define UMICOM_HEALTHCARE_SYNTHETIC_REGISTRY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_HEALTHCARE_SYNTHETIC_ID_CAPACITY 11U
#define UMI_HEALTHCARE_SYNTHETIC_MAX_PATIENTS 32U

typedef struct UmiHealthcareSyntheticRegistry UmiHealthcareSyntheticRegistry;

typedef struct UmiHealthcareSyntheticPatient {
    char patientId[UMI_HEALTHCARE_SYNTHETIC_ID_CAPACITY];
    uint64_t revision;
} UmiHealthcareSyntheticPatient;

/* Create a fresh empty registry. On failure *outRegistry is NULL. */
UmiStatus UmiHealthcareSyntheticRegistryCreate(
    UmiHealthcareSyntheticRegistry **outRegistry);

/* Release a registry previously returned by Create; NULL is accepted. */
void UmiHealthcareSyntheticRegistryDestroy(
    UmiHealthcareSyntheticRegistry *registry);

/* Register an artificial identity only. Duplicate and invalid IDs are refused.
 * The output snapshot is written only on success and does not contain PHI. */
UmiStatus UmiHealthcareSyntheticRegistryRegister(
    UmiHealthcareSyntheticRegistry *registry,
    const char *syntheticPatientId,
    UmiHealthcareSyntheticPatient *outPatient);

/* Read the current in-memory snapshot by its synthetic identifier. */
UmiStatus UmiHealthcareSyntheticRegistryFind(
    const UmiHealthcareSyntheticRegistry *registry,
    const char *syntheticPatientId,
    UmiHealthcareSyntheticPatient *outPatient);

/* Return the number of synthetic identities currently held in the registry. */
UmiStatus UmiHealthcareSyntheticRegistryCount(
    const UmiHealthcareSyntheticRegistry *registry,
    size_t *outCount);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_HEALTHCARE_SYNTHETIC_REGISTRY_H */
