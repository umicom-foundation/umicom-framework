/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/config.c
 *
 * PURPOSE:
 *   Implement conservative defaults and validation for AI coding execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/config.h"
#include "../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise ai coding runtime config from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_coding_runtime_config_init(UmiAiCodingRuntimeConfig *config)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (config == NULL) return;

    (void)memset(config, 0, sizeof(*config));
    config->provider_kind = UMI_AI_PROVIDER_LOCAL;
    config->context_token_budget = 24000U;
    config->max_output_tokens = 8192U;
    config->maximum_iterations = 3U;
    config->maximum_context_files = 12U;
    config->temperature = 0.15;
    config->allow_tools = 0;
    config->auto_apply_approved_patch = 0;
    config->auto_approve = 0;
    config->rollback_on_validation_failure = 1;
    config->require_validation = 1;
    config->allow_sensitive_context = 0;
}

/*
 * Check that ai coding runtime config satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_ai_coding_runtime_config_validate(
    const UmiAiCodingRuntimeConfig *config)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (config == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(config->provider_id, '\0', sizeof(config->provider_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(config->model_id, '\0', sizeof(config->model_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (config == NULL ||
        config->provider_id[0] == '\0' ||
        config->model_id[0] == '\0' ||
        config->context_token_budget == 0U ||
        config->max_output_tokens == 0U ||
        config->maximum_iterations == 0U ||
        config->maximum_iterations > UMI_AI_CODING_RUNTIME_MAX_ITERATIONS ||
        config->maximum_context_files == 0U ||
        config->maximum_context_files > UMI_AI_CODING_RUNTIME_CONTEXT_CAPACITY ||
        config->temperature < 0.0 ||
        config->temperature > 2.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAiCodingRuntimeConfigArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x38a527ba6f366d58);
    schema = (schema ^ (uint64_t)sizeof(((UmiAiCodingRuntimeConfig *)0)->provider_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAiCodingRuntimeConfig *)0)->model_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAiCodingRuntimeConfigArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAiCodingRuntimeConfig *)0)->provider_id) - 1U +
        8U + sizeof(((UmiAiCodingRuntimeConfig *)0)->model_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAiCodingRuntimeConfigArchiveWrite(UmiArchiveWriter *writer, const UmiAiCodingRuntimeConfig *value)
{
    UmiArchiveWriteText(writer, value->provider_id, sizeof(value->provider_id));
    UmiArchiveWriteText(writer, value->model_id, sizeof(value->model_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->provider_kind);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->context_token_budget);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->max_output_tokens);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->maximum_iterations);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->maximum_context_files);
    UmiArchiveWriteDouble(writer, value->temperature);
    UmiArchiveWriteSigned(writer, (int64_t)value->allow_tools);
    UmiArchiveWriteSigned(writer, (int64_t)value->auto_apply_approved_patch);
    UmiArchiveWriteSigned(writer, (int64_t)value->auto_approve);
    UmiArchiveWriteSigned(writer, (int64_t)value->rollback_on_validation_failure);
    UmiArchiveWriteSigned(writer, (int64_t)value->require_validation);
    UmiArchiveWriteSigned(writer, (int64_t)value->allow_sensitive_context);
}
static void UmiAiCodingRuntimeConfigArchiveRead(UmiArchiveReader *reader, UmiAiCodingRuntimeConfig *value)
{
    UmiArchiveReadText(reader, value->provider_id, sizeof(value->provider_id));
    UmiArchiveReadText(reader, value->model_id, sizeof(value->model_id));
    value->provider_kind = (UmiAiProviderKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->context_token_budget = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->max_output_tokens = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->maximum_iterations = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->maximum_context_files = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->temperature = UmiArchiveReadDouble(reader);
    value->allow_tools = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->auto_apply_approved_patch = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->auto_approve = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->rollback_on_validation_failure = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->require_validation = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->allow_sensitive_context = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiAiCodingRuntimeConfigArchiveValidate(const UmiAiCodingRuntimeConfig *value)
{
    return umi_ai_coding_runtime_config_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ai_coding_runtime_config_archive_encode, umi_ai_coding_runtime_config_archive_decode,
    UmiAiCodingRuntimeConfig, UmiAiCodingRuntimeConfigArchiveSchema, UmiAiCodingRuntimeConfigArchiveBound, UmiAiCodingRuntimeConfigArchiveWrite, UmiAiCodingRuntimeConfigArchiveRead, UmiAiCodingRuntimeConfigArchiveValidate)
