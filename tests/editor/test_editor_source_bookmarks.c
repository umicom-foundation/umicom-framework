/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_source_bookmarks.c
 *
 * PURPOSE:
 *   Implement the test editor source bookmarks behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework source bookmark registry tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/source_bookmark.h"

static UmiEditorSourceBookmark bookmark(const char *id,
                                        const char *group,
                                        uint64_t line,
                                        int enabled)
{
    UmiEditorSourceBookmark value = {0};
    value.struct_size = (uint32_t)sizeof(value);
    value.api_version = UMI_EDITOR_SOURCE_BOOKMARK_API_VERSION;
    (void)strcpy(value.id, id);
    (void)strcpy(value.group, group);
    value.enabled = enabled;
    value.persistent = 1;
    assert(umi_editor_source_location_initialize(&value.location,
                                                  "file:///project/main.c",
                                                  line,
                                                  0U) == UMI_STATUS_OK);
    value.location.kind = UMI_EDITOR_SOURCE_LOCATION_BOOKMARK;
    return value;
}

int main(void)
{
    UmiEditorSourceBookmarkRegistry *registry = NULL;
    UmiEditorSourceBookmark first = bookmark("first", "review", 2U, 1);
    UmiEditorSourceBookmark second = bookmark("second", "review", 7U, 1);
    UmiEditorSourceBookmark disabled = bookmark("disabled", "review", 5U, 0);
    UmiEditorSourceBookmark other = bookmark("other", "debug", 10U, 1);
    UmiEditorSourceBookmark found;
    UmiEditorSourceLocation from;

    assert(umi_editor_source_bookmark_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_editor_source_bookmark_registry_upsert(registry, &second) ==
           UMI_STATUS_OK);
    assert(umi_editor_source_bookmark_registry_upsert(registry, &first) ==
           UMI_STATUS_OK);
    assert(umi_editor_source_bookmark_registry_upsert(registry, &disabled) ==
           UMI_STATUS_OK);
    assert(umi_editor_source_bookmark_registry_upsert(registry, &other) ==
           UMI_STATUS_OK);
    assert(umi_editor_source_bookmark_registry_count(registry) == 4U);

    assert(umi_editor_source_location_initialize(&from,
                                                  "file:///project/main.c",
                                                  3U,
                                                  0U) == UMI_STATUS_OK);
    assert(umi_editor_source_bookmark_registry_next(registry,
                                                     &from,
                                                     "review",
                                                     0,
                                                     &found) == UMI_STATUS_OK);
    assert(strcmp(found.id, "second") == 0);
    assert(umi_editor_source_bookmark_registry_previous(registry,
                                                         &from,
                                                         "review",
                                                         0,
                                                         &found) ==
           UMI_STATUS_OK);
    assert(strcmp(found.id, "first") == 0);
    from.line = 20U;
    from.end_line = 20U;
    assert(umi_editor_source_bookmark_registry_next(registry,
                                                     &from,
                                                     "review",
                                                     1,
                                                     &found) == UMI_STATUS_OK);
    assert(strcmp(found.id, "first") == 0);

    (void)strcpy(first.description, "entry point");
    assert(umi_editor_source_bookmark_registry_upsert(registry, &first) ==
           UMI_STATUS_OK);
    assert(umi_editor_source_bookmark_registry_find(registry, "first", &found) ==
           UMI_STATUS_OK);
    assert(strcmp(found.description, "entry point") == 0);
    assert(umi_editor_source_bookmark_registry_remove(registry, "second") ==
           UMI_STATUS_OK);
    assert(umi_editor_source_bookmark_registry_count(registry) == 3U);

    umi_editor_source_bookmark_registry_destroy(registry);
    return 0;
}
