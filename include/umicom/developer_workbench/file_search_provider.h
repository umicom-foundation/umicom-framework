/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/file_search_provider.h
 *
 * PURPOSE:
 *   Adapt the Framework file index into Search Everywhere and Quick Open.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_FILE_SEARCH_PROVIDER_H
#define UMICOM_DEVELOPER_WORKBENCH_FILE_SEARCH_PROVIDER_H

#include "umicom/developer_workbench/search_provider.h"
#include "umicom/platform/file_index.h"

#ifdef __cplusplus
extern "C" {
#endif

void umi_developer_workbench_file_search_provider_init(
    UmiDeveloperWorkbenchSearchProvider *provider,
    UmiFileIndex *file_index);

#ifdef __cplusplus
}
#endif

#endif
