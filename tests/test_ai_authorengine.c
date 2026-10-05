/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_authorengine.c
 *
 * PURPOSE:
 *   Validate the reusable AI and Author Engine configuration foundation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This focused executable keeps one contract easy to diagnose when the larger test suite reports a failure.
 */



/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>
#include "umicom/umicom.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "value_archive/transfer_cases.h"

#include "umicom/ai/authorengine.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAiAuthorEngineConfigTransferEqual(const UmiAiAuthorEngineConfig *a, const UmiAiAuthorEngineConfig *b)
{
    return strcmp(a->executable, b->executable) == 0 &&
        strcmp(a->workspace, b->workspace) == 0 &&
        strcmp(a->provider, b->provider) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAiAuthorEngineConfigTransferTails(UmiAiAuthorEngineConfig *value)
{
    (void)value;
    {
        size_t used = strlen(value->executable) + 1U;
        memset(value->executable + used, 0xa5, sizeof(value->executable) - used);
    }
    {
        size_t used = strlen(value->workspace) + 1U;
        memset(value->workspace + used, 0xa5, sizeof(value->workspace) - used);
    }
    {
        size_t used = strlen(value->provider) + 1U;
        memset(value->provider + used, 0xa5, sizeof(value->provider) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAiAuthorEngineConfigTransferMalformed(const UmiAiAuthorEngineConfig *sample)
{
    (void)sample;
    {
        UmiAiAuthorEngineConfig invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.executable, 'x', sizeof(invalid.executable));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ai_authorengine_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ai_authorengine_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated executable was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAiAuthorEngineConfig invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.workspace, 'x', sizeof(invalid.workspace));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ai_authorengine_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ai_authorengine_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated workspace was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAiAuthorEngineConfig invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.provider, 'x', sizeof(invalid.provider));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ai_authorengine_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ai_authorengine_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated provider was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAiAuthorEngineConfigTransferCases, UmiAiAuthorEngineConfig,
    umi_ai_authorengine_archive_encode, umi_ai_authorengine_archive_decode,
    UmiAiAuthorEngineConfigTransferEqual, UmiAiAuthorEngineConfigTransferTails, UmiAiAuthorEngineConfigTransferMalformed)

int main(void)
{
    UmiAiAuthorEngineConfig config = {0};
    (void)strcpy(config.executable, "uaengine");
    (void)strcpy(config.workspace, ".");
    (void)strcpy(config.provider, "local");
    assert(umi_ai_authorengine_validate(&config) == UMI_STATUS_OK);
    if (UmiAiAuthorEngineConfigTransferCases(&config) != 0) return 1;

    return 0;
}
