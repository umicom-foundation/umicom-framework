/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/qualification/reuse_main.c
 *
 * PURPOSE:
 *   Bridge CMake test discovery to native receipt parsing and atomic persistence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/reuse.h"
#include "umicom/platform/native_arguments.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/atomic_file.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static UmiStatus Load(const char *file, UmiTestReuseState **state)
{
    unsigned char *bytes = NULL; size_t length = 0U;
    UmiStatus status = UmiInputFileRead(file, UMI_TEST_REUSE_MAX_BYTES, &bytes, &length);
    if (status == UMI_STATUS_OK) status = UmiTestReuseImport((const char *)bytes, length, state);
    UmiInputFileFree(bytes);
    return status;
}
static UmiStatus Save(const char *file, UmiTestReuseState *state)
{
    size_t length = 0U;
    UmiStatus status = UmiTestReuseExport(state, NULL, 0U, &length);
    char *text = status == UMI_STATUS_OK ? malloc(length) : NULL;
    if (status == UMI_STATUS_OK && text == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    if (status == UMI_STATUS_OK) status = UmiTestReuseExport(state, text, length, &length);
    if (status == UMI_STATUS_OK) status = umi_atomic_file_write(file, text, length - 1U);
    free(text); return status;
}
static int RunReceipts(int argc, char **argv)
{
    UmiTestReuseState *current = NULL, *previous = NULL, *next = NULL;
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    /* The driver owns the directory lease and fresh run directory. This tool
     * never launches tests or trusts instructions embedded in a result file. */
    if (argc == 5 && strcmp(argv[1], "prepare") == 0) {
        status = Load(argv[2], &current);
        if (status == UMI_STATUS_OK && umi_fs_exists(argv[3])) {
            UmiStatus old = Load(argv[3], &previous);
            if (old != UMI_STATUS_OK)
                fprintf(stderr, "Test receipts unavailable (%d); all tests remain selected.\n", (int)old);
        }
        if (status == UMI_STATUS_OK) status = UmiTestReusePrepare(current, previous, &next);
        size_t length = 1U;
        for (size_t i = 0U; i < UmiTestReuseCount(next); ++i)
            if (UmiTestReuseOutcome(next, i) == UMI_TEST_STATE_PASSED)
                length += strlen(UmiTestReuseName(next, i)) + 1U;
        char *excluded = status == UMI_STATUS_OK ? malloc(length) : NULL;
        if (status == UMI_STATUS_OK && excluded == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
        size_t used = 0U, reused = 0U;
        if (status == UMI_STATUS_OK) {
            excluded[0] = '\0';
            for (size_t i = 0U; i < UmiTestReuseCount(next); ++i)
                if (UmiTestReuseOutcome(next, i) == UMI_TEST_STATE_PASSED) {
                    const char *name = UmiTestReuseName(next, i); size_t size = strlen(name);
                    memcpy(excluded + used, name, size); used += size; excluded[used++] = '\n'; ++reused;
                }
            /* Invalidate selected receipts first. A failed write cannot leave
             * an exclusion file that accidentally blesses an interrupted run. */
            status = Save(argv[3], next);
            if (status == UMI_STATUS_OK) status = umi_atomic_file_write(argv[4], excluded, used);
            if (status == UMI_STATUS_OK) printf("Reusing %zu unchanged passing test receipts.\n", reused);
        }
        free(excluded);
    } else if (argc == 4 && strcmp(argv[1], "record") == 0) {
        status = Load(argv[2], &current);
        unsigned char *xml = NULL; size_t length = 0U;
        if (status == UMI_STATUS_OK) status = UmiInputFileRead(argv[3], UMI_CTEST_COLLECTION_MAX_BYTES, &xml, &length);
        if (status == UMI_STATUS_OK) status = UmiTestReuseRecord(current, (const char *)xml, length);
        if (status == UMI_STATUS_OK) status = Save(argv[2], current);
        UmiInputFileFree(xml);
    } else {
        fprintf(stderr, "Usage: umicom-test-receipts prepare MANIFEST STATE EXCLUSIONS | record STATE JUNIT\n");
    }
    UmiTestReuseDestroy(next); UmiTestReuseDestroy(previous); UmiTestReuseDestroy(current);
    if (status != UMI_STATUS_OK) fprintf(stderr, "Test receipt operation failed (%d).\n", (int)status);
    return status == UMI_STATUS_OK ? 0 : 1;
}

/* Platform argument decoding preserves Unicode workspace paths on Windows. */
int main(int argc, char **argv)
{
    int result = 1;
    UmiStatus status = UmiNativeArgumentsDispatch(argc, argv, RunReceipts, &result);
    if (status != UMI_STATUS_OK) fprintf(stderr, "Could not decode receipt paths (%d).\n", (int)status);
    return status == UMI_STATUS_OK ? result : 1;
}
