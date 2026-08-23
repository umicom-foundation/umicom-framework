/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_language_health.c
 *
 * PURPOSE:
 *   Verify tool-free language packs are immediately healthy without depending
 *   on the machine's installed developer tools.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/language_health.h"
#include "umicom/developer_project/languages/markdown.h"

int main(void)
{
    UmiDeveloperProjectLanguageHealth health;

    assert(umi_developer_project_language_health_probe(
        umi_developer_project_language_markdown(),
        &health) == UMI_STATUS_OK);
    assert(health.tool_count == 0U);
    assert(health.missing_count == 0U);
    assert(health.ready == 1);
    return 0;
}
