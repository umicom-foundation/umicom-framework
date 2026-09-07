/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/native_discovery.h
 * PURPOSE: Reconcile trusted built-in GUI installation evidence into the
 * existing application runtime catalogue, without loading or launching code.
 * AUTHOR: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_NATIVE_DISCOVERY_H
#define UMICOM_APPLICATION_NATIVE_DISCOVERY_H
#include "umicom/application/runtime_catalogue.h"
#ifdef __cplusplus
extern "C" {
#endif

/* A probe returns OK and false for an absent/non-runnable file. Other failures
 * abort the entire scan. It must not mutate the catalogue or reenter its owner. */
typedef UmiStatus (*UmiApplicationNativeProbeFn)(
    const char *absolute_path, bool *out_present, void *context);

typedef struct UmiApplicationNativeDiscoveryConfig {
    uint32_t structure_size;
    const char *executable_root;
    const char *executable_suffix;
    UmiApplicationNativeProbeFn probe;
    void *probe_context;
    uint64_t interval_ms;
} UmiApplicationNativeDiscoveryConfig;

typedef struct UmiApplicationNativeDiscoveryReport {
    size_t scanned_count;
    size_t installed_count;
    size_t changed_count;
    uint64_t revision;
    UmiStatus status;
    bool skipped;
} UmiApplicationNativeDiscoveryReport;

/* Defaults do not enable discovery: the caller supplies a trusted absolute
 * root. Suffix is the native platform suffix; polling defaults to 1000 ms. */
UmiApplicationNativeDiscoveryConfig umi_application_native_discovery_config_default(void);
/* Reject relative/network roots, dot traversal and non-platform suffixes. No PATH,
 * working-directory fallback, recursive search or external package admission. */
UmiStatus umi_application_native_discovery_config_validate(
    const UmiApplicationNativeDiscoveryConfig *config);
/* Check one canonical GUI name in the configured root. File existence is
 * installation evidence, not signature, ABI, startup or feature readiness. */
UmiStatus umi_application_native_discovery_probe(
    const UmiApplicationNativeDiscoveryConfig *config,
    const char *application_id, bool *out_present);
/* Explicit one-time composition policy for already-registered built-in GUI
 * products. Canonical names and enabled/compatible admission are installed;
 * pinning and actual process state remain untouched. No filesystem work. */
UmiStatus umi_application_runtime_catalogue_admit_native_portfolio(
    UmiApplicationRuntimeCatalogue *catalogue);
/* Stage a bounded scan before publishing. Probe failures leave all records
 * and revisions unchanged. Only presence/name changes increment revisions;
 * enabled, compatible, pinning and live process evidence are preserved.
 * All calls occur on the catalogue's owning thread. */
UmiStatus umi_application_runtime_catalogue_discover_native(
    UmiApplicationRuntimeCatalogue *catalogue,
    const UmiApplicationNativeDiscoveryConfig *config,
    UmiApplicationNativeDiscoveryReport *out_report);
#ifdef __cplusplus
}
#endif
#endif
