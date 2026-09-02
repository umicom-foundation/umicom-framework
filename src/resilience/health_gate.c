/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/resilience/health_gate.c
 *
 * PURPOSE:
 *   Implement health-component requirements and inspectable denial reasons.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/resilience/health_gate.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
typedef struct UmiHealthRequirement { char component_id[UMI_HEALTH_COMPONENT_CAPACITY]; UmiHealthState minimum_state; } UmiHealthRequirement;
struct UmiHealthGate { UmiHealthRequirement items[UMI_HEALTH_GATE_MAX]; size_t count; };
/*
 * Initialise health gate from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_health_gate_create(UmiHealthGate **out_gate)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (out_gate == NULL) return UMI_STATUS_INVALID_ARGUMENT; *out_gate = (UmiHealthGate *)calloc(1U, sizeof(UmiHealthGate)); return *out_gate != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY; }
/* Release or reset state held by health gate so the same storage can be reused safely. */
void umi_health_gate_destroy(UmiHealthGate *gate) { free(gate); }
/*
 * Provide the health gate require operation used by this module and its client
 * applications.
 */
UmiStatus umi_health_gate_require(UmiHealthGate *gate, const char *component_id, UmiHealthState minimum_state)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (gate == NULL || component_id == NULL || component_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < gate->count; ++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(gate->items[i].component_id, component_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (gate->count >= UMI_HEALTH_GATE_MAX || strlen(component_id) >= UMI_HEALTH_COMPONENT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)snprintf(gate->items[gate->count].component_id, UMI_HEALTH_COMPONENT_CAPACITY, "%s", component_id); gate->items[gate->count].minimum_state = minimum_state; ++gate->count; return UMI_STATUS_OK;
}
/*
 * Provide the health gate evaluate operation used by this module and its client
 * applications.
 */
UmiStatus umi_health_gate_evaluate(const UmiHealthGate *gate, const UmiHealthRegistry *registry, char *out_reason, size_t reason_capacity)
{
    size_t i; UmiHealthSnapshot snapshot; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (gate == NULL || registry == NULL || out_reason == NULL || reason_capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    out_reason[0] = '\0';
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < gate->count; ++i) {
        status = umi_health_registry_get(registry, gate->items[i].component_id, &snapshot);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) { (void)snprintf(out_reason, reason_capacity, "missing health record: %s", gate->items[i].component_id); return UMI_STATUS_UNAVAILABLE; }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (snapshot.state != UMI_HEALTH_READY && snapshot.state != gate->items[i].minimum_state) { (void)snprintf(out_reason, reason_capacity, "%s is %s", snapshot.component_id, umi_health_state_text(snapshot.state)); return UMI_STATUS_UNAVAILABLE; }
    }
    (void)snprintf(out_reason, reason_capacity, "all required components are healthy"); return UMI_STATUS_OK;
}
/* Return the number of records represented by health gate without changing their state. */
size_t umi_health_gate_count(const UmiHealthGate *gate) { return gate != NULL ? gate->count : 0U; }
