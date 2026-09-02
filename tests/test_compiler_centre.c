/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_centre.c
 *
 * PURPOSE:
 *   Implement the test compiler centre behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Batch 58 test | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/compiler.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiCompilerCentre *centre = calloc(1U,sizeof(*centre)); UmiCompilerProvider provider; UmiCompilerTarget target; UmiCompilerProfile profile; UmiCompilerProjectManifest manifest; UmiCompilerUnitManifest unit = {0}; UmiCompilerPlan plan; UmiCompilerCentreSnapshot snapshot; assert(centre != NULL); umi_compiler_centre_init(centre); provider = umi_compiler_gcc_provider("gcc","x86_64-unknown-linux-gnu"); assert(umi_compiler_registry_register(&centre->providers,&provider) == UMI_STATUS_OK); assert(umi_compiler_target_host(&target) == UMI_STATUS_OK); assert(umi_compiler_profile_init(&profile,"debug","Debug","gcc",UMI_COMPILER_LANGUAGE_C,&target) == UMI_STATUS_OK); assert(umi_compiler_centre_add_profile(centre,&profile) == UMI_STATUS_OK); assert(umi_compiler_manifest_init(&manifest,"demo","Demo",target.triple) == UMI_STATUS_OK); (void)strcpy(unit.unit_id,"main"); (void)strcpy(unit.source,"main.c"); (void)strcpy(unit.output,"main.o"); unit.language = UMI_COMPILER_LANGUAGE_C; assert(umi_compiler_manifest_add_unit(&manifest,&unit) == UMI_STATUS_OK); assert(umi_compiler_centre_add_manifest(centre,&manifest) == UMI_STATUS_OK); assert(umi_compiler_centre_plan_unit(centre,"demo","main",&plan) == UMI_STATUS_OK); umi_compiler_centre_snapshot(centre,&snapshot); assert(snapshot.compilation_commands == 1U); free(centre); return 0; }
