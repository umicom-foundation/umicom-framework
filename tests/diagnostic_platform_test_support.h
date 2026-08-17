/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/diagnostic_platform_test_support.h
 *
 * PURPOSE:
 *   Share deterministic construction helpers across the focused professional
 *   diagnostics, Problems and SARIF platform tests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_DIAGNOSTIC_PLATFORM_SUPPORT_H
#define UMICOM_TEST_DIAGNOSTIC_PLATFORM_SUPPORT_H

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/diagnostics/diagnostics.h"

static inline void test_copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    int written;
    assert(destination != NULL);
    assert(capacity > 0U);
    written = snprintf(destination, capacity, "%s", source != NULL ? source : "");
    assert(written >= 0);
    assert((size_t)written < capacity);
}

static inline UmiDiagnosticSnapshot test_diagnostic(
    const char *id,
    UmiDiagnosticSeverity severity,
    UmiDiagnosticKind kind,
    const char *source,
    const char *code,
    const char *message,
    const char *uri,
    uint32_t line)
{
    UmiDiagnosticSnapshot diagnostic;
    assert(umi_diagnostic_snapshot_init(&diagnostic, id, severity, kind,
                                        source, message) == UMI_STATUS_OK);
    test_copy_text(diagnostic.code, sizeof(diagnostic.code), code);
    test_copy_text(diagnostic.uri, sizeof(diagnostic.uri), uri);
    test_copy_text(diagnostic.owner, sizeof(diagnostic.owner), "studio");
    test_copy_text(diagnostic.tags, sizeof(diagnostic.tags), "quality;native");
    test_copy_text(diagnostic.detail, sizeof(diagnostic.detail),
                   "Detailed diagnostic evidence.");
    diagnostic.line = line;
    diagnostic.column = 3U;
    diagnostic.end_line = line;
    diagnostic.end_column = 12U;
    diagnostic.timestamp_ns = UINT64_C(1000000) + (uint64_t)line;
    return diagnostic;
}

static inline UmiDiagnosticProviderFinding test_finding(
    const char *provider_id,
    const char *diagnostic_id,
    UmiDiagnosticSeverity severity,
    const char *code,
    const char *message,
    const char *uri,
    uint32_t line,
    uint64_t run_id,
    int fixable)
{
    UmiDiagnosticSnapshot diagnostic = test_diagnostic(
        diagnostic_id, severity, UMI_DIAGNOSTIC_KIND_COMPILER,
        provider_id, code, message, uri, line);
    UmiDiagnosticProviderFinding finding;
    assert(umi_diagnostic_provider_finding_init(&finding, provider_id,
                                                &diagnostic) == UMI_STATUS_OK);
    test_copy_text(finding.rule_name, sizeof(finding.rule_name), code);
    test_copy_text(finding.help_uri, sizeof(finding.help_uri),
                   "https://umicom.org/diagnostics/rule");
    test_copy_text(finding.logical_location,
                   sizeof(finding.logical_location), "main");
    test_copy_text(finding.snippet, sizeof(finding.snippet),
                   "int value = unsafe();");
    finding.run_id = run_id;
    finding.source_revision = 7U;
    if (fixable) {
        finding.has_fix = 1;
        test_copy_text(finding.fix_description,
                       sizeof(finding.fix_description), "Use safe_call");
        test_copy_text(finding.replacement_text,
                       sizeof(finding.replacement_text), "safe_call()");
        finding.replacement_start_line = line;
        finding.replacement_start_column = 13U;
        finding.replacement_end_line = line;
        finding.replacement_end_column = 21U;
    }
    finding.fingerprint = umi_diagnostic_provider_fingerprint(&finding);
    finding.content_fingerprint =
        umi_diagnostic_provider_content_fingerprint(&finding);
    assert(umi_diagnostic_provider_finding_validate(&finding) == UMI_STATUS_OK);
    return finding;
}

static inline UmiDiagnosticProviderDescriptor test_provider(const char *id,
                                                     int32_t priority)
{
    UmiDiagnosticProviderDescriptor provider;
    (void)memset(&provider, 0, sizeof(provider));
    provider.struct_size = (uint32_t)sizeof(provider);
    provider.api_version = UMI_DIAGNOSTIC_PROVIDER_API_VERSION;
    test_copy_text(provider.id, sizeof(provider.id), id);
    test_copy_text(provider.label, sizeof(provider.label), "Umicom Analyzer");
    test_copy_text(provider.tool_name, sizeof(provider.tool_name), "umi-analyzer");
    test_copy_text(provider.tool_version, sizeof(provider.tool_version), "1.0");
    test_copy_text(provider.information_uri, sizeof(provider.information_uri),
                   "https://umicom.org/analyzer");
    provider.category = UMI_DIAGNOSTIC_PROVIDER_LINTER;
    provider.default_kind = UMI_DIAGNOSTIC_KIND_COMPILER;
    provider.priority = priority;
    provider.enabled = 1;
    provider.incremental = 1;
    provider.supports_fixes = 1;
    provider.supports_sarif = 1;
    assert(umi_diagnostic_provider_descriptor_validate(&provider) ==
           UMI_STATUS_OK);
    return provider;
}

#endif /* UMICOM_TEST_DIAGNOSTIC_PLATFORM_SUPPORT_H */
