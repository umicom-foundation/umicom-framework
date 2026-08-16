/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_quick_documentation.c
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/quick_documentation.h"

static UmiEditorQuickDocumentationSection documentation_section(
    const char *id,
    const char *symbol_id,
    UmiEditorQuickDocumentationSectionKind kind,
    int32_t order)
{
    UmiEditorQuickDocumentationSection section;

    (void)memset(&section, 0, sizeof(section));
    section.struct_size = (uint32_t)sizeof(section);
    section.api_version = UMI_EDITOR_QUICK_DOCUMENTATION_API_VERSION;
    (void)strcpy(section.id, id);
    (void)strcpy(section.symbol_id, symbol_id);
    (void)strcpy(section.provider_id, "indexed-manuals");
    (void)strcpy(section.title, "Reference");
    (void)strcpy(section.language_id, "c");
    (void)strcpy(section.content, "Reusable Framework documentation.");
    assert(umi_editor_source_location_initialize(
               &section.location, "file:///workspace/example.c", 2U, 0U) ==
           UMI_STATUS_OK);
    section.section_kind = kind;
    section.content_kind = UMI_EDITOR_DOCUMENTATION_MARKDOWN;
    section.order = order;
    section.trusted = 1;
    section.visible = 1;
    if (kind == UMI_EDITOR_DOCUMENTATION_LINK) {
        (void)strcpy(section.source_uri, "https://docs.example/reference");
    }
    return section;
}

int main(void)
{
    UmiEditorQuickDocumentation *documentation = NULL;
    UmiEditorQuickDocumentationSection summary = documentation_section(
        "doc.summary", "calculate", UMI_EDITOR_DOCUMENTATION_SUMMARY, 10);
    UmiEditorQuickDocumentationSection link = documentation_section(
        "doc.link", "calculate", UMI_EDITOR_DOCUMENTATION_LINK, 50);
    UmiEditorQuickDocumentationSection other = documentation_section(
        "doc.other", "render", UMI_EDITOR_DOCUMENTATION_SUMMARY, 10);
    UmiEditorQuickDocumentationSection actual;
    UmiEditorQuickDocumentationSnapshot snapshot;

    assert(umi_editor_quick_documentation_create(&documentation) ==
           UMI_STATUS_OK);
    assert(umi_editor_quick_documentation_upsert(documentation, &link) ==
           UMI_STATUS_OK);
    assert(umi_editor_quick_documentation_upsert(documentation, &summary) ==
           UMI_STATUS_OK);
    assert(umi_editor_quick_documentation_upsert(documentation, &other) ==
           UMI_STATUS_OK);
    assert(umi_editor_quick_documentation_sort(documentation) == UMI_STATUS_OK);
    assert(umi_editor_quick_documentation_select_symbol(
               documentation, "calculate") == UMI_STATUS_OK);
    assert(umi_editor_quick_documentation_set_pinned(documentation, 1) ==
           UMI_STATUS_OK);
    assert(umi_editor_quick_documentation_selected_at(
               documentation, 0U, &actual) == UMI_STATUS_OK);
    assert(strcmp(actual.id, "doc.summary") == 0);
    assert(umi_editor_quick_documentation_snapshot(documentation, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.section_count == 3U);
    assert(snapshot.selected_section_count == 2U);
    assert(snapshot.trusted_section_count == 3U);
    assert(snapshot.link_section_count == 1U);
    assert(snapshot.pinned);
    link.source_uri[0] = '\0';
    assert(umi_editor_quick_documentation_upsert(documentation, &link) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_editor_quick_documentation_remove_symbol(
               documentation, "calculate") == UMI_STATUS_OK);
    assert(umi_editor_quick_documentation_set_pinned(documentation, 1) ==
           UMI_STATUS_INVALID_STATE);
    umi_editor_quick_documentation_destroy(documentation);
    return 0;
}
