/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/types.h
 *
 * PURPOSE:
 *   Define bounded, allocation-free types shared by the Framework-owned application experience runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_TYPES_H
#define UMICOM_APPLICATION_RUNTIME_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/application/experience.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_RUNTIME_API_VERSION 1U
#define UMI_APPLICATION_RUNTIME_MAX_PANELS 64U
#define UMI_APPLICATION_RUNTIME_MAX_CAPABILITIES 64U
#define UMI_APPLICATION_RUNTIME_MAX_CONTEXT_BINDINGS 16U
#define UMI_APPLICATION_RUNTIME_MAX_LIFECYCLE_EVENTS 32U
#define UMI_APPLICATION_RUNTIME_MAX_COMMANDS 128U
#define UMI_APPLICATION_RUNTIME_MAX_OPERATIONS 64U
#define UMI_APPLICATION_RUNTIME_TEXT_CAPACITY 128U

typedef int (*UmiApplicationCapabilityProbe)(
    const char *capability_id,
    void *user_data);

typedef enum UmiApplicationPanelVisibility {
    UMI_APPLICATION_PANEL_HIDDEN = 0,
    UMI_APPLICATION_PANEL_VISIBLE = 1,
    UMI_APPLICATION_PANEL_AUTO_HIDDEN = 2,
    UMI_APPLICATION_PANEL_FLOATING = 3
} UmiApplicationPanelVisibility;

typedef enum UmiApplicationHealthSeverity {
    UMI_APPLICATION_HEALTH_READY = 0,
    UMI_APPLICATION_HEALTH_DEGRADED = 1,
    UMI_APPLICATION_HEALTH_BLOCKED = 2
} UmiApplicationHealthSeverity;

typedef enum UmiApplicationLifecyclePhase {
    UMI_APPLICATION_LIFECYCLE_DISCOVERED = 0,
    UMI_APPLICATION_LIFECYCLE_CONFIGURED = 1,
    UMI_APPLICATION_LIFECYCLE_INITIALISED = 2,
    UMI_APPLICATION_LIFECYCLE_STARTED = 3,
    UMI_APPLICATION_LIFECYCLE_QUIESCED = 4,
    UMI_APPLICATION_LIFECYCLE_STOPPED = 5,
    UMI_APPLICATION_LIFECYCLE_DESTROYED = 6
} UmiApplicationLifecyclePhase;

typedef enum UmiApplicationCommandKind {
    UMI_APPLICATION_COMMAND_ACTIVATE_PANEL = 1,
    UMI_APPLICATION_COMMAND_SELECT_LAYOUT = 2,
    UMI_APPLICATION_COMMAND_TOGGLE_LAYOUT_LOCK = 3,
    UMI_APPLICATION_COMMAND_ACTIVATE_FEATURE = 4
} UmiApplicationCommandKind;

typedef enum UmiApplicationOperationKind {
    UMI_APPLICATION_OPERATION_SESSION_START = 1,
    UMI_APPLICATION_OPERATION_LAYOUT_CHANGE = 2,
    UMI_APPLICATION_OPERATION_PANEL_ACTIVATE = 3,
    UMI_APPLICATION_OPERATION_PANEL_DEACTIVATE = 4,
    UMI_APPLICATION_OPERATION_CONTEXT_CHANGE = 5,
    UMI_APPLICATION_OPERATION_FEATURE_GATE = 6
} UmiApplicationOperationKind;

#ifdef __cplusplus
}
#endif

#endif
