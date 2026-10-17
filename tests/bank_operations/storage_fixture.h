/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_operations/storage_fixture.h
 * PURPOSE: Reserve fresh database files while retaining evidence from failed test runs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BANK_STORAGE_FIXTURE_H
#define UMICOM_BANK_STORAGE_FIXTURE_H
#include "umicom/platform/output_file.h"
#include <stdio.h>

/* The caller supplies an absolute path inside its CTest build directory.
 * Exclusive creation claims each candidate without opening an existing leaf.
 * A failed earlier run remains available for diagnosis; only the current run
 * may remove its successfully reserved database after its checks finish. */
/* Reservation now belongs to the shared test fixture boundary so database
 * suites use identical collision handling. Keep the original implementation
 * disabled for review of its ownership contract. */
#if 0
static inline UmiStatus BankStorageFixtureReserve(const char *base, char *out, size_t capacity)
{
    if (base == NULL || out == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    out[0] = '\0';
    for (unsigned attempt = 0U; attempt < 1024U; ++attempt)
    {
        int written = attempt == 0U ? snprintf(out, capacity, "%s", base)
                                   : snprintf(out, capacity, "%s.run-%u", base, attempt);
        if (written < 0 || (size_t)written >= capacity)
        {
            out[0] = '\0';
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        UmiOutputFile *file = NULL;
        UmiStatus status = UmiOutputFileCreate(out, &file);
        if (status == UMI_STATUS_ALREADY_EXISTS)
            continue;
        if (status == UMI_STATUS_OK)
            status = UmiOutputFileClose(file);
        UmiOutputFileDestroy(file);
        return status;
    }
    out[0] = '\0';
    return UMI_STATUS_CAPACITY_EXCEEDED;
}
#endif
#include "../storage_fixture.h"

static inline UmiStatus BankStorageFixtureReserve(const char *base, char *out, size_t capacity)
{
    return UmiTestStorageReserve(base, out, capacity);
}
#endif
