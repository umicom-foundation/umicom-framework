/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/file_search_provider.c
 *
 * PURPOSE:
 *   Search the active workspace file index for quick file navigation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/file_search_provider.h"

#include <stdlib.h>
#include <string.h>

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

static UmiStatus search_files(
    void *user_data,
    const char *query,
    UmiDeveloperWorkbenchSearchResult *out_results,
    size_t capacity,
    size_t *out_count)
{
    UmiFileIndex *file_index = (UmiFileIndex *)user_data;
    UmiFileIndexEntry *entries;
    size_t count = 0U;
    size_t index;
    UmiStatus status;

    if (file_index == NULL || query == NULL ||
        out_results == NULL || out_count == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    entries = (UmiFileIndexEntry *)calloc(capacity, sizeof(*entries));
    if (entries == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    status = umi_file_index_find(
        file_index,
        query,
        0,
        entries,
        capacity,
        &count);

    if (status == UMI_STATUS_OK) {
        for (index = 0U; index < count; ++index) {
            (void)memset(&out_results[index],
                         0,
                         sizeof(out_results[index]));
            out_results[index].kind =
                UMI_DEVELOPER_WORKBENCH_SEARCH_FILE;
            copy_text(out_results[index].provider_id,
                      sizeof(out_results[index].provider_id),
                      "developer.search.files");
            copy_text(out_results[index].result_id,
                      sizeof(out_results[index].result_id),
                      entries[index].relative_path);
            copy_text(out_results[index].title,
                      sizeof(out_results[index].title),
                      entries[index].name);
            copy_text(out_results[index].detail,
                      sizeof(out_results[index].detail),
                      entries[index].relative_path);
            copy_text(out_results[index].target,
                      sizeof(out_results[index].target),
                      entries[index].path);
            out_results[index].enabled = 1;
            out_results[index].score =
                umi_developer_workbench_search_score(
                    query,
                    entries[index].name,
                    entries[index].relative_path);
            out_results[index].revision = 1U;
        }
    }

    free(entries);
    *out_count = status == UMI_STATUS_OK ? count : 0U;
    return status;
}

void umi_developer_workbench_file_search_provider_init(
    UmiDeveloperWorkbenchSearchProvider *provider,
    UmiFileIndex *file_index)
{
    if (provider == NULL) return;

    (void)memset(provider, 0, sizeof(*provider));
    copy_text(provider->provider_id,
              sizeof(provider->provider_id),
              "developer.search.files");
    copy_text(provider->title,
              sizeof(provider->title),
              "Files");
    provider->kind = UMI_DEVELOPER_WORKBENCH_SEARCH_FILE;
    provider->priority = 90;
    provider->user_data = file_index;
    provider->search = search_files;
}
