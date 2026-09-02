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

/*
 * Initialise plugin event log from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_plugin_event_log_create(size_t capacity,
                                      UmiPluginEventLog **out_log)
{
    UmiPluginEventLog *log;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (capacity == 0U || capacity > UMI_PLUGIN_EVENT_MAX || out_log == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_log = NULL;
    log = (UmiPluginEventLog *)calloc(1U, sizeof(*log));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    log->items = (UmiPluginEvent *)calloc(capacity, sizeof(*log->items));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log->items == NULL) {
        free(log);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    log->capacity = capacity;
    log->next_sequence = 1U;
    *out_log = log;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by plugin event log so the same storage can be reused
 * safely.
 */
void umi_plugin_event_log_destroy(UmiPluginEventLog *log)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log == NULL) return;
    free(log->items);
    free(log);
}

/* Add plugin event log only after its inputs and available capacity have been checked. */
UmiStatus umi_plugin_event_log_append(UmiPluginEventLog *log,
                                      uint64_t timestamp_ms,
                                      UmiPluginEventKind kind,
                                      UmiStatus status,
                                      const char *plugin_id,
                                      const char *message)
{
    size_t slot;
    UmiPluginEvent *event;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log == NULL || plugin_id == NULL || plugin_id[0] == '\0' ||
        message == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (log->count < log->capacity) {
        slot = (log->first + log->count) % log->capacity;
        ++log->count;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
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

/*
 * Return the number of records represented by plugin event log without changing their
 * state.
 */
size_t umi_plugin_event_log_count(const UmiPluginEventLog *log)
{
    return log != NULL ? log->count : 0U;
}

/*
 * Find plugin event log while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_plugin_event_log_at(const UmiPluginEventLog *log,
                                  size_t index,
                                  UmiPluginEvent *out_event)
{
    size_t slot;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log == NULL || out_event == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= log->count) return UMI_STATUS_NOT_FOUND;
    slot = (log->first + index) % log->capacity;
    *out_event = log->items[slot];
    return UMI_STATUS_OK;
}

/*
 * Provide the plugin event kind text operation used by this module and its client
 * applications.
 */
const char *umi_plugin_event_kind_text(UmiPluginEventKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
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
