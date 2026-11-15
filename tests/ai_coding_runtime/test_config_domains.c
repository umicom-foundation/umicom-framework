/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_config_domains.c
 *
 * PURPOSE:
 *   Verify finite temperature and provider domains through direct admission and all typed archive decoders.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai_coding_runtime/config.h"
/* Only the shared envelope helpers are used to create checksummed hostile
 * input. All domain admission and publication checks use public APIs. */
#include "../../src/base/value_archive_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#define REQUIRE(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); goto cleanup; \
} } while (0)

static int set_case(UmiAiCodingRuntimeConfig *value, const char *mode)
{
    if (strcmp(mode, "local") == 0) value->provider_kind = UMI_AI_PROVIDER_LOCAL;
    else if (strcmp(mode, "remote") == 0) value->provider_kind = UMI_AI_PROVIDER_REMOTE;
    else if (strcmp(mode, "author") == 0) value->provider_kind = UMI_AI_PROVIDER_AUTHOR_ENGINE;
    else if (strcmp(mode, "test") == 0) value->provider_kind = UMI_AI_PROVIDER_TEST;
    else if (strcmp(mode, "zero") == 0) value->temperature = 0.0;
    else if (strcmp(mode, "negative-zero") == 0) value->temperature = -0.0;
    else if (strcmp(mode, "maximum") == 0) value->temperature = 2.0;
    else if (strcmp(mode, "flags") == 0) {
        value->allow_tools = -1; value->auto_apply_approved_patch = INT_MAX;
        value->auto_approve = -7; value->rollback_on_validation_failure = 2;
        value->require_validation = INT_MIN; value->allow_sensitive_context = 42;
    } else {
        if (strcmp(mode, "provider-zero") == 0) value->provider_kind = (UmiAiProviderKind)0;
        else if (strcmp(mode, "provider-negative") == 0) value->provider_kind = (UmiAiProviderKind)-1;
        else if (strcmp(mode, "provider-large") == 0) value->provider_kind = (UmiAiProviderKind)INT_MAX;
        else if (strcmp(mode, "nan") == 0) value->temperature = NAN;
        else if (strcmp(mode, "positive-infinity") == 0) value->temperature = INFINITY;
        else if (strcmp(mode, "negative-infinity") == 0) value->temperature = -INFINITY;
        else if (strcmp(mode, "below-range") == 0) value->temperature = -0.01;
        else if (strcmp(mode, "above-range") == 0) value->temperature = 2.01;
        else return -1;
        return 0;
    }
    return 1;
}

static int equivalent(const UmiAiCodingRuntimeConfig *a, const UmiAiCodingRuntimeConfig *b)
{
    return strcmp(a->provider_id, b->provider_id) == 0 && strcmp(a->model_id, b->model_id) == 0 &&
        a->provider_kind == b->provider_kind && a->temperature == b->temperature &&
        !!signbit(a->temperature) == !!signbit(b->temperature) &&
        a->allow_tools == b->allow_tools && a->auto_apply_approved_patch == b->auto_apply_approved_patch &&
        a->auto_approve == b->auto_approve && a->rollback_on_validation_failure == b->rollback_on_validation_failure &&
        a->require_validation == b->require_validation && a->allow_sensitive_context == b->allow_sensitive_context &&
        a->context_token_budget == b->context_token_budget && a->max_output_tokens == b->max_output_tokens &&
        a->maximum_iterations == b->maximum_iterations && a->maximum_context_files == b->maximum_context_files;
}

int main(int argc, char **argv)
{
    UmiAiCodingRuntimeConfig seed, candidate, output, before, scratch;
    unsigned char *bytes = NULL, *refused = NULL, *refused_before = NULL;
    size_t size = 0U, written = 0U;
    int result = 1;
    REQUIRE(argc == 2);
    umi_ai_coding_runtime_config_init(&seed);
    strcpy(seed.provider_id, "provider"); strcpy(seed.model_id, "model");
    candidate = seed;
    int valid = set_case(&candidate, argv[1]);
    REQUIRE(valid >= 0);
    REQUIRE(umi_ai_coding_runtime_config_validate(&candidate) ==
        (valid ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT));
    REQUIRE(umi_ai_coding_runtime_config_archive_encode(&seed, NULL, 0U, &size) == UMI_STATUS_OK);
    bytes = malloc(size); refused = malloc(size); refused_before = malloc(size);
    REQUIRE(bytes != NULL && refused != NULL && refused_before != NULL);
    REQUIRE(umi_ai_coding_runtime_config_archive_encode(valid ? &candidate : &seed,
        bytes, size, &written) == UMI_STATUS_OK && written == size);
    if (!valid) {
        memset(refused, 0xA5, size); memcpy(refused_before, refused, size); written = 77U;
        REQUIRE(umi_ai_coding_runtime_config_archive_encode(&candidate, refused, size, &written) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(written == 77U && memcmp(refused, refused_before, size) == 0);
        /* The field order is the declared config wire schema: two length-
         * prefixed strings, kind, four integer settings, then temperature. */
        size_t kind_offset = UMI_VALUE_ARCHIVE_HEADER_SIZE + 8U + strlen(seed.provider_id) +
            8U + strlen(seed.model_id);
        size_t temperature_offset = kind_offset + 5U * 8U;
        REQUIRE(temperature_offset + 8U <= size);
        if (strncmp(argv[1], "provider-", 9U) == 0)
            UmiArchiveStoreInteger(bytes + kind_offset, (uint64_t)(int64_t)(int)candidate.provider_kind, 8U);
        else {
            uint64_t bits = 0U;
            REQUIRE(UmiArchiveBinaryDouble());
            memcpy(&bits, &candidate.temperature, sizeof bits);
            UmiArchiveStoreInteger(bytes + temperature_offset, bits, 8U);
        }
        UmiArchiveStoreInteger(bytes + 24U, UmiArchiveChecksum(bytes, size), 4U);
        UmiValueArchiveInfo info;
        REQUIRE(umi_value_archive_inspect(bytes, size, &info) == UMI_STATUS_OK);
    }
    for (int decoder = 0; decoder < 3; ++decoder) {
        memcpy(&output, &seed, sizeof output); memcpy(&before, &output, sizeof before);
        UmiStatus status;
        if (decoder == 0) status = umi_ai_coding_runtime_config_archive_decode(bytes, size, &output);
        else if (decoder == 1) status = umi_ai_coding_runtime_config_archive_decode_with_workspace(bytes, size, &scratch, &output);
        else status = umi_ai_coding_runtime_config_archive_decode_if_unchanged(bytes, size, &before, &output);
        if (valid) REQUIRE(status == UMI_STATUS_OK && equivalent(&candidate, &output));
        else {
            REQUIRE(status == (isfinite(candidate.temperature) ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_PARSE_ERROR));
            REQUIRE(memcmp(&before, &output, sizeof before) == 0);
        }
    }
    result = 0;
cleanup:
    free(bytes); free(refused); free(refused_before); return result;
}
