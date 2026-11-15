/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_runtime_catalogue_admission.c
 *
 * PURPOSE:
 *   Exercise bounded descriptor admission and public lookup without a provider or network.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "support/assertions.h"
#include <assert.h>
#include <string.h>
#include "ai/runtime_catalogue_fixture.h"

static char *field(UmiAiRuntimeDescriptor *value, const char *name, size_t *capacity)
{
#define FIELD(text, member) if (strcmp(name, text) == 0) { *capacity = sizeof(value->member); return value->member; }
    FIELD("runtime", runtime_id)
    FIELD("provider", provider_id)
    FIELD("display", display_name)
    FIELD("model", model_id)
    FIELD("endpoint", endpoint)
    FIELD("health-provider", health.provider_id)
    FIELD("health-message", health.message)
#undef FIELD
    return NULL;
}

int main(int argc, char **argv)
{
    UmiAiRuntimeCatalogue *catalogue = NULL;
    UmiAiRuntimeDescriptor value = catalogue_fixture("runtime.local"), before, output;
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    char *text;
    size_t capacity = 0U;
    assert(argc == 2);
    assert(umi_ai_runtime_catalogue_create(&catalogue) == UMI_STATUS_OK);
    assert(UmiAiRuntimeDescriptorValidate(&value) == UMI_STATUS_OK);
    assert(umi_ai_runtime_catalogue_upsert(catalogue, &value) == UMI_STATUS_OK);
    assert(umi_ai_runtime_catalogue_find(catalogue, value.runtime_id, &before) == UMI_STATUS_OK);
    text = field(&value, argv[1], &capacity);
    if (text != NULL) {
        memset(text, 'x', capacity);
    } else if (strcmp(argv[1], "empty-id") == 0) value.runtime_id[0] = '\0';
    else if (strcmp(argv[1], "empty-provider") == 0) value.provider_id[0] = '\0';
    else if (strcmp(argv[1], "empty-display") == 0) value.display_name[0] = '\0';
    else if (strcmp(argv[1], "empty-model") == 0) value.model_id[0] = '\0';
    else if (strcmp(argv[1], "provider-kind") == 0) value.provider_kind = (UmiAiProviderKind)0;
    else if (strcmp(argv[1], "transport") == 0) value.transport = (UmiAiRuntimeTransport)99;
    else if (strcmp(argv[1], "context") == 0) value.context_tokens = 0U;
    else if (strcmp(argv[1], "output-zero") == 0) value.max_output_tokens = 0U;
    else if (strcmp(argv[1], "output-large") == 0) value.max_output_tokens = value.context_tokens + 1U;
    else if (strcmp(argv[1], "health-identity") == 0) strcpy(value.health.provider_id, "provider.other");
    else if (strcmp(argv[1], "extension-flags") == 0) {
        value.capabilities |= UINT32_C(0x80000000);
        value.configured = -1; value.local = 2; value.health.available = -1;
        expected = UMI_STATUS_OK;
    } else if (strcmp(argv[1], "last-terminator") == 0) {
        memset(value.display_name, 'x', sizeof(value.display_name) - 1U);
        value.display_name[sizeof(value.display_name) - 1U] = '\0';
        memset(value.endpoint, 'e', sizeof(value.endpoint) - 1U);
        value.endpoint[sizeof(value.endpoint) - 1U] = '\0';
        expected = UMI_STATUS_OK;
    } else if (strcmp(argv[1], "lookup") == 0) {
        char bad[UMI_AI_ID_CAPACITY];
        UmiAiRuntimeDescriptor sentinel;
        memset(bad, 'z', sizeof(bad));
        memset(&sentinel, 0x5a, sizeof(sentinel)); output = sentinel;
        assert(umi_ai_runtime_catalogue_find(catalogue, bad, &output) == UMI_STATUS_INVALID_ARGUMENT);
        assert(memcmp(&output, &sentinel, sizeof(output)) == 0);
        assert(umi_ai_runtime_catalogue_find(catalogue, "", &output) == UMI_STATUS_INVALID_ARGUMENT);
        assert(umi_ai_runtime_catalogue_find(catalogue, "missing", &output) == UMI_STATUS_NOT_FOUND);
        assert(umi_ai_runtime_catalogue_at(catalogue, 1U, &output) == UMI_STATUS_NOT_FOUND);
        assert(umi_ai_runtime_catalogue_select(catalogue, UMI_AI_RUNTIME_CHAT, 1U, 1, &output) == UMI_STATUS_NOT_FOUND);
        assert(memcmp(&output, &sentinel, sizeof(output)) == 0);
        assert(UmiAiRuntimeDescriptorValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        assert(UmiAiRuntimeCatalogueValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        umi_ai_runtime_catalogue_destroy(catalogue); return 0;
    } else { assert(!"unknown admission case"); }
    assert(UmiAiRuntimeDescriptorValidate(&value) == expected);
    assert(umi_ai_runtime_catalogue_upsert(catalogue, &value) == expected);
    assert(umi_ai_runtime_catalogue_count(catalogue) == 1U);
    assert(umi_ai_runtime_catalogue_find(catalogue, "runtime.local", &output) == UMI_STATUS_OK);
    if (expected != UMI_STATUS_OK) {
        assert(memcmp(&output, &before, sizeof(output)) == 0);
    } else {
        assert(output.revision > before.revision);
        assert(strcmp(output.endpoint, value.endpoint) == 0);
        assert(output.capabilities == value.capabilities);
    }
    umi_ai_runtime_catalogue_destroy(catalogue);
    return 0;
}
