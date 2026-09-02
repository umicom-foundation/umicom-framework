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

/* Provide the add finding operation used by this module and its client applications. */
static void add_finding(UmiApplicationAuditReport *report,
                        UmiApplicationAuditSeverity severity,
                        const char *rule_id,
                        const char *subject,
                        const char *format,
                        ...)
{
    UmiApplicationAuditFinding *finding;
    va_list arguments;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (severity == UMI_APPLICATION_AUDIT_ERROR) report->error_count += 1U;
    else /* Apply this branch only when its contract condition is satisfied. */ if (severity == UMI_APPLICATION_AUDIT_WARNING)
        report->warning_count += 1U;
    /* Use this fallback path when the earlier condition does not apply. */
    else report->information_count += 1U;
}

/*
 * Provide the duplicate identity operation used by this module and its client
 * applications.
 */
static int duplicate_identity(const UmiApplicationDefinition *const *items,
                              size_t current)
{
    size_t index;
    const UmiApplicationDefinition *candidate = items[current];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (candidate == NULL) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < current; ++index) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (items[index] != NULL &&
            (strcmp(items[index]->application_id,
                    candidate->application_id) == 0 ||
             strcmp(items[index]->repository_slug,
                    candidate->repository_slug) == 0)) return 1;
    }
    return 0;
}

/* Provide the audit definition operation used by this module and its client applications. */
static void audit_definition(const UmiApplicationDefinition *definition,
                             UmiApplicationAuditReport *report)
{
    size_t index;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_application_definition_validate(definition) != UMI_STATUS_OK) {
        add_finding(report, UMI_APPLICATION_AUDIT_ERROR,
                    "UMI-APP-001", definition != NULL
                        ? definition->application_id : "null",
                    "Application definition is incomplete or violates standalone/federated policy.");
        return;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < definition->required_capability_count; ++index) {
        const char *capability_id = definition->required_capabilities[index];
        const UmiFrameworkCapabilityDefinition *capability =
            umi_framework_capability_catalogue_find(capability_id);
        report->capability_reference_count += 1U;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (capability == NULL) {
            add_finding(report, UMI_APPLICATION_AUDIT_ERROR,
                        "UMI-APP-002", definition->application_id,
                        "Required capability '%s' is not owned by the Framework catalogue.",
                        capability_id);
        }
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < definition->optional_capability_count; ++index) {
        const char *capability_id = definition->optional_capabilities[index];
        report->capability_reference_count += 1U;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (umi_framework_capability_catalogue_find(capability_id) == NULL) {
            add_finding(report, UMI_APPLICATION_AUDIT_ERROR,
                        "UMI-APP-003", definition->application_id,
                        "Optional capability '%s' is not a Framework capability.",
                        capability_id);
        }
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < definition->component_domain_count; ++index) {
        const char *domain_id = definition->component_domains[index];
        report->component_domain_reference_count += 1U;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!umi_application_component_domain_exists(domain_id)) {
            add_finding(report, UMI_APPLICATION_AUDIT_ERROR,
                        "UMI-APP-004", definition->application_id,
                        "Component domain '%s' is absent from the Framework catalogue.",
                        domain_id);
        }
    }
}

/*
 * Provide the audit component catalogue operation used by this module and its client
 * applications.
 */
static void audit_component_catalogue(UmiApplicationAuditReport *report)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_application_component_catalogue_count();
         ++index) {
        const UmiApplicationComponentDefinition *component =
            umi_application_component_catalogue_at(index);
        size_t previous;
        report->reusable_component_count += 1U;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
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
        /* Visit each bounded item once so every record receives the same rule. */
        for (previous = 0U; previous < index; ++previous) {
            const UmiApplicationComponentDefinition *other =
                umi_application_component_catalogue_at(previous);
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
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

/*
 * Provide the application boundary audit operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_boundary_audit(
    const UmiApplicationDefinition *const *applications,
    size_t application_count,
    const UmiApplicationDependency *dependencies,
    size_t dependency_count,
    UmiApplicationAuditReport *out_report)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (applications == NULL || application_count == 0U ||
        out_report == NULL || (dependency_count > 0U && dependencies == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_report, 0, sizeof(*out_report));
    out_report->application_count = application_count;
    audit_component_catalogue(out_report);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < application_count; ++index) {
        audit_definition(applications[index], out_report);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (duplicate_identity(applications, index)) {
            add_finding(out_report, UMI_APPLICATION_AUDIT_ERROR,
                        "UMI-APP-005",
                        applications[index] != NULL
                            ? applications[index]->application_id : "null",
                        "Application or repository identity is duplicated.");
        }
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < dependency_count; ++index) {
        const UmiApplicationDependency *dependency = &dependencies[index];
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (dependency->source_application_id == NULL ||
            dependency->target_id == NULL) {
            add_finding(out_report, UMI_APPLICATION_AUDIT_ERROR,
                        "UMI-APP-006", "dependency",
                        "Dependency identity is incomplete.");
        } else /* Apply this branch only when its contract condition is satisfied. */ if (dependency->kind == UMI_APPLICATION_DEPENDENCY_APPLICATION) {
            add_finding(out_report, UMI_APPLICATION_AUDIT_ERROR,
                        "UMI-APP-007", dependency->source_application_id,
                        "Direct dependency on application '%s' is forbidden; request a Framework capability instead.",
                        dependency->target_id);
        } else /* Apply this branch only when its contract condition is satisfied. */ if (dependency->kind !=
                   UMI_APPLICATION_DEPENDENCY_FRAMEWORK_CAPABILITY ||
                   umi_framework_capability_catalogue_find(
                       dependency->target_id) == NULL) {
            add_finding(out_report, UMI_APPLICATION_AUDIT_ERROR,
                        "UMI-APP-008", dependency->source_application_id,
                        "Dependency target '%s' is not a registered Framework capability.",
                        dependency->target_id);
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (out_report->error_count == 0U) {
        add_finding(out_report, UMI_APPLICATION_AUDIT_INFORMATION,
                    "UMI-APP-000", "portfolio",
                    "All application boundaries resolve through Framework-owned capabilities and component domains.");
    }
    out_report->passed = out_report->error_count == 0U;
    return UMI_STATUS_OK;
}

/*
 * Provide the application portfolio audit operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_portfolio_audit(
    UmiApplicationAuditReport *out_report)
{
    const UmiApplicationDefinition *
        applications[UMI_APPLICATION_AUDIT_MAX_APPLICATIONS];
    size_t index;
    const size_t count = umi_application_portfolio_count();
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_report == NULL ||
        count > UMI_APPLICATION_AUDIT_MAX_APPLICATIONS)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index)
        applications[index] = umi_application_portfolio_at(index);
    return umi_application_boundary_audit(applications, count, NULL, 0U,
                                          out_report);
}

/*
 * Provide the application audit severity text operation used by this module and its client
 * applications.
 */
const char *umi_application_audit_severity_text(
    UmiApplicationAuditSeverity severity)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (severity) {
        case UMI_APPLICATION_AUDIT_INFORMATION: return "information";
        case UMI_APPLICATION_AUDIT_WARNING: return "warning";
        case UMI_APPLICATION_AUDIT_ERROR: return "error";
        default: return "unknown";
    }
}
