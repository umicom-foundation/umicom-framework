/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_search_index_limits.c
 *
 * PURPOSE:
 *   Verify exhausted search revisions preserve documents and release all storage.
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
#include <stdlib.h>
#include <string.h>

/* Compile the implementation in this isolated fixture to reach the otherwise
 * impractical final revision without adding a production mutation API.
 * Allocation tracking also checks destruction after refusal. */
static void *allocations[32];
static size_t allocation_count;
static int fail_next_malloc;
static void Remember(void *pointer)
{
    if (pointer == NULL) return;
    assert(allocation_count < sizeof(allocations)/sizeof(allocations[0]));
    allocations[allocation_count++] = pointer;
}
static size_t FindAllocation(void *pointer)
{
    for (size_t i = 0U; i < allocation_count; ++i)
        if (allocations[i] == pointer) return i;
    return allocation_count;
}
static void *IndexMalloc(size_t bytes)
{
    if (fail_next_malloc) { fail_next_malloc = 0; return NULL; }
    void *pointer = malloc(bytes); Remember(pointer); return pointer;
}
static void *IndexCalloc(size_t count, size_t bytes)
{
    void *pointer = calloc(count, bytes); Remember(pointer); return pointer;
}
static void *IndexRealloc(void *pointer, size_t bytes)
{
    size_t index = FindAllocation(pointer);
    assert(pointer == NULL || index < allocation_count);
    void *replacement = realloc(pointer,bytes);
    if (replacement != NULL) {
        if (index < allocation_count) allocations[index] = replacement;
        else Remember(replacement);
    }
    return replacement;
}
static void IndexFree(void *pointer)
{
    if (pointer == NULL) return;
    size_t index = FindAllocation(pointer); assert(index < allocation_count);
    allocations[index] = allocations[--allocation_count];
    free(pointer);
}
#define malloc IndexMalloc
#define calloc IndexCalloc
#define realloc IndexRealloc
#define free IndexFree
#include "../src/editor/workspace_search_index.c"
#undef malloc
#undef calloc
#undef realloc
#undef free

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    UmiEditorWorkspaceSearchIndexConfig config;
    umi_editor_workspace_search_index_config_init(&config);
    UmiEditorWorkspaceSearchIndex *index = NULL;
    assert(umi_editor_workspace_search_index_create(&config,&index) == UMI_STATUS_OK);
    UmiEditorWorkspaceSearchDocumentInput input = {0};
    input.struct_size = (uint32_t)sizeof(input);
    input.api_version = UMI_EDITOR_WORKSPACE_SEARCH_INDEX_API_VERSION;
    input.uri = "file:///example.c"; input.relative_path = "example.c";
    input.language_id = "c"; input.content = "original"; input.content_length = 8U;
    assert(umi_editor_workspace_search_index_upsert(index,&input) == UMI_STATUS_OK);
    int final = strncmp(name,"final-",6U) == 0;
    int allocation = strcmp(name,"allocation-refusal") == 0;
    index->revision = allocation ? 12U : final ? UINT64_MAX - 1U : UINT64_MAX;
    UmiEditorWorkspaceSearchIndexSnapshot before = {0}, after = {0};
    UmiEditorWorkspaceSearchDocumentView borrowed = {0}, retained = {0};
    assert(umi_editor_workspace_search_index_snapshot(index,&before) == UMI_STATUS_OK);
    assert(umi_editor_workspace_search_index_at(index,0U,&borrowed) == UMI_STATUS_OK);
    UmiStatus status = UMI_STATUS_OK;
    int known = 1;
    if (strcmp(name,"clear") == 0 || strcmp(name,"final-clear") == 0)
        status = umi_editor_workspace_search_index_clear(index);
    else if (strcmp(name,"remove") == 0 || strcmp(name,"final-remove") == 0)
        status = umi_editor_workspace_search_index_remove(index,input.uri);
    else if (strcmp(name,"upsert") == 0 || strcmp(name,"new-document") == 0 ||
             strcmp(name,"final-upsert") == 0 || allocation) {
        if (strcmp(name,"new-document") == 0) {
            input.uri = "file:///second.c"; input.relative_path = "second.c";
        }
        input.content = "replacement"; input.content_length = 11U;
        fail_next_malloc = final ? 0 : 1;
        status = umi_editor_workspace_search_index_upsert(index,&input);
        if (!final && !allocation) assert(fail_next_malloc == 1);
        fail_next_malloc = 0;
    } else if (strcmp(name,"missing-remove") == 0) {
        status = umi_editor_workspace_search_index_remove(index,"file:///missing.c");
        assert(status == UMI_STATUS_NOT_FOUND);
    } else known = 0;
    if (known) {
        if (final) {
            assert(status == UMI_STATUS_OK);
            assert(umi_editor_workspace_search_index_revision(index) == UINT64_MAX);
            assert(umi_editor_workspace_search_index_clear(index) == UMI_STATUS_CAPACITY_EXCEEDED);
            if (strcmp(name,"final-upsert") == 0) {
                assert(umi_editor_workspace_search_index_at(index,0U,&retained) == UMI_STATUS_OK);
                assert(strcmp(retained.content,"replacement") == 0);
            } else assert(umi_editor_workspace_search_index_count(index) == 0U);
        } else {
            if (strcmp(name,"missing-remove") != 0)
                assert(status == (allocation ? UMI_STATUS_OUT_OF_MEMORY : UMI_STATUS_CAPACITY_EXCEEDED));
            assert(umi_editor_workspace_search_index_snapshot(index,&after) == UMI_STATUS_OK);
            assert(memcmp(&before,&after,sizeof(before)) == 0);
            assert(umi_editor_workspace_search_index_at(index,0U,&retained) == UMI_STATUS_OK);
            assert(retained.content == borrowed.content && strcmp(borrowed.content,"original") == 0);
        }
    }
    umi_editor_workspace_search_index_destroy(index);
    assert(allocation_count == 0U);
    return known ? 0 : 2;
}
