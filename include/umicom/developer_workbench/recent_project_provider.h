/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/recent_project_provider.h
 *
 * PURPOSE:
 *   Adapt Framework recent-item state into Search Everywhere recent projects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_RECENT_PROJECT_PROVIDER_H
#define UMICOM_DEVELOPER_WORKBENCH_RECENT_PROJECT_PROVIDER_H

#include "umicom/developer_workbench/search_provider.h"
#include "umicom/platform/recent_items.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise developer workbench recent project provider from caller-provided values so
 * later operations receive a known state.
 */
void umi_developer_workbench_recent_project_provider_init(
    UmiDeveloperWorkbenchSearchProvider *provider,
    UmiRecentItemRegistry *recent_items);

#ifdef __cplusplus
}
#endif

#endif
