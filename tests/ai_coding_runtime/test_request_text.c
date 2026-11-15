/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_request_text.c
 *
 * PURPOSE:
 *   Verify bounded request-text storage independently from execution readiness.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/coding_types.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

static const UmiSnapshotTextField fields[] = {
    {"request-id", offsetof(UmiAiCodingRequest, request_id), sizeof(((UmiAiCodingRequest *)0)->request_id), 0},
    {"session-id", offsetof(UmiAiCodingRequest, session_id), sizeof(((UmiAiCodingRequest *)0)->session_id), 0},
    {"runtime-id", offsetof(UmiAiCodingRequest, runtime_id), sizeof(((UmiAiCodingRequest *)0)->runtime_id), 0},
    {"workspace", offsetof(UmiAiCodingRequest, workspace_root), sizeof(((UmiAiCodingRequest *)0)->workspace_root), 0},
    {"active-path", offsetof(UmiAiCodingRequest, active_path), sizeof(((UmiAiCodingRequest *)0)->active_path), 0},
    {"language", offsetof(UmiAiCodingRequest, language_id), sizeof(((UmiAiCodingRequest *)0)->language_id), 0},
    {"instruction", offsetof(UmiAiCodingRequest, instruction), sizeof(((UmiAiCodingRequest *)0)->instruction), 0}
};

int main(int argc, char **argv)
{
    UmiAiCodingRequest request, before;
    umi_ai_coding_request_init(&request, UMI_AI_CODING_TASK_CHAT);
    CHECK(argc >= 2);
    if (strcmp(argv[1], "null") == 0) {
        CHECK(UmiAiCodingRequestValidateText(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_coding_request_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    if (strcmp(argv[1], "draft") == 0) {
        memcpy(&before, &request, sizeof before);
        CHECK(UmiAiCodingRequestValidateText(&request) == UMI_STATUS_OK);
        CHECK(umi_ai_coding_request_validate(&request) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&request, &before, sizeof request) == 0);
        return 0;
    }
    if (strcmp(argv[1], "opaque") == 0) {
        /* Text storage does not reinterpret domain or encoding metadata. */
        request.abi_version = UINT32_MAX;
        request.maximum_context_files = SIZE_MAX;
        request.task = (UmiAiCodingTaskKind)99;
        request.instruction[0] = (char)0xff;
        memcpy(&before, &request, sizeof before);
        CHECK(UmiAiCodingRequestValidateText(&request) == UMI_STATUS_OK);
        CHECK(umi_ai_coding_request_validate(&request) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&request, &before, sizeof request) == 0);
        return 0;
    }
    CHECK(argc == 3);
    for (size_t i = 0U; i < sizeof fields / sizeof fields[0]; ++i) {
        if (strcmp(argv[2], fields[i].name) != 0) continue;
        char *field = (char *)&request + fields[i].offset;
        UmiStatus expected = UMI_STATUS_OK;
        if (strcmp(argv[1], "unterminated") == 0) {
            memset(field, 'x', fields[i].capacity);
            expected = UMI_STATUS_INVALID_ARGUMENT;
        } else if (strcmp(argv[1], "last-byte") == 0) {
            memset(field, 'x', fields[i].capacity);
            field[fields[i].capacity - 1U] = '\0';
        } else if (strcmp(argv[1], "empty") != 0) return 2;
        memcpy(&before, &request, sizeof before);
        CHECK(UmiAiCodingRequestValidateText(&request) == expected);
        CHECK(memcmp(&before, &request, sizeof before) == 0);
        return 0;
    }
    return 2;
}
