/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/types.h
 *
 * PURPOSE:
 *   Define stable toolkit-neutral identifiers, dimensions, placement values and
 *   UI state enumerations shared by Framework view models and frontend adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_TYPES_H
#define UMICOM_UI_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_UI_ID_CAPACITY 128U
#define UMI_UI_TEXT_CAPACITY 256U
#define UMI_UI_DESCRIPTION_CAPACITY 512U
#define UMI_UI_ICON_CAPACITY 128U
#define UMI_UI_ACCELERATOR_CAPACITY 96U
#define UMI_UI_MAX_ITEMS 512U

/**
 * List the named ui orientation values accepted by this public contract.
 */
typedef enum UmiUiOrientation {
    UMI_UI_HORIZONTAL = 1,
    UMI_UI_VERTICAL = 2
} UmiUiOrientation;

/**
 * List the named ui placement values accepted by this public contract.
 */
typedef enum UmiUiPlacement {
    UMI_UI_PLACEMENT_LEFT = 1,
    UMI_UI_PLACEMENT_RIGHT = 2,
    UMI_UI_PLACEMENT_TOP = 3,
    UMI_UI_PLACEMENT_BOTTOM = 4,
    UMI_UI_PLACEMENT_CENTRE = 5,
    UMI_UI_PLACEMENT_FLOATING = 6
} UmiUiPlacement;

/**
 * List the named ui role values accepted by this public contract.
 */
typedef enum UmiUiRole {
    UMI_UI_ROLE_GENERIC = 0,
    UMI_UI_ROLE_APPLICATION = 1,
    UMI_UI_ROLE_WORKBENCH = 2,
    UMI_UI_ROLE_PANE = 3,
    UMI_UI_ROLE_EDITOR = 4,
    UMI_UI_ROLE_MENU = 5,
    UMI_UI_ROLE_TOOLBAR = 6,
    UMI_UI_ROLE_STATUS = 7,
    UMI_UI_ROLE_DIALOG = 8,
    UMI_UI_ROLE_NOTIFICATION = 9
} UmiUiRole;

/**
 * List the named ui severity values accepted by this public contract.
 */
typedef enum UmiUiSeverity {
    UMI_UI_SEVERITY_INFORMATION = 1,
    UMI_UI_SEVERITY_SUCCESS = 2,
    UMI_UI_SEVERITY_WARNING = 3,
    UMI_UI_SEVERITY_ERROR = 4
} UmiUiSeverity;

/**
 * Represent the ui size data shared with callers of this public contract.
 */
typedef struct UmiUiSize {
    int32_t width;
    int32_t height;
} UmiUiSize;

/**
 * Represent the ui point data shared with callers of this public contract.
 */
typedef struct UmiUiPoint {
    int32_t x;
    int32_t y;
} UmiUiPoint;

/**
 * Represent the ui rect data shared with callers of this public contract.
 */
typedef struct UmiUiRect {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
} UmiUiRect;

/**
 * Check that ui id satisfies its contract before another service relies on it.
 */
int umi_ui_id_is_valid(const char *identifier);
/**
 * Provide the ui orientation text operation used by this module and its client
 * applications.
 */
const char *umi_ui_orientation_text(UmiUiOrientation orientation);
/**
 * Provide the ui placement text operation used by this module and its client applications.
 */
const char *umi_ui_placement_text(UmiUiPlacement placement);
/* Parse the same stable placement text emitted by umi_ui_placement_text().
 * Application metadata uses this helper instead of maintaining another region parser. */
UmiStatus umi_ui_placement_parse(const char *text, UmiUiPlacement *out_placement);
/**
 * Provide the ui role text operation used by this module and its client applications.
 */
const char *umi_ui_role_text(UmiUiRole role);
/**
 * Provide the ui severity text operation used by this module and its client applications.
 */
const char *umi_ui_severity_text(UmiUiSeverity severity);

#ifdef __cplusplus
}
#endif

#endif
