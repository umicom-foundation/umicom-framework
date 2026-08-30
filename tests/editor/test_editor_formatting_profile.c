/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_formatting_profile.c
 *
 * PURPOSE:
 *   Implement the test editor formatting profile behavior for
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
 * File: tests/editor/test_editor_formatting_profile.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/formatting_profile.h"

static UmiEditorFormattingProfileItem profile(const char *id,
                                              const char *language,
                                              int32_t priority)
{
    UmiEditorFormattingProfileItem item = {0};

    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_EDITOR_FORMATTING_PROFILE_API_VERSION;
    (void)strcpy(item.id, id);
    (void)strcpy(item.language_id, language);
    (void)strcpy(item.provider_id, "native-formatter");
    (void)strcpy(item.label, "Native Formatter");
    item.tab_size = 4U;
    item.max_line_length = 100U;
    item.brace_style = UMI_EDITOR_FORMATTING_BRACES_K_AND_R;
    item.line_ending = UMI_EDITOR_FORMATTING_LINE_ENDING_LF;
    item.priority = priority;
    item.insert_spaces = 1;
    item.trim_trailing_whitespace = 1;
    item.insert_final_newline = 1;
    item.format_on_save = 1;
    item.enabled = 1;
    return item;
}

int main(void)
{
    UmiEditorFormattingProfileRegistry *registry = NULL;
    UmiEditorFormattingProfileItem wildcard = profile("default", "*", 100);
    UmiEditorFormattingProfileItem native_c = profile("native-c", "c", 10);
    UmiEditorFormattingProfileItem actual;
    UmiEditorFormattingProfileSnapshot snapshot;

    assert(umi_editor_formatting_profile_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_editor_formatting_profile_registry_upsert(registry, &wildcard) ==
           UMI_STATUS_OK);
    assert(umi_editor_formatting_profile_registry_upsert(registry, &native_c) ==
           UMI_STATUS_OK);
    assert(umi_editor_formatting_profile_registry_resolve(
               registry, "c", &actual) == UMI_STATUS_OK);
    assert(strcmp(actual.id, "native-c") == 0);
    assert(umi_editor_formatting_profile_registry_resolve(
               registry, "assembly", &actual) == UMI_STATUS_OK);
    assert(strcmp(actual.id, "default") == 0);
    assert(umi_editor_formatting_profile_registry_snapshot(
               registry, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.profile_count == 2U);
    assert(snapshot.enabled_profile_count == 2U);
    assert(snapshot.language_count == 2U);
    assert(umi_editor_formatting_profile_registry_remove(
               registry, "native-c") == UMI_STATUS_OK);
    assert(umi_editor_formatting_profile_registry_count(registry) == 1U);
    umi_editor_formatting_profile_registry_destroy(registry);
    return 0;
}
