/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_config.c
 *
 * PURPOSE:
 *   Verify the reusable AI coding runtime config contract.
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
#include <assert.h>
#include <string.h>
#include "umicom/ai_coding_runtime/config.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ai_coding_runtime/config.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAiCodingRuntimeConfigTransferEqual(const UmiAiCodingRuntimeConfig *a, const UmiAiCodingRuntimeConfig *b)
{
    return strcmp(a->provider_id, b->provider_id) == 0 &&
        strcmp(a->model_id, b->model_id) == 0 &&
        a->provider_kind == b->provider_kind &&
        a->context_token_budget == b->context_token_budget &&
        a->max_output_tokens == b->max_output_tokens &&
        a->maximum_iterations == b->maximum_iterations &&
        a->maximum_context_files == b->maximum_context_files &&
        a->temperature == b->temperature &&
        a->allow_tools == b->allow_tools &&
        a->auto_apply_approved_patch == b->auto_apply_approved_patch &&
        a->auto_approve == b->auto_approve &&
        a->rollback_on_validation_failure == b->rollback_on_validation_failure &&
        a->require_validation == b->require_validation &&
        a->allow_sensitive_context == b->allow_sensitive_context;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAiCodingRuntimeConfigTransferTails(UmiAiCodingRuntimeConfig *value)
{
    (void)value;
    {
        size_t used = strlen(value->provider_id) + 1U;
        memset(value->provider_id + used, 0xa5, sizeof(value->provider_id) - used);
    }
    {
        size_t used = strlen(value->model_id) + 1U;
        memset(value->model_id + used, 0xa5, sizeof(value->model_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAiCodingRuntimeConfigTransferMalformed(const UmiAiCodingRuntimeConfig *sample)
{
    (void)sample;
    {
        UmiAiCodingRuntimeConfig invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.provider_id, 'x', sizeof(invalid.provider_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ai_coding_runtime_config_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ai_coding_runtime_config_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated provider_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAiCodingRuntimeConfig invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.model_id, 'x', sizeof(invalid.model_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ai_coding_runtime_config_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ai_coding_runtime_config_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated model_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAiCodingRuntimeConfigTransferCases, UmiAiCodingRuntimeConfig,
    umi_ai_coding_runtime_config_archive_encode, umi_ai_coding_runtime_config_archive_decode,
    UmiAiCodingRuntimeConfigTransferEqual, UmiAiCodingRuntimeConfigTransferTails, UmiAiCodingRuntimeConfigTransferMalformed)

int main(void)
{

    UmiAiCodingRuntimeConfig config;
    umi_ai_coding_runtime_config_init(&config);
    assert(config.maximum_iterations == 3U);
    assert(config.rollback_on_validation_failure == 1);
    (void)strcpy(config.provider_id, "provider");
    (void)strcpy(config.model_id, "model");
    assert(umi_ai_coding_runtime_config_validate(&config) == UMI_STATUS_OK);
    if (UmiAiCodingRuntimeConfigTransferCases(&config) != 0) return 1;


    return 0;
}
