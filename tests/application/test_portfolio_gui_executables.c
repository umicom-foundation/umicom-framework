/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_portfolio_gui_executables.c
 *
 * PURPOSE:
 *   Keep the application's native GUI lookup aligned with the 24 supported
 *   desktop products without accidentally offering console-only entry points.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/portfolio.h"

#include <stdio.h>
#include <string.h>

/* Fail with the exact mapping condition so a changed build name is easy to find. */
#define CHECK(condition) do { \
    if (!(condition)) { \
        (void)fprintf(stderr, "[FAIL] %s:%d: %s\n", __FILE__, __LINE__, #condition); \
        return 1; \
    } \
} while (0)

/* Native basenames intentionally differ from historic portfolio command names
 * for Studio, Creator and OS. Platform executable suffixes belong to the host. */
int main(void)
{
    static const struct {
        const char *id;
        const char *executable;
    } expected[] = {
        {"org.umicom.desktop", "umicom-desk"},
        {"org.umicom.studio", "umicom-studio-ide"},
        {"org.umicom.trader", "umicom-trader"},
        {"org.umicom.bank", "umicom-bank"},
        {"org.umicom.tms", "umicom-tms"},
        {"org.umicom.music-studio", "umicom-music-studio"},
        {"org.umicom.accountant", "umicom-accountant"},
        {"org.umicom.cad", "umicom-cad"},
        {"org.umicom.creator", "umicom-ai-creator"},
        {"org.umicom.database-studio", "umicom-database-studio"},
        {"org.umicom.education", "umicom-education"},
        {"org.umicom.exchange", "umicom-exchange"},
        {"org.umicom.games", "umicom-games"},
        {"org.umicom.integration-studio", "umicom-integration-studio"},
        {"org.umicom.kitchen-designer", "umicom-kitchen-designer"},
        {"org.umicom.llm", "umicom-llm"},
        {"org.umicom.marketplace", "umicom-marketplace"},
        {"org.umicom.media-studio", "umicom-media-studio"},
        {"org.umicom.mobile-studio", "umicom-mobile-studio"},
        {"org.umicom.operations", "umicom-operations"},
        {"org.umicom.os", "umicom-os-control-centre-gtk"},
        {"org.umicom.rag", "umicom-rag"},
        {"org.umicom.security-centre", "umicom-security-centre"},
        {"org.umicom.web-studio", "umicom-web-studio"}
    };
    size_t index;
    size_t mapped = 0U;
    CHECK(sizeof(expected) / sizeof(expected[0]) == 24U);
    for (index = 0U; index < sizeof(expected) / sizeof(expected[0]); ++index) {
        const char *actual = umi_application_portfolio_gui_executable(expected[index].id);
        size_t previous;
        CHECK(umi_application_portfolio_find(expected[index].id) != NULL);
        CHECK(actual != NULL && strcmp(actual, expected[index].executable) == 0);
        CHECK(strstr(actual, "-console") == NULL);
        CHECK(strchr(actual, '/') == NULL && strchr(actual, '\\') == NULL);
        /* Each distinct product has a distinct executable, not an alias that
         * silently sends two picker entries to the same application. */
        for (previous = 0U; previous < index; ++previous) {
            CHECK(strcmp(actual, expected[previous].executable) != 0);
        }
    }
    /* Count through the authoritative portfolio, catching accidental mappings
     * for entries which do not yet have native product build targets. */
    for (index = 0U; index < umi_application_portfolio_count(); ++index) {
        const UmiApplicationDefinition *definition = umi_application_portfolio_at(index);
        CHECK(definition != NULL);
        if (umi_application_portfolio_gui_executable(definition->application_id) != NULL) {
            mapped += 1U;
        }
    }
    CHECK(mapped == 24U);
    CHECK(umi_application_portfolio_gui_executable(NULL) == NULL);
    CHECK(umi_application_portfolio_gui_executable("") == NULL);
    CHECK(umi_application_portfolio_gui_executable("org.umicom.missing") == NULL);
    CHECK(umi_application_portfolio_gui_executable("org.umicom.ide") == NULL);
    CHECK(umi_application_portfolio_gui_executable("org.umicom.author") == NULL);
    CHECK(umi_application_portfolio_gui_executable("../umicom-trader") == NULL);
    return 0;
}
