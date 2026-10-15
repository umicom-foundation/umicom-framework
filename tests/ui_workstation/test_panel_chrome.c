/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_panel_chrome.c
 *
 * PURPOSE:
 *   Implement the test panel chrome behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include "../value_archive/construction_cases.h"
#include <string.h>
#include "umicom/ui/workstation/panel_chrome.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted panel chrome value. */
static int UmiWsPanelChromeConstructionCases(void)
{
    UmiWsPanelChrome *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ws_panel_chrome_init(value, "Project") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->title) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_panel_chrome_init(value, oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_panel_chrome_init(value, value->title) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->title, "Project") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

/* Exercise rejected edits and borrowed text through the public setter. */
static int UmiWsPanelChromeTextEditCases(void)
{
    UmiWsPanelChrome *value = calloc(1U, sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ws_panel_chrome_set_badge(value, "prefix-value") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    char oversized[sizeof(value->badge) + 1U];
    memset(oversized, 'x', sizeof(oversized) - 1U);
    oversized[sizeof(oversized) - 1U] = '\0';
    UMI_CONSTRUCTION_REQUIRE(umi_ws_panel_chrome_set_badge(value, oversized) != UMI_STATUS_OK);
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
    UMI_CONSTRUCTION_REQUIRE(umi_ws_panel_chrome_set_badge(value, value->badge + 7U) == UMI_STATUS_OK);
    UMI_CONSTRUCTION_REQUIRE(strcmp(value->badge, "value") == 0);
    UMI_CONSTRUCTION_REQUIRE(umi_ws_panel_chrome_set_badge(NULL, "value") == UMI_STATUS_INVALID_ARGUMENT);
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiWsPanelChromeTextEditCases() != 0) return 1;

    if (UmiWsPanelChromeConstructionCases() != 0) return 1;

    UmiWsPanelChrome c;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_panel_chrome_init(&c, "Project") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_panel_chrome_set_badge(&c, "3") != UMI_STATUS_OK) return 2;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (umi_ws_panel_chrome_set_identity(&c, "project", "left") !=
        UMI_STATUS_OK) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_panel_chrome_set_context(&c, "project-blue", "blue") !=
        UMI_STATUS_OK) return 4;
    umi_ws_panel_chrome_set_compact(&c, true);
    /* Apply this branch only when its contract condition is satisfied. */
    if (!c.compact || c.badge[0] != '3' || !c.show_menu ||
        !c.show_context || !c.show_move || !c.show_float ||
        !c.show_maximise || !c.show_settings) return 5;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(c.panel_id, "project") != 0 ||
        strcmp(c.context_group_id, "project-blue") != 0) return 6;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(umi_ws_panel_action_text(UMI_WS_PANEL_ACTION_CLOSE),
               "Close panel") != 0) return 7;
    /* A docking preference must not be routed as geometry protection merely
     * because both actions happen to use pin-shaped visual affordances. */
    if (UMI_WS_PANEL_ACTION_AUTO_HIDE_TOGGLE == UMI_WS_PANEL_ACTION_PIN_TOGGLE ||
        strcmp(umi_ws_panel_action_text(UMI_WS_PANEL_ACTION_AUTO_HIDE_TOGGLE),
               "Dock or auto-hide panel") != 0) return 8;
    puts("panel chrome: ok");
    return 0;
}
