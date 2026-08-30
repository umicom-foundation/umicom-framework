/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_ui/layout_selector_view.h
 *
 * PURPOSE:
 *   Project canonical named layouts without moving layout definitions into product code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_UI_LAYOUT_SELECTOR_VIEW_H
#define UMICOM_APPLICATION_UI_LAYOUT_SELECTOR_VIEW_H

#include "umicom/application/runtime/experience_projection.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t umi_application_ui_layout_count(
    const UmiApplicationExperienceDefinition *experience);
UmiStatus umi_application_ui_layout_row(
    const UmiApplicationExperienceDefinition *experience,
    size_t index,
    UmiApplicationLayoutProjection *out_row);

#ifdef __cplusplus
}
#endif

#endif
