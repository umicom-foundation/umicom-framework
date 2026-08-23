/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/problem_store.c
 *
 * PURPOSE:
 *   Implement bounded normalized problem storage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/problem_store.h"

#include <stdlib.h>
#include <string.h>

struct UmiDeveloperProblemStore {
    UmiDeveloperProblem problems[UMI_DEVELOPER_PROBLEM_CAPACITY];
    size_t count;
    uint64_t sequence;
    uint64_t revision;
};

UmiStatus umi_developer_problem_store_create(
    UmiDeveloperProblemStore **out_store)
{
    UmiDeveloperProblemStore *store;

    if (out_store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_store = NULL;

    store = (UmiDeveloperProblemStore *)calloc(1U, sizeof(*store));
    if (store == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    store->revision = 1U;
    *out_store = store;
    return UMI_STATUS_OK;
}

void umi_developer_problem_store_destroy(
    UmiDeveloperProblemStore *store)
{
    free(store);
}

UmiStatus umi_developer_problem_store_add(
    UmiDeveloperProblemStore *store,
    const UmiDeveloperProblem *problem)
{
    UmiDeveloperProblem item;
    UmiStatus status;

    if (store == NULL || problem == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_problem_validate(problem);
    if (status != UMI_STATUS_OK) return status;

    if (store->count >= UMI_DEVELOPER_PROBLEM_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    item = *problem;
    item.problem_id = ++store->sequence;
    item.revision = ++store->revision;
    store->problems[store->count++] = item;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_problem_store_at(
    const UmiDeveloperProblemStore *store,
    size_t index,
    UmiDeveloperProblem *out_problem)
{
    if (store == NULL || out_problem == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= store->count) return UMI_STATUS_NOT_FOUND;

    *out_problem = store->problems[index];
    return UMI_STATUS_OK;
}

void umi_developer_problem_store_clear(
    UmiDeveloperProblemStore *store)
{
    if (store == NULL) return;

    (void)memset(store->problems, 0, sizeof(store->problems));
    store->count = 0U;
    store->revision += 1U;
}

void umi_developer_problem_store_clear_source(
    UmiDeveloperProblemStore *store,
    const char *source)
{
    size_t read_index;
    size_t write_index = 0U;

    if (store == NULL || source == NULL) return;

    for (read_index = 0U; read_index < store->count; ++read_index) {
        if (strcmp(store->problems[read_index].source, source) != 0) {
            if (write_index != read_index) {
                store->problems[write_index] =
                    store->problems[read_index];
            }
            write_index += 1U;
        }
    }

    if (write_index != store->count) {
        (void)memset(
            &store->problems[write_index],
            0,
            (store->count - write_index) * sizeof(store->problems[0]));
        store->count = write_index;
        store->revision += 1U;
    }
}

size_t umi_developer_problem_store_count(
    const UmiDeveloperProblemStore *store)
{
    return store != NULL ? store->count : 0U;
}

UmiDeveloperProblemStoreSnapshot umi_developer_problem_store_snapshot(
    const UmiDeveloperProblemStore *store)
{
    UmiDeveloperProblemStoreSnapshot snapshot;
    size_t index;

    (void)memset(&snapshot, 0, sizeof(snapshot));
    if (store == NULL) return snapshot;

    snapshot.count = store->count;
    snapshot.revision = store->revision;

    for (index = 0U; index < store->count; ++index) {
        switch (store->problems[index].severity) {
            case UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_HINT:
                snapshot.hints += 1U;
                break;
            case UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_INFO:
                snapshot.infos += 1U;
                break;
            case UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_WARNING:
                snapshot.warnings += 1U;
                break;
            case UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_ERROR:
                snapshot.errors += 1U;
                break;
            case UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_FATAL:
                snapshot.fatals += 1U;
                break;
            default:
                break;
        }
    }

    return snapshot;
}
