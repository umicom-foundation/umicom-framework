/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/event.h
 *
 * PURPOSE:
 *   Retain bounded extension-management audit events for diagnostics,
 *   observability and operator-facing history views.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EVENT_H
#define UMICOM_PLUGIN_EVENT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/plugin/manifest.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PLUGIN_EVENT_MAX 1024U

typedef enum UmiPluginEventKind {
    UMI_PLUGIN_EVENT_DISCOVERED = 0,
    UMI_PLUGIN_EVENT_REGISTERED = 1,
    UMI_PLUGIN_EVENT_ENABLED = 2,
    UMI_PLUGIN_EVENT_DISABLED = 3,
    UMI_PLUGIN_EVENT_ACTIVATED = 4,
    UMI_PLUGIN_EVENT_DEACTIVATED = 5,
    UMI_PLUGIN_EVENT_INSTALLED = 6,
    UMI_PLUGIN_EVENT_UPDATED = 7,
    UMI_PLUGIN_EVENT_UNINSTALLED = 8,
    UMI_PLUGIN_EVENT_FAILED = 9
} UmiPluginEventKind;

typedef struct UmiPluginEvent {
    uint64_t sequence;
    uint64_t timestamp_ms;
    UmiPluginEventKind kind;
    UmiStatus status;
    char plugin_id[UMI_PLUGIN_ID_CAPACITY];
    char message[256];
} UmiPluginEvent;

typedef struct UmiPluginEventLog UmiPluginEventLog;

UmiStatus umi_plugin_event_log_create(size_t capacity,
                                      UmiPluginEventLog **out_log);
void umi_plugin_event_log_destroy(UmiPluginEventLog *log);
UmiStatus umi_plugin_event_log_append(UmiPluginEventLog *log,
                                      uint64_t timestamp_ms,
                                      UmiPluginEventKind kind,
                                      UmiStatus status,
                                      const char *plugin_id,
                                      const char *message);
size_t umi_plugin_event_log_count(const UmiPluginEventLog *log);
UmiStatus umi_plugin_event_log_at(const UmiPluginEventLog *log,
                                  size_t index,
                                  UmiPluginEvent *out_event);
const char *umi_plugin_event_kind_text(UmiPluginEventKind kind);

#ifdef __cplusplus
}
#endif

#endif
