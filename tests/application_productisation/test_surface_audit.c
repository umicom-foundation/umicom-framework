/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation/test_surface_audit.c
 *
 * PURPOSE:
 *   Verify that every portfolio application has a layout, a user-interface
 *   experience, a presentation record and a declared start entry point.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>

#include "umicom/application/portfolio.h"
#include "umicom/application/productisation/surface_audit.h"
#include "umicom/test_runtime/check.h"

/* The test exercises the same report that Desk and installers will consume. */
int main(void)
{
    UmiProductPortfolioSurfaceAudit *audit =
        (UmiProductPortfolioSurfaceAudit *)calloc(1U, sizeof(*audit));
    const UmiProductApplicationSurfaceAuditEntry *studio;
    const UmiProductApplicationSurfaceAuditEntry *trader;
    size_t index;

    UMI_TEST_REQUIRE(audit != NULL);
    UMI_TEST_REQUIRE(umi_product_portfolio_surface_audit_build(audit) ==
                     UMI_STATUS_OK);
    UMI_TEST_REQUIRE(audit->application_count ==
                     umi_application_portfolio_count());
    UMI_TEST_REQUIRE(audit->issue_count == 0U);
    UMI_TEST_REQUIRE(audit->launch_surface_ready_count ==
                     audit->application_count);
    UMI_TEST_REQUIRE(audit->user_interface_ready_count ==
                     audit->application_count);
    UMI_TEST_REQUIRE(audit->start_entrypoint_ready_count ==
                     audit->application_count);
    for (index = 0U; index < audit->application_count; ++index) {
        /* Every row must provide enough evidence for a launcher to explain it. */
        UMI_TEST_REQUIRE(audit->applications[index].application_id[0] != '\0');
        UMI_TEST_REQUIRE(audit->applications[index].display_name[0] != '\0');
        UMI_TEST_REQUIRE(audit->applications[index].executable_name[0] != '\0');
        UMI_TEST_REQUIRE(audit->applications[index].status == UMI_STATUS_OK);
    }
    studio = umi_product_portfolio_surface_audit_find(audit,
                                                       "org.umicom.studio");
    trader = umi_product_portfolio_surface_audit_find(audit,
                                                       "org.umicom.trader");
    UMI_TEST_REQUIRE(studio != NULL && trader != NULL);
    UMI_TEST_REQUIRE(strcmp(studio->default_layout_id, "development") == 0);
    UMI_TEST_REQUIRE(strcmp(trader->default_layout_id, "trading") == 0);
    UMI_TEST_REQUIRE(studio->launch_surface_ready);
    UMI_TEST_REQUIRE(trader->launch_surface_ready);
    free(audit);
    return 0;
}
