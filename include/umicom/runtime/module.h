#ifndef UMICOM_RUNTIME_MODULE_H
#define UMICOM_RUNTIME_MODULE_H

#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/version.h"
#include "umicom/diagnostics/diagnostic.h"
#include "umicom/messaging/event_bus.h"
#include "umicom/messaging/command_bus.h"
#include "umicom/messaging/query_bus.h"
#include "umicom/data/data_server.h"
#include "umicom/platform/config.h"
#include "umicom/platform/clock.h"
#include "umicom/runtime/scheduler.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiModuleKind {
    UMI_MODULE_CORE = 1,
    UMI_MODULE_SERVICE = 2,
    UMI_MODULE_UI = 3,
    UMI_MODULE_ADAPTER = 4,
    UMI_MODULE_WORKER = 5,
    UMI_MODULE_AGENT = 6
} UmiModuleKind;

typedef enum UmiModuleState {
    UMI_MODULE_DISCOVERED = 0,
    UMI_MODULE_CONFIGURED = 1,
    UMI_MODULE_INITIALISED = 2,
    UMI_MODULE_STARTED = 3,
    UMI_MODULE_QUIESCED = 4,
    UMI_MODULE_STOPPED = 5,
    UMI_MODULE_DESTROYED = 6,
    UMI_MODULE_FAILED = 7
} UmiModuleState;

typedef struct UmiModuleContext {
    void *module_state;
    UmiEventBus *events;
    UmiCommandBus *commands;
    UmiQueryBus *queries;
    UmiDataServer *data_server;
    UmiConfig *config;
    UmiClock *clock;
    UmiScheduler *scheduler;
    UmiDiagnosticSink diagnostic_sink;
    void *diagnostic_user_data;
} UmiModuleContext;

typedef UmiStatus (*UmiModulePhaseFn)(UmiModuleContext *context);
typedef void (*UmiModuleDestroyFn)(UmiModuleContext *context);

typedef struct UmiModuleLifecycle {
    UmiModulePhaseFn configure;
    UmiModulePhaseFn initialise;
    UmiModulePhaseFn start;
    UmiModulePhaseFn quiesce;
    UmiModulePhaseFn stop;
    UmiModuleDestroyFn destroy;
} UmiModuleLifecycle;

typedef struct UmiModuleDescriptor {
    uint32_t structure_size;
    uint32_t abi_version;
    const char *module_id;
    const char *display_name;
    UmiVersion module_version;
    UmiModuleKind kind;
    const char *const *provided_capabilities;
    const char *const *required_capabilities;
    void *module_state;
    UmiModuleLifecycle lifecycle;
} UmiModuleDescriptor;

#ifdef __cplusplus
}
#endif

#endif
