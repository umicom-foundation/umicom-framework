/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/command_search_provider.h
 *
 * PURPOSE:
 *   Adapt the canonical Framework command registry into Search Everywhere.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_COMMAND_SEARCH_PROVIDER_H
#define UMICOM_DEVELOPER_WORKBENCH_COMMAND_SEARCH_PROVIDER_H

#include "umicom/developer_workbench/search_provider.h"
#include "umicom/runtime/command_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise developer workbench command search provider from caller-provided values so
 * later operations receive a known state.
 */
void umi_developer_workbench_command_search_provider_init(
    UmiDeveloperWorkbenchSearchProvider *provider,
    UmiCommandRegistry *commands);

#ifdef __cplusplus
}
#endif

#endif
