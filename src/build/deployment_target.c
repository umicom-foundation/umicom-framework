/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/deployment_target.c
 *
 * PURPOSE:
 *   Implement bounded provider-neutral deployment-target registration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/build/deployment_target.h"

#include <stdlib.h>
#include <string.h>

struct UmiBuildDeploymentTargetRegistry {
    UmiBuildDeploymentTargetSnapshot targets[UMI_BUILD_DEPLOYMENT_TARGET_CAPACITY];
    size_t count;
    uint64_t revision;
};

static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

static void write_message(char *destination, size_t capacity,
                          const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

static size_t find_index(const UmiBuildDeploymentTargetRegistry *registry,
                         const char *target_id)
{
    size_t index;
    if (registry == NULL || target_id == NULL) return SIZE_MAX;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->targets[index].target_id, target_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

void umi_build_deployment_target_init(
    UmiBuildDeploymentTargetSnapshot *target,
    const char *target_id,
    const char *name,
    UmiBuildDeploymentTargetKind kind)
{
    if (target == NULL) return;
    (void)memset(target, 0, sizeof(*target));
    target->structure_size = (uint32_t)sizeof(*target);
    target->api_version = UMI_BUILD_DEPLOYMENT_TARGET_API_VERSION;
    if (target_id != NULL)
        (void)copy_text(target->target_id, sizeof(target->target_id), target_id);
    if (name != NULL)
        (void)copy_text(target->name, sizeof(target->name), name);
    target->kind = kind;
    target->strategy = UMI_BUILD_DEPLOYMENT_COPY;
    target->flags = UMI_BUILD_DEPLOYMENT_TARGET_ENABLED;
    target->timeout_ms = 300000U;
    target->health_timeout_ms = 30000U;
    target->maximum_parallel = 1U;
    target->revision = 1U;
}

UmiStatus umi_build_deployment_target_validate(
    const UmiBuildDeploymentTargetSnapshot *target,
    char *out_message,
    size_t message_capacity)
{
    if (target == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (target->target_id[0] == '\0' || target->name[0] == '\0' ||
        target->destination[0] == '\0') {
        write_message(out_message, message_capacity,
                      "Target id, name and destination are required.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (target->kind < UMI_BUILD_DEPLOYMENT_LOCAL_DIRECTORY ||
        target->kind > UMI_BUILD_DEPLOYMENT_PACKAGE_REPOSITORY ||
        target->strategy < UMI_BUILD_DEPLOYMENT_COPY ||
        target->strategy > UMI_BUILD_DEPLOYMENT_IN_PLACE) {
        write_message(out_message, message_capacity,
                      "Deployment target kind or strategy is invalid.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((target->flags &
         ~(UmiBuildDeploymentTargetFlags)
             UMI_BUILD_DEPLOYMENT_TARGET_ALL_FLAGS) != 0U ||
        target->maximum_parallel == 0U) {
        write_message(out_message, message_capacity,
                      "Deployment target flags or parallelism are invalid.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((target->flags & UMI_BUILD_DEPLOYMENT_TARGET_HEALTH_CHECK) != 0U &&
        target->health_uri[0] == '\0') {
        write_message(out_message, message_capacity,
                      "Health-enabled targets require a health URI.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((target->flags & UMI_BUILD_DEPLOYMENT_TARGET_REMOTE) != 0U &&
        target->endpoint[0] == '\0') {
        write_message(out_message, message_capacity,
                      "Remote targets require an endpoint.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    write_message(out_message, message_capacity,
                  "Deployment target is valid.");
    return UMI_STATUS_OK;
}

UmiStatus umi_build_deployment_target_registry_create(
    UmiBuildDeploymentTargetRegistry **out_registry)
{
    UmiBuildDeploymentTargetRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiBuildDeploymentTargetRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_build_deployment_target_registry_destroy(
    UmiBuildDeploymentTargetRegistry *registry)
{
    free(registry);
}

void umi_build_deployment_target_registry_clear(
    UmiBuildDeploymentTargetRegistry *registry)
{
    if (registry == NULL) return;
    (void)memset(registry->targets, 0, sizeof(registry->targets));
    registry->count = 0U;
    registry->revision += 1U;
}

UmiStatus umi_build_deployment_target_registry_upsert(
    UmiBuildDeploymentTargetRegistry *registry,
    const UmiBuildDeploymentTargetSnapshot *target)
{
    UmiBuildDeploymentTargetSnapshot copy;
    size_t index;
    char message[256];
    UmiStatus status;
    if (registry == NULL || target == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_deployment_target_validate(
        target, message, sizeof(message));
    if (status != UMI_STATUS_OK) return status;
    copy = *target;
    copy.structure_size = (uint32_t)sizeof(copy);
    copy.api_version = UMI_BUILD_DEPLOYMENT_TARGET_API_VERSION;
    if (copy.revision == 0U) copy.revision = 1U;
    index = find_index(registry, target->target_id);
    if (index == SIZE_MAX) {
        if (registry->count >= UMI_BUILD_DEPLOYMENT_TARGET_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        registry->targets[registry->count++] = copy;
    } else {
        copy.revision = registry->targets[index].revision + 1U;
        registry->targets[index] = copy;
    }
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_deployment_target_registry_remove(
    UmiBuildDeploymentTargetRegistry *registry,
    const char *target_id)
{
    size_t index;
    if (registry == NULL || target_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(registry, target_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < registry->count) {
        (void)memmove(&registry->targets[index], &registry->targets[index + 1U],
                      (registry->count - index - 1U) *
                          sizeof(registry->targets[0]));
    }
    registry->count -= 1U;
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_deployment_target_registry_find(
    const UmiBuildDeploymentTargetRegistry *registry,
    const char *target_id,
    UmiBuildDeploymentTargetSnapshot *out_target)
{
    size_t index;
    if (registry == NULL || target_id == NULL || out_target == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(registry, target_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_target = registry->targets[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_build_deployment_target_registry_at(
    const UmiBuildDeploymentTargetRegistry *registry,
    size_t position,
    UmiBuildDeploymentTargetSnapshot *out_target)
{
    if (registry == NULL || out_target == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (position >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_target = registry->targets[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_build_deployment_target_registry_snapshot(
    const UmiBuildDeploymentTargetRegistry *registry,
    UmiBuildDeploymentTargetRegistrySnapshot *out_snapshot)
{
    size_t index;
    if (registry == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_BUILD_DEPLOYMENT_TARGET_API_VERSION;
    out_snapshot->target_count = registry->count;
    out_snapshot->revision = registry->revision;
    for (index = 0U; index < registry->count; ++index) {
        UmiBuildDeploymentTargetFlags flags = registry->targets[index].flags;
        if ((flags & UMI_BUILD_DEPLOYMENT_TARGET_ENABLED) != 0U)
            out_snapshot->enabled_count += 1U;
        if ((flags & UMI_BUILD_DEPLOYMENT_TARGET_PRODUCTION) != 0U)
            out_snapshot->production_count += 1U;
        if ((flags & UMI_BUILD_DEPLOYMENT_TARGET_REMOTE) != 0U)
            out_snapshot->remote_count += 1U;
        if ((flags & UMI_BUILD_DEPLOYMENT_TARGET_REQUIRES_APPROVAL) != 0U)
            out_snapshot->approval_count += 1U;
        if ((flags & UMI_BUILD_DEPLOYMENT_TARGET_SUPPORTS_ROLLBACK) != 0U)
            out_snapshot->rollback_count += 1U;
    }
    return UMI_STATUS_OK;
}

size_t umi_build_deployment_target_registry_count(
    const UmiBuildDeploymentTargetRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_build_deployment_target_registry_revision(
    const UmiBuildDeploymentTargetRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}

const char *umi_build_deployment_target_kind_text(
    UmiBuildDeploymentTargetKind kind)
{
    switch (kind) {
    case UMI_BUILD_DEPLOYMENT_LOCAL_DIRECTORY: return "Local directory";
    case UMI_BUILD_DEPLOYMENT_LOCAL_PROCESS: return "Local process";
    case UMI_BUILD_DEPLOYMENT_SERVICE: return "Service";
    case UMI_BUILD_DEPLOYMENT_ARCHIVE: return "Archive";
    case UMI_BUILD_DEPLOYMENT_CONTAINER: return "Container";
    case UMI_BUILD_DEPLOYMENT_REMOTE_HOST: return "Remote host";
    case UMI_BUILD_DEPLOYMENT_STATIC_SITE: return "Static site";
    case UMI_BUILD_DEPLOYMENT_PACKAGE_REPOSITORY: return "Package repository";
    default: return "Unknown";
    }
}

const char *umi_build_deployment_strategy_text(
    UmiBuildDeploymentStrategy strategy)
{
    switch (strategy) {
    case UMI_BUILD_DEPLOYMENT_COPY: return "Copy";
    case UMI_BUILD_DEPLOYMENT_REPLACE: return "Replace";
    case UMI_BUILD_DEPLOYMENT_ROLLING: return "Rolling";
    case UMI_BUILD_DEPLOYMENT_BLUE_GREEN: return "Blue-green";
    case UMI_BUILD_DEPLOYMENT_CANARY: return "Canary";
    case UMI_BUILD_DEPLOYMENT_IN_PLACE: return "In-place";
    default: return "Unknown";
    }
}
