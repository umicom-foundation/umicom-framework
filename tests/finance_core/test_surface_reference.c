/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_surface_reference.c
 *
 * PURPOSE:
 *   Exercise the surface reference financial-core contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <string.h>
#include "umicom/finance/core/surface_reference.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/core/surface_reference.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiSurfaceReferenceTransferEqual(const UmiSurfaceReference *a, const UmiSurfaceReference *b)
{
    return strcmp(a->reference_id.value, b->reference_id.value) == 0 &&
        strcmp(a->name, b->name) == 0 &&
        strcmp(a->code, b->code) == 0 &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiSurfaceReferenceTransferTails(UmiSurfaceReference *value)
{
    (void)value;
    {
        size_t used = strlen(value->reference_id.value) + 1U;
        memset(value->reference_id.value + used, 0xa5, sizeof(value->reference_id.value) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->code) + 1U;
        memset(value->code + used, 0xa5, sizeof(value->code) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiSurfaceReferenceTransferMalformed(const UmiSurfaceReference *sample)
{
    (void)sample;
    {
        UmiSurfaceReference invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.reference_id.value, 'x', sizeof(invalid.reference_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_surface_reference_is_valid(&invalid)) ||
            umi_surface_reference_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated reference_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiSurfaceReference invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_surface_reference_is_valid(&invalid)) ||
            umi_surface_reference_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiSurfaceReference invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.code, 'x', sizeof(invalid.code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_surface_reference_is_valid(&invalid)) ||
            umi_surface_reference_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated code was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiSurfaceReferenceTransferCases, UmiSurfaceReference,
    umi_surface_reference_archive_encode, umi_surface_reference_archive_decode,
    UmiSurfaceReferenceTransferEqual, UmiSurfaceReferenceTransferTails, UmiSurfaceReferenceTransferMalformed)

int main(void)
{
    UmiSurfaceReference x; CHECK(umi_surface_reference_init(&x,"ID","Name","CODE")==UMI_STATUS_OK); CHECK(umi_surface_reference_is_valid(&x));
    if (UmiSurfaceReferenceTransferCases(&x) != 0) return 1;

    return 0;
}
