/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_editing_productivity.c
 *
 * PURPOSE:
 *   Implement the test editor editing productivity behavior for
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
 * Umicom Framework
 * File: tests/editor/test_editor_editing_productivity.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/editing_productivity.h"

int main(void)
{
    UmiEditorEditingProductivity *productivity = NULL;
    UmiEditorFormattingProfileItem profile = {0};
    UmiEditorEditingProductivitySnapshot snapshot;
    uint64_t revision;

    assert(umi_editor_editing_productivity_create(&productivity) ==
           UMI_STATUS_OK);
    assert(umi_editor_editing_productivity_formatting_profiles(productivity) !=
           NULL);
    assert(umi_editor_editing_productivity_formatting_session(productivity) !=
           NULL);
    assert(umi_editor_editing_productivity_import_organisation(productivity) !=
           NULL);
    assert(umi_editor_editing_productivity_snippet_session(productivity) !=
           NULL);
    assert(umi_editor_editing_productivity_linked_editing(productivity) !=
           NULL);
    revision = umi_editor_editing_productivity_revision(productivity);
    profile.struct_size = (uint32_t)sizeof(profile);
    profile.api_version = UMI_EDITOR_FORMATTING_PROFILE_API_VERSION;
    (void)strcpy(profile.id, "native-c");
    (void)strcpy(profile.language_id, "c");
    (void)strcpy(profile.provider_id, "native-c");
    (void)strcpy(profile.label, "Native C");
    profile.tab_size = 4U;
    profile.max_line_length = 100U;
    profile.enabled = 1;
    assert(umi_editor_formatting_profile_registry_upsert(
               umi_editor_editing_productivity_formatting_profiles(
                   productivity), &profile) == UMI_STATUS_OK);
    assert(umi_editor_editing_productivity_set_feature_enabled(
               productivity, UMI_EDITOR_PRODUCTIVITY_SNIPPETS, 0) ==
           UMI_STATUS_OK);
    assert(!umi_editor_editing_productivity_feature_enabled(
        productivity, UMI_EDITOR_PRODUCTIVITY_SNIPPETS));
    assert(umi_editor_editing_productivity_snapshot(productivity, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.formatting_profile_count == 1U);
    assert(!snapshot.snippets_enabled);
    assert(snapshot.revision > revision);
    umi_editor_editing_productivity_destroy(productivity);
    return 0;
}
