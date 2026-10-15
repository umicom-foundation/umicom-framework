/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/test_conditional.c
 * PURPOSE: Verify stale-state refusal and each temporary-allocation failure during guarded import.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "construction_cases.h"
#include "../../src/base/conditional_archive_internal.h"

typedef struct ImportSample { char name[16]; uint64_t revision; } ImportSample;
static uint64_t ImportSchema(void) { return UINT64_C(0x2b1176595a735d90); }
static size_t ImportBound(void) { return UMI_VALUE_ARCHIVE_HEADER_SIZE + 8U + 15U + 8U; }
static UmiStatus ImportValidate(const ImportSample *value)
{
    return value != NULL && value->name[0] != '\0' &&
        memchr(value->name, '\0', sizeof(value->name)) != NULL
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
static void ImportWrite(UmiArchiveWriter *writer, const ImportSample *value)
{
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteUnsigned(writer, value->revision);
}
static void ImportRead(UmiArchiveReader *reader, ImportSample *value)
{
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    value->revision = UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
UMI_DEFINE_VALUE_ARCHIVE(ImportEncode, ImportDecode, ImportSample,
    ImportSchema, ImportBound, ImportWrite, ImportRead, ImportValidate)

/* Fail each allocation independently without introducing a production allocator
 * hook. The counters also detect leaked comparison buffers on a refused decode. */
static unsigned attempts, outstanding, fail_at;
static void *ImportAllocate(size_t size)
{
    ++attempts;
    if (attempts == fail_at) return NULL;
    void *memory = malloc(size);
    if (memory != NULL) ++outstanding;
    return memory;
}
static void ImportRelease(void *memory)
{
    if (memory != NULL) --outstanding;
    free(memory);
}
#define malloc ImportAllocate
#define free ImportRelease
UMI_DEFINE_CONDITIONAL_ARCHIVE(ImportRestore, ImportEncode, ImportSample,
    ImportSchema, ImportBound, ImportRead, ImportValidate)
#undef free
#undef malloc

int main(void)
{
    ImportSample expected = {"old", 9U}, value = {"old", 9U}, imported = {"new", 4U};
    unsigned char bytes[128], before[sizeof(value)];
    size_t size = 0U;
    int result = 1;
    UMI_CONSTRUCTION_REQUIRE(ImportEncode(&imported, bytes, sizeof(bytes), &size) == UMI_STATUS_OK);
    memcpy(before, &value, sizeof(value));
    for (unsigned index = 1U; index <= 2U; ++index) {
        attempts = 0U; fail_at = index;
        UMI_CONSTRUCTION_REQUIRE(ImportRestore(bytes, size, &expected, &value) == UMI_STATUS_OUT_OF_MEMORY);
        UMI_CONSTRUCTION_REQUIRE(outstanding == 0U && attempts == index);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, &value, sizeof(value)) == 0);
    }
    attempts = 0U; fail_at = 0U;
    value.revision = 10U;
    memcpy(before, &value, sizeof(value));
    UMI_CONSTRUCTION_REQUIRE(ImportRestore(bytes, size, &expected, &value) == UMI_STATUS_INVALID_STATE);
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, &value, sizeof(value)) == 0 && outstanding == 0U);
    expected = value;
    UMI_CONSTRUCTION_REQUIRE(ImportRestore(bytes, size, &expected, &value) == UMI_STATUS_OK);
    UMI_CONSTRUCTION_REQUIRE(strcmp(value.name, "new") == 0 && value.revision == 4U && outstanding == 0U);

    /* Recompute the checksum after damaging a field length. Envelope integrity
     * must never bypass bounded decoding, and failed candidates must be freed. */
    expected = value;
    memcpy(before, &value, sizeof(value));
    UmiArchiveStoreInteger(bytes + UMI_VALUE_ARCHIVE_HEADER_SIZE, 16U, 8U);
    UmiArchiveFinish(bytes, size, ImportSchema());
    UMI_CONSTRUCTION_REQUIRE(ImportRestore(bytes, size, &expected, &value) == UMI_STATUS_PARSE_ERROR);
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, &value, sizeof(value)) == 0 && outstanding == 0U);
    result = 0;
construction_cleanup:
    return result;
}
