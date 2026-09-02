/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_ui/capability_view.h
 *
 * PURPOSE:
 *   Project capability status entries into frontend-neutral rows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_UI_CAPABILITY_VIEW_H
#define UMICOM_APPLICATION_UI_CAPABILITY_VIEW_H

#include "umicom/application/runtime/capability_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application ui capability row data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationUiCapabilityRow {
    const char *capability_id;
    int available;
    size_t consumer_count;
} UmiApplicationUiCapabilityRow;

/**
 * Provide the application ui capability row operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_ui_capability_row(
    const UmiApplicationCapabilityStatus *status,
    size_t index,
    UmiApplicationUiCapabilityRow *out_row);

#ifdef __cplusplus
}
#endif

#endif
