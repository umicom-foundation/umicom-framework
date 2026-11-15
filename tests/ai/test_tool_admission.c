/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai/test_tool_admission.c
 *
 * PURPOSE:
 *   Exercise tool metadata admission, catalogue corruption and refusal publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/runtime.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; \
} } while (0)

typedef struct Fixture {
    UmiAiRuntime runtime;
    UmiAiToolRegistry saved;
    UmiAiTool tool;
    UmiAiTool previous;
    unsigned calls;
    UmiStatus callback_status;
} Fixture;

static UmiStatus Invoke(const char *arguments, char *output, size_t capacity, void *data)
{
    Fixture *fixture = data;
    ++fixture->calls;
    if (strcmp(arguments, "{}") != 0) return UMI_STATUS_INVALID_ARGUMENT;
    const char text[] = "tool-output";
    if (capacity < sizeof(text)) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(output, text, sizeof(text));
    return fixture->callback_status;
}

static int Setup(Fixture *fixture)
{
    umi_ai_runtime_init(&fixture->runtime);
    fixture->runtime.policy.allow_tools = 1;
    CHECK(umi_ai_tool_init(&fixture->tool, "echo", "Echo a value", "test.echo",
        Invoke, fixture) == UMI_STATUS_OK);
    CHECK(umi_ai_tool_registry_add(&fixture->runtime.tools, &fixture->tool) == UMI_STATUS_OK);
    return 0;
}

static char *Field(UmiAiTool *tool, const char *name, size_t *capacity)
{
    if (strcmp(name, "id") == 0) { *capacity = sizeof(tool->tool_id); return tool->tool_id; }
    if (strcmp(name, "description") == 0) { *capacity = sizeof(tool->description); return tool->description; }
    if (strcmp(name, "permission") == 0) { *capacity = sizeof(tool->permission); return tool->permission; }
    return NULL;
}

static int Descriptor(Fixture *fixture, const char *mode, const char *name)
{
    size_t capacity = 0U;
    char *field = Field(&fixture->tool, name, &capacity);
    CHECK(field != NULL);
    memset(field, 'x', capacity);
    if (strcmp(mode, "boundary") == 0) {
        field[capacity - 1U] = '\0';
        CHECK(UmiAiToolValidate(&fixture->tool) == UMI_STATUS_OK);
        return 0;
    }
    CHECK(UmiAiToolValidate(&fixture->tool) == UMI_STATUS_INVALID_ARGUMENT);
    memcpy(&fixture->saved, &fixture->runtime.tools, sizeof(fixture->saved));
    CHECK(umi_ai_tool_registry_add(&fixture->runtime.tools, &fixture->tool) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&fixture->saved, &fixture->runtime.tools, sizeof(fixture->saved)) == 0);
    return 0;
}

static int Construct(Fixture *fixture, const char *mode, const char *name)
{
    char text[UMI_AI_SMALL_TEXT_CAPACITY + 1U];
    size_t capacity = 0U;
    CHECK(Field(&fixture->tool, name, &capacity) != NULL);
    memset(text, 'v', sizeof(text));
    text[strcmp(mode, "construct-boundary") == 0 ? capacity - 1U : capacity] = '\0';
    memcpy(&fixture->previous, &fixture->tool, sizeof(fixture->previous));
    UmiStatus status = umi_ai_tool_init(&fixture->tool,
        strcmp(name, "id") == 0 ? text : "new-tool",
        strcmp(name, "description") == 0 ? text : "New description",
        strcmp(name, "permission") == 0 ? text : "test.new", Invoke, fixture);
    if (strcmp(mode, "construct-boundary") == 0) {
        CHECK(status == UMI_STATUS_OK);
        CHECK(UmiAiToolValidate(&fixture->tool) == UMI_STATUS_OK);
        CHECK(strcmp(Field(&fixture->tool, name, &capacity), text) == 0);
    } else {
        CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&fixture->previous, &fixture->tool, sizeof(fixture->previous)) == 0);
    }
    return 0;
}

static int CorruptRegistry(Fixture *fixture, const char *name)
{
    UmiAiToolRegistry *registry = &fixture->runtime.tools;
    if (strcmp(name, "count") == 0) registry->count = UMI_AI_MAX_TOOLS + 1U;
    else if (strcmp(name, "huge-count") == 0) registry->count = SIZE_MAX;
    else if (strcmp(name, "duplicate") == 0) { registry->tools[1] = registry->tools[0]; registry->count = 2U; }
    else if (strcmp(name, "late-record") == 0) {
        CHECK(umi_ai_tool_init(&fixture->tool, "later", "Later", "test.later", Invoke, fixture) == UMI_STATUS_OK);
        CHECK(umi_ai_tool_registry_add(registry, &fixture->tool) == UMI_STATUS_OK);
        memset(registry->tools[1].description, 'x', sizeof(registry->tools[1].description));
    } else if (strcmp(name, "callback") == 0) registry->tools[0].invoke = NULL;
    else if (strcmp(name, "empty-id") == 0) registry->tools[0].tool_id[0] = '\0';
    else {
        size_t capacity = 0U;
        char *field = Field(&registry->tools[0], name, &capacity);
        CHECK(field != NULL);
        memset(field, 'x', capacity);
    }
    memcpy(&fixture->saved, registry, sizeof(fixture->saved));
    CHECK(UmiAiToolRegistryValidate(registry) == UMI_STATUS_INVALID_STATE);
    UmiAiTool *resolved = &fixture->previous;
    CHECK(UmiAiToolRegistryResolve(registry, "echo", &resolved) == UMI_STATUS_INVALID_STATE);
    CHECK(resolved == &fixture->previous);
    CHECK(umi_ai_tool_registry_find(registry, "echo") == NULL);
    CHECK(umi_ai_tool_init(&fixture->tool, "new-tool", "New", "test.new", Invoke, fixture) == UMI_STATUS_OK);
    CHECK(umi_ai_tool_registry_add(registry, &fixture->tool) == UMI_STATUS_INVALID_STATE);
    char output[64] = "previous";
    CHECK(umi_ai_runtime_invoke_tool(&fixture->runtime, "echo", 1, "{}", output, sizeof(output)) == UMI_STATUS_INVALID_STATE);
    CHECK(strcmp(output, "previous") == 0 && fixture->calls == 0U);
    CHECK(memcmp(&fixture->saved, registry, sizeof(fixture->saved)) == 0);
    return 0;
}

static int Capacity(Fixture *fixture)
{
    UmiAiToolRegistry *registry = &fixture->runtime.tools;
    for (size_t index = 1U; index < UMI_AI_MAX_TOOLS; ++index) {
        char id[UMI_AI_ID_CAPACITY];
        int written = snprintf(id, sizeof(id), "tool-%zu", index);
        CHECK(written > 0 && (size_t)written < sizeof(id));
        CHECK(umi_ai_tool_init(&fixture->tool, id, "", "", Invoke, fixture) == UMI_STATUS_OK);
        CHECK(umi_ai_tool_registry_add(registry, &fixture->tool) == UMI_STATUS_OK);
    }
    CHECK(UmiAiToolRegistryValidate(registry) == UMI_STATUS_OK);
    UmiAiTool *resolved = NULL;
    CHECK(UmiAiToolRegistryResolve(registry, fixture->tool.tool_id, &resolved) == UMI_STATUS_OK);
    CHECK(resolved == &registry->tools[UMI_AI_MAX_TOOLS - 1U]);
    memcpy(&fixture->saved, registry, sizeof(fixture->saved));
    CHECK(umi_ai_tool_registry_add(registry, &fixture->tool) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_ai_tool_init(&fixture->tool, "overflow", "", "", Invoke, fixture) == UMI_STATUS_OK);
    CHECK(umi_ai_tool_registry_add(registry, &fixture->tool) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&fixture->saved, registry, sizeof(fixture->saved)) == 0);
    return 0;
}

static int Miscellaneous(Fixture *fixture, const char *name)
{
    UmiAiToolRegistry *registry = &fixture->runtime.tools;
    UmiAiTool *resolved = &fixture->previous;
    memcpy(&fixture->previous, &fixture->tool, sizeof(fixture->previous));
    if (strcmp(name, "alias") == 0) {
        CHECK(umi_ai_tool_init(&fixture->tool, fixture->tool.tool_id, fixture->tool.description,
            fixture->tool.permission, Invoke, fixture) == UMI_STATUS_OK);
        CHECK(strcmp(fixture->tool.tool_id, "echo") == 0);
        CHECK(strcmp(fixture->tool.description, "Echo a value") == 0);
        CHECK(strcmp(fixture->tool.permission, "test.echo") == 0);
    } else if (strcmp(name, "empty-id") == 0 || strcmp(name, "null-callback") == 0) {
        CHECK(umi_ai_tool_init(&fixture->tool, strcmp(name, "empty-id") == 0 ? "" : "echo",
            "", "", strcmp(name, "null-callback") == 0 ? NULL : Invoke, fixture) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&fixture->previous, &fixture->tool, sizeof(fixture->previous)) == 0);
    } else if (strcmp(name, "opaque-metadata") == 0) {
        fixture->tool.description[0] = '\0'; fixture->tool.permission[0] = '\0';
        fixture->tool.flags = UINT32_MAX; fixture->tool.user_data = NULL;
        CHECK(UmiAiToolValidate(&fixture->tool) == UMI_STATUS_OK);
    } else if (strcmp(name, "inactive") == 0) {
        memset(&registry->tools[1], 0x7f, sizeof(registry->tools[1]));
        CHECK(UmiAiToolRegistryValidate(registry) == UMI_STATUS_OK);
        CHECK(UmiAiToolRegistryResolve(registry, "echo", &resolved) == UMI_STATUS_OK);
        CHECK(resolved == &registry->tools[0]);
    } else if (strcmp(name, "copied") == 0) {
        memset(fixture->tool.tool_id, 'x', sizeof(fixture->tool.tool_id));
        CHECK(UmiAiToolRegistryResolve(registry, "echo", &resolved) == UMI_STATUS_OK);
        CHECK(resolved->user_data == fixture && resolved->invoke == Invoke);
    } else if (strcmp(name, "missing") == 0 || strcmp(name, "empty-query") == 0 || strcmp(name, "long-query") == 0) {
        char query[UMI_AI_ID_CAPACITY + 1U]; memset(query, 'q', sizeof(query)); query[UMI_AI_ID_CAPACITY] = '\0';
        const char *id = strcmp(name, "missing") == 0 ? "missing" : strcmp(name, "empty-query") == 0 ? "" : query;
        UmiStatus expected = strcmp(name, "missing") == 0 ? UMI_STATUS_NOT_FOUND : UMI_STATUS_INVALID_ARGUMENT;
        CHECK(UmiAiToolRegistryResolve(registry, id, &resolved) == expected);
        CHECK(resolved == &fixture->previous && umi_ai_tool_registry_find(registry, id) == NULL);
    } else if (strcmp(name, "nulls") == 0) {
        CHECK(UmiAiToolValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiToolRegistryValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiToolRegistryResolve(NULL, "echo", &resolved) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiToolRegistryResolve(registry, NULL, &resolved) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiToolRegistryResolve(registry, "echo", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(resolved == &fixture->previous);
        CHECK(umi_ai_tool_registry_add(NULL, &fixture->tool) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_tool_registry_add(registry, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_tool_init(NULL, "echo", "", "", Invoke, fixture) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_tool_init(&fixture->tool, NULL, "", "", Invoke, fixture) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_tool_init(&fixture->tool, "echo", NULL, "", Invoke, fixture) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_tool_init(&fixture->tool, "echo", "", NULL, Invoke, fixture) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&fixture->previous, &fixture->tool, sizeof(fixture->previous)) == 0);
    } else return 1;
    return 0;
}

static int Dispatch(Fixture *fixture, const char *name)
{
    int approved = strcmp(name, "approval") == 0 ? 0 : 1;
    if (strcmp(name, "policy") == 0) fixture->runtime.policy.allow_tools = 0;
    if (strcmp(name, "callback-error") == 0) fixture->callback_status = UMI_STATUS_IO_ERROR;
    char output[64] = "previous";
    UmiStatus status = umi_ai_runtime_invoke_tool(&fixture->runtime,
        strcmp(name, "missing") == 0 ? "missing" : "echo", approved, "{}", output, sizeof(output));
    if (strcmp(name, "policy") == 0 || strcmp(name, "approval") == 0 || strcmp(name, "missing") == 0) {
        CHECK(status == (strcmp(name, "missing") == 0 ? UMI_STATUS_NOT_FOUND : UMI_STATUS_PERMISSION_DENIED));
        CHECK(fixture->calls == 0U && strcmp(output, "previous") == 0);
    } else {
        CHECK(status == fixture->callback_status);
        CHECK(fixture->calls == 1U && strcmp(output, "tool-output") == 0);
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    Fixture *fixture = calloc(1U, sizeof(*fixture));
    if (fixture == NULL) return 3;
    int result = Setup(fixture);
    if (result == 0) {
        if (strcmp(argv[1], "descriptor") == 0 || strcmp(argv[1], "boundary") == 0)
            result = Descriptor(fixture, argv[1], argv[2]);
        else if (strcmp(argv[1], "construct-boundary") == 0 || strcmp(argv[1], "construct-overflow") == 0)
            result = Construct(fixture, argv[1], argv[2]);
        else if (strcmp(argv[1], "registry") == 0) result = CorruptRegistry(fixture, argv[2]);
        else if (strcmp(argv[1], "capacity") == 0) result = Capacity(fixture);
        else if (strcmp(argv[1], "misc") == 0) result = Miscellaneous(fixture, argv[2]);
        else if (strcmp(argv[1], "dispatch") == 0) result = Dispatch(fixture, argv[2]);
        else result = 2;
    }
    /* Fixtures own no provider or user_data allocation beyond this one owner. */
    free(fixture);
    return result;
}
