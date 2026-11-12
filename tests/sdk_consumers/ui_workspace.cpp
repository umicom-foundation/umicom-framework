/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_consumers/ui_workspace.cpp
 *
 * PURPOSE:
 *   Exercise workspace lock, clone and brand-theme contracts from C++ without opening a window.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/



#include "umicom/ui/workspace_layout.h"
#include "umicom/ui/theme_profile.h"
#include "umicom/ui/brand_palette.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expression) do { if (!(expression)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #expression); return 1; \
} } while (0)

static int exercise(UmiUiWorkspaceLayout *layout, UmiUiWorkspaceLayout *copy)
{
    UmiUiWorkspaceWindow window = {};
    UmiUiThemeProfile theme = {};
    char reason[256] = {};
    CHECK(umi_ui_workspace_layout_init(layout, "research", "Research") == UMI_STATUS_OK);
    CHECK(layout->locked && layout->revision == 1U);
    (void)snprintf(window.window_id, sizeof(window.window_id), "%s", "prices");
    (void)snprintf(window.title, sizeof(window.title), "%s", "Prices");
    (void)snprintf(window.tool_id, sizeof(window.tool_id), "%s", "price-grid");
    window.width = 1.0;
    window.height = 1.0;
    window.visible = true;
    window.closable = true;
    window.resizable = true;
    CHECK(umi_ui_workspace_layout_add_window(layout, &window) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(layout->window_count == 0U);
    CHECK(umi_ui_workspace_layout_set_locked(layout, false) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_layout_add_window(layout, &window) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_layout_set_group(layout, "prices", "market") == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_layout_validate(layout, reason, sizeof(reason)) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_layout_clone(layout, "research-copy", "Research copy", copy) == UMI_STATUS_OK);
    CHECK(copy->revision == 1U && copy->window_count == 1U);
    CHECK(strcmp(copy->windows[0].group_id, "market") == 0);
    CHECK(umi_ui_workspace_layout_rename(copy, "Independent copy") == UMI_STATUS_OK);
    CHECK(strcmp(layout->name, "Research") == 0);
    CHECK(umi_ui_workspace_layout_count_tool(copy, "price-grid") == 1U);

    /* The client consumes the shared palette; it does not invent product colours. */
    CHECK(umi_ui_theme_profile_init(&theme, "umicom-dark", "Umicom Dark",
        UMI_UI_THEME_MODE_DARK, UMI_UI_DENSITY_COMFORTABLE) == UMI_STATUS_OK);
    CHECK(umi_ui_theme_profile_validate(&theme, reason, sizeof(reason)) == UMI_STATUS_OK);
    const UmiUiBrandPalette *palette = umi_ui_brand_palette_get(UMI_UI_BRAND_SURFACE_DARK);
    CHECK(palette != nullptr);
    CHECK(strcmp(theme.background, palette->background) == 0);
    CHECK(strcmp(theme.accent, palette->accent) == 0);
    return 0;
}

int main()
{
    UmiUiWorkspaceLayout *layout =
        static_cast<UmiUiWorkspaceLayout *>(malloc(sizeof(UmiUiWorkspaceLayout)));
    UmiUiWorkspaceLayout *copy =
        static_cast<UmiUiWorkspaceLayout *>(malloc(sizeof(UmiUiWorkspaceLayout)));
    if (layout == nullptr || copy == nullptr) {
        free(copy);
        free(layout);
        return 2;
    }
    const int result = exercise(layout, copy);
    free(copy);
    free(layout);
    if (result == 0) (void)puts("Umicom workspace and brand-theme values checked from C++.");
    return result;
}
