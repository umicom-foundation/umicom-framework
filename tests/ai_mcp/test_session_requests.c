/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_session_requests.c
 *
 * PURPOSE:
 *   Verify reply publication across ready-session callbacks and ordinary MCP clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "test_support.h"
#include <stdlib.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; \
} } while (0)

typedef enum Mode { DIRECT, TOOL, RESOURCE, PROMPT, PING, TOOLS, RESOURCES, PROMPTS } Mode;
typedef struct Fixture {
    Mode mode;
    const char *action;
    /* The callback selector lives as long as its owning fixture, including
     * early-return diagnostics after Exercise has released its stack. */
    char action_storage[32];
    UmiAiMcpSession session;
    unsigned calls, depth;
    int error, unterminated, empty, last_byte, no_output, nested, nested_retire;
    int alternate_data;
    UmiStatus inner_status;
    char output[64], previous[64];
    void *catalogue, *old_catalogue;
    size_t catalogue_size;
} Fixture;
UmiStatus UmiTestMcpSessionRequestAllocationFailure(UmiAiMcpSession *, const char *, const char *, char *, size_t);
static UmiStatus AlternateRequest(void *data, const char *method, const char *params, char *out, size_t capacity)
{ (void)data; (void)method; (void)params; (void)out; (void)capacity; return UMI_STATUS_INTERNAL_ERROR; }
static UmiStatus AlternateNotify(void *data, const char *method, const char *params)
{ (void)data; (void)method; (void)params; return UMI_STATUS_INTERNAL_ERROR; }
static UmiStatus Request(void *data, const char *method, const char *params, char *out, size_t capacity)
{
    Fixture *f = data; (void)method; (void)params; ++f->calls;
    if (f->nested && f->depth == 0U) {
        char inner[32]; ++f->depth;
        f->inner_status = UmiAiMcpSessionRequestChecked(&f->session, "inner", "{}", inner, sizeof(inner));
        --f->depth;
    }
    if (!f->nested_retire || f->depth != 0U) {
        if (strcmp(f->action, "revision") == 0) ++f->session.revision;
        else if (strcmp(f->action, "state") == 0) f->session.state = UMI_AI_MCP_SESSION_DISCONNECTED;
        else if (strcmp(f->action, "disconnect") == 0) (void)UmiAiMcpSessionDisconnectChecked(&f->session);
        else if (strcmp(f->action, "request") == 0) f->session.transport.request = AlternateRequest;
        else if (strcmp(f->action, "notify") == 0) f->session.transport.notify = AlternateNotify;
        else if (strcmp(f->action, "data") == 0) f->session.transport.user_data = &f->alternate_data;
    }
    if (!f->no_output) {
        if (f->unterminated) memset(out, 'x', capacity);
        else if (f->empty) out[0] = '\0';
        else if (f->last_byte) { memset(out, 'x', capacity - 1U); out[capacity - 1U] = '\0'; }
        else {
            const char *reply = f->mode == TOOLS ? "{\"tools\":[{\"name\":\"current\"}]}" :
                f->mode == RESOURCES ? "{\"resources\":[{\"uri\":\"current\"}]}" :
                f->mode == PROMPTS ? "{\"prompts\":[{\"name\":\"current\"}]}" : "{}";
            size_t length = strlen(reply);
            if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
            memcpy(out, reply, length + 1U);
        }
    }
    return f->error ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
static UmiStatus Invoke(Fixture *f)
{
    switch (f->mode) {
    case DIRECT: return UmiAiMcpSessionRequestChecked(&f->session, "custom", "{}", f->output, sizeof(f->output));
    case TOOL: return umi_ai_mcp_call_tool(&f->session, "tool", "{}", f->output, sizeof(f->output));
    case RESOURCE: return umi_ai_mcp_read_resource(&f->session, "file:///example", f->output, sizeof(f->output));
    case PROMPT: return umi_ai_mcp_get_prompt(&f->session, "prompt", "{}", f->output, sizeof(f->output));
    case PING: return umi_ai_mcp_session_ping(&f->session);
    case TOOLS: return umi_ai_mcp_discover_tools(&f->session, f->catalogue);
    case RESOURCES: return umi_ai_mcp_discover_resources(&f->session, f->catalogue);
    case PROMPTS: return umi_ai_mcp_discover_prompts(&f->session, f->catalogue);
    }
    return UMI_STATUS_INVALID_ARGUMENT;
}
static int SeedCatalogue(Fixture *f)
{
    if (f->mode < TOOLS) return 0;
    f->catalogue_size = f->mode == TOOLS ? sizeof(UmiAiMcpToolCatalogue) :
        f->mode == RESOURCES ? sizeof(UmiAiMcpResourceCatalogue) : sizeof(UmiAiMcpPromptCatalogue);
    f->catalogue = calloc(1U, f->catalogue_size); f->old_catalogue = malloc(f->catalogue_size);
    CHECK(f->catalogue != NULL && f->old_catalogue != NULL);
    UmiStatus status;
    if (f->mode == TOOLS) {
        UmiAiMcpToolDescriptor item = {0}; strcpy(item.name, "previous");
        umi_ai_mcp_tool_catalogue_init(f->catalogue); status = umi_ai_mcp_tool_catalogue_add(f->catalogue, &item);
    } else if (f->mode == RESOURCES) {
        UmiAiMcpResourceDescriptor item = {0}; strcpy(item.uri, "previous");
        umi_ai_mcp_resource_catalogue_init(f->catalogue); status = umi_ai_mcp_resource_catalogue_add(f->catalogue, &item);
    } else {
        UmiAiMcpPromptDescriptor item = {0}; strcpy(item.name, "previous");
        umi_ai_mcp_prompt_catalogue_init(f->catalogue); status = umi_ai_mcp_prompt_catalogue_add(f->catalogue, &item);
    }
    CHECK(status == UMI_STATUS_OK); memcpy(f->old_catalogue, f->catalogue, f->catalogue_size); return 0;
}
static int CatalogueAccepted(const Fixture *f)
{
    if (f->mode == TOOLS) {
        const UmiAiMcpToolCatalogue *c = f->catalogue;
        return c->count == 1U && umi_ai_mcp_tool_catalogue_find(c, "current") != NULL && umi_ai_mcp_tool_catalogue_find(c, "previous") == NULL;
    }
    if (f->mode == RESOURCES) {
        const UmiAiMcpResourceCatalogue *c = f->catalogue;
        return c->count == 1U && umi_ai_mcp_resource_catalogue_find(c, "current") != NULL && umi_ai_mcp_resource_catalogue_find(c, "previous") == NULL;
    }
    const UmiAiMcpPromptCatalogue *c = f->catalogue;
    return c->count == 1U && umi_ai_mcp_prompt_catalogue_find(c, "current") != NULL && umi_ai_mcp_prompt_catalogue_find(c, "previous") == NULL;
}

static int Exercise(Fixture *f, const char *name)
{
    f->action = "none";
    f->session.state = UMI_AI_MCP_SESSION_READY; f->session.revision = 7U;
    f->session.transport = (UmiAiMcpTransport){Request, NULL, f};
    memset(f->output, 0x5a, sizeof(f->output)); memcpy(f->previous, f->output, sizeof(f->output));
    UmiStatus expected = UMI_STATUS_OK; unsigned expected_calls = 1U;
/* A retained fixture pointer must not borrow function-local action storage. The former implementation is retained for owner review. */
#if 0
    int allocation = 0; char action[32];
#endif
    int allocation = 0;
    char *action = f->action_storage;
    if (strcmp(name, "success") == 0) { /* Ordinary ready owner. */ }
    else if (strcmp(name, "error") == 0) { f->error = 1; expected = UMI_STATUS_IO_ERROR; }
    else if (strcmp(name, "unterminated") == 0) { f->unterminated = 1; expected = UMI_STATUS_INTERNAL_ERROR; }
    else if (strcmp(name, "no-output") == 0) { f->no_output = 1; expected = UMI_STATUS_INTERNAL_ERROR; }
    else if (strcmp(name, "empty") == 0) f->empty = 1;
    else if (strcmp(name, "last-byte") == 0) f->last_byte = 1;
    else if (strcmp(name, "not-ready") == 0) { f->session.state = UMI_AI_MCP_SESSION_CONNECTED; expected = UMI_STATUS_INVALID_STATE; expected_calls = 0U; }
    else if (strcmp(name, "missing-request") == 0) { f->session.transport.request = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; expected_calls = 0U; }
    else if (strcmp(name, "revision-max") == 0) f->session.revision = UINT64_MAX;
    else if (strcmp(name, "allocation") == 0) { allocation = 1; expected = UMI_STATUS_OUT_OF_MEMORY; expected_calls = 0U; }
    else if (strcmp(name, "nested") == 0 || strcmp(name, "nested-retire") == 0) {
        f->nested = 1; expected_calls = 2U;
        if (strcmp(name, "nested-retire") == 0) {
            f->nested_retire = 1; f->action = "disconnect"; expected = UMI_STATUS_INVALID_STATE;
        }
    } else if (strcmp(name, "nulls") == 0) {
        CHECK(UmiAiMcpSessionRequestChecked(NULL, "x", "{}", f->output, sizeof(f->output)) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiMcpSessionRequestChecked(&f->session, NULL, "{}", f->output, sizeof(f->output)) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiMcpSessionRequestChecked(&f->session, "x", NULL, f->output, sizeof(f->output)) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiMcpSessionRequestChecked(&f->session, "x", "{}", NULL, sizeof(f->output)) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiMcpSessionRequestChecked(&f->session, "x", "{}", f->output, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(f->calls == 0U && memcmp(f->output, f->previous, sizeof(f->output)) == 0); return 0;
    } else {
        const char *separator = strchr(name, '-'); CHECK(separator != NULL);
        size_t length = (size_t)(separator - name); /* Check the owner buffer capacity rather than the local pointer size. The former implementation is retained for owner review. */
#if 0
CHECK(length < sizeof(action));
#endif
CHECK(length < sizeof(f->action_storage));
        memcpy(action, name, length); action[length] = '\0'; f->action = action;
        CHECK(strcmp(action, "revision") == 0 || strcmp(action, "state") == 0 || strcmp(action, "disconnect") == 0 ||
            strcmp(action, "request") == 0 || strcmp(action, "notify") == 0 || strcmp(action, "data") == 0);
        if (strcmp(separator + 1, "ok") == 0) expected = UMI_STATUS_INVALID_STATE;
        else if (strcmp(separator + 1, "error") == 0) { f->error = 1; expected = UMI_STATUS_IO_ERROR; }
        else if (strcmp(separator + 1, "unterminated") == 0) { f->unterminated = 1; expected = UMI_STATUS_INTERNAL_ERROR; }
        else return 2;
    }
    UmiAiMcpSession previous_session = f->session;
    CHECK(SeedCatalogue(f) == 0);
    UmiStatus status = allocation ? UmiTestMcpSessionRequestAllocationFailure(&f->session, "custom", "{}", f->output, sizeof(f->output)) : Invoke(f);
    CHECK(status == expected && f->calls == expected_calls);
    if (f->mode >= TOOLS) {
        if (status == UMI_STATUS_OK) CHECK(CatalogueAccepted(f));
        else CHECK(memcmp(f->catalogue, f->old_catalogue, f->catalogue_size) == 0);
    } else if (f->mode == PING || status != UMI_STATUS_OK) CHECK(memcmp(f->output, f->previous, sizeof(f->output)) == 0);
    else if (f->empty) CHECK(f->output[0] == '\0');
    else if (f->last_byte) CHECK(strlen(f->output) == sizeof(f->output) - 1U);
    else CHECK(strcmp(f->output, "{}") == 0);
    if (strcmp(f->action, "none") == 0) CHECK(memcmp(&f->session, &previous_session, sizeof(previous_session)) == 0);
    if (f->nested) CHECK(f->inner_status == (f->nested_retire ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK));
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *modes[] = {"direct", "tool", "resource", "prompt", "ping", "tools", "resources", "prompts"};
    int mode = -1;
    for (size_t i = 0U; i < sizeof(modes) / sizeof(modes[0]); ++i)
        if (strcmp(argv[1], modes[i]) == 0) mode = (int)i;
    if (mode < 0) return 2;
    Fixture *f = calloc(1U, sizeof(*f)); if (f == NULL) return 3;
    f->mode = (Mode)mode; int result = Exercise(f, argv[2]);
    free(f->catalogue); free(f->old_catalogue); free(f); return result;
}
