/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/storage_fixture.h
 * PURPOSE: Reserve independent database fixtures without overwriting earlier evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_STORAGE_FIXTURE_H
#define UMICOM_TEST_STORAGE_FIXTURE_H

#include "umicom/platform/output_file.h"
#include <stdio.h>

/* An absolute base path comes from a test-owned directory. Exclusive native
 * creation arbitrates parallel processes; a prior failed run remains intact.
 * The caller owns only the returned path after OK and closes database handles
 * before removing that file. Exhaustion is reported rather than deleting files
 * to make room. This is fixture isolation, not a filesystem security boundary. */
static inline UmiStatus UmiTestStorageReserve(const char *base, char *out, size_t capacity)
{
    if (out == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    out[0] = '\0';
    UmiStatus status = UmiOutputFileValidatePath(base);
    if (status != UMI_STATUS_OK)
        return status;
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
        status = UmiOutputFileCreate(out, &file);
        if (status == UMI_STATUS_ALREADY_EXISTS)
            continue;
        if (status == UMI_STATUS_OK)
            status = UmiOutputFileClose(file);
        UmiOutputFileDestroy(file);
        /* A close failure retains its evidence on disk, but does not publish
         * ownership to a caller that might otherwise open a damaged fixture. */
        if (status != UMI_STATUS_OK)
            out[0] = '\0';
        return status;
    }
    out[0] = '\0';
    return UMI_STATUS_CAPACITY_EXCEEDED;
}
#endif
