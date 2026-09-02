/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/centre.h
 *
 * PURPOSE:
 *   Publish the public centre contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_CENTRE_H
#define UMICOM_COMPILER_CENTRE_H
#include "umicom/compiler/artifact.h"
#include "umicom/compiler/cache.h"
#include "umicom/compiler/capability.h"
#include "umicom/compiler/compilation_database.h"
#include "umicom/compiler/interop_graph.h"
#include "umicom/compiler/manifest.h"
#include "umicom/compiler/registry.h"
#define UMI_COMPILER_MAX_MANIFESTS 32U
/**
 * Represent the compiler centre data shared with callers of this public contract.
 */
typedef struct UmiCompilerCentre { UmiCompilerRegistry providers; UmiCompilerProfile profiles[UMI_COMPILER_MAX_PROFILES]; size_t profile_count; UmiCompilerProjectManifest manifests[UMI_COMPILER_MAX_MANIFESTS]; size_t manifest_count; UmiCompilerArtifactStore artifacts; UmiCompilerCache cache; UmiPolyglotCompilationDatabase compilation_database; char active_profile_id[UMI_COMPILER_ID_CAPACITY]; uint64_t revision; } UmiCompilerCentre;
/**
 * Represent the compiler centre snapshot data shared with callers of this public contract.
 */
typedef struct UmiCompilerCentreSnapshot { size_t providers; size_t available_providers; size_t profiles; size_t manifests; size_t units; size_t artifacts; size_t compilation_commands; uint64_t cache_hits; uint64_t cache_misses; char active_profile_id[UMI_COMPILER_ID_CAPACITY]; uint64_t revision; } UmiCompilerCentreSnapshot;
/**
 * Initialise compiler centre from caller-provided values so later operations receive a
 * known state.
 */
void umi_compiler_centre_init(UmiCompilerCentre *centre);
/**
 * Provide the compiler centre add profile operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_centre_add_profile(UmiCompilerCentre *centre,const UmiCompilerProfile *profile);
/**
 * Provide the compiler centre activate profile operation used by this module and its
 * client applications.
 */
UmiStatus umi_compiler_centre_activate_profile(UmiCompilerCentre *centre,const char *profile_id);
/**
 * Provide the compiler centre active profile operation used by this module and its client
 * applications.
 */
const UmiCompilerProfile *umi_compiler_centre_active_profile(const UmiCompilerCentre *centre);
/**
 * Provide the compiler centre add manifest operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_centre_add_manifest(UmiCompilerCentre *centre,const UmiCompilerProjectManifest *manifest);
/**
 * Provide the compiler centre plan unit operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_centre_plan_unit(UmiCompilerCentre *centre,const char *project_id,const char *unit_id,UmiCompilerPlan *out_plan);
/**
 * Provide the compiler centre snapshot operation used by this module and its client
 * applications.
 */
void umi_compiler_centre_snapshot(const UmiCompilerCentre *centre,UmiCompilerCentreSnapshot *out_snapshot);
#endif
