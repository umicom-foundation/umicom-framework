/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_runtime.c
 *
 * PURPOSE:
 *   Exercise native run-plan projection without inspecting files, opening sockets or starting engines.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../support/assertions.h"
#include <assert.h>
#include <string.h>
#include "umicom/local_models/runtime.h"
#ifdef _WIN32
#define SERVER_PATH "C:/umicom-test/llama-server.exe"
#define MODEL_PATH "C:/umicom-test/local.gguf"
#else
#define SERVER_PATH "/umicom-test/llama-server"
#define MODEL_PATH "/umicom-test/local.gguf"
#endif
static const char digest[] = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";

int main(int argc, char **argv)
{
    UmiModelRunPlan plan;
    UmiAiRuntimeDescriptor output, sentinel;
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    const char *id = "runtime.text", *label = "Local text";
    uint32_t maximum = 256U;
    char longId[UMI_AI_ID_CAPACITY], longLabel[UMI_AI_SMALL_TEXT_CAPACITY];
    assert(argc == 2);
    memset(longId, 'x', sizeof(longId)); memset(longLabel, 'x', sizeof(longLabel));
    assert(UmiModelRunPlanCreate(SERVER_PATH, MODEL_PATH, digest, "fixture",
        18080U, 2048U, 4U, &plan) == UMI_STATUS_OK);
    memset(&sentinel, 0x5a, sizeof(sentinel)); memcpy(&output, &sentinel, sizeof(output));
    if (strcmp(argv[1], "metadata") == 0) {
        UmiAiRuntimeCatalogue *catalogue = NULL;
        UmiAiRuntimeDescriptor selected;
        assert(UmiModelRuntimeDescriptorCreate(&plan, id, label, maximum, &output) == UMI_STATUS_OK);
        assert(strcmp(output.provider_id, "umicom.local-chat") == 0);
        assert(strcmp(output.model_id, "fixture") == 0);
        assert(strcmp(output.endpoint, "http://127.0.0.1:18080/v1/chat/completions") == 0);
        assert(output.local && output.configured && !output.health.available);
        assert(output.health.checked_at_ns == 0U && output.revision == 0U);
        assert(output.context_tokens == 2048U && output.max_output_tokens == 256U);
        assert(output.capabilities == (UMI_AI_RUNTIME_CHAT | UMI_AI_RUNTIME_STREAMING));
        assert(UmiAiRuntimeDescriptorValidate(&output) == UMI_STATUS_OK);
        assert(umi_ai_runtime_catalogue_create(&catalogue) == UMI_STATUS_OK);
        assert(UmiAiRuntimeCatalogueUpsertIfRevision(catalogue, &output, 0U) == UMI_STATUS_OK);
        assert(umi_ai_runtime_catalogue_select(catalogue, UMI_AI_RUNTIME_CHAT, 1U, 1, &selected) == UMI_STATUS_NOT_FOUND);
        assert(umi_ai_runtime_catalogue_find(catalogue, id, &selected) == UMI_STATUS_OK);
        assert(selected.revision != 0U);
        /* A copied plan remains a description, even if the application changes
         * its next launch choices after creating this record. */
        strcpy(plan.alias, "next-model"); strcpy(plan.port, "19090");
        assert(strcmp(output.model_id, "fixture") == 0);
        umi_ai_runtime_catalogue_destroy(catalogue); return 0;
    }
    if (strcmp(argv[1], "null") == 0) {
        UmiModelRunPlan planOut, planSentinel;
        memset(&planSentinel, 0x5a, sizeof(planSentinel)); memcpy(&planOut, &planSentinel, sizeof(planOut));
        assert(UmiModelRuntimeDescriptorCreate(NULL, id, label, maximum, &output) == UMI_STATUS_INVALID_ARGUMENT);
        assert(UmiModelRuntimeDescriptorCreate(&plan, id, label, maximum, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        assert(UmiModelSessionRuntimeDescribe(NULL, id, label, maximum, &output) == UMI_STATUS_INVALID_ARGUMENT);
        assert(UmiModelSessionReadPlan(NULL, &planOut) == UMI_STATUS_INVALID_ARGUMENT);
        assert(memcmp(&planOut, &planSentinel, sizeof(planOut)) == 0);
        assert(memcmp(&output, &sentinel, sizeof(output)) == 0); return 0;
    }
    if (strcmp(argv[1], "id") == 0) id = longId;
    else if (strcmp(argv[1], "id-empty") == 0) id = "";
    else if (strcmp(argv[1], "id-space") == 0) id = "invalid id";
    else if (strcmp(argv[1], "label") == 0) label = longLabel;
    else if (strcmp(argv[1], "label-empty") == 0) label = "";
    else if (strcmp(argv[1], "output-zero") == 0) maximum = 0U;
    else if (strcmp(argv[1], "output-large") == 0) maximum = 2049U;
    else if (strcmp(argv[1], "output-context") == 0) { maximum = 2048U; expected = UMI_STATUS_OK; }
    else if (strcmp(argv[1], "port") == 0) memset(plan.port, '9', sizeof(plan.port));
    else if (strcmp(argv[1], "context") == 0) memset(plan.context, '9', sizeof(plan.context));
    else if (strcmp(argv[1], "threads") == 0) memset(plan.threads, '9', sizeof(plan.threads));
    else if (strcmp(argv[1], "alias") == 0) memset(plan.alias, 'a', sizeof(plan.alias));
    else if (strcmp(argv[1], "digest") == 0) plan.sha256[0] = 'z';
    else if (strcmp(argv[1], "guard") == 0) strcpy(plan.guard, MODEL_PATH);
    else if (strcmp(argv[1], "directory") == 0) strcpy(plan.directory, "relative");
    else if (strcmp(argv[1], "executable") == 0) strcpy(plan.executable, "relative");
    else if (strcmp(argv[1], "model") == 0) strcpy(plan.model, "relative");
    else assert(!"unknown projection case");
    assert(UmiModelRuntimeDescriptorCreate(&plan, id, label, maximum, &output) == expected);
    if (expected != UMI_STATUS_OK) assert(memcmp(&output, &sentinel, sizeof(output)) == 0);
    else assert(output.max_output_tokens == 2048U && !output.health.available);
    return 0;
}
