/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/builtin_profiles.c
 *
 * PURPOSE:
 *   Enumerate and install every reusable Framework application-shell profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application_shell/builtin_profiles.h"

#include <string.h>

#include "umicom/application_shell/profiles/file_menu.h"
#include "umicom/application_shell/profiles/edit_menu.h"
#include "umicom/application_shell/profiles/view_menu.h"
#include "umicom/application_shell/profiles/navigate_menu.h"
#include "umicom/application_shell/profiles/build_menu.h"
#include "umicom/application_shell/profiles/run_menu.h"
#include "umicom/application_shell/profiles/test_menu.h"
#include "umicom/application_shell/profiles/source_control_menu.h"
#include "umicom/application_shell/profiles/tools_menu.h"
#include "umicom/application_shell/profiles/window_menu.h"
#include "umicom/application_shell/profiles/help_menu.h"
#include "umicom/application_shell/profiles/main_toolbar.h"
#include "umicom/application_shell/profiles/activity_bar.h"
#include "umicom/application_shell/profiles/status_bar.h"
#include "umicom/application_shell/profiles/start_center.h"
#include "umicom/application_shell/profiles/project_explorer.h"
#include "umicom/application_shell/profiles/editor.h"
#include "umicom/application_shell/profiles/search.h"
#include "umicom/application_shell/profiles/problems.h"
#include "umicom/application_shell/profiles/output.h"
#include "umicom/application_shell/profiles/terminal.h"
#include "umicom/application_shell/profiles/source_control.h"
#include "umicom/application_shell/profiles/test_explorer.h"
#include "umicom/application_shell/profiles/debug.h"
#include "umicom/application_shell/profiles/build_dashboard.h"
#include "umicom/application_shell/profiles/deploy_dashboard.h"
#include "umicom/application_shell/profiles/extensions.h"
#include "umicom/application_shell/profiles/settings.h"
#include "umicom/application_shell/profiles/ai_assistant.h"
#include "umicom/application_shell/profiles/project_wizard.h"
#include "umicom/application_shell/profiles/layout_manager.h"
#include "umicom/application_shell/profiles/application_launcher.h"

typedef const UmiApplicationShellProfileDefinition *
    (*BuiltinProfileFactory)(void);

static const BuiltinProfileFactory FACTORIES[] = {
    umi_application_shell_profile_file_menu,
    umi_application_shell_profile_edit_menu,
    umi_application_shell_profile_view_menu,
    umi_application_shell_profile_navigate_menu,
    umi_application_shell_profile_build_menu,
    umi_application_shell_profile_run_menu,
    umi_application_shell_profile_test_menu,
    umi_application_shell_profile_source_control_menu,
    umi_application_shell_profile_tools_menu,
    umi_application_shell_profile_window_menu,
    umi_application_shell_profile_help_menu,
    umi_application_shell_profile_main_toolbar,
    umi_application_shell_profile_activity_bar,
    umi_application_shell_profile_status_bar,
    umi_application_shell_profile_start_center,
    umi_application_shell_profile_project_explorer,
    umi_application_shell_profile_editor,
    umi_application_shell_profile_search,
    umi_application_shell_profile_problems,
    umi_application_shell_profile_output,
    umi_application_shell_profile_terminal,
    umi_application_shell_profile_source_control,
    umi_application_shell_profile_test_explorer,
    umi_application_shell_profile_debug,
    umi_application_shell_profile_build_dashboard,
    umi_application_shell_profile_deploy_dashboard,
    umi_application_shell_profile_extensions,
    umi_application_shell_profile_settings,
    umi_application_shell_profile_ai_assistant,
    umi_application_shell_profile_project_wizard,
    umi_application_shell_profile_layout_manager,
    umi_application_shell_profile_application_launcher
};

size_t umi_application_shell_builtin_profile_count(void)
{
    return sizeof(FACTORIES) / sizeof(FACTORIES[0]);
}

const UmiApplicationShellProfileDefinition *
umi_application_shell_builtin_profile_at(size_t index)
{
    return index < umi_application_shell_builtin_profile_count()
        ? FACTORIES[index]()
        : NULL;
}

const UmiApplicationShellProfileDefinition *
umi_application_shell_builtin_profile_find(const char *profile_id)
{
    size_t index;

    if (profile_id == NULL) return NULL;

    for (index = 0U;
         index < umi_application_shell_builtin_profile_count();
         ++index) {
        const UmiApplicationShellProfileDefinition *profile =
            FACTORIES[index]();

        if (profile != NULL &&
            strcmp(profile->profile_id, profile_id) == 0) {
            return profile;
        }
    }

    return NULL;
}

UmiStatus umi_application_shell_builtin_profiles_install(
    UmiApplicationShellRegistry *registry)
{
    size_t index;

    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    for (index = 0U;
         index < umi_application_shell_builtin_profile_count();
         ++index) {
        const UmiStatus status =
            umi_application_shell_profile_install(
                registry,
                FACTORIES[index]());

        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
