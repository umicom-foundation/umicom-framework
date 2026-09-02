/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/property_binding.h
 *
 * PURPOSE:
 *   Describe a visual property binding backed by the canonical reactive UI state layer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_PROPERTY_BINDING_H
#define UMICOM_DESIGNER_RAD_PROPERTY_BINDING_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad property binding data shared with callers of this public contract.
 */
typedef struct UmiRadPropertyBinding {
    char binding_id[UMI_RAD_ID_CAPACITY];
    char source_path[UMI_RAD_PATH_CAPACITY];
    char target_path[UMI_RAD_PATH_CAPACITY];
    bool two_way;
    bool enabled;
} UmiRadPropertyBinding;
/**
 * Initialise rad property binding from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_property_binding_init(UmiRadPropertyBinding *item);
/**
 * Check that rad property binding satisfies its contract before another service relies on
 * it.
 */
int umi_rad_property_binding_is_valid(const UmiRadPropertyBinding *item);
#ifdef __cplusplus
}
#endif
#endif
