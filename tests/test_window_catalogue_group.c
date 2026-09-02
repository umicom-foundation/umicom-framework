/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_window_catalogue_group.c
 *
 * PURPOSE:
 *   Implement the test window catalogue group behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/ui/window_catalogue.h"
#include "umicom/ui/window_group.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiUiWindowCatalogue catalogue = {0};
    UmiUiWindowGroupStore groups = {0};
    UmiUiWindowDescriptor editor = {"editor","Editor","Edit files","text-editor",UMI_UI_WINDOW_CATEGORY_DEVELOPMENT,true,0.6,0.8};
    UmiUiWindowDescriptor profiler = {"profiler","Profiler","CPU and memory profiles","speedometer",UMI_UI_WINDOW_CATEGORY_OPERATIONS,false,0.5,0.5};
    const UmiUiWindowDescriptor *matches[4U];
    const char *routes[4U];
    UmiUiWindowGroupRole role = UMI_UI_WINDOW_GROUP_SOURCE;
    const UmiUiWindowGroup *assigned;
    assert(umi_ui_window_catalogue_register(&catalogue,&editor) == UMI_STATUS_OK);
    assert(umi_ui_window_catalogue_register(&catalogue,&profiler) == UMI_STATUS_OK);
    assert(umi_ui_window_catalogue_search(&catalogue,"memory",0,matches,4U) == 1U);
    assert(strcmp(matches[0]->tool_id,"profiler") == 0);
    assert(umi_ui_window_group_define(&groups,"project-blue","accent-blue",UMI_UI_WINDOW_CONTEXT_PROJECT) == UMI_STATUS_OK);
    assert(umi_ui_window_group_join(&groups,"project-blue","explorer",UMI_UI_WINDOW_GROUP_SOURCE) == UMI_STATUS_OK);
    assert(umi_ui_window_group_join(&groups,"project-blue","editor",UMI_UI_WINDOW_GROUP_DESTINATION) == UMI_STATUS_OK);
    assert(umi_ui_window_group_route(&groups,"project-blue","explorer",routes,4U) == 1U);
    assert(strcmp(routes[0],"editor") == 0);
    assert(umi_ui_window_group_define(
               &groups,
               "project-green",
               "accent-green",
               UMI_UI_WINDOW_CONTEXT_PROJECT) == UMI_STATUS_OK);
    assert(umi_ui_window_group_assign(
               &groups,
               "project-green",
               "editor",
               UMI_UI_WINDOW_GROUP_BIDIRECTIONAL) == UMI_STATUS_OK);
    assigned = umi_ui_window_group_for_window(&groups, "editor", &role);
    assert(assigned != NULL &&
           strcmp(assigned->group_id, "project-green") == 0 &&
           role == UMI_UI_WINDOW_GROUP_BIDIRECTIONAL);
    assert(umi_ui_window_group_route(
               &groups, "project-blue", "explorer", routes, 4U) == 0U);
    assert(umi_ui_window_group_unassign(&groups, "editor") == UMI_STATUS_OK);
    assert(umi_ui_window_group_for_window(&groups, "editor", NULL) == NULL);
    return 0;
}
