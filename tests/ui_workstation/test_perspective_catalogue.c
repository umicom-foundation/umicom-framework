/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_perspective_catalogue.c
 *
 * PURPOSE:
 *   Implement the test perspective catalogue behavior for
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
#include "umicom/ui/workstation/perspective_catalogue.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted perspective template value. */
static int UmiWsPerspectiveTemplateConstructionCases(void)
{
    UmiWsPerspectiveTemplate *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ws_perspective_template_init(value, "trader.research", "Research", UMI_WS_DOMAIN_TRADER) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->perspective_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_perspective_template_init(value, oversized, "Research", UMI_WS_DOMAIN_TRADER) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_perspective_template_init(value, value->perspective_id, "Research", UMI_WS_DOMAIN_TRADER) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->perspective_id, "trader.research") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->label) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_perspective_template_init(value, "trader.research", oversized, UMI_WS_DOMAIN_TRADER) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_perspective_template_init(value, "trader.research", value->label, UMI_WS_DOMAIN_TRADER) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->label, "Research") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiWsPerspectiveTemplateConstructionCases() != 0) return 1;

    UmiWsPerspectiveCatalogue c;
    UmiWsPerspectiveTemplate p;
    umi_ws_perspective_catalogue_init(&c);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_perspective_template_init(&p, "trader.research", "Research", UMI_WS_DOMAIN_TRADER) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_perspective_catalogue_upsert(&c, &p) != UMI_STATUS_OK) return 2;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_ws_perspective_catalogue_find(&c, "trader.research") == NULL) return 3;
    puts("perspective catalogue: ok");
    return 0;
}
