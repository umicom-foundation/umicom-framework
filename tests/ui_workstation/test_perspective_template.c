/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_perspective_template.c
 *
 * PURPOSE:
 *   Implement the test perspective template behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../value_archive/construction_cases.h"
#include <stdio.h>
#include "umicom/ui/workstation/perspective_template.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* Exercise rejected edits and borrowed text through the public setter. */
static int UmiWsPerspectiveTemplateTextEditCases(void)
{
    UmiWsPerspectiveTemplate *value = calloc(1U, sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ws_perspective_template_set_default_layout(value, "prefix-value") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    char oversized[sizeof(value->default_layout_id) + 1U];
    memset(oversized, 'x', sizeof(oversized) - 1U);
    oversized[sizeof(oversized) - 1U] = '\0';
    UMI_CONSTRUCTION_REQUIRE(umi_ws_perspective_template_set_default_layout(value, oversized) != UMI_STATUS_OK);
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
    UMI_CONSTRUCTION_REQUIRE(umi_ws_perspective_template_set_default_layout(value, value->default_layout_id + 7U) == UMI_STATUS_OK);
    UMI_CONSTRUCTION_REQUIRE(strcmp(value->default_layout_id, "value") == 0);
    UMI_CONSTRUCTION_REQUIRE(umi_ws_perspective_template_set_default_layout(NULL, "value") == UMI_STATUS_INVALID_ARGUMENT);
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiWsPerspectiveTemplateTextEditCases() != 0) return 1;

    UmiWsPerspectiveTemplate p;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_perspective_template_init(&p, "studio.debug", "Debug", UMI_WS_DOMAIN_STUDIO) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_perspective_template_set_default_layout(&p, "studio.debug.desktop") != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_perspective_template_add_surface(&p, "debug.variables") != UMI_STATUS_OK) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (p.preferred_surface_count != 1U) return 4;
    puts("perspective template: ok");
    return 0;
}
