/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/strategy_project.h
 *
 * PURPOSE:
 *   Define a reusable C23 strategy-project descriptor and render a conservative
 *   research-only source template for Studio and other developer frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_STRATEGY_PROJECT_H
#define UMICOM_STRATEGY_RESEARCH_STRATEGY_PROJECT_H

#include <stddef.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STRATEGY_PROJECT_TEXT_CAPACITY 128U

typedef struct UmiStrategyProjectConfig {
    char projectName[UMI_STRATEGY_PROJECT_TEXT_CAPACITY];
    char strategyName[UMI_STRATEGY_PROJECT_TEXT_CAPACITY];
    char instrument[UMI_STRATEGY_PROJECT_TEXT_CAPACITY];
    char timeframe[32];
    size_t warmupBars;
    int simulationOnly;
} UmiStrategyProjectConfig;

void umi_strategy_project_config_init(
    UmiStrategyProjectConfig *config);

UmiStatus umi_strategy_project_config_validate(
    const UmiStrategyProjectConfig *config);

UmiStatus umi_strategy_project_render_c23(
    const UmiStrategyProjectConfig *config,
    char *outSource,
    size_t capacity,
    size_t *outRequired);

#ifdef __cplusplus
}
#endif
#endif
