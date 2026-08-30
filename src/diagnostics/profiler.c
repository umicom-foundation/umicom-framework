/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/profiler.c
 *
 * PURPOSE:
 *   Aggregate profile-scope durations into count, total, minimum and maximum
 *   samples.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/profiler.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "diagnostic_lock.h"
struct UmiProfiler { UmiProfileSample samples[UMI_PROFILE_SAMPLE_MAX]; size_t count; UmiDiagnosticLock lock; };
static size_t sample_find(const UmiProfiler *profiler, const char *name)
{ size_t i; for (i = 0U; i < profiler->count; ++i) if (strcmp(profiler->samples[i].name, name) == 0) return i; return SIZE_MAX; }
UmiStatus umi_profiler_create(UmiProfiler **out_profiler)
{
    UmiProfiler *profiler;
    if (out_profiler == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_profiler = NULL; profiler = (UmiProfiler *)calloc(1U, sizeof(*profiler)); if (profiler == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_diagnostic_lock_init(&profiler->lock);
    *out_profiler = profiler; return UMI_STATUS_OK;
}
void umi_profiler_destroy(UmiProfiler *profiler) { if (profiler != NULL) { free(profiler); } }
UmiStatus umi_profiler_begin(const char *name, uint64_t now_ns, UmiProfileScope *out_scope)
{
    if (name == NULL || name[0] == '\0' || out_scope == NULL || strlen(name) >= UMI_PROFILE_NAME_CAPACITY) return UMI_STATUS_INVALID_ARGUMENT;
    (void)snprintf(out_scope->name, sizeof(out_scope->name), "%s", name); out_scope->started_at_ns = now_ns; return UMI_STATUS_OK;
}
UmiStatus umi_profiler_end(UmiProfiler *profiler, const UmiProfileScope *scope, uint64_t now_ns)
{
    size_t index; uint64_t elapsed; UmiProfileSample *sample;
    if (profiler == NULL || scope == NULL || scope->name[0] == '\0' || now_ns < scope->started_at_ns) return UMI_STATUS_INVALID_ARGUMENT;
    elapsed = now_ns - scope->started_at_ns; umi_diagnostic_lock_acquire(&profiler->lock);
    index = sample_find(profiler, scope->name);
    if (index == SIZE_MAX) { if (profiler->count >= UMI_PROFILE_SAMPLE_MAX) { umi_diagnostic_lock_release(&profiler->lock); return UMI_STATUS_CAPACITY_EXCEEDED; } index = profiler->count++; sample = &profiler->samples[index]; (void)memset(sample, 0, sizeof(*sample)); (void)snprintf(sample->name, sizeof(sample->name), "%s", scope->name); sample->minimum_ns = elapsed; }
    sample = &profiler->samples[index]; ++sample->count; sample->total_ns += elapsed; if (elapsed < sample->minimum_ns) sample->minimum_ns = elapsed; if (elapsed > sample->maximum_ns) sample->maximum_ns = elapsed;
    umi_diagnostic_lock_release(&profiler->lock); return UMI_STATUS_OK;
}
size_t umi_profiler_count(const UmiProfiler *profiler)
{ size_t n = 0U; if (profiler != NULL) { umi_diagnostic_lock_acquire(&profiler->lock); n = profiler->count; umi_diagnostic_lock_release(&profiler->lock); } return n; }
UmiStatus umi_profiler_at(const UmiProfiler *profiler, size_t index, UmiProfileSample *out_sample)
{
    if (profiler == NULL || out_sample == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&profiler->lock);
    if (index >= profiler->count) { umi_diagnostic_lock_release(&profiler->lock); return UMI_STATUS_NOT_FOUND; }
    *out_sample = profiler->samples[index]; umi_diagnostic_lock_release(&profiler->lock); return UMI_STATUS_OK;
}
