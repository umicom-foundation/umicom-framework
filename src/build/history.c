/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/history.c
 *
 * PURPOSE:
 *   Implement bounded chronological build-history retention.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/history.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/platform/threading.h"

struct UmiBuildHistory {
    UmiBuildResult *items;
    size_t capacity;
    size_t count;
    size_t head;
    UmiMutex *mutex;
};

UmiStatus umi_build_history_create(size_t capacity,
                                   UmiBuildHistory **out_history)
{
    UmiBuildHistory *history;
    if (out_history == NULL || capacity == 0U ||
        capacity > UMI_BUILD_HISTORY_MAX) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_history = NULL;
    history = (UmiBuildHistory *)calloc(1U, sizeof(*history));
    if (history == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    history->items = (UmiBuildResult *)calloc(capacity,
                                              sizeof(*history->items));
    if (history->items == NULL ||
        umi_mutex_create(&history->mutex) != UMI_STATUS_OK) {
        umi_build_history_destroy(history);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    history->capacity = capacity;
    *out_history = history;
    return UMI_STATUS_OK;
}

void umi_build_history_destroy(UmiBuildHistory *history)
{
    if (history == NULL) {
        return;
    }
    umi_mutex_destroy(history->mutex);
    free(history->items);
    free(history);
}

UmiStatus umi_build_history_append(UmiBuildHistory *history,
                                   const UmiBuildResult *result)
{
    size_t index;
    if (history == NULL || result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(history->mutex);
    index = (history->head + history->count) % history->capacity;
    if (history->count == history->capacity) {
        index = history->head;
        history->head = (history->head + 1U) % history->capacity;
    } else {
        history->count += 1U;
    }
    history->items[index] = *result;
    (void)umi_mutex_unlock(history->mutex);
    return UMI_STATUS_OK;
}

size_t umi_build_history_count(const UmiBuildHistory *history)
{
    size_t count;
    if (history == NULL) {
        return 0U;
    }
    (void)umi_mutex_lock(history->mutex);
    count = history->count;
    (void)umi_mutex_unlock(history->mutex);
    return count;
}

UmiStatus umi_build_history_at(const UmiBuildHistory *history,
                               size_t index,
                               UmiBuildResult *out_result)
{
    size_t position;
    if (history == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(history->mutex);
    if (index >= history->count) {
        (void)umi_mutex_unlock(history->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    position = (history->head + index) % history->capacity;
    *out_result = history->items[position];
    (void)umi_mutex_unlock(history->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_build_history_latest(const UmiBuildHistory *history,
                                   UmiBuildResult *out_result)
{
    size_t position;
    if (history == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(history->mutex);
    if (history->count == 0U) {
        (void)umi_mutex_unlock(history->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    position = (history->head + history->count - 1U) % history->capacity;
    *out_result = history->items[position];
    (void)umi_mutex_unlock(history->mutex);
    return UMI_STATUS_OK;
}

void umi_build_history_clear(UmiBuildHistory *history)
{
    if (history == NULL) {
        return;
    }
    (void)umi_mutex_lock(history->mutex);
    history->count = 0U;
    history->head = 0U;
    (void)memset(history->items,
                 0,
                 history->capacity * sizeof(history->items[0]));
    (void)umi_mutex_unlock(history->mutex);
}
