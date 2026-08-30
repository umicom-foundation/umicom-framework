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
typedef struct UmiProfileScope { char name[UMI_PROFILE_NAME_CAPACITY]; uint64_t started_at_ns; } UmiProfileScope;
typedef struct UmiProfileSample { char name[UMI_PROFILE_NAME_CAPACITY]; uint64_t count; uint64_t total_ns; uint64_t minimum_ns; uint64_t maximum_ns; } UmiProfileSample;
typedef struct UmiProfiler UmiProfiler;
UmiStatus umi_profiler_create(UmiProfiler **out_profiler);
void umi_profiler_destroy(UmiProfiler *profiler);
UmiStatus umi_profiler_begin(const char *name, uint64_t now_ns, UmiProfileScope *out_scope);
UmiStatus umi_profiler_end(UmiProfiler *profiler, const UmiProfileScope *scope, uint64_t now_ns);
size_t umi_profiler_count(const UmiProfiler *profiler);
UmiStatus umi_profiler_at(const UmiProfiler *profiler, size_t index, UmiProfileSample *out_sample);
#ifdef __cplusplus
}
#endif

#endif
