/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/resilience/supervisor.c
 *
 * PURPOSE:
 *   Implement component supervision records, failure classification and restart
 *   decisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/resilience/supervisor.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "umicom/platform/threading.h"
typedef struct UmiSupervisedComponent { UmiSupervisedSnapshot snapshot; UmiRestartTracker tracker; } UmiSupervisedComponent;
struct UmiResilienceSupervisor { UmiSupervisedComponent items[UMI_RESILIENCE_SUPERVISOR_MAX]; size_t count; UmiMutex *mutex; };
/* Find supervised while leaving the underlying catalogue or model owned by this module. */
static size_t supervised_find(const UmiResilienceSupervisor *s, const char *id)
{ size_t i; /* Visit each bounded item once so every record receives the same rule. */ for (i = 0U; i < s->count; ++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(s->items[i].snapshot.component_id, id) == 0) return i; return SIZE_MAX; }
/*
 * Initialise resilience supervisor from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_resilience_supervisor_create(UmiResilienceSupervisor **out_supervisor)
{
    UmiResilienceSupervisor *s; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_supervisor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_supervisor = NULL; s = (UmiResilienceSupervisor *)calloc(1U, sizeof(*s)); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (s == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&s->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) { free(s); return status; }
    *out_supervisor = s; return UMI_STATUS_OK;
}
/*
 * Release or reset state held by resilience supervisor so the same storage can be reused
 * safely.
 */
void umi_resilience_supervisor_destroy(UmiResilienceSupervisor *supervisor) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (supervisor != NULL) { umi_mutex_destroy(supervisor->mutex); free(supervisor); } }
/*
 * Add resilience supervisor only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_resilience_supervisor_register(UmiResilienceSupervisor *s, const char *component_id, const UmiRestartPolicy *policy, uint64_t now_ns)
{
    UmiSupervisedComponent *c; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (s == NULL || component_id == NULL || component_id[0] == '\0' || strlen(component_id) >= UMI_RESILIENCE_COMPONENT_ID_CAPACITY) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(s->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (supervised_find(s, component_id) != SIZE_MAX) { (void)umi_mutex_unlock(s->mutex); return UMI_STATUS_ALREADY_EXISTS; }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (s->count >= UMI_RESILIENCE_SUPERVISOR_MAX) { (void)umi_mutex_unlock(s->mutex); return UMI_STATUS_CAPACITY_EXCEEDED; }
    c = &s->items[s->count++]; (void)memset(c, 0, sizeof(*c)); (void)snprintf(c->snapshot.component_id, sizeof(c->snapshot.component_id), "%s", component_id); c->snapshot.state = UMI_SUPERVISED_STOPPED; c->snapshot.last_status = UMI_STATUS_OK;
    status = umi_restart_tracker_init(&c->tracker, policy, now_ns); (void)umi_mutex_unlock(s->mutex); return status;
}
/*
 * Provide the resilience supervisor success operation used by this module and its client
 * applications.
 */
UmiStatus umi_resilience_supervisor_success(UmiResilienceSupervisor *s, const char *component_id, uint64_t now_ns)
{
    size_t index; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (s == NULL || component_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(s->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    index = supervised_find(s, component_id); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (index == SIZE_MAX) { (void)umi_mutex_unlock(s->mutex); return UMI_STATUS_NOT_FOUND; }
    s->items[index].snapshot.state = UMI_SUPERVISED_RUNNING; s->items[index].snapshot.last_status = UMI_STATUS_OK; umi_restart_tracker_success(&s->items[index].tracker, now_ns);
    (void)umi_mutex_unlock(s->mutex); return UMI_STATUS_OK;
}
/*
 * Provide the resilience supervisor failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_resilience_supervisor_failure(UmiResilienceSupervisor *s, const char *component_id, UmiStatus failure, uint64_t now_ns, UmiRestartDecision *out_decision)
{
    size_t index; UmiRestartDecision decision = {0}; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (s == NULL || component_id == NULL || out_decision == NULL || failure == UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(s->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    index = supervised_find(s, component_id); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (index == SIZE_MAX) { (void)umi_mutex_unlock(s->mutex); return UMI_STATUS_NOT_FOUND; }
    ++s->items[index].snapshot.failures; s->items[index].snapshot.last_status = failure;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_fault_retryable(failure)) decision = umi_restart_tracker_failure(&s->items[index].tracker, now_ns);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (decision.restart) { ++s->items[index].snapshot.restarts; s->items[index].snapshot.state = UMI_SUPERVISED_DEGRADED; }
    /* Use this fallback path when the earlier condition does not apply. */
    else s->items[index].snapshot.state = UMI_SUPERVISED_FAILED;
    *out_decision = decision; (void)umi_mutex_unlock(s->mutex); return UMI_STATUS_OK;
}
/*
 * Return the number of records represented by resilience supervisor without changing their
 * state.
 */
size_t umi_resilience_supervisor_count(const UmiResilienceSupervisor *s)
{ size_t n = 0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (s != NULL && umi_mutex_lock(s->mutex) == UMI_STATUS_OK) { n = s->count; (void)umi_mutex_unlock(s->mutex); } return n; }
/*
 * Find resilience supervisor while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_resilience_supervisor_at(const UmiResilienceSupervisor *s, size_t index, UmiSupervisedSnapshot *out_snapshot)
{
    UmiStatus status; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (s == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(s->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= s->count) { (void)umi_mutex_unlock(s->mutex); return UMI_STATUS_NOT_FOUND; }
    *out_snapshot = s->items[index].snapshot; (void)umi_mutex_unlock(s->mutex); return UMI_STATUS_OK;
}
