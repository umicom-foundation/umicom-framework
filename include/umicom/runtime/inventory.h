/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/inventory.h
 *
 * PURPOSE:
 *   Provide reusable, toolkit-independent runtime inventory introspection over
 *   command, service and capability registries.  Applications can validate
 *   composition contracts without relying on unexplained hard-coded totals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_INVENTORY_H
#define UMICOM_RUNTIME_INVENTORY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/runtime/capability_registry.h"
#include "umicom/runtime/command_registry.h"
#include "umicom/runtime/service_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_RUNTIME_INVENTORY_API_VERSION 1U

typedef struct UmiRuntimeInventorySnapshot {
    uint32_t structure_size;
    uint32_t api_version;

    size_t command_count;
    size_t mutating_command_count;
    size_t background_command_count;
    size_t audited_command_count;
    size_t trusted_command_count;

    size_t service_count;
    size_t singleton_service_count;
    size_t owned_service_count;
    size_t thread_safe_service_count;
    size_t replaceable_service_count;

    size_t capability_count;
    size_t singleton_capability_count;
    size_t external_capability_count;
    size_t reload_safe_capability_count;
    size_t optional_capability_count;
} UmiRuntimeInventorySnapshot;

/*
 * Registry arguments are independently optional.  A NULL registry contributes
 * zero items to the snapshot, which makes the API useful to partial/headless
 * compositions without manufacturing placeholder registries.
 */
UmiStatus umi_runtime_inventory_snapshot(
    const UmiCommandRegistry *commands,
    const UmiServiceRegistry *services,
    const UmiCapabilityRegistry *capabilities,
    UmiRuntimeInventorySnapshot *out_snapshot);

int umi_runtime_inventory_has_command(const UmiCommandRegistry *registry,
                                      const char *command_id);
int umi_runtime_inventory_has_service(const UmiServiceRegistry *registry,
                                      const char *service_id);
int umi_runtime_inventory_has_capability(
    const UmiCapabilityRegistry *registry,
    const char *capability_id);

UmiStatus umi_runtime_inventory_count_command_prefix(
    const UmiCommandRegistry *registry,
    const char *prefix,
    size_t *out_count);
UmiStatus umi_runtime_inventory_count_service_prefix(
    const UmiServiceRegistry *registry,
    const char *prefix,
    size_t *out_count);
UmiStatus umi_runtime_inventory_count_capability_prefix(
    const UmiCapabilityRegistry *registry,
    const char *prefix,
    size_t *out_count);

#ifdef __cplusplus
}
#endif

#endif
