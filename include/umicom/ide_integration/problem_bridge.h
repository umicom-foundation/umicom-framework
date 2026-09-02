/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/problem_bridge.h
 *
 * PURPOSE:
 *   Convert normalized Problems records into cross-navigation and AI repair
 *   context without changing the authoritative Problem Store.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_PROBLEM_BRIDGE_H
#define UMICOM_IDE_INTEGRATION_PROBLEM_BRIDGE_H
#include "umicom/developer_productivity/problem_store.h"
#include "umicom/ide_integration/location.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ide problem target operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_problem_target(
    const UmiDeveloperProblemStore *problems,
    size_t index,
    UmiIdeNavigationTarget *out_target);

/**
 * Provide the ide problem ai summary operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_problem_ai_summary(
    const UmiDeveloperProblemStore *problems,
    size_t index,
    char *out_text,
    size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
