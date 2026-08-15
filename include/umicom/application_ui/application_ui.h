/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_ui/application_ui.h
 *
 * PURPOSE:
 *   Project the Framework application portfolio, boundary audit, reusable
 *   component catalogue and GTK4 coverage into toolkit-neutral view models.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_UI_APPLICATION_UI_H
#define UMICOM_APPLICATION_UI_APPLICATION_UI_H

#include "umicom/application/application.h"
#include "umicom/ui/view_model.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_UI_VIEW_PORTFOLIO \
    "umicom.application.portfolio"
#define UMI_APPLICATION_UI_VIEW_BOUNDARY_AUDIT \
    "umicom.application.boundary-audit"
#define UMI_APPLICATION_UI_VIEW_COMPONENTS \
    "umicom.application.components"
#define UMI_APPLICATION_UI_VIEW_GTK4_COVERAGE \
    "umicom.application.gtk4-coverage"

UmiStatus umi_application_ui_portfolio_view_create(
    const char *view_id,
    const char *selected_application_id,
    UmiUiViewModel **out_view);
UmiStatus umi_application_ui_boundary_audit_view_create(
    const char *view_id,
    UmiUiViewModel **out_view);
UmiStatus umi_application_ui_component_catalogue_view_create(
    const char *view_id,
    const char *domain_id,
    UmiUiViewModel **out_view);
UmiStatus umi_application_ui_gtk4_coverage_view_create(
    const char *view_id,
    UmiUiViewModel **out_view);

#ifdef __cplusplus
}
#endif

#endif
