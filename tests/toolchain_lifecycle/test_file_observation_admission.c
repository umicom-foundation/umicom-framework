/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_file_observation_admission.c
 *
 * PURPOSE:
 *   Verify whole-selection admission and aggregate read budgets without large allocations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/toolchain/file_observation.h"
#include "umicom/platform/input_file.h"
#include "umicom/base/sha256.h"
#include "../support/assertions.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static size_t calls, fakeLength, failedCall = SIZE_MAX, limits[64];
static UmiStatus failure = UMI_STATUS_IO_ERROR;
/* Replace only native reads and digest computation so the real orchestration
 * can exercise aggregate limits without allocating hundreds of megabytes. */
static UmiStatus Read(const char *path, size_t maximum, unsigned char **bytes, size_t *length)
{
    assert(path != NULL && calls < 64U);
    limits[calls] = maximum;
    *bytes = NULL; *length = 0U;
    if (calls++ == failedCall) return failure;
    if (fakeLength > maximum) return UMI_STATUS_CAPACITY_EXCEEDED;
    *bytes = malloc(1U); assert(*bytes != NULL); (*bytes)[0] = 0U;
    *length = fakeLength;
    return UMI_STATUS_OK;
}
static UmiStatus Digest(const void *bytes, size_t length, char out[65])
{
    assert(bytes != NULL && length == fakeLength);
    memset(out, 'a', 64U); out[64] = '\0'; return UMI_STATUS_OK;
}
#define UmiInputFileRead Read
#define UmiSha256Buffer Digest
#define UmiFileObservationCapture Capture
#define UmiFileObservationDestroy Destroy
#define UmiFileObservationCount Count
#define UmiFileObservationAt At
#define UmiFileObservationFormat Format
#define UmiFileObservationSave Save
#include "../../src/toolchain/file_observation.c"
#undef UmiFileObservationSave
#undef UmiFileObservationFormat
#undef UmiFileObservationAt
#undef UmiFileObservationCount
#undef UmiFileObservationDestroy
#undef UmiFileObservationCapture
#undef UmiSha256Buffer
#undef UmiInputFileRead
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    char names[65][80]; const char *paths[65];
    for (size_t i = 0U; i < 65U; ++i) {
#ifdef _WIN32
        (void)snprintf(names[i], sizeof names[i], "C:/observation/file-%zu", i);
#else
        (void)snprintf(names[i], sizeof names[i], "/observation/file-%zu", i);
#endif
        paths[i] = names[i];
    }
    const char *mode = argv[1];
    size_t count = 2U, maximum = UMI_FILE_OBSERVATION_MAX_BYTES, failed = 0U;
    UmiFileObservation *value = NULL, *sentinel = (UmiFileObservation *)&names;
    UmiFileObservation **out = &value;
    const char *const *selection = paths;
    if (!strcmp(mode, "count-zero")) count = 0U;
    else if (!strcmp(mode, "count-excess")) count = 65U;
    else if (!strcmp(mode, "null-vector")) selection = NULL;
    else if (!strcmp(mode, "null-output")) out = NULL;
    else if (!strcmp(mode, "occupied-output")) value = sentinel;
    else if (!strcmp(mode, "limit-zero")) maximum = 0U;
    else if (!strcmp(mode, "limit-excess")) maximum++;
    else if (!strcmp(mode, "last-relative")) paths[1] = "relative";
    else if (!strcmp(mode, "last-null")) paths[1] = NULL;
    else if (!strcmp(mode, "last-duplicate")) paths[1] = paths[0];
    else if (!strcmp(mode, "last-utf8")) { names[1][3] = (char)0xff; }
    else if (!strcmp(mode, "budget")) { count = 5U; fakeLength = UMI_FILE_OBSERVATION_MAX_BYTES; }
    else if (!strcmp(mode, "at-budget")) { count = 4U; fakeLength = UMI_FILE_OBSERVATION_MAX_BYTES; }
    else if (!strcmp(mode, "maximum-files")) count = 64U;
    else if (!strcmp(mode, "read-error")) { failedCall = 1U; failure = UMI_STATUS_IO_ERROR; }
    else if (!strcmp(mode, "changed-file")) { failedCall = 1U; failure = UMI_STATUS_BUSY; }
    else return 2;
    UmiStatus status = Capture(selection, count, maximum, out, &failed);
    if (!strcmp(mode, "at-budget") || !strcmp(mode, "maximum-files")) {
        assert(status == UMI_STATUS_OK && value != NULL && Count(value) == count);
        assert(calls == count && failed == SIZE_MAX); Destroy(value);
    } else {
        assert(status != UMI_STATUS_OK && value == (!strcmp(mode, "occupied-output") ? sentinel : NULL));
        if (!strcmp(mode, "budget")) {
            assert(status == UMI_STATUS_CAPACITY_EXCEEDED && calls == 5U && failed == 4U && limits[4] == 0U);
        } else if (!strcmp(mode, "read-error") || !strcmp(mode, "changed-file")) {
            assert(status == failure && calls == 2U && failed == 1U);
        } else {
            assert(calls == 0U && status == UMI_STATUS_INVALID_ARGUMENT);
            assert(failed == (!strncmp(mode, "last-", 5U) ? 1U : SIZE_MAX));
        }
    }
    return 0;
}
