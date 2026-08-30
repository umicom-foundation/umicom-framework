/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/boundary_audit.c
 *
 * PURPOSE:
 *   Audit application composition and reject direct application dependencies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/boundary_audit.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "umicom/application/component_catalogue.h"
#include "umicom/runtime/capability_catalogue.h"

static void add_finding(UmiApplicationAuditReport *report,
                        UmiApplicationAuditSeverity severity,
                        const char *rule_id,
                        const char *subject,
                        const char *format,
                        ...)
{
    UmiApplicationAuditFinding *finding;
    va_list arguments;
    if (report == NULL ||
        report->finding_count >= UMI_APPLICATION_AUDIT_MAX_FINDINGS) return;
    finding = &report->findings[report->finding_count++];
    finding->severity = severity;
    (void)snprintf(finding->rule_id, sizeof(finding->rule_id), "%s",
                   rule_id != NULL ? rule_id : "unknown");
    (void)snprintf(finding->subject, sizeof(finding->subject), "%s",
                   subject != NULL ? subject : "portfolio");
    va_start(arguments, format);
    (void)vsnprintf(finding->message, sizeof(finding->message), format,
                    arguments);
    va_end(arguments);
    if (severity == UMI_APPLICATION_AUDIT_ERROR) report->error_count += 1U;
    else if (severity == UMI_APPLICATION_AUDIT_WARNING)
        report->warning_count += 1U;
    else report->information_count += 1U;
}

static int duplicate_identity(const UmiApplicationDefinition *const *items,
                              size_t current)
{
    size_t index;
    const UmiApplicationDefinition *candidate = items[current];
    if (candidate == NULL) return 0;
    for (index = 0U; index < current; ++index) {
        if (items[index] != NULL &&
            (strcmp(items[index]->application_id,
                    candidate->application_id) == 0 ||
             strcmp(items[index]->repository_slug,
                    candidate->repository_slug) == 0)) return 1;
    }
    return 0;
}

static void audit_definition(const UmiApplicationDefinition *definition,
                             UmiApplicationAuditReport *report)
{
    size_t index;
    if (umi_application_definition_validate(definition) != UMI_STATUS_OK) {
        add_finding(report, UMI_APPLICATION_AUDIT_ERROR,
                    "UMI-APP-001", definition != NULL
                        ? definition->application_id : "null",
                    "Application definition is incomplete or violates standalone/federated policy.");
        return;
    }
    for (index = 0U; index < definition->required_capability_count; ++index) {
        const char *capability_id = definition->required_capabilities[index];
        const UmiFrameworkCapabilityDefinition *capability =
            umi_framework_capability_catalogue_find(capability_id);
        report->capability_reference_count += 1U;
        if (capability == NULL) {
            add_finding(report, UMI_APPLICATION_AUDIT_ERROR,
                        "UMI-APP-002", definition->application_id,
                        "Required capability '%s' is not owned by the Framework catalogue.",
                        capability_id);
        }
    }
    for (index = 0U; index < definition->optional_capability_count; ++index) {
        const char *capability_id = definition->optional_capabilities[index];
        report->capability_reference_count += 1U;
        if (umi_framework_capability_catalogue_find(capability_id) == NULL) {
            add_finding(report, UMI_APPLICATION_AUDIT_ERROR,
                        "UMI-APP-003", definition->application_id,
                        "Optional capability '%s' is not a Framework capability.",
                        capability_id);
        }
    }
    for (index = 0U; index < definition->component_domain_count; ++index) {
        const char *domain_id = definition->component_domains[index];
        report->component_domain_reference_count += 1U;
        if (!umi_application_component_domain_exists(domain_id)) {
            add_finding(report, UMI_APPLICATION_AUDIT_ERROR,
                        "UMI-APP-004", definition->application_id,
                        "Component domain '%s' is absent from the Framework catalogue.",
                        domain_id);
        }
    }
}

static void audit_component_catalogue(UmiApplicationAuditReport *report)
{
    size_t index;
    for (index = 0U; index < umi_application_component_catalogue_count();
         ++index) {
        const UmiApplicationComponentDefinition *component =
            umi_application_component_catalogue_at(index);
        size_t previous;
        report->reusable_component_count += 1U;
        if (component == NULL || component->component_id == NULL ||
            component->domain_id == NULL || component->capability_id == NULL ||
            umi_framework_capability_catalogue_find(
                component->capability_id) == NULL) {
            add_finding(report, UMI_APPLICATION_AUDIT_ERROR,
                        "UMI-APP-009",
                        component != NULL ? component->component_id : "null",
                        "Reusable component is incomplete or refers to a non-Framework capability.");
            continue;
        }
        for (previous = 0U; previous < index; ++previous) {
            const UmiApplicationComponentDefinition *other =
                umi_application_component_catalogue_at(previous);
            if (other != NULL && strcmp(other->component_id,
                                        component->component_id) == 0) {
                add_finding(report, UMI_APPLICATION_AUDIT_ERROR,
                            "UMI-APP-010", component->component_id,
                            "Reusable component identity is duplicated.");
                break;
            }
        }
    }
}

UmiStatus umi_application_boundary_audit(
    const UmiApplicationDefinition *const *applications,
    size_t application_count,
    const UmiApplicationDependency *dependencies,
    size_t dependency_count,
    UmiApplicationAuditReport *out_report)
{
    size_t index;
    if (applications == NULL || application_count == 0U ||
        out_report == NULL || (dependency_count > 0U && dependencies == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_report, 0, sizeof(*out_report));
    out_report->application_count = application_count;
    audit_component_catalogue(out_report);

    for (index = 0U; index < application_count; ++index) {
        audit_definition(applications[index], out_report);
        if (duplicate_identity(applications, index)) {
            add_finding(out_report, UMI_APPLICATION_AUDIT_ERROR,
                        "UMI-APP-005",
                        applications[index] != NULL
                            ? applications[index]->application_id : "null",
                        "Application or repository identity is duplicated.");
        }
    }
    for (index = 0U; index < dependency_count; ++index) {
        const UmiApplicationDependency *dependency = &dependencies[index];
        if (dependency->source_application_id == NULL ||
            dependency->target_id == NULL) {
            add_finding(out_report, UMI_APPLICATION_AUDIT_ERROR,
                        "UMI-APP-006", "dependency",
                        "Dependency identity is incomplete.");
        } else if (dependency->kind == UMI_APPLICATION_DEPENDENCY_APPLICATION) {
            add_finding(out_report, UMI_APPLICATION_AUDIT_ERROR,
                        "UMI-APP-007", dependency->source_application_id,
                        "Direct dependency on application '%s' is forbidden; request a Framework capability instead.",
                        dependency->target_id);
        } else if (dependency->kind !=
                   UMI_APPLICATION_DEPENDENCY_FRAMEWORK_CAPABILITY ||
                   umi_framework_capability_catalogue_find(
                       dependency->target_id) == NULL) {
            add_finding(out_report, UMI_APPLICATION_AUDIT_ERROR,
                        "UMI-APP-008", dependency->source_application_id,
                        "Dependency target '%s' is not a registered Framework capability.",
                        dependency->target_id);
        }
    }
    if (out_report->error_count == 0U) {
        add_finding(out_report, UMI_APPLICATION_AUDIT_INFORMATION,
                    "UMI-APP-000", "portfolio",
                    "All application boundaries resolve through Framework-owned capabilities and component domains.");
    }
    out_report->passed = out_report->error_count == 0U;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_portfolio_audit(
    UmiApplicationAuditReport *out_report)
{
    const UmiApplicationDefinition *
        applications[UMI_APPLICATION_AUDIT_MAX_APPLICATIONS];
    size_t index;
    const size_t count = umi_application_portfolio_count();
    if (out_report == NULL ||
        count > UMI_APPLICATION_AUDIT_MAX_APPLICATIONS)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < count; ++index)
        applications[index] = umi_application_portfolio_at(index);
    return umi_application_boundary_audit(applications, count, NULL, 0U,
                                          out_report);
}

const char *umi_application_audit_severity_text(
    UmiApplicationAuditSeverity severity)
{
    switch (severity) {
        case UMI_APPLICATION_AUDIT_INFORMATION: return "information";
        case UMI_APPLICATION_AUDIT_WARNING: return "warning";
        case UMI_APPLICATION_AUDIT_ERROR: return "error";
        default: return "unknown";
    }
}
