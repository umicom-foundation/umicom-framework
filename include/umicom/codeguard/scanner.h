/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/scanner.h
 *
 * PURPOSE:
 *   Coordinate directory traversal, per-file security analysis, architecture
 *   checks and duplicates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#ifndef UMICOM_CODEGUARD_SCANNER_H
#define UMICOM_CODEGUARD_SCANNER_H
#include "umicom/base/status.h"
#include "umicom/codeguard/config.h"
#include "umicom/codeguard/result.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the codeguard scan operation used by this module and its client applications.
 */
/* Recursive scans apply generated/vendor and rule-source exclusions to
 * directory components beneath config.root. An ancestor outside that root
 * does not exclude its contents. Template/workspace payloads are scanned but
 * omitted from duplicate comparisons. Path normalization and platform case
 * semantics come from Framework. Direct-file scanning bypasses this workspace
 * selection policy. Traversal still does not follow symbolic links; lexical
 * containment is not a filesystem sandbox or protection against link races. */
/* scan_generated opts into build and _CPack_Packages children; other
 * exclusions remain. Language flags apply to every source-analysis entry
 * point and duplicate admission. scan_source_names remains an independent
 * workspace filename policy, even for a disabled source language. */
/* Every profile-driven file read enforces max_file_bytes through native
 * regular-file input. Zero admits empty files only; final links/reparse
 * points are refused, and embedded NUL text is INVALID_ARGUMENT. Relative
 * input is resolved once per visit; diagnostics keep its original spelling.
 * Architecture and duplicate passes reopen and recheck their budgets, so
 * successful scanning is not an immutable multi-pass filesystem snapshot. */
UmiStatus umi_codeguard_scan(const UmiCodeGuardConfig *config, UmiCodeGuardResult *result);
/**
 * Provide the codeguard scan file operation used by this module and its client
 * applications.
 */
UmiStatus umi_codeguard_scan_file(const UmiCodeGuardConfig *config, const char *path,
                                  UmiCodeGuardResult *result);

/* Cooperative counterparts retain the same append-only low-level result
 * contract: failure/cancellation can leave partial findings in result. Use
 * UmiCodeGuardService when only complete reports may replace prior evidence.
 * NULL token selects ordinary scanning. Keep the token alive until return;
 * request it from another thread, but do not reset/destroy it during a scan.
 * Checks occur during traversal, before/after each read, and around analysis
 * phases. An OS call, rule, architecture pass or duplicate pass already in
 * progress is not interrupted. No worker thread or filesystem snapshot is
 * created. An existing operation error takes precedence over a later request. */
UmiStatus umi_codeguard_scan_cancellable(const UmiCodeGuardConfig *config,
    UmiCodeGuardResult *result, const UmiCancellationToken *cancellation);
UmiStatus umi_codeguard_scan_file_cancellable(const UmiCodeGuardConfig *config,
    const char *path, UmiCodeGuardResult *result,
    const UmiCancellationToken *cancellation);
#ifdef __cplusplus
}
#endif
#endif
