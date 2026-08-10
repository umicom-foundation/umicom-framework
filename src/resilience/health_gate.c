/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/resilience/health_gate.c
 *
 * PURPOSE:
 *   Implement health-component requirements and inspectable denial reasons.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/resilience/health_gate.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
typedef struct UmiHealthRequirement { char component_id[UMI_HEALTH_COMPONENT_CAPACITY]; UmiHealthState minimum_state; } UmiHealthRequirement;
struct UmiHealthGate { UmiHealthRequirement items[UMI_HEALTH_GATE_MAX]; size_t count; };
UmiStatus umi_health_gate_create(UmiHealthGate **out_gate)
{ if (out_gate == NULL) return UMI_STATUS_INVALID_ARGUMENT; *out_gate = (UmiHealthGate *)calloc(1U, sizeof(UmiHealthGate)); return *out_gate != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY; }
void umi_health_gate_destroy(UmiHealthGate *gate) { free(gate); }
UmiStatus umi_health_gate_require(UmiHealthGate *gate, const char *component_id, UmiHealthState minimum_state)
{
    size_t i;
    if (gate == NULL || component_id == NULL || component_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < gate->count; ++i) if (strcmp(gate->items[i].component_id, component_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (gate->count >= UMI_HEALTH_GATE_MAX || strlen(component_id) >= UMI_HEALTH_COMPONENT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)snprintf(gate->items[gate->count].component_id, UMI_HEALTH_COMPONENT_CAPACITY, "%s", component_id); gate->items[gate->count].minimum_state = minimum_state; ++gate->count; return UMI_STATUS_OK;
}
UmiStatus umi_health_gate_evaluate(const UmiHealthGate *gate, const UmiHealthRegistry *registry, char *out_reason, size_t reason_capacity)
{
    size_t i; UmiHealthSnapshot snapshot; UmiStatus status;
    if (gate == NULL || registry == NULL || out_reason == NULL || reason_capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    out_reason[0] = '\0';
    for (i = 0U; i < gate->count; ++i) {
        status = umi_health_registry_get(registry, gate->items[i].component_id, &snapshot);
        if (status != UMI_STATUS_OK) { (void)snprintf(out_reason, reason_capacity, "missing health record: %s", gate->items[i].component_id); return UMI_STATUS_UNAVAILABLE; }
        if (snapshot.state != UMI_HEALTH_READY && snapshot.state != gate->items[i].minimum_state) { (void)snprintf(out_reason, reason_capacity, "%s is %s", snapshot.component_id, umi_health_state_text(snapshot.state)); return UMI_STATUS_UNAVAILABLE; }
    }
    (void)snprintf(out_reason, reason_capacity, "all required components are healthy"); return UMI_STATUS_OK;
}
size_t umi_health_gate_count(const UmiHealthGate *gate) { return gate != NULL ? gate->count : 0U; }
