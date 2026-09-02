/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_application_portfolio.c
 *
 * PURPOSE:
 *   Verify the canonical portfolio contains valid standalone and federated
 *   definitions for every strategic application family.
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

#include "umicom/application/application.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    size_t index;
    const UmiApplicationDefinition *trader;
    const UmiApplicationDefinition *os;
    const UmiApplicationDefinition *creator;

    assert(umi_application_portfolio_count() >= 22U);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_application_portfolio_count(); ++index) {
        const UmiApplicationDefinition *definition =
            umi_application_portfolio_at(index);
        assert(definition != NULL);
        assert(umi_application_definition_validate(definition) ==
               UMI_STATUS_OK);
        assert((definition->flags & UMI_APPLICATION_STANDALONE) != 0U);
        assert((definition->flags & UMI_APPLICATION_FEDERATED) != 0U);
    }

    trader = umi_application_portfolio_find("org.umicom.trader");
    os = umi_application_portfolio_find("org.umicom.os");
    creator = umi_application_portfolio_find("org.umicom.creator");
    assert(trader != NULL && trader->family == UMI_APPLICATION_FAMILY_FINANCE);
    assert(umi_application_definition_declares_capability(
        trader, "umicom.trading"));
    assert(os != NULL && strcmp(os->repository_slug, "umicom-os") == 0);
    assert(creator != NULL && umi_application_definition_uses_domain(
        creator, "creator"));
    assert(umi_application_portfolio_family_count(
        UMI_APPLICATION_FAMILY_AI) >= 3U);
    return 0;
}
