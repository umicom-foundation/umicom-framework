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
#include "umicom/base/value_archive.h"
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

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_strategy_project_config_archive_encode(const UmiStrategyProjectConfig *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_strategy_project_config_archive_decode(const void *bytes, size_t byte_count,
    UmiStrategyProjectConfig *value);

#ifdef __cplusplus
}
#endif
#endif
