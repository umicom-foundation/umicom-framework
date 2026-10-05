/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_cache_layout.c
 *
 * PURPOSE:
 *   Focused regression coverage for cache namespace and eviction-budget configuration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/distribution/runtime/cache_layout.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/cache_layout.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrCacheLayoutTransferEqual(const UmiDrCacheLayout *a, const UmiDrCacheLayout *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->namespace_id, b->namespace_id) == 0 &&
        a->max_bytes == b->max_bytes &&
        a->disposable == b->disposable;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrCacheLayoutTransferTails(UmiDrCacheLayout *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->namespace_id) + 1U;
        memset(value->namespace_id + used, 0xa5, sizeof(value->namespace_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrCacheLayoutTransferMalformed(const UmiDrCacheLayout *sample)
{
    (void)sample;
    {
        UmiDrCacheLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_cache_layout_valid(&invalid)) ||
            umi_dr_cache_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrCacheLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.namespace_id, 'x', sizeof(invalid.namespace_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_cache_layout_valid(&invalid)) ||
            umi_dr_cache_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated namespace_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrCacheLayoutTransferCases, UmiDrCacheLayout,
    umi_dr_cache_layout_archive_encode, umi_dr_cache_layout_archive_decode,
    UmiDrCacheLayoutTransferEqual, UmiDrCacheLayoutTransferTails, UmiDrCacheLayoutTransferMalformed)

int main(void) {
    UmiDrCacheLayout value; umi_dr_cache_layout_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"cache")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.namespace_id,sizeof(value.namespace_id),"studio")==UMI_STATUS_OK); value.max_bytes=1024U; CHECK(umi_dr_cache_layout_valid(&value));
    if (UmiDrCacheLayoutTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_cache_layout_fingerprint(&value) != 0U);
    return 0;
}
