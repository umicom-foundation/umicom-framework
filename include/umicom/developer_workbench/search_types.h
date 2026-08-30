/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/search_types.h
 *
 * PURPOSE:
 *   Define cross-provider Search Everywhere results for commands, files, recent
 *   projects and future symbols/settings/source-control providers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_SEARCH_TYPES_H
#define UMICOM_DEVELOPER_WORKBENCH_SEARCH_TYPES_H

#include "umicom/developer_workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiDeveloperWorkbenchSearchKind {
    UMI_DEVELOPER_WORKBENCH_SEARCH_COMMAND = 1,
    UMI_DEVELOPER_WORKBENCH_SEARCH_FILE = 2,
    UMI_DEVELOPER_WORKBENCH_SEARCH_RECENT_PROJECT = 3,
    UMI_DEVELOPER_WORKBENCH_SEARCH_SYMBOL = 4,
    UMI_DEVELOPER_WORKBENCH_SEARCH_SETTING = 5,
    UMI_DEVELOPER_WORKBENCH_SEARCH_SOURCE_CONTROL = 6
} UmiDeveloperWorkbenchSearchKind;

typedef struct UmiDeveloperWorkbenchSearchResult {
    UmiDeveloperWorkbenchSearchKind kind;
    char provider_id[UMI_DEVELOPER_WORKBENCH_ID_CAPACITY];
    char result_id[UMI_DEVELOPER_WORKBENCH_ID_CAPACITY];
    char title[UMI_DEVELOPER_WORKBENCH_TITLE_CAPACITY];
    char detail[UMI_DEVELOPER_WORKBENCH_TEXT_CAPACITY];
    char target[UMI_DEVELOPER_WORKBENCH_PATH_CAPACITY];
    char command_id[UMI_DEVELOPER_WORKBENCH_COMMAND_CAPACITY];
    int enabled;
    int32_t score;
    uint64_t revision;
} UmiDeveloperWorkbenchSearchResult;

const char *umi_developer_workbench_search_kind_text(
    UmiDeveloperWorkbenchSearchKind kind);

int32_t umi_developer_workbench_search_score(
    const char *query,
    const char *primary,
    const char *secondary);

#ifdef __cplusplus
}
#endif

#endif
