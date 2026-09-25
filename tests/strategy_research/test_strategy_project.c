/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/strategy_research/test_strategy_project.c
 *
 * PURPOSE:
 *   Verify the C23 strategy project template remains simulation-only and
 *   renders a compilable research-service entry point without order authority.
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

#include "umicom/strategy_research/strategy_project.h"

int main(void)
{
    UmiStrategyProjectConfig config;
    char source[2048];
    size_t required = 0U;

    umi_strategy_project_config_init(&config);
    assert(umi_strategy_project_config_validate(&config) == UMI_STATUS_OK);
    assert(config.simulationOnly);
    assert(umi_strategy_project_render_c23(
               &config, source, sizeof(source), &required) == UMI_STATUS_OK);
    assert(required > 0U);
    assert(strstr(source, "strategy.signal-score") != NULL);
    assert(strstr(source, "ResearchStrategyEvaluate") != NULL);
    assert(strstr(source, "submit") == NULL);
    assert(strstr(source, "broker") == NULL);

    config.simulationOnly = 0;
    assert(umi_strategy_project_config_validate(&config) ==
           UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
