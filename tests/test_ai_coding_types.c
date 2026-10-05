/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_coding_types.c
 *
 * PURPOSE:
 *   Verify stable coding task vocabulary, request validation, path containment
 *   and deterministic content hashes.
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

#include "umicom/ai/coding_types.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "value_archive/transfer_cases.h"

#include "umicom/ai/coding_types.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAiCodingRequestTransferEqual(const UmiAiCodingRequest *a, const UmiAiCodingRequest *b)
{
    return a->structure_size == b->structure_size &&
        a->abi_version == b->abi_version &&
        a->task == b->task &&
        a->classification == b->classification &&
        strcmp(a->request_id, b->request_id) == 0 &&
        strcmp(a->session_id, b->session_id) == 0 &&
        strcmp(a->runtime_id, b->runtime_id) == 0 &&
        strcmp(a->workspace_root, b->workspace_root) == 0 &&
        strcmp(a->active_path, b->active_path) == 0 &&
        strcmp(a->language_id, b->language_id) == 0 &&
        strcmp(a->instruction, b->instruction) == 0 &&
        a->selection_start_line == b->selection_start_line &&
        a->selection_end_line == b->selection_end_line &&
        a->context_token_budget == b->context_token_budget &&
        a->maximum_context_files == b->maximum_context_files &&
        a->timestamp_ns == b->timestamp_ns &&
        a->sensitive_approved == b->sensitive_approved;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAiCodingRequestTransferTails(UmiAiCodingRequest *value)
{
    (void)value;
    {
        size_t used = strlen(value->request_id) + 1U;
        memset(value->request_id + used, 0xa5, sizeof(value->request_id) - used);
    }
    {
        size_t used = strlen(value->session_id) + 1U;
        memset(value->session_id + used, 0xa5, sizeof(value->session_id) - used);
    }
    {
        size_t used = strlen(value->runtime_id) + 1U;
        memset(value->runtime_id + used, 0xa5, sizeof(value->runtime_id) - used);
    }
    {
        size_t used = strlen(value->workspace_root) + 1U;
        memset(value->workspace_root + used, 0xa5, sizeof(value->workspace_root) - used);
    }
    {
        size_t used = strlen(value->active_path) + 1U;
        memset(value->active_path + used, 0xa5, sizeof(value->active_path) - used);
    }
    {
        size_t used = strlen(value->language_id) + 1U;
        memset(value->language_id + used, 0xa5, sizeof(value->language_id) - used);
    }
    {
        size_t used = strlen(value->instruction) + 1U;
        memset(value->instruction + used, 0xa5, sizeof(value->instruction) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAiCodingRequestTransferMalformed(const UmiAiCodingRequest *sample)
{
    (void)sample;
    {
        UmiAiCodingRequest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.request_id, 'x', sizeof(invalid.request_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ai_coding_request_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ai_coding_request_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated request_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAiCodingRequest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.session_id, 'x', sizeof(invalid.session_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ai_coding_request_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ai_coding_request_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated session_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAiCodingRequest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.runtime_id, 'x', sizeof(invalid.runtime_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ai_coding_request_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ai_coding_request_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated runtime_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAiCodingRequest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.workspace_root, 'x', sizeof(invalid.workspace_root));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ai_coding_request_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ai_coding_request_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated workspace_root was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAiCodingRequest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.active_path, 'x', sizeof(invalid.active_path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ai_coding_request_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ai_coding_request_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated active_path was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAiCodingRequest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.language_id, 'x', sizeof(invalid.language_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ai_coding_request_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ai_coding_request_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated language_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAiCodingRequest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.instruction, 'x', sizeof(invalid.instruction));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ai_coding_request_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ai_coding_request_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated instruction was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAiCodingRequestTransferCases, UmiAiCodingRequest,
    umi_ai_coding_request_archive_encode, umi_ai_coding_request_archive_decode,
    UmiAiCodingRequestTransferEqual, UmiAiCodingRequestTransferTails, UmiAiCodingRequestTransferMalformed)

int main(void)
{
    UmiAiCodingRequest request;
    umi_ai_coding_request_init(&request, UMI_AI_CODING_TASK_REFACTOR);
    (void)strcpy(request.request_id, "request.48");
    (void)strcpy(request.session_id, "studio.session.default");
    (void)strcpy(request.runtime_id, "authorengine.local.chat");
    (void)strcpy(request.workspace_root, "C:\\Dev\\umicom\\umicom-studio");
    (void)strcpy(request.active_path, "applications/studio/src/app/main.c");
    (void)strcpy(request.language_id, "c23");
    (void)strcpy(request.instruction, "Extract the repeated validation logic.");
    request.selection_start_line = 10U;
    request.selection_end_line = 30U;

    assert(umi_ai_coding_request_validate(&request) == UMI_STATUS_OK);
    if (UmiAiCodingRequestTransferCases(&request) != 0) return 1;

    assert(umi_ai_coding_path_is_safe_relative("src/app.c"));
    assert(umi_ai_coding_path_is_safe_relative("src\\app.c"));
    assert(!umi_ai_coding_path_is_safe_relative("../secret.txt"));
    assert(!umi_ai_coding_path_is_safe_relative("C:\\secret.txt"));
    assert(!umi_ai_coding_path_is_safe_relative("/etc/passwd"));
    assert(umi_ai_coding_text_hash("abc", 3U) ==
           umi_ai_coding_text_hash("abc", 3U));
    assert(strcmp(umi_ai_coding_task_kind_text(request.task), "refactoring") == 0);
    return 0;
}
