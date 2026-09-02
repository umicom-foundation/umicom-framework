/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_ui/ownership_view.h
 *
 * PURPOSE:
 *   Summarise feature ownership so reusable work remains visibly Framework-owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_UI_OWNERSHIP_VIEW_H
#define UMICOM_APPLICATION_UI_OWNERSHIP_VIEW_H

#include "umicom/application/runtime/readiness.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application ui ownership row data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationUiOwnershipRow {
    UmiExperienceOwnership owner;
    const char *owner_text;
    size_t feature_count;
} UmiApplicationUiOwnershipRow;

#define UMI_APPLICATION_UI_OWNERSHIP_ROW_COUNT 3U

/**
 * Provide the application ui ownership rows operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_ui_ownership_rows(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationUiOwnershipRow rows[UMI_APPLICATION_UI_OWNERSHIP_ROW_COUNT]);

#ifdef __cplusplus
}
#endif

#endif
