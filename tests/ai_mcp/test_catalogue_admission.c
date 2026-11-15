/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_catalogue_admission.c
 *
 * PURPOSE:
 *   Verify bounded resource and prompt storage, checked resolution and failure preservation.
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

typedef struct Field { const char *name; size_t offset, capacity; } Field;
typedef struct Adapter {
    size_t item_size, catalogue_size, limit, identity_capacity, count_offset, revision_offset;
    Field fields[4]; size_t field_count;
    void (*init)(void *);
    UmiStatus (*descriptor)(const void *);
    UmiStatus (*validate)(const void *);
    UmiStatus (*add)(void *, const void *);
    UmiStatus (*resolve)(const void *, const char *, const void **);
    const void *(*find)(const void *, const char *);
    const void *(*at)(const void *, size_t);
} Adapter;

static void ResourceInit(void *c) { umi_ai_mcp_resource_catalogue_init(c); }
static UmiStatus ResourceDescriptor(const void *p) { return UmiAiMcpResourceDescriptorValidate(p); }
static UmiStatus ResourceValidate(const void *c) { return UmiAiMcpResourceCatalogueValidate(c); }
static UmiStatus ResourceAdd(void *c, const void *p) { return umi_ai_mcp_resource_catalogue_add(c, p); }
static UmiStatus ResourceResolve(const void *c, const char *id, const void **out)
{
    if (out == NULL) return UmiAiMcpResourceCatalogueResolve(c, id, NULL);
    const UmiAiMcpResourceDescriptor *item = *out;
    UmiStatus status = UmiAiMcpResourceCatalogueResolve(c, id, &item);
    *out = item; return status;
}
static const void *ResourceFind(const void *c, const char *id) { return umi_ai_mcp_resource_catalogue_find(c, id); }
static const void *ResourceAt(const void *c, size_t i) { return umi_ai_mcp_resource_catalogue_at(c, i); }
static const Adapter resource_adapter = {
    sizeof(UmiAiMcpResourceDescriptor), sizeof(UmiAiMcpResourceCatalogue), UMI_AI_MCP_MAX_RESOURCES, UMI_AI_TEXT_CAPACITY,
    offsetof(UmiAiMcpResourceCatalogue, count), offsetof(UmiAiMcpResourceCatalogue, revision),
    {{"identity", offsetof(UmiAiMcpResourceDescriptor, uri), sizeof(((UmiAiMcpResourceDescriptor *)0)->uri)}, {"name", offsetof(UmiAiMcpResourceDescriptor, name), sizeof(((UmiAiMcpResourceDescriptor *)0)->name)}, {"description", offsetof(UmiAiMcpResourceDescriptor, description), sizeof(((UmiAiMcpResourceDescriptor *)0)->description)}, {"mime_type", offsetof(UmiAiMcpResourceDescriptor, mime_type), sizeof(((UmiAiMcpResourceDescriptor *)0)->mime_type)}}, 4U,
    ResourceInit, ResourceDescriptor, ResourceValidate, ResourceAdd, ResourceResolve, ResourceFind, ResourceAt
};

static void PromptInit(void *c) { umi_ai_mcp_prompt_catalogue_init(c); }
static UmiStatus PromptDescriptor(const void *p) { return UmiAiMcpPromptDescriptorValidate(p); }
static UmiStatus PromptValidate(const void *c) { return UmiAiMcpPromptCatalogueValidate(c); }
static UmiStatus PromptAdd(void *c, const void *p) { return umi_ai_mcp_prompt_catalogue_add(c, p); }
static UmiStatus PromptResolve(const void *c, const char *id, const void **out)
{
    if (out == NULL) return UmiAiMcpPromptCatalogueResolve(c, id, NULL);
    const UmiAiMcpPromptDescriptor *item = *out;
    UmiStatus status = UmiAiMcpPromptCatalogueResolve(c, id, &item);
    *out = item; return status;
}
static const void *PromptFind(const void *c, const char *id) { return umi_ai_mcp_prompt_catalogue_find(c, id); }
static const void *PromptAt(const void *c, size_t i) { return umi_ai_mcp_prompt_catalogue_at(c, i); }
static const Adapter prompt_adapter = {
    sizeof(UmiAiMcpPromptDescriptor), sizeof(UmiAiMcpPromptCatalogue), UMI_AI_MCP_MAX_PROMPTS, UMI_AI_ID_CAPACITY,
    offsetof(UmiAiMcpPromptCatalogue, count), offsetof(UmiAiMcpPromptCatalogue, revision),
    {{"identity", offsetof(UmiAiMcpPromptDescriptor, name), sizeof(((UmiAiMcpPromptDescriptor *)0)->name)}, {"description", offsetof(UmiAiMcpPromptDescriptor, description), sizeof(((UmiAiMcpPromptDescriptor *)0)->description)}, {"arguments_schema", offsetof(UmiAiMcpPromptDescriptor, arguments_schema), sizeof(((UmiAiMcpPromptDescriptor *)0)->arguments_schema)}, {NULL, 0U, 0U}}, 3U,
    PromptInit, PromptDescriptor, PromptValidate, PromptAdd, PromptResolve, PromptFind, PromptAt
};

typedef struct Fixture { const Adapter *api; void *catalogue, *previous, *item; } Fixture;
static size_t *Count(Fixture *f) { return (size_t *)((unsigned char *)f->catalogue + f->api->count_offset); }
static uint64_t *Revision(Fixture *f) { return (uint64_t *)((unsigned char *)f->catalogue + f->api->revision_offset); }
static void *RawItem(Fixture *f, size_t index) { return (unsigned char *)f->catalogue + index * f->api->item_size; }
static void Candidate(Fixture *f, size_t index)
{
    memset(f->item, 0, f->api->item_size);
    (void)snprintf(f->item, f->api->identity_capacity, "entry-%zu", index);
}
static void Save(Fixture *f) { memcpy(f->previous, f->catalogue, f->api->catalogue_size); }
static int Unchanged(Fixture *f) { return memcmp(f->previous, f->catalogue, f->api->catalogue_size) == 0; }
static int Setup(Fixture *f)
{
    f->api->init(f->catalogue); Candidate(f, 0U);
    CHECK(f->api->add(f->catalogue, f->item) == UMI_STATUS_OK);
    CHECK(*Count(f) == 1U && *Revision(f) == 2U);
    return 0;
}
static int RefuseMalformed(Fixture *f)
{
    Save(f); const void *output = f->item;
    CHECK(f->api->validate(f->catalogue) == UMI_STATUS_INVALID_STATE);
    CHECK(f->api->resolve(f->catalogue, "entry-0", &output) == UMI_STATUS_INVALID_STATE && output == f->item);
    CHECK(f->api->find(f->catalogue, "entry-0") == NULL && f->api->at(f->catalogue, 0U) == NULL);
    Candidate(f, 9U);
    CHECK(f->api->add(f->catalogue, f->item) == UMI_STATUS_INVALID_STATE);
    CHECK(Unchanged(f)); return 0;
}
static int FieldCase(Fixture *f, const char *mode, const char *name)
{
    const Field *field = NULL;
    for (size_t i = 0U; i < f->api->field_count; ++i)
        if (strcmp(name, f->api->fields[i].name) == 0) field = &f->api->fields[i];
    CHECK(field != NULL);
    if (strcmp(mode, "candidate-unterminated") == 0 || strcmp(mode, "candidate-boundary") == 0) {
        Candidate(f, 1U); unsigned char *text = (unsigned char *)f->item + field->offset;
        memset(text, 'x', field->capacity);
        const int valid = strcmp(mode, "candidate-boundary") == 0;
        if (valid) text[field->capacity - 1U] = 0;
        Save(f);
        CHECK(f->api->descriptor(f->item) == (valid ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT));
        CHECK(f->api->add(f->catalogue, f->item) == (valid ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT));
        if (valid) {
            CHECK(*Count(f) == 2U && *Revision(f) == 3U);
            CHECK(memcmp(RawItem(f, 1U), f->item, f->api->item_size) == 0);
        } else CHECK(Unchanged(f));
        return 0;
    }
    size_t index = strcmp(mode, "existing-late") == 0 ? 1U : 0U;
    if (index == 1U) { Candidate(f, 1U); CHECK(f->api->add(f->catalogue, f->item) == UMI_STATUS_OK); }
    memset((unsigned char *)RawItem(f, index) + field->offset, 'x', field->capacity);
    return RefuseMalformed(f);
}

static int Basic(Fixture *f, const char *name)
{
    if (strcmp(name, "empty") == 0) {
        f->api->init(f->catalogue); Save(f); const void *out = f->item;
        CHECK(f->api->validate(f->catalogue) == UMI_STATUS_OK);
        CHECK(f->api->resolve(f->catalogue, "missing", &out) == UMI_STATUS_NOT_FOUND && out == f->item);
        CHECK(f->api->find(f->catalogue, "missing") == NULL && f->api->at(f->catalogue, 0U) == NULL);
        CHECK(Unchanged(f));
    } else if (strcmp(name, "resolve") == 0) {
        Save(f); const void *out = NULL;
        CHECK(f->api->resolve(f->catalogue, "entry-0", &out) == UMI_STATUS_OK && out == RawItem(f, 0U));
        CHECK(f->api->find(f->catalogue, "entry-0") == out && f->api->at(f->catalogue, 0U) == out);
        CHECK(f->api->resolve(f->catalogue, "missing", &out) == UMI_STATUS_NOT_FOUND && out == RawItem(f, 0U));
        CHECK(f->api->at(f->catalogue, 1U) == NULL && f->api->at(f->catalogue, SIZE_MAX) == NULL);
        CHECK(Unchanged(f));
    } else if (strcmp(name, "count-overflow") == 0 || strcmp(name, "count-max") == 0) {
        *Count(f) = strcmp(name, "count-max") == 0 ? SIZE_MAX : f->api->limit + 1U;
        return RefuseMalformed(f);
    } else if (strcmp(name, "duplicate-active") == 0) {
        memcpy(RawItem(f, 1U), RawItem(f, 0U), f->api->item_size); *Count(f) = 2U;
        return RefuseMalformed(f);
    } else if (strcmp(name, "empty-active") == 0) {
        ((char *)RawItem(f, 0U))[0] = '\0'; return RefuseMalformed(f);
    } else if (strcmp(name, "duplicate-candidate") == 0) {
        Save(f); CHECK(f->api->add(f->catalogue, RawItem(f, 0U)) == UMI_STATUS_ALREADY_EXISTS); CHECK(Unchanged(f));
    } else if (strcmp(name, "empty-candidate") == 0) {
        ((char *)f->item)[0] = '\0'; Save(f);
        CHECK(f->api->descriptor(f->item) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(f->api->add(f->catalogue, f->item) == UMI_STATUS_INVALID_ARGUMENT); CHECK(Unchanged(f));
    } else if (strcmp(name, "full") == 0) {
        for (size_t i = 1U; i < f->api->limit; ++i) { Candidate(f, i); CHECK(f->api->add(f->catalogue, f->item) == UMI_STATUS_OK); }
        CHECK(f->api->validate(f->catalogue) == UMI_STATUS_OK && *Count(f) == f->api->limit);
        const void *out = NULL; CHECK(f->api->resolve(f->catalogue, f->item, &out) == UMI_STATUS_OK && out == RawItem(f, f->api->limit - 1U));
        Save(f); CHECK(f->api->add(f->catalogue, RawItem(f, 0U)) == UMI_STATUS_ALREADY_EXISTS);
        Candidate(f, f->api->limit); CHECK(f->api->add(f->catalogue, f->item) == UMI_STATUS_CAPACITY_EXCEEDED); CHECK(Unchanged(f));
    } else if (strcmp(name, "unused") == 0) {
        memset(RawItem(f, f->api->limit - 1U), 0x7f, f->api->item_size);
        CHECK(f->api->validate(f->catalogue) == UMI_STATUS_OK);
        CHECK(f->api->find(f->catalogue, "entry-0") == RawItem(f, 0U));
        Candidate(f, 1U); CHECK(f->api->add(f->catalogue, f->item) == UMI_STATUS_OK);
        CHECK(((unsigned char *)RawItem(f, f->api->limit - 1U))[0] == 0x7f);
    } else if (strcmp(name, "alias-inactive") == 0) {
        Candidate(f, 1U); memcpy(RawItem(f, 1U), f->item, f->api->item_size);
        CHECK(f->api->add(f->catalogue, RawItem(f, 1U)) == UMI_STATUS_OK);
        CHECK(*Count(f) == 2U && memcmp(RawItem(f, 1U), f->item, f->api->item_size) == 0);
    } else if (strcmp(name, "revision-max") == 0) {
        *Revision(f) = UINT64_MAX; Candidate(f, 1U); Save(f);
        CHECK(f->api->validate(f->catalogue) == UMI_STATUS_OK);
        CHECK(f->api->add(f->catalogue, f->item) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(f->api->add(f->catalogue, NULL) == UMI_STATUS_CAPACITY_EXCEEDED); CHECK(Unchanged(f));
    } else if (strcmp(name, "revision-last") == 0) {
        *Revision(f) = UINT64_MAX - 1U; Candidate(f, 1U);
        CHECK(f->api->add(f->catalogue, f->item) == UMI_STATUS_OK && *Revision(f) == UINT64_MAX);
        Candidate(f, 2U); Save(f); CHECK(f->api->add(f->catalogue, f->item) == UMI_STATUS_CAPACITY_EXCEEDED); CHECK(Unchanged(f));
    } else if (strcmp(name, "query-boundary") == 0) {
        memset(f->item, 'q', f->api->identity_capacity - 1U);
        ((char *)f->item)[f->api->identity_capacity - 1U] = '\0';
        CHECK(f->api->add(f->catalogue, f->item) == UMI_STATUS_OK);
        const void *out = NULL; CHECK(f->api->resolve(f->catalogue, f->item, &out) == UMI_STATUS_OK && out == RawItem(f, 1U));
    } else if (strcmp(name, "query-overflow") == 0) {
        char *query = malloc(f->api->identity_capacity + 1U); CHECK(query != NULL);
        memset(query, 'q', f->api->identity_capacity); query[f->api->identity_capacity] = '\0';
        const void *out = f->item; Save(f);
        UmiStatus status = f->api->resolve(f->catalogue, query, &out);
        const void *found = f->api->find(f->catalogue, query); free(query);
        CHECK(status == UMI_STATUS_INVALID_ARGUMENT && out == f->item && found == NULL); CHECK(Unchanged(f));
    } else if (strcmp(name, "nulls") == 0) {
        const void *out = f->item; Save(f); f->api->init(NULL);
        CHECK(f->api->descriptor(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(f->api->validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(f->api->add(NULL, f->item) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(f->api->add(f->catalogue, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(f->api->resolve(NULL, "entry-0", &out) == UMI_STATUS_INVALID_ARGUMENT && out == f->item);
        CHECK(f->api->resolve(f->catalogue, NULL, &out) == UMI_STATUS_INVALID_ARGUMENT && out == f->item);
        CHECK(f->api->resolve(f->catalogue, "", &out) == UMI_STATUS_INVALID_ARGUMENT && out == f->item);
        CHECK(f->api->resolve(f->catalogue, "entry-0", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(f->api->find(NULL, "entry-0") == NULL && f->api->find(f->catalogue, NULL) == NULL);
        CHECK(f->api->at(NULL, 0U) == NULL); CHECK(Unchanged(f));
    } else return 2;
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 4) return 2;
    const Adapter *api = strcmp(argv[1], "resource") == 0 ? &resource_adapter :
        strcmp(argv[1], "prompt") == 0 ? &prompt_adapter : NULL;
    if (api == NULL) return 2;
    Fixture fixture = {api, calloc(1U, api->catalogue_size), malloc(api->catalogue_size), calloc(1U, api->item_size)};
    if (fixture.catalogue == NULL || fixture.previous == NULL || fixture.item == NULL) {
        free(fixture.item); free(fixture.previous); free(fixture.catalogue); return 3;
    }
    int result = Setup(&fixture);
    if (result == 0) result = strcmp(argv[3], "all") == 0 ? Basic(&fixture, argv[2]) : FieldCase(&fixture, argv[2], argv[3]);
    free(fixture.item); free(fixture.previous); free(fixture.catalogue); return result;
}
