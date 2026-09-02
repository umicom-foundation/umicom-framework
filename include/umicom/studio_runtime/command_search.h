/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/command_search.h
 *
 * PURPOSE:
 *   Provide professional command and menu search across the authoritative
 *   runtime Command Registry plus Application Shell contributions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_COMMAND_SEARCH_H
#define UMICOM_STUDIO_RUNTIME_COMMAND_SEARCH_H

#include "umicom/studio_runtime/bindings.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_COMMAND_SEARCH_CAPACITY 64U

/**
 * Represent the studio runtime command search result data shared with callers of this
 * public contract.
 */
typedef struct UmiStudioRuntimeCommandSearchResult {
    char command_id[UMI_COMMAND_ID_CAPACITY];
    char title[UMI_COMMAND_TITLE_CAPACITY];
    char category[UMI_COMMAND_CATEGORY_CAPACITY];
    char contribution_id[UMI_APPLICATION_SHELL_ID_CAPACITY];
    uint32_t score;
    int enabled;
    int from_shell;
} UmiStudioRuntimeCommandSearchResult;

/**
 * Represent the studio runtime command search results data shared with callers of this
 * public contract.
 */
typedef struct UmiStudioRuntimeCommandSearchResults {
    UmiStudioRuntimeCommandSearchResult
        items[UMI_STUDIO_COMMAND_SEARCH_CAPACITY];
    size_t count;
    int truncated;
} UmiStudioRuntimeCommandSearchResults;

/**
 * Provide the studio command search operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_command_search(
    const UmiStudioRuntimeBindings *bindings,
    const char *query,
    UmiStudioRuntimeCommandSearchResults *out_results);

#ifdef __cplusplus
}
#endif
#endif
