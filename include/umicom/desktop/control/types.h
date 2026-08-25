/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/types.h
 *
 * PURPOSE:
 *   Define bounded IDs, text, geometry and helper functions shared by the additive Desk control plane.
 *
 * ARCHITECTURE:
 *   This is additive Framework-owned Desk control state. It extends the
 *   established desktop/workbench/layout runtime; it does not replace existing
 *   models and applications remain thin consumers of this public contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_CONTROL_TYPES_H
#define UMICOM_DESKTOP_CONTROL_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_CONTROL_API_VERSION 1U
#define UMI_DESKTOP_CONTROL_ID_CAPACITY 128U
#define UMI_DESKTOP_CONTROL_TEXT_CAPACITY 256U
#define UMI_DESKTOP_CONTROL_PATH_CAPACITY 1024U
#define UMI_DESKTOP_CONTROL_ITEM_CAPACITY 64U

typedef enum UmiDesktopControlSeverity {
    UMI_DESKTOP_CONTROL_SEVERITY_NONE = 0,
    UMI_DESKTOP_CONTROL_SEVERITY_INFO = 1,
    UMI_DESKTOP_CONTROL_SEVERITY_WARNING = 2,
    UMI_DESKTOP_CONTROL_SEVERITY_ERROR = 3,
    UMI_DESKTOP_CONTROL_SEVERITY_CRITICAL = 4
} UmiDesktopControlSeverity;

typedef enum UmiDesktopControlAvailability {
    UMI_DESKTOP_CONTROL_AVAILABILITY_UNKNOWN = 0,
    UMI_DESKTOP_CONTROL_AVAILABILITY_AVAILABLE = 1,
    UMI_DESKTOP_CONTROL_AVAILABILITY_DEGRADED = 2,
    UMI_DESKTOP_CONTROL_AVAILABILITY_UNAVAILABLE = 3
} UmiDesktopControlAvailability;

typedef enum UmiDesktopControlLifecycleState {
    UMI_DESKTOP_CONTROL_LIFECYCLE_STOPPED = 0,
    UMI_DESKTOP_CONTROL_LIFECYCLE_STARTING = 1,
    UMI_DESKTOP_CONTROL_LIFECYCLE_RUNNING = 2,
    UMI_DESKTOP_CONTROL_LIFECYCLE_STOPPING = 3,
    UMI_DESKTOP_CONTROL_LIFECYCLE_FAILED = 4
} UmiDesktopControlLifecycleState;

typedef enum UmiDesktopControlDirection {
    UMI_DESKTOP_CONTROL_DIRECTION_NONE = 0,
    UMI_DESKTOP_CONTROL_DIRECTION_SOURCE = 1,
    UMI_DESKTOP_CONTROL_DIRECTION_DESTINATION = 2,
    UMI_DESKTOP_CONTROL_DIRECTION_BIDIRECTIONAL = 3
} UmiDesktopControlDirection;

typedef struct UmiDesktopControlRect {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
} UmiDesktopControlRect;

/* Copy text only when it fits so callers never receive silently truncated IDs. */
UmiStatus umi_desktop_control_copy_text(char *destination, size_t capacity,
                                        const char *source);
/* Stable IDs are non-empty and must fit the public ID capacity. */
bool umi_desktop_control_id_valid(const char *value);
/* Validate positive geometry without imposing toolkit or monitor policy. */
bool umi_desktop_control_rect_valid(const UmiDesktopControlRect *rect);
/* Clamp geometry into a monitor work area while retaining at least one pixel. */
UmiStatus umi_desktop_control_rect_clamp(UmiDesktopControlRect *rect,
                                         const UmiDesktopControlRect *bounds);
/* Lightweight deterministic hash is used for comparison/provenance metadata. */
uint64_t umi_desktop_control_hash_text(const char *value);
/* ASCII case-insensitive substring matching supports toolkit-neutral filtering. */
bool umi_desktop_control_text_contains(const char *text, const char *needle);

#ifdef __cplusplus
}
#endif
#endif
