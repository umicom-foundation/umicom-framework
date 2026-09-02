/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/profiler.h
 *
 * PURPOSE:
 *   Define lightweight timing scopes and aggregate duration samples without
 *   coupling callers to a platform clock.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_PROFILER_H
#define UMICOM_DIAGNOSTICS_PROFILER_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_PROFILE_NAME_CAPACITY 128U
#define UMI_PROFILE_SAMPLE_MAX 1024U
/**
 * Represent the profile scope data shared with callers of this public contract.
 */
typedef struct UmiProfileScope { char name[UMI_PROFILE_NAME_CAPACITY]; uint64_t started_at_ns; } UmiProfileScope;
/**
 * Represent the profile sample data shared with callers of this public contract.
 */
typedef struct UmiProfileSample { char name[UMI_PROFILE_NAME_CAPACITY]; uint64_t count; uint64_t total_ns; uint64_t minimum_ns; uint64_t maximum_ns; } UmiProfileSample;
/**
 * Represent the profiler data shared with callers of this public contract.
 */
typedef struct UmiProfiler UmiProfiler;
/**
 * Initialise profiler from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_profiler_create(UmiProfiler **out_profiler);
/**
 * Release or reset state held by profiler so the same storage can be reused safely.
 */
void umi_profiler_destroy(UmiProfiler *profiler);
/**
 * Provide the profiler begin operation used by this module and its client applications.
 */
UmiStatus umi_profiler_begin(const char *name, uint64_t now_ns, UmiProfileScope *out_scope);
/**
 * Provide the profiler end operation used by this module and its client applications.
 */
UmiStatus umi_profiler_end(UmiProfiler *profiler, const UmiProfileScope *scope, uint64_t now_ns);
/**
 * Return the number of records represented by profiler without changing their state.
 */
size_t umi_profiler_count(const UmiProfiler *profiler);
/**
 * Find profiler while leaving the underlying catalogue or model owned by this module.
 */
UmiStatus umi_profiler_at(const UmiProfiler *profiler, size_t index, UmiProfileSample *out_sample);
#ifdef __cplusplus
}
#endif

#endif
