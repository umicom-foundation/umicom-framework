/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/deployment_target.h
 *
 * PURPOSE:
 *   Define provider-neutral deployment destinations, strategies, trust gates,
 *   health checks and rollback capabilities for local and remote environments.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_DEPLOYMENT_TARGET_H
#define UMICOM_BUILD_DEPLOYMENT_TARGET_H

#include "umicom/base/status.h"
#include "umicom/build/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_DEPLOYMENT_TARGET_API_VERSION 1U
#define UMI_BUILD_DEPLOYMENT_TARGET_CAPACITY 64U

typedef enum UmiBuildDeploymentTargetKind {
    UMI_BUILD_DEPLOYMENT_LOCAL_DIRECTORY = 0,
    UMI_BUILD_DEPLOYMENT_LOCAL_PROCESS = 1,
    UMI_BUILD_DEPLOYMENT_SERVICE = 2,
    UMI_BUILD_DEPLOYMENT_ARCHIVE = 3,
    UMI_BUILD_DEPLOYMENT_CONTAINER = 4,
    UMI_BUILD_DEPLOYMENT_REMOTE_HOST = 5,
    UMI_BUILD_DEPLOYMENT_STATIC_SITE = 6,
    UMI_BUILD_DEPLOYMENT_PACKAGE_REPOSITORY = 7
} UmiBuildDeploymentTargetKind;

typedef enum UmiBuildDeploymentStrategy {
    UMI_BUILD_DEPLOYMENT_COPY = 0,
    UMI_BUILD_DEPLOYMENT_REPLACE = 1,
    UMI_BUILD_DEPLOYMENT_ROLLING = 2,
    UMI_BUILD_DEPLOYMENT_BLUE_GREEN = 3,
    UMI_BUILD_DEPLOYMENT_CANARY = 4,
    UMI_BUILD_DEPLOYMENT_IN_PLACE = 5
} UmiBuildDeploymentStrategy;

typedef uint32_t UmiBuildDeploymentTargetFlags;
enum {
    UMI_BUILD_DEPLOYMENT_TARGET_ENABLED = 1U << 0,
    UMI_BUILD_DEPLOYMENT_TARGET_REQUIRES_TRUST = 1U << 1,
    UMI_BUILD_DEPLOYMENT_TARGET_REQUIRES_APPROVAL = 1U << 2,
    UMI_BUILD_DEPLOYMENT_TARGET_HEALTH_CHECK = 1U << 3,
    UMI_BUILD_DEPLOYMENT_TARGET_SUPPORTS_ROLLBACK = 1U << 4,
    UMI_BUILD_DEPLOYMENT_TARGET_REMOTE = 1U << 5,
    UMI_BUILD_DEPLOYMENT_TARGET_DESTRUCTIVE = 1U << 6,
    UMI_BUILD_DEPLOYMENT_TARGET_PRODUCTION = 1U << 7,
    UMI_BUILD_DEPLOYMENT_TARGET_ALL_FLAGS = (1U << 8) - 1U
};

typedef struct UmiBuildDeploymentTargetSnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    char target_id[UMI_BUILD_ID_CAPACITY];
    char name[UMI_BUILD_NAME_CAPACITY];
    char description[UMI_BUILD_NAME_CAPACITY];
    char environment[UMI_BUILD_NAME_CAPACITY];
    char provider_id[UMI_BUILD_ID_CAPACITY];
    char endpoint[UMI_BUILD_PATH_CAPACITY];
    char destination[UMI_BUILD_PATH_CAPACITY];
    char working_directory[UMI_BUILD_PATH_CAPACITY];
    char health_uri[UMI_BUILD_PATH_CAPACITY];
    UmiBuildDeploymentTargetKind kind;
    UmiBuildDeploymentStrategy strategy;
    UmiBuildDeploymentTargetFlags flags;
    uint32_t timeout_ms;
    uint32_t health_timeout_ms;
    uint32_t maximum_parallel;
    uint64_t revision;
} UmiBuildDeploymentTargetSnapshot;

typedef struct UmiBuildDeploymentTargetRegistrySnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    size_t target_count;
    size_t enabled_count;
    size_t production_count;
    size_t remote_count;
    size_t approval_count;
    size_t rollback_count;
    uint64_t revision;
} UmiBuildDeploymentTargetRegistrySnapshot;

typedef struct UmiBuildDeploymentTargetRegistry
    UmiBuildDeploymentTargetRegistry;

void umi_build_deployment_target_init(
    UmiBuildDeploymentTargetSnapshot *target,
    const char *target_id,
    const char *name,
    UmiBuildDeploymentTargetKind kind);
UmiStatus umi_build_deployment_target_validate(
    const UmiBuildDeploymentTargetSnapshot *target,
    char *out_message,
    size_t message_capacity);
UmiStatus umi_build_deployment_target_registry_create(
    UmiBuildDeploymentTargetRegistry **out_registry);
void umi_build_deployment_target_registry_destroy(
    UmiBuildDeploymentTargetRegistry *registry);
void umi_build_deployment_target_registry_clear(
    UmiBuildDeploymentTargetRegistry *registry);
UmiStatus umi_build_deployment_target_registry_upsert(
    UmiBuildDeploymentTargetRegistry *registry,
    const UmiBuildDeploymentTargetSnapshot *target);
UmiStatus umi_build_deployment_target_registry_remove(
    UmiBuildDeploymentTargetRegistry *registry,
    const char *target_id);
UmiStatus umi_build_deployment_target_registry_find(
    const UmiBuildDeploymentTargetRegistry *registry,
    const char *target_id,
    UmiBuildDeploymentTargetSnapshot *out_target);
UmiStatus umi_build_deployment_target_registry_at(
    const UmiBuildDeploymentTargetRegistry *registry,
    size_t position,
    UmiBuildDeploymentTargetSnapshot *out_target);
UmiStatus umi_build_deployment_target_registry_snapshot(
    const UmiBuildDeploymentTargetRegistry *registry,
    UmiBuildDeploymentTargetRegistrySnapshot *out_snapshot);
size_t umi_build_deployment_target_registry_count(
    const UmiBuildDeploymentTargetRegistry *registry);
uint64_t umi_build_deployment_target_registry_revision(
    const UmiBuildDeploymentTargetRegistry *registry);
const char *umi_build_deployment_target_kind_text(
    UmiBuildDeploymentTargetKind kind);
const char *umi_build_deployment_strategy_text(
    UmiBuildDeploymentStrategy strategy);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_BUILD_DEPLOYMENT_TARGET_H */
