/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/validation_marker.h
 *
 * PURPOSE:
 *   Attach a validation severity/message to a component or property.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_VALIDATION_MARKER_H
#define UMICOM_DESIGNER_RAD_VALIDATION_MARKER_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad validation marker data shared with callers of this public contract.
 */
typedef struct UmiRadValidationMarker {
    char component_id[UMI_RAD_ID_CAPACITY];
    char property_id[UMI_RAD_ID_CAPACITY];
    UmiRadSeverity severity;
    char message[UMI_RAD_TEXT_CAPACITY];
} UmiRadValidationMarker;
/**
 * Initialise rad validation marker from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_validation_marker_init(UmiRadValidationMarker *item);
/**
 * Check that rad validation marker satisfies its contract before another service relies on
 * it.
 */
int umi_rad_validation_marker_is_valid(const UmiRadValidationMarker *item);
#ifdef __cplusplus
}
#endif
#endif
