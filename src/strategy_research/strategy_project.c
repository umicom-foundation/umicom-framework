/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/strategy_project.c
 *
 * PURPOSE:
 *   Render a C23 research strategy starting point without creating files,
 *   placing orders or granting broker authority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/strategy_project.h"

#include <stdio.h>
#include <string.h>

void umi_strategy_project_config_init(
    UmiStrategyProjectConfig *config)
{
    if (config == NULL) return;
    (void)memset(config, 0, sizeof(*config));
    (void)snprintf(config->projectName, sizeof(config->projectName),
                   "%s", "UmicomStrategy");
    (void)snprintf(config->strategyName, sizeof(config->strategyName),
                   "%s", "ResearchStrategy");
    (void)snprintf(config->instrument, sizeof(config->instrument),
                   "%s", "SIMULATION");
    (void)snprintf(config->timeframe, sizeof(config->timeframe),
                   "%s", "5m");
    config->warmupBars = 100U;
    config->simulationOnly = 1;
}

UmiStatus umi_strategy_project_config_validate(
    const UmiStrategyProjectConfig *config)
{
    if (config == NULL ||
        config->projectName[0] == '\0' ||
        config->strategyName[0] == '\0' ||
        config->instrument[0] == '\0' ||
        config->timeframe[0] == '\0' ||
        config->warmupBars == 0U ||
        !config->simulationOnly) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_strategy_project_render_c23(
    const UmiStrategyProjectConfig *config,
    char *outSource,
    size_t capacity,
    size_t *outRequired)
{
    int required;

    if (umi_strategy_project_config_validate(config) != UMI_STATUS_OK ||
        outRequired == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    required = snprintf(
        NULL, 0,
        "/* Generated Umicom research strategy: %s */\n"
        "#include <stddef.h>\n"
        "#include \"umicom/strategy_research/strategy_research.h\"\n\n"
        "/* Instrument: %s | Timeframe: %s | Warm-up bars: %zu */\n"
        "UmiStatus %sEvaluate(\n"
        "    const UmiStrategyResearchInput *input,\n"
        "    UmiStrategyResearchSnapshot *outSnapshot)\n"
        "{\n"
        "    /* Research-only: this function returns evidence, not an order. */\n"
        "    return umi_strategy_research_service_evaluate(\n"
        "        \"strategy.signal-score\", input, outSnapshot);\n"
        "}\n",
        config->projectName,
        config->instrument,
        config->timeframe,
        config->warmupBars,
        config->strategyName);

    if (required < 0) return UMI_STATUS_INTERNAL_ERROR;
    *outRequired = (size_t)required + 1U;

    if (outSource == NULL || capacity < *outRequired) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)snprintf(
        outSource, capacity,
        "/* Generated Umicom research strategy: %s */\n"
        "#include <stddef.h>\n"
        "#include \"umicom/strategy_research/strategy_research.h\"\n\n"
        "/* Instrument: %s | Timeframe: %s | Warm-up bars: %zu */\n"
        "UmiStatus %sEvaluate(\n"
        "    const UmiStrategyResearchInput *input,\n"
        "    UmiStrategyResearchSnapshot *outSnapshot)\n"
        "{\n"
        "    /* Research-only: this function returns evidence, not an order. */\n"
        "    return umi_strategy_research_service_evaluate(\n"
        "        \"strategy.signal-score\", input, outSnapshot);\n"
        "}\n",
        config->projectName,
        config->instrument,
        config->timeframe,
        config->warmupBars,
        config->strategyName);
    return UMI_STATUS_OK;
}
