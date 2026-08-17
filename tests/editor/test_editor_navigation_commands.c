/*-----------------------------------------------------------------------------
 * Umicom Framework source navigation command catalogue tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/navigation_command.h"

int main(void)
{
    const UmiEditorNavigationCommandDescriptor *descriptor;
    size_t index;

    assert(umi_editor_navigation_command_count() == 38U);
    for (index = 0U; index < umi_editor_navigation_command_count(); ++index) {
        descriptor = umi_editor_navigation_command_at(index);
        assert(descriptor != NULL);
        assert(descriptor->struct_size == (uint32_t)sizeof(*descriptor));
        assert(descriptor->api_version ==
               UMI_EDITOR_NAVIGATION_COMMAND_API_VERSION);
        assert(descriptor->id[0] != '\0' && descriptor->label[0] != '\0');
        assert(umi_editor_navigation_command_find(descriptor->id) == descriptor);
        assert(umi_editor_navigation_command_for_kind(descriptor->kind) ==
               descriptor);
    }
    descriptor = umi_editor_navigation_command_find(
        "editor.navigate.definition");
    assert(descriptor != NULL && strcmp(descriptor->default_key, "F12") == 0);
    assert(descriptor->requires_symbol);
    descriptor = umi_editor_navigation_command_find(
        "editor.navigate.call-hierarchy");
    assert(descriptor != NULL && descriptor->requires_symbol);
    descriptor = umi_editor_navigation_command_find(
        "editor.navigate.peek-close");
    assert(descriptor != NULL &&
           strcmp(descriptor->default_key, "Escape") == 0);
    descriptor = umi_editor_navigation_command_find(
        "editor.navigate.reference-next");
    assert(descriptor != NULL && strcmp(descriptor->default_key, "F4") == 0);
    assert(descriptor->query_kind == UMI_EDITOR_NAVIGATION_QUERY_REFERENCE);
    descriptor = umi_editor_navigation_command_find(
        "editor.navigate.provider-diagnostics");
    assert(descriptor != NULL && descriptor->opens_multiple_results);
    assert(umi_editor_navigation_command_find("editor.navigate.missing") == NULL);
    return 0;
}
