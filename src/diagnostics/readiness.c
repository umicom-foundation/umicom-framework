/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/readiness.c
 *
 * PURPOSE:
 *   Implement replaceable readiness checks and aggregate readiness evaluation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/readiness.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "diagnostic_lock.h"
struct UmiReadinessRegistry { UmiReadinessCheck checks[UMI_READINESS_MAX]; size_t count; UmiDiagnosticLock lock; };
/* Find readiness while leaving the underlying catalogue or model owned by this module. */
static size_t readiness_find(const UmiReadinessRegistry *registry, const char *id)
{ size_t i; /* Visit each bounded item once so every record receives the same rule. */ for (i = 0U; i < registry->count; ++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(registry->checks[i].check_id, id) == 0) return i; return SIZE_MAX; }
/*
 * Initialise readiness registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_readiness_registry_create(UmiReadinessRegistry **out_registry)
{
    UmiReadinessRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL; registry = (UmiReadinessRegistry *)calloc(1U, sizeof(*registry)); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_diagnostic_lock_init(&registry->lock);
    *out_registry = registry; return UMI_STATUS_OK;
}
/*
 * Release or reset state held by readiness registry so the same storage can be reused
 * safely.
 */
void umi_readiness_registry_destroy(UmiReadinessRegistry *registry) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (registry != NULL) { free(registry); } }
/*
 * Provide the readiness registry update operation used by this module and its client
 * applications.
 */
UmiStatus umi_readiness_registry_update(UmiReadinessRegistry *registry, const char *check_id, int ready, const char *message, uint64_t now_ns)
{
    size_t index; UmiReadinessCheck *check;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || check_id == NULL || check_id[0] == '\0' || message == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(check_id) >= UMI_READINESS_ID_CAPACITY || strlen(message) >= UMI_READINESS_MESSAGE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    umi_diagnostic_lock_acquire(&registry->lock);
    index = readiness_find(registry, check_id); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (index == SIZE_MAX) { /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (registry->count >= UMI_READINESS_MAX) { umi_diagnostic_lock_release(&registry->lock); return UMI_STATUS_CAPACITY_EXCEEDED; } index = registry->count++; }
    check = &registry->checks[index]; (void)memset(check, 0, sizeof(*check)); (void)snprintf(check->check_id, sizeof(check->check_id), "%s", check_id); (void)snprintf(check->message, sizeof(check->message), "%s", message); check->ready = ready != 0; check->updated_at_ns = now_ns;
    umi_diagnostic_lock_release(&registry->lock); return UMI_STATUS_OK;
}
/*
 * Provide the readiness registry ready operation used by this module and its client
 * applications.
 */
int umi_readiness_registry_ready(const UmiReadinessRegistry *registry)
{
    size_t i; int ready = 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return 0;
    umi_diagnostic_lock_acquire(&registry->lock);
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < registry->count; ++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (!registry->checks[i].ready) { ready = 0; break; }
    umi_diagnostic_lock_release(&registry->lock); return ready;
}
/*
 * Return the number of records represented by readiness registry without changing their
 * state.
 */
size_t umi_readiness_registry_count(const UmiReadinessRegistry *registry)
{ size_t n = 0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (registry != NULL) { umi_diagnostic_lock_acquire(&registry->lock); n = registry->count; umi_diagnostic_lock_release(&registry->lock); } return n; }
/*
 * Find readiness registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_readiness_registry_at(const UmiReadinessRegistry *registry, size_t index, UmiReadinessCheck *out_check)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_check == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&registry->lock);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= registry->count) { umi_diagnostic_lock_release(&registry->lock); return UMI_STATUS_NOT_FOUND; }
    *out_check = registry->checks[index]; umi_diagnostic_lock_release(&registry->lock); return UMI_STATUS_OK;
}
