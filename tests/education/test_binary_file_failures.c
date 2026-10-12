/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_binary_file_failures.c
 * PURPOSE: Exercise binary-file inspection resource ownership and unchanged
 *          output under deterministic file and allocation failures.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/binary_inspect.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expression) do { \
    if (!(expression)) { \
        fprintf(stderr, "Line %d: %s\n", __LINE__, #expression); \
        return 1; \
    } \
} while (0)

typedef enum FileFault {
    FILE_FAULT_NONE,
    FILE_FAULT_OPEN,
    FILE_FAULT_SEEK_END,
    FILE_FAULT_TELL,
    FILE_FAULT_SEEK_START,
    FILE_FAULT_ALLOCATE,
    FILE_FAULT_READ
} FileFault;

/* Keep the large byte fixture out of the thread stack. Each case owns one
 * synthetic stream and, if allocation succeeds, one real heap buffer. */
static struct {
    unsigned char bytes[65536];
    long fileBytes;
    FileFault fault;
    int closeFails;
    int streamOpen;
    int violation;
    unsigned opens;
    unsigned closes;
    unsigned allocations;
    unsigned releases;
    unsigned reads;
    size_t allocationBytes;
    size_t readBytes;
    void *allocation;
} fileState;

/* Accept only the fixture requested by the wrapper; opening failure acquires
 * no stream and therefore must not cause a later close operation. */
static FILE *FaultOpen(const char *path, const char *mode)
{
    ++fileState.opens;
    if (path == NULL || strcmp(path, "fixture.obj") != 0 ||
        strcmp(mode, "rb") != 0 || fileState.streamOpen) {
        fileState.violation = 1;
        return NULL;
    }
    if (fileState.fault == FILE_FAULT_OPEN) return NULL;
    fileState.streamOpen = 1;
    /* stdin is only an identity token. Every operation on it is intercepted;
     * the test never reads, seeks or closes the process's real input stream. */
    return stdin;
}

/* Distinguish measuring the file from returning to its beginning. Either
 * operation can fail before the parser is allowed to consume any bytes. */
static int FaultSeek(FILE *stream, long offset, int origin)
{
    if (stream != stdin || !fileState.streamOpen || offset != 0L ||
        (origin != SEEK_END && origin != SEEK_SET)) {
        fileState.violation = 1;
        return -1;
    }
    if ((origin == SEEK_END && fileState.fault == FILE_FAULT_SEEK_END) ||
        (origin == SEEK_SET && fileState.fault == FILE_FAULT_SEEK_START))
        return -1;
    return 0;
}

/* A failed size query is negative, not an empty file. Keep those cases
 * separate so zero-length input still reaches normal parser validation. */
static long FaultTell(FILE *stream)
{
    if (stream != stdin || !fileState.streamOpen) {
        fileState.violation = 1;
        return -1L;
    }
    return fileState.fault == FILE_FAULT_TELL ? -1L : fileState.fileBytes;
}

/* These wrappers are defined before the substitutions below, so malloc/free
 * here remain the real C allocation functions. Bad ownership is recorded
 * without attempting a second free or writing through an unknown pointer. */
static void *FaultAllocate(size_t bytes)
{
    ++fileState.allocations;
    fileState.allocationBytes = bytes;
    if (fileState.allocation != NULL || bytes > sizeof(fileState.bytes)) {
        fileState.violation = 1;
        return NULL;
    }
    if (fileState.fault == FILE_FAULT_ALLOCATE) return NULL;
    fileState.allocation = malloc(bytes);
    return fileState.allocation;
}

static void FaultRelease(void *allocation)
{
    if (allocation == NULL || allocation != fileState.allocation) {
        fileState.violation = 1;
        return;
    }
    ++fileState.releases;
    free(allocation);
    fileState.allocation = NULL;
}

/* Bound the copy by both the supplied allocation and our fixture storage.
 * A short read supplies fewer elements than requested, as fread would. */
static size_t FaultRead(void *buffer, size_t size, size_t count, FILE *stream)
{
    ++fileState.reads;
    fileState.readBytes = count;
    if (stream != stdin || !fileState.streamOpen || size != 1U ||
        buffer != fileState.allocation || count > fileState.allocationBytes ||
        count > sizeof(fileState.bytes) || (uint64_t)count > (uint64_t)fileState.fileBytes) {
        fileState.violation = 1;
        return 0U;
    }
    const size_t copied = fileState.fault == FILE_FAULT_READ && count != 0U ?
        count - 1U : count;
    memcpy(buffer, fileState.bytes, copied);
    return copied;
}

/* Closing consumes this mock stream even when it reports failure. This
 * catches retries or leaked ownership without touching the real stdin. */
static int FaultClose(FILE *stream)
{
    ++fileState.closes;
    if (stream != stdin || !fileState.streamOpen) {
        fileState.violation = 1;
        return EOF;
    }
    fileState.streamOpen = 0;
    return fileState.closeFails ? EOF : 0;
}

/* Compile the actual implementation with file-local fault substitutions.
 * This reaches late close failures without introducing test controls or global
 * I/O hooks into the public library. Never link this target to the library
 * that already defines these functions. Add other fault cases here, keeping
 * substitutions confined to this single translation unit. */
#define fopen FaultOpen
#define fseek FaultSeek
#define ftell FaultTell
#define fread FaultRead
#define fclose FaultClose
#define malloc FaultAllocate
#define free FaultRelease
#include "../../src/education/binary_inspect.c"
#undef fopen
#undef fseek
#undef ftell
#undef fread
#undef fclose
#undef malloc
#undef free

typedef enum HeaderFixture {
    HEADER_COFF,
    HEADER_EMPTY,
    HEADER_TRUNCATED_ELF,
    HEADER_UNKNOWN,
    HEADER_DISTANT_PE
} HeaderFixture;

typedef struct FailureCase {
    const char *name;
    FileFault fault;
    HeaderFixture fixture;
    long fileBytes;
    UmiStatus status;
    unsigned allocations;
    unsigned reads;
    unsigned releases;
} FailureCase;

/* Start with a valid one-section COFF object. The variations isolate parser
 * rejection from I/O rejection so a close error cannot hide the first error. */
static void PrepareFixture(const FailureCase *testCase, int closeFails)
{
    memset(&fileState, 0, sizeof(fileState));
    fileState.fileBytes = testCase->fileBytes;
    fileState.fault = testCase->fault;
    fileState.closeFails = closeFails;
    fileState.bytes[0] = 0x64U;
    fileState.bytes[1] = 0x86U;
    fileState.bytes[2] = 1U;
    switch (testCase->fixture) {
        case HEADER_TRUNCATED_ELF:
            memcpy(fileState.bytes, "\x7f" "ELF", 4U);
            break;
        case HEADER_UNKNOWN:
            fileState.bytes[0] = 0U;
            fileState.bytes[1] = 0U;
            break;
        case HEADER_DISTANT_PE:
            fileState.bytes[0] = 'M';
            fileState.bytes[1] = 'Z';
            /* The PE header starts at 65536, just outside the bounded prefix,
             * although the reported file size leaves room for that header. */
            fileState.bytes[0x3eU] = 1U;
            break;
        case HEADER_EMPTY:
        case HEADER_COFF:
            break;
    }
}

static int CheckCase(const FailureCase *testCase, int closeFails)
{
    PrepareFixture(testCase, closeFails);
    UmiEducationBinaryHeader output = {0};
    output.format = UMI_EDUCATION_BINARY_ELF;
    output.entryPoint = UINT64_C(0x123456789abcdef0);
    unsigned char before[sizeof(output)];
    /* Copy object bytes, including padding, without reading an uninitialised
     * member or relying on structure assignment to preserve padding. */
    memcpy(before, &output, sizeof(output));
    UmiStatus expected = testCase->status;
    if (expected == UMI_STATUS_OK && closeFails) expected = UMI_STATUS_IO_ERROR;
    const UmiStatus actual = UmiEducationBinaryInspectFile("fixture.obj", &output);
    if (actual != expected)
        fprintf(stderr, "%s (close failure %d): status %d, expected %d\n",
            testCase->name, closeFails, (int)actual, (int)expected);
    CHECK(actual == expected);
    if (actual == UMI_STATUS_OK) {
        CHECK(output.format == UMI_EDUCATION_BINARY_COFF);
        CHECK(output.machine == 0x8664U && output.sections == 1U);
        CHECK(output.fileBytes == (uint64_t)testCase->fileBytes);
    } else {
        CHECK(memcmp(before, &output, sizeof(output)) == 0);
    }
    CHECK(!fileState.violation && !fileState.streamOpen);
    CHECK(fileState.allocation == NULL);
    CHECK(fileState.opens == 1U);
    CHECK(fileState.closes == (testCase->fault == FILE_FAULT_OPEN ? 0U : 1U));
    CHECK(fileState.allocations == testCase->allocations);
    CHECK(fileState.reads == testCase->reads);
    CHECK(fileState.releases == testCase->releases);
    if (testCase->allocations != 0U) {
        const size_t prefixBytes = testCase->fileBytes > 65536L ?
            65536U : (size_t)testCase->fileBytes;
        CHECK(fileState.allocationBytes == (prefixBytes == 0U ? 1U : prefixBytes));
        if (testCase->reads != 0U) CHECK(fileState.readBytes == prefixBytes);
    }
    return 0;
}

static int CheckInvalidArguments(void)
{
    memset(&fileState, 0, sizeof(fileState));
    UmiEducationBinaryHeader output = {0};
    unsigned char before[sizeof(output)];
    memcpy(before, &output, sizeof(output));
    CHECK(UmiEducationBinaryInspectFile(NULL, &output) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEducationBinaryInspectFile("", &output) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEducationBinaryInspectFile("fixture.obj", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(before, &output, sizeof(output)) == 0);
    CHECK(fileState.opens == 0U && fileState.closes == 0U);
    CHECK(fileState.allocations == 0U && fileState.reads == 0U);
    return 0;
}

int main(void)
{
    static const FailureCase cases[] = {
        {"open", FILE_FAULT_OPEN, HEADER_COFF, 60L, UMI_STATUS_IO_ERROR, 0U, 0U, 0U},
        {"seek end", FILE_FAULT_SEEK_END, HEADER_COFF, 60L, UMI_STATUS_IO_ERROR, 0U, 0U, 0U},
        {"tell", FILE_FAULT_TELL, HEADER_COFF, 60L, UMI_STATUS_IO_ERROR, 0U, 0U, 0U},
        {"seek start", FILE_FAULT_SEEK_START, HEADER_COFF, 60L, UMI_STATUS_IO_ERROR, 0U, 0U, 0U},
        {"allocation", FILE_FAULT_ALLOCATE, HEADER_COFF, 60L, UMI_STATUS_OUT_OF_MEMORY, 1U, 0U, 0U},
        {"short read", FILE_FAULT_READ, HEADER_COFF, 60L, UMI_STATUS_IO_ERROR, 1U, 1U, 1U},
        {"valid object", FILE_FAULT_NONE, HEADER_COFF, 60L, UMI_STATUS_OK, 1U, 1U, 1U},
        {"empty file", FILE_FAULT_NONE, HEADER_EMPTY, 0L, UMI_STATUS_PARSE_ERROR, 1U, 1U, 1U},
        {"truncated ELF", FILE_FAULT_NONE, HEADER_TRUNCATED_ELF, 4L, UMI_STATUS_PARSE_ERROR, 1U, 1U, 1U},
        {"unknown format", FILE_FAULT_NONE, HEADER_UNKNOWN, 60L, UMI_STATUS_NOT_IMPLEMENTED, 1U, 1U, 1U},
        {"distant PE header", FILE_FAULT_NONE, HEADER_DISTANT_PE, 70000L, UMI_STATUS_CAPACITY_EXCEEDED, 1U, 1U, 1U},
        {"bounded prefix", FILE_FAULT_NONE, HEADER_COFF, 70000L, UMI_STATUS_OK, 1U, 1U, 1U}
    };
    if (CheckInvalidArguments() != 0) return 1;
    for (size_t index = 0U; index < sizeof(cases) / sizeof(cases[0]); ++index) {
        if (CheckCase(&cases[index], 0) != 0) return 1;
        /* An unopened stream cannot also fail to close. Every acquired stream
         * is checked with both close outcomes, including prior parse errors. */
        if (cases[index].fault != FILE_FAULT_OPEN &&
            CheckCase(&cases[index], 1) != 0) return 1;
    }
    puts("[OK] Binary-file failures preserve output, release resources and bound reads.");
    return 0;
}
