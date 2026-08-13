/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/event.c
 *
 * PURPOSE:
 *   Implement a bounded chronological plug-in management audit log.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include "umicom/plugin/event.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiPluginEventLog {
    UmiPluginEvent *items;
    size_t capacity;
    size_t count;
    size_t first;
    uint64_t next_sequence;
};

UmiStatus umi_plugin_event_log_create(size_t capacity,
                                      UmiPluginEventLog **out_log)
{
    UmiPluginEventLog *log;
    if (capacity == 0U || capacity > UMI_PLUGIN_EVENT_MAX || out_log == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_log = NULL;
    log = (UmiPluginEventLog *)calloc(1U, sizeof(*log));
    if (log == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    log->items = (UmiPluginEvent *)calloc(capacity, sizeof(*log->items));
    if (log->items == NULL) {
        free(log);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    log->capacity = capacity;
    log->next_sequence = 1U;
    *out_log = log;
    return UMI_STATUS_OK;
}

void umi_plugin_event_log_destroy(UmiPluginEventLog *log)
{
    if (log == NULL) return;
    free(log->items);
    free(log);
}

UmiStatus umi_plugin_event_log_append(UmiPluginEventLog *log,
                                      uint64_t timestamp_ms,
                                      UmiPluginEventKind kind,
                                      UmiStatus status,
                                      const char *plugin_id,
                                      const char *message)
{
    size_t slot;
    UmiPluginEvent *event;
    if (log == NULL || plugin_id == NULL || plugin_id[0] == '\0' ||
        message == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (log->count < log->capacity) {
        slot = (log->first + log->count) % log->capacity;
        ++log->count;
    } else {
        slot = log->first;
        log->first = (log->first + 1U) % log->capacity;
    }
    event = &log->items[slot];
    (void)memset(event, 0, sizeof(*event));
    event->sequence = log->next_sequence++;
    event->timestamp_ms = timestamp_ms;
    event->kind = kind;
    event->status = status;
    (void)snprintf(event->plugin_id, sizeof(event->plugin_id), "%s", plugin_id);
    (void)snprintf(event->message, sizeof(event->message), "%s", message);
    return UMI_STATUS_OK;
}

size_t umi_plugin_event_log_count(const UmiPluginEventLog *log)
{
    return log != NULL ? log->count : 0U;
}

UmiStatus umi_plugin_event_log_at(const UmiPluginEventLog *log,
                                  size_t index,
                                  UmiPluginEvent *out_event)
{
    size_t slot;
    if (log == NULL || out_event == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= log->count) return UMI_STATUS_NOT_FOUND;
    slot = (log->first + index) % log->capacity;
    *out_event = log->items[slot];
    return UMI_STATUS_OK;
}

const char *umi_plugin_event_kind_text(UmiPluginEventKind kind)
{
    switch (kind) {
        case UMI_PLUGIN_EVENT_DISCOVERED: return "discovered";
        case UMI_PLUGIN_EVENT_REGISTERED: return "registered";
        case UMI_PLUGIN_EVENT_ENABLED: return "enabled";
        case UMI_PLUGIN_EVENT_DISABLED: return "disabled";
        case UMI_PLUGIN_EVENT_ACTIVATED: return "activated";
        case UMI_PLUGIN_EVENT_DEACTIVATED: return "deactivated";
        case UMI_PLUGIN_EVENT_INSTALLED: return "installed";
        case UMI_PLUGIN_EVENT_UPDATED: return "updated";
        case UMI_PLUGIN_EVENT_UNINSTALLED: return "uninstalled";
        case UMI_PLUGIN_EVENT_FAILED: return "failed";
        default: return "unknown";
    }
}
