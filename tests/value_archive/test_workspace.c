/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/test_workspace.c
 * PURPOSE: Check bounded-stack staging, late refusal and partial overlap.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "../../src/base/value_archive_internal.h"
#include <stdio.h>
#include <stdlib.h>

/* This record is intentionally larger than a typical default thread stack.
 * Only pointers to it are automatic; the caller owns all staging allocations. */
typedef struct WorkspaceLargeRecord {
    uint64_t serial;
    unsigned char payload[2U * 1024U * 1024U];
} WorkspaceLargeRecord;

static uint64_t LargeSchema(void) { return UINT64_C(0x756d69576f726b73); }
static size_t LargeBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE + 8U +
        sizeof(((WorkspaceLargeRecord *)0)->payload);
}
static void LargeRead(UmiArchiveReader *reader, WorkspaceLargeRecord *value)
{
    value->serial = UmiArchiveReadUnsigned(reader, UINT64_MAX);
    const unsigned char *payload = UmiArchiveReadBytes(reader, sizeof(value->payload));
    if (payload != NULL) memcpy(value->payload, payload, sizeof(value->payload));
}
static UmiStatus LargeValidate(const WorkspaceLargeRecord *value)
{
    return value->serial == 42U && value->payload[0] == 0x19U &&
        value->payload[sizeof(value->payload) - 1U] == 0xe4U
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UmiStatus LargeDecode(const void *bytes, size_t byte_count,
    WorkspaceLargeRecord *workspace, WorkspaceLargeRecord *value);

/* A future allocation added directly to the shared macro must fail this build.
 * Fixture allocation remains explicit below, outside the guarded definition. */
#define malloc UMICOM_WORKSPACE_DECODER_MUST_NOT_MALLOC
#define calloc UMICOM_WORKSPACE_DECODER_MUST_NOT_CALLOC
#define realloc UMICOM_WORKSPACE_DECODER_MUST_NOT_REALLOC
#define free UMICOM_WORKSPACE_DECODER_MUST_NOT_FREE
UMI_DEFINE_VALUE_ARCHIVE_WORKSPACE(LargeDecode, WorkspaceLargeRecord,
    LargeSchema, LargeBound, LargeRead, LargeValidate)
#undef free
#undef realloc
#undef calloc
#undef malloc

#define REQUIRE(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); goto done; \
} } while (0)

int main(void)
{
    int result = 1;
    const size_t size = LargeBound();
    unsigned char *bytes = malloc(size), *before = malloc(sizeof(WorkspaceLargeRecord));
    unsigned char *arena = malloc(2U * sizeof(WorkspaceLargeRecord));
    WorkspaceLargeRecord *workspace = malloc(sizeof(*workspace));
    WorkspaceLargeRecord *value = malloc(sizeof(*value));
    REQUIRE(bytes != NULL && before != NULL && arena != NULL && workspace != NULL && value != NULL);
    memset(bytes, 0, size);
    UmiArchiveStoreInteger(bytes + UMI_VALUE_ARCHIVE_HEADER_SIZE, 42U, 8U);
    bytes[UMI_VALUE_ARCHIVE_HEADER_SIZE + 8U] = 0x19U;
    bytes[size - 1U] = 0xe4U;
    UmiArchiveFinish(bytes, size, LargeSchema());
    REQUIRE(LargeDecode(bytes, size, workspace, value) == UMI_STATUS_OK);
    REQUIRE(LargeValidate(value) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));

    /* Correct checksum, wrong domain value: refusal occurs after scratch has
     * changed, but the caller's published destination must remain byte-exact. */
    UmiArchiveStoreInteger(bytes + UMI_VALUE_ARCHIVE_HEADER_SIZE, 0U, 8U);
    UmiArchiveFinish(bytes, size, LargeSchema());
    REQUIRE(LargeDecode(bytes, size, workspace, value) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(workspace->serial == 0U);
    REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
    UmiArchiveStoreInteger(bytes + UMI_VALUE_ARCHIVE_HEADER_SIZE, 42U, 8U);
    UmiArchiveFinish(bytes, size, LargeSchema());

    /* Both pointers are aligned, but their record extents overlap. No end
     * pointer arithmetic or staging write is allowed to hide that conflict. */
    memset(arena, 0xa5, 2U * sizeof(WorkspaceLargeRecord));
    WorkspaceLargeRecord *first = (WorkspaceLargeRecord *)(void *)arena;
    WorkspaceLargeRecord *second = (WorkspaceLargeRecord *)(void *)
        (arena + _Alignof(WorkspaceLargeRecord));
    REQUIRE(LargeDecode(bytes, size, first, second) == UMI_STATUS_INVALID_ARGUMENT);
    for (size_t i = 0U; i < 2U * sizeof(WorkspaceLargeRecord); ++i)
        REQUIRE(arena[i] == 0xa5U);
    REQUIRE(LargeDecode((unsigned char *)workspace + 1U, 1U, workspace, value) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(LargeDecode((unsigned char *)value + 1U, 1U, workspace, value) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
    REQUIRE(LargeDecode(bytes, size, workspace, value) == UMI_STATUS_OK);
    REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
    result = 0;
done:
    free(value); free(workspace); free(arena); free(before); free(bytes);
    return result;
}
