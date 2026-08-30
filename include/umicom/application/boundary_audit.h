/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/boundary_audit.h
 *
 * PURPOSE:
 *   Enforce the Framework-authority rule: applications may consume Framework
 *   capabilities but may never create direct application-to-application links.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_BOUNDARY_AUDIT_H
#define UMICOM_APPLICATION_BOUNDARY_AUDIT_H

#include <stddef.h>

#include "umicom/application/portfolio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_AUDIT_MAX_FINDINGS 128U
#define UMI_APPLICATION_AUDIT_MAX_APPLICATIONS 128U
#define UMI_APPLICATION_AUDIT_TEXT_CAPACITY 256U

typedef enum UmiApplicationDependencyKind {
    UMI_APPLICATION_DEPENDENCY_FRAMEWORK_CAPABILITY = 1,
    UMI_APPLICATION_DEPENDENCY_APPLICATION = 2
} UmiApplicationDependencyKind;

typedef enum UmiApplicationAuditSeverity {
    UMI_APPLICATION_AUDIT_INFORMATION = 1,
    UMI_APPLICATION_AUDIT_WARNING = 2,
    UMI_APPLICATION_AUDIT_ERROR = 3
} UmiApplicationAuditSeverity;

typedef struct UmiApplicationDependency {
    const char *source_application_id;
    UmiApplicationDependencyKind kind;
    const char *target_id;
} UmiApplicationDependency;

typedef struct UmiApplicationAuditFinding {
    UmiApplicationAuditSeverity severity;
    char rule_id[64];
    char subject[128];
    char message[UMI_APPLICATION_AUDIT_TEXT_CAPACITY];
} UmiApplicationAuditFinding;

typedef struct UmiApplicationAuditReport {
    UmiApplicationAuditFinding findings[UMI_APPLICATION_AUDIT_MAX_FINDINGS];
    size_t finding_count;
    size_t information_count;
    size_t warning_count;
    size_t error_count;
    size_t application_count;
    size_t capability_reference_count;
    size_t reusable_component_count;
    size_t component_domain_reference_count;
    int passed;
} UmiApplicationAuditReport;

UmiStatus umi_application_boundary_audit(
    const UmiApplicationDefinition *const *applications,
    size_t application_count,
    const UmiApplicationDependency *dependencies,
    size_t dependency_count,
    UmiApplicationAuditReport *out_report);
UmiStatus umi_application_portfolio_audit(
    UmiApplicationAuditReport *out_report);
const char *umi_application_audit_severity_text(
    UmiApplicationAuditSeverity severity);

#ifdef __cplusplus
}
#endif

#endif
