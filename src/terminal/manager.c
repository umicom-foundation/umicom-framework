/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/manager.c
 *
 * PURPOSE:
 *   Implement thread-safe terminal session ownership and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/terminal/manager.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/platform/threading.h"

struct UmiTerminalManager {
    UmiTerminalSession *sessions[UMI_TERMINAL_MANAGER_MAX_SESSIONS];
    size_t count;
    UmiClock *clock;
    UmiMutex *mutex;
};

/*
 * Initialise terminal manager from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_terminal_manager_create(UmiClock *clock,
                                      UmiTerminalManager **out_manager)
{
    UmiTerminalManager *manager;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_manager == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_manager = NULL;
    manager = (UmiTerminalManager *)calloc(1U, sizeof(*manager));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL ||
        umi_mutex_create(&manager->mutex) != UMI_STATUS_OK) {
        free(manager);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    manager->clock = clock;
    *out_manager = manager;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by terminal manager so the same storage can be reused
 * safely.
 */
void umi_terminal_manager_destroy(UmiTerminalManager *manager)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL) {
        return;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < manager->count; ++index) {
        umi_terminal_session_destroy(manager->sessions[index]);
    }
    umi_mutex_destroy(manager->mutex);
    free(manager);
}

/*
 * Provide the terminal manager open operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_manager_open(UmiTerminalManager *manager,
                                    const char *session_id,
                                    const char *title,
                                    const char *working_directory,
                                    UmiTerminalSession **out_session)
{
    UmiTerminalSessionConfig config;
    UmiTerminalSession *session;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL || session_id == NULL || out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_terminal_manager_find(manager, session_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    (void)umi_mutex_lock(manager->mutex);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (manager->count >= UMI_TERMINAL_MANAGER_MAX_SESSIONS) {
        (void)umi_mutex_unlock(manager->mutex);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)umi_mutex_unlock(manager->mutex);
    config.session_id = session_id;
    config.title = title;
    config.working_directory = working_directory;
    config.transcript_capacity = 256U;
    config.clock = manager->clock;
    status = umi_terminal_session_create(&config, &session);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    (void)umi_mutex_lock(manager->mutex);
    manager->sessions[manager->count++] = session;
    (void)umi_mutex_unlock(manager->mutex);
    *out_session = session;
    return UMI_STATUS_OK;
}

/*
 * Provide the terminal manager close operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_manager_close(UmiTerminalManager *manager,
                                     const char *session_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL || session_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(manager->mutex);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < manager->count; ++index) {
        UmiTerminalSessionSnapshot snapshot;
        (void)umi_terminal_session_snapshot(manager->sessions[index],
                                             &snapshot);
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(snapshot.session_id, session_id) == 0) {
            UmiTerminalSession *session = manager->sessions[index];
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (index + 1U < manager->count) {
                (void)memmove(&manager->sessions[index],
                              &manager->sessions[index + 1U],
                              (manager->count - index - 1U) *
                                  sizeof(manager->sessions[0]));
            }
            manager->count -= 1U;
            (void)umi_mutex_unlock(manager->mutex);
            (void)umi_terminal_session_close(session);
            umi_terminal_session_destroy(session);
            return UMI_STATUS_OK;
        }
    }
    (void)umi_mutex_unlock(manager->mutex);
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Find terminal manager while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiTerminalSession *umi_terminal_manager_find(UmiTerminalManager *manager,
                                              const char *session_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL || session_id == NULL) {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < manager->count; ++index) {
        UmiTerminalSessionSnapshot snapshot;
        (void)umi_terminal_session_snapshot(manager->sessions[index],
                                             &snapshot);
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(snapshot.session_id, session_id) == 0) {
            return manager->sessions[index];
        }
    }
    return NULL;
}

/*
 * Find terminal manager while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiTerminalSession *umi_terminal_manager_at(UmiTerminalManager *manager,
                                            size_t index)
{
    return manager != NULL && index < manager->count
        ? manager->sessions[index]
        : NULL;
}

/*
 * Return the number of records represented by terminal manager without changing their
 * state.
 */
size_t umi_terminal_manager_count(const UmiTerminalManager *manager)
{
    return manager != NULL ? manager->count : 0U;
}
