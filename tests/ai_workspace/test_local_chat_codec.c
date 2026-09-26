/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_local_chat_codec.c
 *
 * PURPOSE:
 *   Exercise the real optional JSON codec with valid and hostile replies without contacting a model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "local_chat_internal.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(expression) do { if (!(expression)) { fprintf(stderr, "line %d: %s\n", __LINE__, #expression); return 1; } } while (0)
typedef struct WireCase { const char *name; const char *json; UmiStatus expected; } WireCase;
static const WireCase CASES[] = {
    {"valid_reply", "{\"model\":\"test-model\",\"choices\":[{\"message\":{\"role\":\"assistant\",\"content\":\"A local draft.\"},\"finish_reason\":\"stop\"}],\"usage\":{\"prompt_tokens\":2,\"completion_tokens\":3,\"total_tokens\":5}}", UMI_STATUS_OK},
    {"unicode_reply", "{\"model\":\"test-model\",\"choices\":[{\"message\":{\"role\":\"assistant\",\"content\":\"Café — مرحبا £10\"},\"finish_reason\":\"stop\"}]}", UMI_STATUS_OK},
    {"length_finish", "{\"choices\":[{\"message\":{\"role\":\"assistant\",\"content\":\"Partial text\"},\"finish_reason\":\"length\"}]}", UMI_STATUS_OK},
    {"malformed_json", "{\"choices\": [", UMI_STATUS_PARSE_ERROR},
    {"trailing_json", "{\"choices\":[]} {}", UMI_STATUS_PARSE_ERROR},
    {"empty_choices", "{\"choices\":[]}", UMI_STATUS_PARSE_ERROR},
    {"null_content", "{\"choices\":[{\"message\":{\"role\":\"assistant\",\"content\":null},\"finish_reason\":\"stop\"}]}", UMI_STATUS_PARSE_ERROR},
    {"wrong_role", "{\"choices\":[{\"message\":{\"role\":\"system\",\"content\":\"Bad\"},\"finish_reason\":\"stop\"}]}", UMI_STATUS_PARSE_ERROR},
    {"tool_call_rejected", "{\"choices\":[{\"message\":{\"role\":\"assistant\",\"content\":\"Call\",\"tool_calls\":[{}]},\"finish_reason\":\"stop\"}]}", UMI_STATUS_PERMISSION_DENIED},
    {"function_call_rejected", "{\"choices\":[{\"message\":{\"role\":\"assistant\",\"content\":\"Call\",\"function_call\":{}},\"finish_reason\":\"stop\"}]}", UMI_STATUS_PERMISSION_DENIED},
    {"unsupported_finish", "{\"choices\":[{\"message\":{\"role\":\"assistant\",\"content\":\"text\"},\"finish_reason\":\"tool_calls\"}]}", UMI_STATUS_PARSE_ERROR},
    {"wrong_model", "{\"model\":\"other-model\",\"choices\":[{\"message\":{\"role\":\"assistant\",\"content\":\"text\"},\"finish_reason\":\"stop\"}]}", UMI_STATUS_PARSE_ERROR},
    {"embedded_nul", "{\"choices\":[{\"message\":{\"role\":\"assistant\",\"content\":\"x\\u0000y\"},\"finish_reason\":\"stop\"}]}", UMI_STATUS_PARSE_ERROR},
    {"usage_mismatch", "{\"choices\":[{\"message\":{\"role\":\"assistant\",\"content\":\"text\"},\"finish_reason\":\"stop\"}],\"usage\":{\"prompt_tokens\":2,\"completion_tokens\":3,\"total_tokens\":4}}", UMI_STATUS_PARSE_ERROR},
    {"usage_negative", "{\"choices\":[{\"message\":{\"role\":\"assistant\",\"content\":\"text\"},\"finish_reason\":\"stop\"}],\"usage\":{\"prompt_tokens\":-2,\"completion_tokens\":3,\"total_tokens\":1}}", UMI_STATUS_PARSE_ERROR},
    {"error_payload", "{\"error\":{\"message\":\"unavailable\"}}", UMI_STATUS_PARSE_ERROR},
    {"plain_action_text_is_inert", "{\"choices\":[{\"message\":{\"role\":\"assistant\",\"content\":\"Please run a shell command\"},\"finish_reason\":\"stop\"}]}", UMI_STATUS_OK},
};
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!UmiAiWorkspaceLocalProviderAvailable()) return 77;
    UmiAiRequest *request = calloc(1U, sizeof(*request)); CHECK(request != NULL);
    strcpy(request->request_id, "wire.test"); strcpy(request->model_id, "test-model"); request->message_count = 1U;
    request->max_output_tokens = 64U; request->messages[0].role = UMI_AI_ROLE_USER;
    strcpy(request->messages[0].text, "Quoted \"text\"\nPath C:\\practice");
    int result = 2; char *encoded = NULL;
    if (strcmp(argv[1], "encode_escaping") == 0) {
        result = AwLocalChatEncode(request, &encoded) == UMI_STATUS_OK &&
            strstr(encoded, "\\\"text\\\"") != NULL && strstr(encoded, "\\nPath") != NULL &&
            strstr(encoded, "\"stream\":false") != NULL && strstr(encoded, "tools") == NULL ? 0 : 1;
    } else if (strcmp(argv[1], "encode_tools_disabled") == 0) {
        request->allow_tools = 1; result = AwLocalChatEncode(request, &encoded) == UMI_STATUS_INVALID_ARGUMENT ? 0 : 1;
    } else if (strcmp(argv[1], "encode_role_rejected") == 0) {
        request->messages[0].role = UMI_AI_ROLE_TOOL; result = AwLocalChatEncode(request, &encoded) == UMI_STATUS_PERMISSION_DENIED ? 0 : 1;
    } else if (strcmp(argv[1], "encode_temperature_rejected") == 0) {
        request->temperature = NAN; result = AwLocalChatEncode(request, &encoded) == UMI_STATUS_INVALID_ARGUMENT ? 0 : 1;
    } else if (strcmp(argv[1], "encode_count_rejected") == 0) {
        request->message_count = UMI_AI_MAX_MESSAGES + 1U; result = AwLocalChatEncode(request, &encoded) == UMI_STATUS_INVALID_ARGUMENT ? 0 : 1;
    } else if (strcmp(argv[1], "encode_body_capacity") == 0) {
        request->message_count = UMI_AI_MAX_MESSAGES;
        for (size_t i = 0U; i < request->message_count; ++i) {
            request->messages[i].role = UMI_AI_ROLE_USER;
            memset(request->messages[i].text, '\n', sizeof(request->messages[i].text) - 1U);
        }
        result = AwLocalChatEncode(request, &encoded) == UMI_STATUS_CAPACITY_EXCEEDED ? 0 : 1;
    } else if (strcmp(argv[1], "reply_over_capacity") == 0) {
        UmiAiResponse response = {0}; char *body = calloc(5000U, 1U); CHECK(body != NULL);
        strcpy(body, "{\"choices\":[{\"message\":{\"role\":\"assistant\",\"content\":\"");
        size_t offset = strlen(body); memset(body + offset, 'a', 4096U);
        strcpy(body + offset + 4096U, "\"},\"finish_reason\":\"stop\"}]}");
        result = AwLocalChatDecode(request, body, strlen(body), &response) == UMI_STATUS_PARSE_ERROR ? 0 : 1; free(body);
    } else {
        for (size_t i = 0U; i < sizeof(CASES) / sizeof(CASES[0]); ++i) if (strcmp(argv[1], CASES[i].name) == 0) {
            UmiAiResponse response, before; memset(&response, 0x5A, sizeof(response)); before = response;
            UmiStatus status = AwLocalChatDecode(request, CASES[i].json, strlen(CASES[i].json), &response);
            result = status == CASES[i].expected ? 0 : 1;
            if (result) fprintf(stderr, "case %s: expected %d, got %d\n", CASES[i].name, (int)CASES[i].expected, (int)status);
            if (status != UMI_STATUS_OK && memcmp(&response, &before, sizeof(response)) != 0) result = 1;
            if (status == UMI_STATUS_OK && (strcmp(response.request_id, request->request_id) != 0 ||
                strcmp(response.provider_id, "umicom.local-chat") != 0)) result = 1;
            break;
        }
    }
    AwLocalChatFree(encoded); free(request); return result;
}
