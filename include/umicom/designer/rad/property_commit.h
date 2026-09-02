/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/property_commit.h
 *
 * PURPOSE:
 *   Record before/after property values for review, undo and audit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_PROPERTY_COMMIT_H
#define UMICOM_DESIGNER_RAD_PROPERTY_COMMIT_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad property commit data shared with callers of this public contract.
 */
typedef struct UmiRadPropertyCommit {
    char component_id[UMI_RAD_ID_CAPACITY];
    char property_id[UMI_RAD_ID_CAPACITY];
    char before_value[UMI_RAD_VALUE_CAPACITY];
    char after_value[UMI_RAD_VALUE_CAPACITY];
    uint64_t revision;
} UmiRadPropertyCommit;
/**
 * Initialise rad property commit from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_property_commit_init(UmiRadPropertyCommit *item);
/**
 * Check that rad property commit satisfies its contract before another service relies on
 * it.
 */
int umi_rad_property_commit_is_valid(const UmiRadPropertyCommit *item);
#ifdef __cplusplus
}
#endif
#endif
