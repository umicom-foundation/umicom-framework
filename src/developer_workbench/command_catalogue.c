/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/command_catalogue.c
 *
 * PURPOSE:
 *   Combine all professional developer-workbench command groups into one
 *   searchable Framework-owned catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/command_catalogue.h"

#include <string.h>

#include "umicom/developer_workbench/commands/commands.h"

typedef const UmiDeveloperWorkbenchCommandDefinition *
    (*CommandProvider)(void);
typedef size_t (*CommandCountProvider)(void);

typedef struct CommandGroupProvider {
    CommandProvider commands;
    CommandCountProvider count;
} CommandGroupProvider;

static const CommandGroupProvider PROVIDERS[] = {
    {
        umi_developer_workbench_file_commands,
        umi_developer_workbench_file_command_count
    },
    {
        umi_developer_workbench_edit_commands,
        umi_developer_workbench_edit_command_count
    },
    {
        umi_developer_workbench_view_commands,
        umi_developer_workbench_view_command_count
    },
    {
        umi_developer_workbench_navigate_commands,
        umi_developer_workbench_navigate_command_count
    },
    {
        umi_developer_workbench_build_commands,
        umi_developer_workbench_build_command_count
    },
    {
        umi_developer_workbench_run_commands,
        umi_developer_workbench_run_command_count
    },
    {
        umi_developer_workbench_test_commands,
        umi_developer_workbench_test_command_count
    },
    {
        umi_developer_workbench_source_control_commands,
        umi_developer_workbench_source_control_command_count
    },
    {
        umi_developer_workbench_tools_commands,
        umi_developer_workbench_tools_command_count
    },
    {
        umi_developer_workbench_window_commands,
        umi_developer_workbench_window_command_count
    },
    {
        umi_developer_workbench_help_commands,
        umi_developer_workbench_help_command_count
    },
    {
        umi_developer_workbench_terminal_commands,
        umi_developer_workbench_terminal_command_count
    },
    {
        umi_developer_workbench_debug_commands,
        umi_developer_workbench_debug_command_count
    },
    {
        umi_developer_workbench_project_commands,
        umi_developer_workbench_project_command_count
    },
    {
        umi_developer_workbench_layout_commands,
        umi_developer_workbench_layout_command_count
    },
    {
        umi_developer_workbench_ai_commands,
        umi_developer_workbench_ai_command_count
    },
    {
        umi_developer_workbench_extensions_commands,
        umi_developer_workbench_extensions_command_count
    },
    {
        umi_developer_workbench_settings_commands,
        umi_developer_workbench_settings_command_count
    },
    {
        umi_developer_workbench_search_commands,
        umi_developer_workbench_search_command_count
    },
    {
        umi_developer_workbench_application_commands,
        umi_developer_workbench_application_command_count
    },
    {
        umi_developer_workbench_extensions_settings_commands,
        umi_developer_workbench_extensions_settings_command_count
    },
    {
        umi_developer_workbench_workspace_commands,
        umi_developer_workbench_workspace_command_count
    }
};

/*
 * Return the number of records represented by developer workbench command catalogue
 * without changing their state.
 */
size_t umi_developer_workbench_command_catalogue_count(void)
{
    size_t total = 0U;
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < sizeof(PROVIDERS) / sizeof(PROVIDERS[0]);
         ++index) {
        total += PROVIDERS[index].count();
    }

    return total;
}

/*
 * Find developer workbench command catalogue while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_command_catalogue_at(size_t index)
{
    size_t group;

    /* Visit each bounded item once so every record receives the same rule. */
    for (group = 0U;
         group < sizeof(PROVIDERS) / sizeof(PROVIDERS[0]);
         ++group) {
        const size_t count = PROVIDERS[group].count();

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index < count) {
            return &PROVIDERS[group].commands()[index];
        }

        index -= count;
    }

    return NULL;
}

/*
 * Find developer workbench command catalogue while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_command_catalogue_find(const char *command_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_id == NULL) return NULL;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_developer_workbench_command_catalogue_count();
         ++index) {
        const UmiDeveloperWorkbenchCommandDefinition *definition =
            umi_developer_workbench_command_catalogue_at(index);

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (definition != NULL &&
            strcmp(definition->command_id, command_id) == 0) {
            return definition;
        }
    }

    return NULL;
}

/*
 * Check that developer workbench command catalogue satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_developer_workbench_command_catalogue_validate(void)
{
    size_t index;
    size_t other;
    const size_t count =
        umi_developer_workbench_command_catalogue_count();

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count == 0U || count > UMI_DEVELOPER_WORKBENCH_MAX_COMMANDS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        const UmiDeveloperWorkbenchCommandDefinition *definition =
            umi_developer_workbench_command_catalogue_at(index);

        /* Apply this operation only while the related capability or state is available. */
        if (umi_developer_workbench_command_definition_validate(definition) !=
            UMI_STATUS_OK) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        /* Visit each bounded item once so every record receives the same rule. */
        for (other = index + 1U; other < count; ++other) {
            const UmiDeveloperWorkbenchCommandDefinition *candidate =
                umi_developer_workbench_command_catalogue_at(other);

            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (candidate != NULL &&
                strcmp(definition->command_id, candidate->command_id) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }

    return UMI_STATUS_OK;
}
