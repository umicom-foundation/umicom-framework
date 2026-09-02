/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/quick_open.h
 *
 * PURPOSE:
 *   Provide a file-only query model for familiar IDE Quick Open navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_QUICK_OPEN_H
#define UMICOM_DEVELOPER_WORKBENCH_QUICK_OPEN_H

#include "umicom/developer_workbench/file_search_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer workbench quick open query operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_workbench_quick_open_query(
    UmiFileIndex *file_index,
    const char *query,
    UmiDeveloperWorkbenchSearchResult *out_results,
    size_t capacity,
    size_t *out_count);

#ifdef __cplusplus
}
#endif

#endif
