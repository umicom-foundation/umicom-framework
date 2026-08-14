/* Umicom Framework | Compiler and polyglot centre | Sammy Hegab | Umicom Foundation | MIT */
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
typedef struct UmiCompilerCentre { UmiCompilerRegistry providers; UmiCompilerProfile profiles[UMI_COMPILER_MAX_PROFILES]; size_t profile_count; UmiCompilerProjectManifest manifests[UMI_COMPILER_MAX_MANIFESTS]; size_t manifest_count; UmiCompilerArtifactStore artifacts; UmiCompilerCache cache; UmiPolyglotCompilationDatabase compilation_database; char active_profile_id[UMI_COMPILER_ID_CAPACITY]; uint64_t revision; } UmiCompilerCentre;
typedef struct UmiCompilerCentreSnapshot { size_t providers; size_t available_providers; size_t profiles; size_t manifests; size_t units; size_t artifacts; size_t compilation_commands; uint64_t cache_hits; uint64_t cache_misses; char active_profile_id[UMI_COMPILER_ID_CAPACITY]; uint64_t revision; } UmiCompilerCentreSnapshot;
void umi_compiler_centre_init(UmiCompilerCentre *centre);
UmiStatus umi_compiler_centre_add_profile(UmiCompilerCentre *centre,const UmiCompilerProfile *profile);
UmiStatus umi_compiler_centre_activate_profile(UmiCompilerCentre *centre,const char *profile_id);
const UmiCompilerProfile *umi_compiler_centre_active_profile(const UmiCompilerCentre *centre);
UmiStatus umi_compiler_centre_add_manifest(UmiCompilerCentre *centre,const UmiCompilerProjectManifest *manifest);
UmiStatus umi_compiler_centre_plan_unit(UmiCompilerCentre *centre,const char *project_id,const char *unit_id,UmiCompilerPlan *out_plan);
void umi_compiler_centre_snapshot(const UmiCompilerCentre *centre,UmiCompilerCentreSnapshot *out_snapshot);
#endif
