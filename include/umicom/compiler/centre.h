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

/* Keep the public C symbol names when this header is included directly by a
 * C++ client. Includes remain outside the linkage block; layouts are unchanged. */
#ifdef __cplusplus
extern "C" {
#endif
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

/** Admit all active nested storage before lookup or publication. This checks
 * storage, not SDK capability or every project/profile semantic relationship. */
bool umi_compiler_centre_storage_valid(const UmiCompilerCentre *centre);
/** Plan with an explicit directory recorded in both the returned command and
 * compilation database. No filesystem or process operation occurs. The legacy
 * plan_unit wrapper retains "." as its relative working-directory convention.
 *
 * Any refusal preserves the complete centre and output. A repeated source updates
 * its stored command through database_upsert; add-only database semantics remain
 * unchanged. Revisions never wrap. Disabled active profiles cannot plan.
 * Output must be separate from centre storage; identifiers/directory may borrow
 * centre text. Argument views are rebased into both receiving owners.
 * This is serialized publication, not a lock-free transaction: callers exclude
 * concurrent mutation and reads for the complete operation. */
UmiStatus umi_compiler_centre_plan_unit_in_directory(UmiCompilerCentre *centre,const char *project_id,
    const char *unit_id,const char *working_directory,UmiCompilerPlan *out_plan);
/** Checked snapshots preserve output on refusal and reject overlapping owner
 * storage. The void compatibility form produces zero on invalid input/storage,
 * except that an overlapping destination is left untouched. A zero legacy result
 * is not evidence of a successfully observed empty centre. */
UmiStatus umi_compiler_centre_snapshot_checked(const UmiCompilerCentre *centre,UmiCompilerCentreSnapshot *out_snapshot);
/** Stage provider upserts and a profile replacement/addition, then publish both
 * with optional activation. Every failure preserves the complete centre. Existing
 * rows can be replaced at full capacity, but revisions never wrap. Zero providers
 * is allowed when the profile's provider is already registered. Inputs may borrow
 * centre rows; callers exclude concurrent reads/mutations throughout publication.
 * Other profiles and historical command records are retained, not replanned or
 * requalified. This operation does not discover or execute tools. */
UmiStatus umi_compiler_centre_configure_profile(UmiCompilerCentre *centre,
    const UmiCompilerProvider *providers,size_t provider_count,
    const UmiCompilerProfile *profile,bool activate);
/** Format a complete human-readable snapshot without partially replacing output.
 * Checks bounded counts and active-profile text. Output must not overlap snapshot
 * storage. Failure preserves destination bytes; success copies text and its zero
 * terminator only. Metadata availability is not a fresh executable probe.
 * Callers keep the snapshot stable during this read. */
UmiStatus umi_compiler_centre_format_snapshot(const UmiCompilerCentreSnapshot *snapshot,
    char *out_text,size_t capacity);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
