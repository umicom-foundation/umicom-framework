/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_ui/application_views.c
 *
 * PURPOSE:
 *   Build reusable application-architecture view models without GTK or Studio
 *   dependencies.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application_ui/application_ui.h"

#include <stdint.h>

#include "umicom/ui/gtk4/widget_catalogue.h"

static UmiStatus set_string(UmiUiViewModel *view, const char *key,
                            const char *text)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_string(&value, text != NULL ? text : "");
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus set_integer(UmiUiViewModel *view, const char *key,
                             size_t number)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_integer(&value, (int64_t)number);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus set_boolean(UmiUiViewModel *view, const char *key, int enabled)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_boolean(&value, enabled != 0);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus create_view(const char *view_id, const char *view_type,
                             const char *title, const char *summary,
                             UmiUiViewModel **out_view)
{
    UmiStatus status;
    if (view_id == NULL || out_view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_view = NULL;
    status = umi_ui_view_model_create(view_id, view_type, UMI_UI_ROLE_PANE,
                                      out_view);
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "title", title);
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "summary", summary);
    if (status != UMI_STATUS_OK && *out_view != NULL) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}

UmiStatus umi_application_ui_portfolio_view_create(
    const char *view_id,
    const char *selected_application_id,
    UmiUiViewModel **out_view)
{
    const UmiApplicationDefinition *selected = selected_application_id != NULL
        ? umi_application_portfolio_find(selected_application_id) : NULL;
    UmiStatus status = create_view(
        view_id, UMI_APPLICATION_UI_VIEW_PORTFOLIO, "Umicom Applications",
        "Independent applications composed from one Framework capability and component authority.",
        out_view);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "applications.total",
                             umi_application_portfolio_count());
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "applications.available",
            umi_application_portfolio_maturity_count(UMI_APPLICATION_AVAILABLE));
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "applications.foundation",
            umi_application_portfolio_maturity_count(UMI_APPLICATION_FOUNDATION));
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "applications.roadmap",
            umi_application_portfolio_maturity_count(UMI_APPLICATION_ROADMAP));
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "applications.communication",
                            "Framework capability router only");
    if (status == UMI_STATUS_OK)
        status = set_boolean(*out_view, "applications.direct-links", 0);
    if (status == UMI_STATUS_OK && selected != NULL)
        status = set_string(*out_view, "selected.id", selected->application_id);
    if (status == UMI_STATUS_OK && selected != NULL)
        status = set_string(*out_view, "selected.name", selected->display_name);
    if (status == UMI_STATUS_OK && selected != NULL)
        status = set_string(*out_view, "selected.repository",
                            selected->repository_slug);
    if (status == UMI_STATUS_OK && selected != NULL)
        status = set_string(*out_view, "selected.family",
                            umi_application_family_text(selected->family));
    if (status == UMI_STATUS_OK && selected != NULL)
        status = set_string(*out_view, "selected.maturity",
                            umi_application_maturity_text(selected->maturity));
    if (status == UMI_STATUS_OK && selected != NULL)
        status = set_string(*out_view, "selected.purpose", selected->purpose);
    return status;
}

UmiStatus umi_application_ui_boundary_audit_view_create(
    const char *view_id,
    UmiUiViewModel **out_view)
{
    UmiApplicationAuditReport report;
    UmiStatus status = umi_application_portfolio_audit(&report);
    if (status != UMI_STATUS_OK) return status;
    status = create_view(
        view_id, UMI_APPLICATION_UI_VIEW_BOUNDARY_AUDIT,
        "Application Boundary Audit",
        "Evidence that applications remain standalone, federated and dependent only on Framework authorities.",
        out_view);
    if (status == UMI_STATUS_OK)
        status = set_boolean(*out_view, "audit.passed", report.passed);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "audit.applications",
                             report.application_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "audit.capability-references",
                             report.capability_reference_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "audit.component-domains",
                             report.component_domain_reference_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "audit.reusable-components",
                             report.reusable_component_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "audit.errors", report.error_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "audit.warnings", report.warning_count);
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "audit.rule",
                            "No application-to-application dependencies");
    return status;
}

UmiStatus umi_application_ui_component_catalogue_view_create(
    const char *view_id,
    const char *domain_id,
    UmiUiViewModel **out_view)
{
    UmiStatus status = create_view(
        view_id, UMI_APPLICATION_UI_VIEW_COMPONENTS,
        "Reusable Component Catalogue",
        "Framework-owned professional surfaces shared by IDE, finance, media, AI, design, games and Umicom OS.",
        out_view);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "components.total",
                             umi_application_component_catalogue_count());
    if (status == UMI_STATUS_OK && domain_id != NULL)
        status = set_string(*out_view, "components.domain", domain_id);
    if (status == UMI_STATUS_OK && domain_id != NULL)
        status = set_integer(*out_view, "components.domain-count",
                             umi_application_component_domain_count(domain_id));
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "components.owner",
                            "Umicom Framework");
    return status;
}

UmiStatus umi_application_ui_gtk4_coverage_view_create(
    const char *view_id,
    UmiUiViewModel **out_view)
{
    const UmiGtk4CoverageReport coverage = umi_gtk4_widget_coverage();
    UmiStatus status = create_view(
        view_id, UMI_APPLICATION_UI_VIEW_GTK4_COVERAGE,
        "GTK4 Component Coverage",
        "Semantic factories, GtkBuilder-native widgets, internal classes and deprecated compatibility are explicitly inventoried.",
        out_view);
    if (status == UMI_STATUS_OK)
        status = set_boolean(*out_view, "gtk4.complete", coverage.complete);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "gtk4.widgets", coverage.widget_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "gtk4.semantic-factories",
                             coverage.semantic_factory_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "gtk4.builder-native",
                             coverage.builder_native_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "gtk4.internal",
                             coverage.framework_internal_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "gtk4.deprecated-compatibility",
                             coverage.deprecated_compatibility_count);
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "gtk4.minimum-version", "4.10");
    return status;
}
