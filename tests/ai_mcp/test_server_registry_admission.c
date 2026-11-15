/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_server_registry_admission.c
 *
 * PURPOSE:
 *   Verify bounded server registry lookup, mutation and revision preservation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "test_support.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; \
} } while (0)

typedef struct Field { const char *name; size_t offset, capacity; int required; } Field;
static const Field fields[] = {
    {"id", offsetof(UmiAiMcpServerDescriptor, server_id), UMI_AI_ID_CAPACITY, 1},
    {"display", offsetof(UmiAiMcpServerDescriptor, display_name), UMI_AI_SMALL_TEXT_CAPACITY, 1},
    {"endpoint", offsetof(UmiAiMcpServerDescriptor, endpoint), UMI_AI_SMALL_TEXT_CAPACITY, 1},
    {"permission", offsetof(UmiAiMcpServerDescriptor, permission_prefix), UMI_AI_ID_CAPACITY, 0}
};
typedef struct Fixture {
    UmiAiMcpServerRegistry registry, previous;
    UmiAiMcpServerDescriptor candidate;
} Fixture;
static UmiStatus Server(UmiAiMcpServerDescriptor *server, const char *id)
{ return umi_ai_mcp_server_init(server, id, "Display", "stdio://server", UMI_AI_MCP_TRUST_LOCAL); }
static int Seed(Fixture *f)
{
    umi_ai_mcp_server_registry_init(&f->registry);
    const char *ids[] = {"one", "two", "three"};
    for (size_t i = 0U; i < 3U; ++i) {
        UmiAiMcpServerDescriptor server; CHECK(Server(&server, ids[i]) == UMI_STATUS_OK);
        CHECK(umi_ai_mcp_server_registry_add(&f->registry, &server) == UMI_STATUS_OK);
    }
    CHECK(Server(&f->candidate, "new") == UMI_STATUS_OK); return 0;
}
static int Unchanged(Fixture *f) { return memcmp(&f->registry, &f->previous, sizeof(f->registry)) == 0; }
static void Capture(Fixture *f) { memcpy(&f->previous, &f->registry, sizeof(f->registry)); }
static int Malformed(Fixture *f)
{
    Capture(f);
    const UmiAiMcpServerDescriptor *found = &f->candidate;
    CHECK(UmiAiMcpServerRegistryValidate(&f->registry) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiAiMcpServerRegistryResolve(&f->registry, "one", &found) == UMI_STATUS_INVALID_STATE && found == &f->candidate);
    CHECK(umi_ai_mcp_server_registry_find(&f->registry, "one") == NULL);
    CHECK(umi_ai_mcp_server_registry_at(&f->registry, 0U) == NULL);
    CHECK(umi_ai_mcp_server_registry_add(&f->registry, &f->candidate) == UMI_STATUS_INVALID_STATE && Unchanged(f));
    CHECK(umi_ai_mcp_server_registry_remove(&f->registry, "one") == UMI_STATUS_INVALID_STATE && Unchanged(f));
    return 0;
}
static int Added(Fixture *f, const UmiAiMcpServerDescriptor *candidate)
{
    UmiAiMcpServerDescriptor expected = *candidate; size_t count = f->registry.count; uint64_t revision = f->registry.revision;
    CHECK(umi_ai_mcp_server_registry_add(&f->registry, candidate) == UMI_STATUS_OK);
    const UmiAiMcpServerDescriptor *found = NULL;
    CHECK(f->registry.count == count + 1U && f->registry.revision == revision + 1U);
    CHECK(UmiAiMcpServerRegistryResolve(&f->registry, expected.server_id, &found) == UMI_STATUS_OK && found == &f->registry.servers[count]);
    CHECK(strcmp(found->display_name, expected.display_name) == 0 && strcmp(found->endpoint, expected.endpoint) == 0 && strcmp(found->permission_prefix, expected.permission_prefix) == 0);
    return 0;
}
static int FieldCase(Fixture *f, const char *name, const Field *field)
{
    if (strcmp(name, "existing-first") == 0 || strcmp(name, "existing-last") == 0) {
        size_t index = strcmp(name, "existing-first") == 0 ? 0U : f->registry.count - 1U;
        memset((char *)&f->registry.servers[index] + field->offset, 'x', field->capacity);
        return Malformed(f);
    }
    char *text = (char *)&f->candidate + field->offset;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(name, "candidate-unterminated") == 0) { memset(text, 'x', field->capacity); expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "candidate-last") == 0) { memset(text, 'x', field->capacity); text[field->capacity - 1U] = '\0'; }
    else if (strcmp(name, "empty") == 0) { text[0] = '\0'; if (field->required) expected = UMI_STATUS_INVALID_ARGUMENT; }
    else return 2;
    if (expected == UMI_STATUS_OK) return Added(f, &f->candidate);
    Capture(f); CHECK(umi_ai_mcp_server_registry_add(&f->registry, &f->candidate) == expected && Unchanged(f)); return 0;
}

static int Basic(Fixture *f, const char *name)
{
    const UmiAiMcpServerDescriptor *found = &f->candidate;
    if (strcmp(name, "empty") == 0) {
        umi_ai_mcp_server_registry_init(&f->registry); Capture(f);
        CHECK(UmiAiMcpServerRegistryValidate(&f->registry) == UMI_STATUS_OK);
        CHECK(UmiAiMcpServerRegistryResolve(&f->registry, "one", &found) == UMI_STATUS_NOT_FOUND && found == &f->candidate);
        CHECK(umi_ai_mcp_server_registry_find(&f->registry, "one") == NULL && umi_ai_mcp_server_registry_at(&f->registry, 0U) == NULL);
        CHECK(umi_ai_mcp_server_registry_remove(&f->registry, "one") == UMI_STATUS_NOT_FOUND && Unchanged(f));
        return Added(f, &f->candidate);
    }
    if (strcmp(name, "resolve") == 0 || strcmp(name, "mutable") == 0) {
        Capture(f);
        CHECK(UmiAiMcpServerRegistryResolve(&f->registry, "two", &found) == UMI_STATUS_OK && found == &f->registry.servers[1]);
        CHECK(umi_ai_mcp_server_registry_at(&f->registry, 1U) == found && Unchanged(f));
        UmiAiMcpServerDescriptor *mutable = umi_ai_mcp_server_registry_find(&f->registry, "two"); CHECK(mutable == found);
        if (strcmp(name, "mutable") == 0) { mutable->enabled = 0; CHECK(f->registry.servers[1].enabled == 0 && UmiAiMcpServerRegistryValidate(&f->registry) == UMI_STATUS_OK); }
        return 0;
    }
    if (strcmp(name, "missing") == 0) {
        Capture(f); CHECK(UmiAiMcpServerRegistryResolve(&f->registry, "absent", &found) == UMI_STATUS_NOT_FOUND && found == &f->candidate);
        CHECK(umi_ai_mcp_server_registry_find(&f->registry, "absent") == NULL && umi_ai_mcp_server_registry_at(&f->registry, SIZE_MAX) == NULL && Unchanged(f)); return 0;
    }
    if (strcmp(name, "count-overflow") == 0 || strcmp(name, "count-max") == 0) {
        f->registry.count = strcmp(name, "count-max") == 0 ? SIZE_MAX : UMI_AI_MCP_MAX_SERVERS + 1U; return Malformed(f);
    }
    if (strcmp(name, "duplicate-active") == 0) { f->registry.servers[2] = f->registry.servers[0]; return Malformed(f); }
    if (strcmp(name, "trust-existing") == 0) { f->registry.servers[2].trust = (UmiAiMcpTrust)3; return Malformed(f); }
    if (strcmp(name, "trust-candidate") == 0 || strcmp(name, "duplicate-candidate") == 0) {
        UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
        if (strcmp(name, "trust-candidate") == 0) f->candidate.trust = (UmiAiMcpTrust)3;
        else { strcpy(f->candidate.server_id, "one"); expected = UMI_STATUS_ALREADY_EXISTS; }
        Capture(f); CHECK(umi_ai_mcp_server_registry_add(&f->registry, &f->candidate) == expected && Unchanged(f)); return 0;
    }
    if (strcmp(name, "full") == 0 || strcmp(name, "full-duplicate") == 0) {
        umi_ai_mcp_server_registry_init(&f->registry);
        for (size_t i = 0U; i < UMI_AI_MCP_MAX_SERVERS; ++i) {
            char id[64]; (void)snprintf(id, sizeof(id), "server-%zu", i);
            UmiAiMcpServerDescriptor item; CHECK(Server(&item, id) == UMI_STATUS_OK);
            CHECK(umi_ai_mcp_server_registry_add(&f->registry, &item) == UMI_STATUS_OK);
        }
        UmiStatus expected = UMI_STATUS_CAPACITY_EXCEEDED;
        if (strcmp(name, "full-duplicate") == 0) { strcpy(f->candidate.server_id, "server-0"); expected = UMI_STATUS_ALREADY_EXISTS; }
        Capture(f); CHECK(umi_ai_mcp_server_registry_add(&f->registry, &f->candidate) == expected && Unchanged(f)); return 0;
    }
    if (strcmp(name, "unused") == 0) {
        memset(&f->registry.servers[f->registry.count], 0xff, sizeof(f->registry.servers[0]));
        CHECK(UmiAiMcpServerRegistryValidate(&f->registry) == UMI_STATUS_OK);
        CHECK(UmiAiMcpServerRegistryResolve(&f->registry, "one", &found) == UMI_STATUS_OK);
        return Added(f, &f->candidate);
    }
    if (strcmp(name, "alias-inactive") == 0) {
        f->registry.servers[f->registry.count] = f->candidate;
        return Added(f, &f->registry.servers[f->registry.count]);
    }
    if (strncmp(name, "query-", 6U) == 0) {
        char query[UMI_AI_ID_CAPACITY]; memset(query, 'q', sizeof(query));
        UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
        if (strcmp(name, "query-empty") == 0) query[0] = '\0';
        else if (strcmp(name, "query-boundary") == 0) { query[sizeof(query) - 1U] = '\0'; memcpy(f->registry.servers[0].server_id, query, sizeof(query)); expected = UMI_STATUS_OK; }
        else CHECK(strcmp(name, "query-overflow") == 0);
        Capture(f); CHECK(UmiAiMcpServerRegistryResolve(&f->registry, query, &found) == expected && Unchanged(f));
        if (expected == UMI_STATUS_OK) CHECK(found == &f->registry.servers[0]);
        else {
            CHECK(found == &f->candidate && umi_ai_mcp_server_registry_find(&f->registry, query) == NULL);
            CHECK(umi_ai_mcp_server_registry_remove(&f->registry, query) == expected && Unchanged(f));
        }
        return 0;
    }
    if (strncmp(name, "remove-", 7U) == 0) {
        const char *id = strcmp(name, "remove-first") == 0 ? "one" : strcmp(name, "remove-last") == 0 ? "three" : "two";
        if (strcmp(name, "remove-missing") == 0) id = "absent";
        if (strcmp(name, "remove-alias") == 0) id = f->registry.servers[1].server_id;
        Capture(f);
        UmiStatus expected = strcmp(name, "remove-missing") == 0 ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK;
        CHECK(umi_ai_mcp_server_registry_remove(&f->registry, id) == expected);
        if (expected != UMI_STATUS_OK) { CHECK(Unchanged(f)); return 0; }
        const char *first = strcmp(name, "remove-first") == 0 ? "two" : "one";
        const char *second = strcmp(name, "remove-last") == 0 ? "two" : "three";
        CHECK(f->registry.count == 2U && f->registry.revision == f->previous.revision + 1U);
        CHECK(strcmp(f->registry.servers[0].server_id, first) == 0 && strcmp(f->registry.servers[1].server_id, second) == 0);
        CHECK(memcmp(&f->registry.servers[2], &f->previous.servers[2], sizeof(f->registry.servers[2])) == 0);
        CHECK(UmiAiMcpServerRegistryValidate(&f->registry) == UMI_STATUS_OK); return 0;
    }
    if (strcmp(name, "revision-max") == 0 || strcmp(name, "revision-add-last") == 0 || strcmp(name, "revision-remove-last") == 0) {
        f->registry.revision = strcmp(name, "revision-max") == 0 ? UINT64_MAX : UINT64_MAX - 1U;
        if (strcmp(name, "revision-add-last") == 0) CHECK(Added(f, &f->candidate) == 0);
        if (strcmp(name, "revision-remove-last") == 0) CHECK(umi_ai_mcp_server_registry_remove(&f->registry, "three") == UMI_STATUS_OK);
        CHECK(f->registry.revision == UINT64_MAX);
        Capture(f);
        CHECK(umi_ai_mcp_server_registry_add(&f->registry, &f->candidate) == UMI_STATUS_CAPACITY_EXCEEDED && Unchanged(f));
        CHECK(umi_ai_mcp_server_registry_remove(&f->registry, "one") == UMI_STATUS_CAPACITY_EXCEEDED && Unchanged(f));
        CHECK(UmiAiMcpServerRegistryResolve(&f->registry, "one", &found) == UMI_STATUS_OK && found == &f->registry.servers[0]); return 0;
    }
    if (strcmp(name, "nulls") == 0) {
        Capture(f); umi_ai_mcp_server_registry_init(NULL);
        CHECK(UmiAiMcpServerRegistryValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiMcpServerRegistryResolve(NULL, "one", &found) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiMcpServerRegistryResolve(&f->registry, NULL, &found) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiMcpServerRegistryResolve(&f->registry, "one", NULL) == UMI_STATUS_INVALID_ARGUMENT && found == &f->candidate);
        CHECK(umi_ai_mcp_server_registry_add(NULL, &f->candidate) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_mcp_server_registry_add(&f->registry, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_mcp_server_registry_remove(NULL, "one") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_mcp_server_registry_remove(&f->registry, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_mcp_server_registry_find(NULL, "one") == NULL && umi_ai_mcp_server_registry_find(&f->registry, NULL) == NULL && umi_ai_mcp_server_registry_at(NULL, 0U) == NULL);
        CHECK(Unchanged(f)); return 0;
    }
    return 2;
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const Field *field = NULL;
    if (strcmp(argv[2], "all") != 0) {
        for (size_t i = 0U; i < sizeof(fields) / sizeof(fields[0]); ++i)
            if (strcmp(argv[2], fields[i].name) == 0) field = &fields[i];
        if (field == NULL) return 2;
    }
    Fixture *f = calloc(1U, sizeof(*f)); if (f == NULL) return 3;
    int result = Seed(f);
    if (result == 0) result = field == NULL ? Basic(f, argv[1]) : FieldCase(f, argv[1], field);
    free(f); return result;
}
