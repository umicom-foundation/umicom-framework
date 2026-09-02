/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_ui/panel_catalogue_view.h
 *
 * PURPOSE:
 *   Project canonical panel metadata into dense frontend-neutral rows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_UI_PANEL_CATALOGUE_VIEW_H
#define UMICOM_APPLICATION_UI_PANEL_CATALOGUE_VIEW_H

#include "umicom/application/runtime/experience_projection.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Return the number of records represented by application ui panel without changing their
 * state.
 */
size_t umi_application_ui_panel_count(
    const UmiApplicationExperienceDefinition *experience);
/**
 * Provide the application ui panel row operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_ui_panel_row(
    const UmiApplicationExperienceDefinition *experience,
    size_t index,
    UmiApplicationPanelProjection *out_row);

#ifdef __cplusplus
}
#endif

#endif
