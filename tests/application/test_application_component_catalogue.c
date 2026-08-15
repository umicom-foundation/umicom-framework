/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_application_component_catalogue.c
 *
 * PURPOSE:
 *   Verify all strategic product domains are represented by reusable Framework
 *   components rather than application-private surface definitions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application/application.h"

int main(void)
{
    const char *domains[] = {
        "shell", "development", "trading", "treasury", "media", "music",
        "creator", "ai", "rag", "llm", "games", "cad", "kitchen",
        "author", "web", "database", "integration", "operations", "os",
        "education"
    };
    size_t index;
    assert(umi_application_component_catalogue_count() >= 90U);
    for (index = 0U; index < sizeof(domains) / sizeof(domains[0]); ++index)
        assert(umi_application_component_domain_exists(domains[index]));
    assert(umi_application_component_catalogue_find(
        "umicom.shell.context-links") != NULL);
    assert(umi_application_component_catalogue_find(
        "umicom.trading.strategy") != NULL);
    assert(umi_application_component_catalogue_find(
        "umicom.treasury.settlement") != NULL);
    assert(umi_application_component_catalogue_find(
        "umicom.media.timeline") != NULL);
    assert(umi_application_component_catalogue_find(
        "umicom.cad.viewport") != NULL);
    return 0;
}
