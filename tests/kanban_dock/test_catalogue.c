/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_dock/test_catalogue.c
 *
 * PURPOSE:
 *   Verify additive surface identities, command activation and existing-layout opening.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/studio_runtime/activation_catalogue.h"
#include "umicom/studio_runtime/command_alias_catalogue.h"
#include "umicom/studio_runtime/contract.h"
#include "umicom/studio_runtime/workspace_canvas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                   \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
int main(int argc, char **argv) {
  CHECK(argc == 2);
  if (!strcmp(argv[1], "identity")) {
    CHECK(UMI_STUDIO_SURFACE_EDITOR == 2 && UMI_STUDIO_SURFACE_LIVE_PREVIEW == 27 &&
          UMI_STUDIO_SURFACE_KANBAN == 28);
    CHECK(!strcmp(umi_studio_runtime_surface_kind_text(UMI_STUDIO_SURFACE_KANBAN), "kanban"));
    const UmiStudioRuntimeSurfaceBinding *binding =
        umi_studio_surface_catalogue_for_kind(UMI_STUDIO_SURFACE_KANBAN);
    CHECK(binding != NULL && !strcmp(binding->ide_surface_id, "umicom.ide.kanban"));
    CHECK(binding->region == UMI_APPLICATION_SHELL_REGION_BOTTOM_PANEL &&
          !binding->default_visible);
  } else if (!strcmp(argv[1], "activation")) {
    UmiStudioRuntimeActivationRule rule;
    CHECK(umi_studio_activation_resolve("view.kanban", &rule) == UMI_STATUS_OK &&
          rule.surface == UMI_STUDIO_SURFACE_KANBAN);
    CHECK(umi_studio_activation_resolve("workbench.view.kanban", &rule) == UMI_STATUS_OK &&
          rule.focus);
    CHECK(umi_studio_activation_resolve("test.run", &rule) == UMI_STATUS_OK &&
          rule.surface == UMI_STUDIO_SURFACE_TEST_EXPLORER);
  } else if (!strcmp(argv[1], "alias")) {
    const UmiStudioRuntimeCommandAliasDefinition *alias =
        umi_studio_command_alias_find("view.kanban");
    CHECK(alias != NULL && alias->surface == UMI_STUDIO_SURFACE_KANBAN && alias->activate_surface);
    CHECK(umi_studio_command_alias_validate(alias) == UMI_STATUS_OK);
  } else if (!strcmp(argv[1], "contract")) {
    CHECK(umi_studio_runtime_contract_validate(umi_studio_runtime_contract()) == UMI_STATUS_OK);
  } else if (!strcmp(argv[1], "layout") || !strcmp(argv[1], "old-layout")) {
    UmiUiWorkspaceCustomisation *model = calloc(1, sizeof *model);
    CHECK(model != NULL);
    umi_ui_workspace_customisation_init(model);
    CHECK(umi_studio_runtime_workspace_seed(model) == UMI_STATUS_OK);
    UmiUiWorkspaceLayout *layout = umi_ui_workspace_customisation_active(model);
    CHECK(layout != NULL);
    if (!strcmp(argv[1], "old-layout")) {
      /* Model a previously saved arrangement that predates this panel.
       * Opening the new tool must not replace any existing placement. */
      size_t at = 0U;
      while (at < layout->window_count && strcmp(layout->windows[at].window_id, "kanban"))
        ++at;
      CHECK(at < layout->window_count);
      memmove(&layout->windows[at], &layout->windows[at + 1U],
              (layout->window_count - at - 1U) * sizeof layout->windows[0]);
      --layout->window_count;
    }
    size_t before = layout->window_count;
    CHECK(umi_studio_runtime_workspace_set_visible(model, UMI_STUDIO_SURFACE_KANBAN, true) ==
          UMI_STATUS_OK);
    layout = umi_ui_workspace_customisation_active(model);
    const UmiUiWorkspaceWindow *window = umi_ui_workspace_layout_find_window(layout, "kanban");
    CHECK(window != NULL && window->visible);
    CHECK(layout->window_count == before + (!strcmp(argv[1], "old-layout") ? 1U : 0U));
    free(model);
  } else
    return 2;
  return 0;
}
