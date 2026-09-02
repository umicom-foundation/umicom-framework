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
/* Find sample while leaving the underlying catalogue or model owned by this module. */
static size_t sample_find(const UmiProfiler *profiler, const char *name)
{ size_t i; /* Visit each bounded item once so every record receives the same rule. */ for (i = 0U; i < profiler->count; ++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(profiler->samples[i].name, name) == 0) return i; return SIZE_MAX; }
/*
 * Initialise profiler from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_profiler_create(UmiProfiler **out_profiler)
{
    UmiProfiler *profiler;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_profiler == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_profiler = NULL; profiler = (UmiProfiler *)calloc(1U, sizeof(*profiler)); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (profiler == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_diagnostic_lock_init(&profiler->lock);
    *out_profiler = profiler; return UMI_STATUS_OK;
}
/* Release or reset state held by profiler so the same storage can be reused safely. */
void umi_profiler_destroy(UmiProfiler *profiler) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (profiler != NULL) { free(profiler); } }
/* Provide the profiler begin operation used by this module and its client applications. */
UmiStatus umi_profiler_begin(const char *name, uint64_t now_ns, UmiProfileScope *out_scope)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (name == NULL || name[0] == '\0' || out_scope == NULL || strlen(name) >= UMI_PROFILE_NAME_CAPACITY) return UMI_STATUS_INVALID_ARGUMENT;
    (void)snprintf(out_scope->name, sizeof(out_scope->name), "%s", name); out_scope->started_at_ns = now_ns; return UMI_STATUS_OK;
}
/* Provide the profiler end operation used by this module and its client applications. */
UmiStatus umi_profiler_end(UmiProfiler *profiler, const UmiProfileScope *scope, uint64_t now_ns)
{
    size_t index; uint64_t elapsed; UmiProfileSample *sample;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profiler == NULL || scope == NULL || scope->name[0] == '\0' || now_ns < scope->started_at_ns) return UMI_STATUS_INVALID_ARGUMENT;
    elapsed = now_ns - scope->started_at_ns; umi_diagnostic_lock_acquire(&profiler->lock);
    index = sample_find(profiler, scope->name);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) { /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (profiler->count >= UMI_PROFILE_SAMPLE_MAX) { umi_diagnostic_lock_release(&profiler->lock); return UMI_STATUS_CAPACITY_EXCEEDED; } index = profiler->count++; sample = &profiler->samples[index]; (void)memset(sample, 0, sizeof(*sample)); (void)snprintf(sample->name, sizeof(sample->name), "%s", scope->name); sample->minimum_ns = elapsed; }
    sample = &profiler->samples[index]; ++sample->count; sample->total_ns += elapsed; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (elapsed < sample->minimum_ns) sample->minimum_ns = elapsed; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (elapsed > sample->maximum_ns) sample->maximum_ns = elapsed;
    umi_diagnostic_lock_release(&profiler->lock); return UMI_STATUS_OK;
}
/* Return the number of records represented by profiler without changing their state. */
size_t umi_profiler_count(const UmiProfiler *profiler)
{ size_t n = 0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (profiler != NULL) { umi_diagnostic_lock_acquire(&profiler->lock); n = profiler->count; umi_diagnostic_lock_release(&profiler->lock); } return n; }
/* Find profiler while leaving the underlying catalogue or model owned by this module. */
UmiStatus umi_profiler_at(const UmiProfiler *profiler, size_t index, UmiProfileSample *out_sample)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profiler == NULL || out_sample == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&profiler->lock);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= profiler->count) { umi_diagnostic_lock_release(&profiler->lock); return UMI_STATUS_NOT_FOUND; }
    *out_sample = profiler->samples[index]; umi_diagnostic_lock_release(&profiler->lock); return UMI_STATUS_OK;
}
