/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/problem_store.c
 *
 * PURPOSE:
 *   Implement bounded normalized problem storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/*
 * Initialise developer problem store from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_developer_problem_store_create(
    UmiDeveloperProblemStore **out_store)
{
    UmiDeveloperProblemStore *store;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_store = NULL;

    store = (UmiDeveloperProblemStore *)calloc(1U, sizeof(*store));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    store->revision = 1U;
    *out_store = store;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by developer problem store so the same storage can be reused
 * safely.
 */
void umi_developer_problem_store_destroy(
    UmiDeveloperProblemStore *store)
{
    free(store);
}

/*
 * Add developer problem store only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_developer_problem_store_add(
    UmiDeveloperProblemStore *store,
    const UmiDeveloperProblem *problem)
{
    UmiDeveloperProblem item;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || problem == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_problem_validate(problem);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (store->count >= UMI_DEVELOPER_PROBLEM_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    item = *problem;
    item.problem_id = ++store->sequence;
    item.revision = ++store->revision;
    store->problems[store->count++] = item;
    return UMI_STATUS_OK;
}

/*
 * Find developer problem store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_developer_problem_store_at(
    const UmiDeveloperProblemStore *store,
    size_t index,
    UmiDeveloperProblem *out_problem)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || out_problem == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= store->count) return UMI_STATUS_NOT_FOUND;

    *out_problem = store->problems[index];
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by developer problem store so the same storage can be reused
 * safely.
 */
void umi_developer_problem_store_clear(
    UmiDeveloperProblemStore *store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) return;

    (void)memset(store->problems, 0, sizeof(store->problems));
    store->count = 0U;
    store->revision += 1U;
}

/*
 * Provide the developer problem store clear source operation used by this module and its
 * client applications.
 */
void umi_developer_problem_store_clear_source(
    UmiDeveloperProblemStore *store,
    const char *source)
{
    size_t read_index;
    size_t write_index = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || source == NULL) return;

    /* Visit each bounded item once so every record receives the same rule. */
    for (read_index = 0U; read_index < store->count; ++read_index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(store->problems[read_index].source, source) != 0) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (write_index != read_index) {
                store->problems[write_index] =
                    store->problems[read_index];
            }
            write_index += 1U;
        }
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (write_index != store->count) {
        (void)memset(
            &store->problems[write_index],
            0,
            (store->count - write_index) * sizeof(store->problems[0]));
        store->count = write_index;
        store->revision += 1U;
    }
}

/*
 * Return the number of records represented by developer problem store without changing
 * their state.
 */
size_t umi_developer_problem_store_count(
    const UmiDeveloperProblemStore *store)
{
    return store != NULL ? store->count : 0U;
}

/*
 * Provide the developer problem store snapshot operation used by this module and its
 * client applications.
 */
UmiDeveloperProblemStoreSnapshot umi_developer_problem_store_snapshot(
    const UmiDeveloperProblemStore *store)
{
    UmiDeveloperProblemStoreSnapshot snapshot;
    size_t index;

    (void)memset(&snapshot, 0, sizeof(snapshot));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) return snapshot;

    snapshot.count = store->count;
    snapshot.revision = store->revision;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < store->count; ++index) {
        /* Select the behaviour associated with the requested command or state value. */
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
