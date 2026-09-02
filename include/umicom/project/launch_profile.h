/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/launch_profile.h
 *
 * PURPOSE:
 *   Define a reusable project-system record used by Studio and future Umicom development products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_PROJECT_LAUNCH_PROFILE_H
#define UMICOM_PROJECT_LAUNCH_PROFILE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_LAUNCH_PROFILE_CAPACITY 1024U
#define UMI_PROJECT_LAUNCH_PROFILE_API_VERSION 1U

/**
 * Represent the project launch profile snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectLaunchProfileSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char project_id[128];
    char name[256];
    char program[1024];
    char arguments[1024];
    char working_directory[1024];
    char environment_id[128];
    int debug;
    int default_profile;
    uint64_t revision;
} UmiProjectLaunchProfileSnapshot;

/**
 * Represent the project launch profile registry data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectLaunchProfileRegistry UmiProjectLaunchProfileRegistry;

/**
 * Initialise project launch profile registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_project_launch_profile_registry_create(UmiProjectLaunchProfileRegistry **out_registry);
/**
 * Release or reset state held by project launch profile registry so the same storage can
 * be reused safely.
 */
void umi_project_launch_profile_registry_destroy(UmiProjectLaunchProfileRegistry *registry);
/**
 * Provide the project launch profile registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_launch_profile_registry_upsert(UmiProjectLaunchProfileRegistry *registry, const UmiProjectLaunchProfileSnapshot *item);
/**
 * Remove project launch profile registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_project_launch_profile_registry_remove(UmiProjectLaunchProfileRegistry *registry, const char *id);
/**
 * Find project launch profile registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_project_launch_profile_registry_find(const UmiProjectLaunchProfileRegistry *registry, const char *id, UmiProjectLaunchProfileSnapshot *out_item);
/**
 * Find project launch profile registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_project_launch_profile_registry_at(const UmiProjectLaunchProfileRegistry *registry, size_t index, UmiProjectLaunchProfileSnapshot *out_item);
/**
 * Return the number of records represented by project launch profile registry without
 * changing their state.
 */
size_t umi_project_launch_profile_registry_count(const UmiProjectLaunchProfileRegistry *registry);
/**
 * Provide the project launch profile registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_project_launch_profile_registry_revision(const UmiProjectLaunchProfileRegistry *registry);
/**
 * Release or reset state held by project launch profile registry so the same storage can
 * be reused safely.
 */
void umi_project_launch_profile_registry_clear(UmiProjectLaunchProfileRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
