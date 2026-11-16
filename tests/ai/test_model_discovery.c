/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai/test_model_discovery.c
 *
 * PURPOSE:
 *   Verify discovery evidence, immutable ownership and refusal of ambiguous or oversized provider model lists.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/model_discovery.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(e) do { if (!(e)) { fprintf(stderr, "%d: %s\n", __LINE__, #e); return 1; } } while (0)
static const char openai[] = "{\"object\":\"list\",\"data\":[{\"id\":\"example\",\"object\":\"model\"}]}";
static const char available[] = "{\"models\":[{\"model\":\"example:tag\",\"name\":\"Example\",\"size\":1234,\"digest\":\"opaque-digest\",\"modified_at\":\"2026-01-01T00:00:00Z\",\"details\":{\"format\":\"gguf\",\"family\":\"example\",\"parameter_size\":\"1B\",\"quantization_level\":\"Q4\"}}]}";
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1], *body = openai;
    char scratch[4096] = {0};
    char *large = NULL;
    UmiAiDiscoveryFormat format = UMI_AI_DISCOVERY_OPENAI_LIST;
    UmiStatus expected = UMI_STATUS_OK;
    UmiAiDiscoverySnapshot *snapshot = NULL;
    UmiCancellationToken *cancel = NULL;
    if (strcmp(mode, "openai") == 0 || strcmp(mode, "ownership") == 0 || strcmp(mode, "read-refusal") == 0) {}
    else if (strcmp(mode, "available") == 0) { format = UMI_AI_DISCOVERY_OLLAMA_AVAILABLE; body = available; }
    else if (strcmp(mode, "loaded") == 0) { format = UMI_AI_DISCOVERY_OLLAMA_LOADED; body = "{\"models\":[{\"model\":\"example\",\"size\":10,\"size_vram\":7,\"context_length\":4096,\"expires_at\":\"later\"}]}"; }
    else if (strcmp(mode, "empty") == 0) body = "{\"object\":\"list\",\"data\":[]}";
    else if (strcmp(mode, "source-order") == 0) { format = UMI_AI_DISCOVERY_OLLAMA_AVAILABLE; body = "{\"models\":[{\"model\":\"z\"},{\"model\":\"a\"},{\"model\":\"m\"}]}"; }
    else if (strcmp(mode, "case-sensitive") == 0) { format = UMI_AI_DISCOVERY_OLLAMA_AVAILABLE; body = "{\"models\":[{\"model\":\"A\"},{\"model\":\"a\"}]}"; }
    else if (strcmp(mode, "unicode") == 0) { format = UMI_AI_DISCOVERY_OLLAMA_AVAILABLE; body = "{\"models\":[{\"model\":\"caf\\u00e9\"}]}"; }
    else if (strcmp(mode, "max-id") == 0 || strcmp(mode, "long-id") == 0) {
        char id[UMI_AI_DISCOVERY_ID_CAPACITY + 1U];
        memset(id, 'x', sizeof(id));
        id[strcmp(mode, "max-id") == 0 ? UMI_AI_DISCOVERY_ID_CAPACITY - 1U : UMI_AI_DISCOVERY_ID_CAPACITY] = '\0';
        CHECK(snprintf(scratch, sizeof(scratch), "{\"models\":[{\"model\":\"%s\"}]}", id) > 0);
        body = scratch; format = UMI_AI_DISCOVERY_OLLAMA_AVAILABLE;
        if (strcmp(mode, "long-id") == 0) expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(mode, "duplicate-model") == 0 || strcmp(mode, "duplicate-escaped-model") == 0) {
        format = UMI_AI_DISCOVERY_OLLAMA_AVAILABLE; expected = UMI_STATUS_ALREADY_EXISTS;
        body = strcmp(mode, "duplicate-model") == 0 ? "{\"models\":[{\"model\":\"a\"},{\"model\":\"a\"}]}" : "{\"models\":[{\"model\":\"a\"},{\"model\":\"\\u0061\"}]}";
    } else if (strcmp(mode, "duplicate-key") == 0) { body = "{\"object\":\"list\",\"data\":[],\"d\\u0061ta\":[]}"; expected = UMI_STATUS_ALREADY_EXISTS; }
    else if (strcmp(mode, "duplicate-extension") == 0) { body = "{\"object\":\"list\",\"data\":[],\"extension\":{\"x\":1,\"x\":2}}"; expected = UMI_STATUS_ALREADY_EXISTS; }
    else if (strcmp(mode, "wrong-root") == 0) { body = "[]"; expected = UMI_STATUS_PARSE_ERROR; }
    else if (strcmp(mode, "missing-list") == 0) { body = "{\"object\":\"list\"}"; expected = UMI_STATUS_PARSE_ERROR; }
    else if (strcmp(mode, "wrong-list") == 0) { body = "{\"object\":\"list\",\"data\":{}}"; expected = UMI_STATUS_PARSE_ERROR; }
    else if (strcmp(mode, "wrong-item") == 0) { body = "{\"object\":\"list\",\"data\":[1]}"; expected = UMI_STATUS_PARSE_ERROR; }
    else if (strcmp(mode, "missing-id") == 0) { body = "{\"object\":\"list\",\"data\":[{\"object\":\"model\"}]}"; expected = UMI_STATUS_PARSE_ERROR; }
    else if (strcmp(mode, "empty-id") == 0) { body = "{\"object\":\"list\",\"data\":[{\"object\":\"model\",\"id\":\"\"}]}"; expected = UMI_STATUS_PARSE_ERROR; }
    else if (strcmp(mode, "wrong-id") == 0) { body = "{\"object\":\"list\",\"data\":[{\"object\":\"model\",\"id\":42}]}"; expected = UMI_STATUS_PARSE_ERROR; }
    else if (strcmp(mode, "wrong-object") == 0) { body = "{\"object\":\"list\",\"data\":[{\"object\":\"other\",\"id\":\"a\"}]}"; expected = UMI_STATUS_PARSE_ERROR; }
    else if (strcmp(mode, "negative-size") == 0 || strcmp(mode, "fractional-size") == 0 || strcmp(mode, "overflow-size") == 0) {
        const char *value = strcmp(mode, "negative-size") == 0 ? "-1" : strcmp(mode, "fractional-size") == 0 ? "1.0" : "9223372036854775808";
        CHECK(snprintf(scratch, sizeof(scratch), "{\"models\":[{\"model\":\"a\",\"size\":%s}]}", value) > 0);
        body = scratch; format = UMI_AI_DISCOVERY_OLLAMA_AVAILABLE;
        expected = strcmp(mode, "overflow-size") == 0 ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_PARSE_ERROR;
    } else if (strcmp(mode, "wrong-details") == 0) { format = UMI_AI_DISCOVERY_OLLAMA_AVAILABLE; body = "{\"models\":[{\"model\":\"a\",\"details\":null}]}"; expected = UMI_STATUS_PARSE_ERROR; }
    else if (strcmp(mode, "null-name") == 0) { format = UMI_AI_DISCOVERY_OLLAMA_AVAILABLE; body = "{\"models\":[{\"model\":\"a\",\"name\":null}]}"; expected = UMI_STATUS_PARSE_ERROR; }
    else if (strcmp(mode, "known-zero") == 0) { format = UMI_AI_DISCOVERY_OLLAMA_LOADED; body = "{\"models\":[{\"model\":\"a\",\"size\":0,\"size_vram\":0,\"context_length\":0}]}"; }
    else if (strcmp(mode, "available-not-loaded") == 0) { format = UMI_AI_DISCOVERY_OLLAMA_AVAILABLE; body = "{\"models\":[{\"model\":\"a\",\"size_vram\":99,\"context_length\":100}]}"; }
    else if (strcmp(mode, "unknown-fields") == 0) { body = "{\"object\":\"list\",\"data\":[],\"future\":[1,true,null,{\"ok\":\"yes\"}]}"; }
    else if (strcmp(mode, "trailing") == 0) { body = "{\"object\":\"list\",\"data\":[]}true"; expected = UMI_STATUS_PARSE_ERROR; }
    else if (strcmp(mode, "embedded-nul") == 0) {
        memcpy(scratch, openai, sizeof(openai)); scratch[12] = '\0';
        CHECK(UmiAiDiscoveryDecode(format, scratch, sizeof(openai)-1U, NULL, &snapshot) == UMI_STATUS_PARSE_ERROR);
        CHECK(snapshot == NULL); return 0;
    } else if (strcmp(mode, "depth") == 0) {
        memset(scratch, '[', 40); memset(scratch+40, ']', 40); scratch[80] = '\0';
        body = scratch; expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(mode, "model-limit") == 0) {
        large = malloc(65536); CHECK(large != NULL);
        size_t used = (size_t)snprintf(large, 65536, "{\"models\":[");
        for (size_t i = 0; i <= UMI_AI_DISCOVERY_MAX_MODELS; ++i) {
            int wrote = snprintf(large+used, 65536-used, "%s{\"model\":\"m%zu\"}", i ? "," : "", i);
            CHECK(wrote > 0 && (size_t)wrote < 65536-used); used += (size_t)wrote;
        }
        memcpy(large+used, "]}", 3); body = large; format = UMI_AI_DISCOVERY_OLLAMA_AVAILABLE;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(mode, "nulls") == 0) {
        CHECK(UmiAiDiscoveryDecode(format, openai, strlen(openai), NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiDiscoveryDecode(format, NULL, 1, NULL, &snapshot) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiDiscoveryDecode(format, openai, 0, NULL, &snapshot) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiDiscoveryDecode((UmiAiDiscoveryFormat)0, openai, strlen(openai), NULL, &snapshot) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiDiscoveryDecode(format, openai, (size_t)UMI_AI_DISCOVERY_MAX_BYTES+1U, NULL, &snapshot) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(snapshot == NULL && UmiAiDiscoveryCount(NULL) == 0 && UmiAiDiscoveryFormatOf(NULL) == 0);
        UmiAiDiscoveryDestroy(NULL); return 0;
    } else if (strcmp(mode, "cancel") == 0) {
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        umi_cancellation_token_request(cancel); expected = UMI_STATUS_CANCELLED;
    } else CHECK(0);
    if (strcmp(mode, "ownership") == 0) { memcpy(scratch, openai, sizeof(openai)); body = scratch; }
    CHECK(UmiAiDiscoveryDecode(format, body, strlen(body), cancel, &snapshot) == expected);
    free(large); umi_cancellation_token_destroy(cancel);
    if (expected != UMI_STATUS_OK) { CHECK(snapshot == NULL); return 0; }
    CHECK(snapshot != NULL && UmiAiDiscoveryFormatOf(snapshot) == format);
    UmiAiDiscoveredModel model = {0}, before;
    if (strcmp(mode, "empty") == 0 || strcmp(mode, "unknown-fields") == 0) {
        CHECK(UmiAiDiscoveryCount(snapshot) == 0);
        CHECK(UmiAiDiscoveryAt(snapshot, 0, &model) == UMI_STATUS_NOT_FOUND);
    } else {
        if (strcmp(mode, "ownership") == 0) memset(scratch, 'x', sizeof(scratch));
        CHECK(UmiAiDiscoveryAt(snapshot, 0, &model) == UMI_STATUS_OK);
        if (strcmp(mode, "openai") == 0 || strcmp(mode, "ownership") == 0) CHECK(strcmp(model.id, "example") == 0 && model.known == 0);
        if (strcmp(mode, "available") == 0) CHECK(model.size_bytes == 1234 && model.known == UMI_AI_DISCOVERY_SIZE && strcmp(model.quantization,"Q4") == 0 && strcmp(model.display_name,"Example") == 0);
        if (strcmp(mode, "loaded") == 0) CHECK(model.vram_bytes == 7 && model.context_tokens == 4096 && model.size_bytes == 10 && model.known == 7 && strcmp(model.expires_at,"later") == 0);
        if (strcmp(mode, "known-zero") == 0) CHECK(model.known == 7 && model.size_bytes == 0 && model.vram_bytes == 0 && model.context_tokens == 0);
        if (strcmp(mode, "available-not-loaded") == 0) CHECK(model.known == 0 && model.vram_bytes == 0 && model.context_tokens == 0);
        if (strcmp(mode, "source-order") == 0) { CHECK(strcmp(model.id,"z") == 0); CHECK(UmiAiDiscoveryFind(snapshot,"a",&model) == UMI_STATUS_OK); CHECK(strcmp(model.id,"a") == 0); CHECK(UmiAiDiscoveryAt(snapshot,2,&model) == UMI_STATUS_OK); CHECK(strcmp(model.id,"m") == 0); }
        if (strcmp(mode, "case-sensitive") == 0) { CHECK(UmiAiDiscoveryCount(snapshot) == 2); CHECK(UmiAiDiscoveryFind(snapshot,"A",&model) == UMI_STATUS_OK); CHECK(UmiAiDiscoveryFind(snapshot,"a",&model) == UMI_STATUS_OK); }
        if (strcmp(mode, "unicode") == 0) CHECK(UmiAiDiscoveryFind(snapshot,"caf\xc3\xa9",&model) == UMI_STATUS_OK);
        if (strcmp(mode, "max-id") == 0) CHECK(strlen(model.id) == 512);
    }
    memcpy(&before, &model, sizeof(model));
    CHECK(UmiAiDiscoveryAt(snapshot, SIZE_MAX, &model) == UMI_STATUS_NOT_FOUND);
    CHECK(UmiAiDiscoveryFind(snapshot, "not-present", &model) == UMI_STATUS_NOT_FOUND);
    CHECK(UmiAiDiscoveryFind(snapshot, "", &model) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiDiscoveryFind(NULL, "x", &model) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiDiscoveryAt(NULL, 0, &model) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiDiscoveryFind(snapshot, "x", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&model, &before, sizeof(model)) == 0);
    UmiAiDiscoveryDestroy(snapshot);
    return 0;
}
