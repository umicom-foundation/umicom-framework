/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_ui/application_ui.h
 *
 * PURPOSE:
 *   Aggregate toolkit-neutral application portfolio and runtime-experience
 *   view contracts, including readiness, health, ownership and catalogues.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_UI_APPLICATION_UI_H
#define UMICOM_APPLICATION_UI_APPLICATION_UI_H

#include "umicom/application/application.h"
#include "umicom/application_ui/capability_view.h"
#include "umicom/application_ui/feature_backlog_view.h"
#include "umicom/application_ui/health_view.h"
#include "umicom/application_ui/layout_selector_view.h"
#include "umicom/application_ui/ownership_view.h"
#include "umicom/application_ui/panel_catalogue_view.h"
#include "umicom/application_ui/readiness_view.h"
#include "umicom/application_ui/session_view.h"
#include "umicom/application_ui/summary_view.h"
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

/**
 * Initialise application ui portfolio view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_application_ui_portfolio_view_create(
    const char *view_id,
    const char *selected_application_id,
    UmiUiViewModel **out_view);
/**
 * Initialise application ui boundary audit view from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_application_ui_boundary_audit_view_create(
    const char *view_id,
    UmiUiViewModel **out_view);
/**
 * Initialise application ui component catalogue view from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_application_ui_component_catalogue_view_create(
    const char *view_id,
    const char *domain_id,
    UmiUiViewModel **out_view);
/**
 * Initialise application ui gtk4 coverage view from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_application_ui_gtk4_coverage_view_create(
    const char *view_id,
    UmiUiViewModel **out_view);

#ifdef __cplusplus
}
#endif

#endif
