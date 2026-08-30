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
typedef struct UmiRadValidationMarker {
    char component_id[UMI_RAD_ID_CAPACITY];
    char property_id[UMI_RAD_ID_CAPACITY];
    UmiRadSeverity severity;
    char message[UMI_RAD_TEXT_CAPACITY];
} UmiRadValidationMarker;
UmiStatus umi_rad_validation_marker_init(UmiRadValidationMarker *item);
int umi_rad_validation_marker_is_valid(const UmiRadValidationMarker *item);
#ifdef __cplusplus
}
#endif
#endif
