/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/manager.c
 *
 * PURPOSE:
 *   Implement thread-safe terminal session ownership and lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

UmiStatus umi_terminal_manager_create(UmiClock *clock,
                                      UmiTerminalManager **out_manager)
{
    UmiTerminalManager *manager;
    if (out_manager == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_manager = NULL;
    manager = (UmiTerminalManager *)calloc(1U, sizeof(*manager));
    if (manager == NULL ||
        umi_mutex_create(&manager->mutex) != UMI_STATUS_OK) {
        free(manager);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    manager->clock = clock;
    *out_manager = manager;
    return UMI_STATUS_OK;
}

void umi_terminal_manager_destroy(UmiTerminalManager *manager)
{
    size_t index;
    if (manager == NULL) {
        return;
    }
    for (index = 0U; index < manager->count; ++index) {
        umi_terminal_session_destroy(manager->sessions[index]);
    }
    umi_mutex_destroy(manager->mutex);
    free(manager);
}

UmiStatus umi_terminal_manager_open(UmiTerminalManager *manager,
                                    const char *session_id,
                                    const char *title,
                                    const char *working_directory,
                                    UmiTerminalSession **out_session)
{
    UmiTerminalSessionConfig config;
    UmiTerminalSession *session;
    UmiStatus status;
    if (manager == NULL || session_id == NULL || out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_terminal_manager_find(manager, session_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    (void)umi_mutex_lock(manager->mutex);
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
    if (status != UMI_STATUS_OK) {
        return status;
    }
    (void)umi_mutex_lock(manager->mutex);
    manager->sessions[manager->count++] = session;
    (void)umi_mutex_unlock(manager->mutex);
    *out_session = session;
    return UMI_STATUS_OK;
}

UmiStatus umi_terminal_manager_close(UmiTerminalManager *manager,
                                     const char *session_id)
{
    size_t index;
    if (manager == NULL || session_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(manager->mutex);
    for (index = 0U; index < manager->count; ++index) {
        UmiTerminalSessionSnapshot snapshot;
        (void)umi_terminal_session_snapshot(manager->sessions[index],
                                             &snapshot);
        if (strcmp(snapshot.session_id, session_id) == 0) {
            UmiTerminalSession *session = manager->sessions[index];
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

UmiTerminalSession *umi_terminal_manager_find(UmiTerminalManager *manager,
                                              const char *session_id)
{
    size_t index;
    if (manager == NULL || session_id == NULL) {
        return NULL;
    }
    for (index = 0U; index < manager->count; ++index) {
        UmiTerminalSessionSnapshot snapshot;
        (void)umi_terminal_session_snapshot(manager->sessions[index],
                                             &snapshot);
        if (strcmp(snapshot.session_id, session_id) == 0) {
            return manager->sessions[index];
        }
    }
    return NULL;
}

UmiTerminalSession *umi_terminal_manager_at(UmiTerminalManager *manager,
                                            size_t index)
{
    return manager != NULL && index < manager->count
        ? manager->sessions[index]
        : NULL;
}

size_t umi_terminal_manager_count(const UmiTerminalManager *manager)
{
    return manager != NULL ? manager->count : 0U;
}
