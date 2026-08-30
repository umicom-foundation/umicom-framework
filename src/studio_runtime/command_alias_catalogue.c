/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_alias_catalogue.c
 *
 * PURPOSE:
 *   Implement Studio command alias discovery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_alias_catalogue.h"

#include <string.h>

typedef const UmiStudioRuntimeCommandAliasDefinition *(*AliasFactory)(void);

static const AliasFactory FACTORIES[] = {
    umi_studio_command_alias_view_explorer,
    umi_studio_command_alias_view_search,
    umi_studio_command_alias_view_problems,
    umi_studio_command_alias_view_output,
    umi_studio_command_alias_view_terminal,
    umi_studio_command_alias_view_source_control,
    umi_studio_command_alias_view_test_explorer,
    umi_studio_command_alias_view_debug,
    umi_studio_command_alias_view_build_dashboard,
    umi_studio_command_alias_view_outline,
    umi_studio_command_alias_ai_assistant_open,
    umi_studio_command_alias_file_save,
    umi_studio_command_alias_document_save,
    umi_studio_command_alias_view_ai_review,
    umi_studio_command_alias_view_ai_approvals,
    umi_studio_command_alias_view_ai_tools,
    umi_studio_command_alias_view_ai_validation,
    umi_studio_command_alias_navigate_back,
    umi_studio_command_alias_navigate_forward,
    umi_studio_command_alias_layout_manager,
    umi_studio_command_alias_layout_save,
    umi_studio_command_alias_layout_restore
};

size_t umi_studio_command_alias_count(void)
{
    return sizeof(FACTORIES) / sizeof(FACTORIES[0]);
}

const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_at(size_t index)
{
    return index < umi_studio_command_alias_count()
        ? FACTORIES[index]()
        : NULL;
}

const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_find(const char *alias_id)
{
    size_t index;

    if (alias_id == NULL) return NULL;

    for (index = 0U; index < umi_studio_command_alias_count(); ++index) {
        const UmiStudioRuntimeCommandAliasDefinition *item = FACTORIES[index]();

        if (item != NULL && strcmp(item->alias_id, alias_id) == 0) {
            return item;
        }
    }

    return NULL;
}
