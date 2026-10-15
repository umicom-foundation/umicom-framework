/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_layout_catalogue.c
 *
 * PURPOSE:
 *   Implement the test layout catalogue behavior for
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
#include "umicom/ui/workstation/layout_catalogue.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted layout template value. */
static int UmiWsLayoutTemplateConstructionCases(void)
{
    UmiWsLayoutTemplate *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ws_layout_template_init(value, "trader.active", "Active Trading", UMI_WS_DOMAIN_TRADER) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->layout_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_layout_template_init(value, oversized, "Active Trading", UMI_WS_DOMAIN_TRADER) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_layout_template_init(value, value->layout_id, "Active Trading", UMI_WS_DOMAIN_TRADER) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->layout_id, "trader.active") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->label) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_layout_template_init(value, "trader.active", oversized, UMI_WS_DOMAIN_TRADER) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_layout_template_init(value, "trader.active", value->label, UMI_WS_DOMAIN_TRADER) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->label, "Active Trading") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiWsLayoutTemplateConstructionCases() != 0) return 1;

    UmiWsLayoutCatalogue c;
    UmiWsLayoutTemplate l;
    umi_ws_layout_catalogue_init(&c);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_layout_template_init(&l, "trader.active", "Active Trading", UMI_WS_DOMAIN_TRADER) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_layout_catalogue_upsert(&c, &l) != UMI_STATUS_OK) return 2;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_ws_layout_catalogue_find(&c, "trader.active") == NULL) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_layout_catalogue_count_domain(&c, UMI_WS_DOMAIN_TRADER) != 1U) return 4;
    puts("layout catalogue: ok");
    return 0;
}
