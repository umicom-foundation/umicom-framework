/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/generator.h
 *
 * PURPOSE:
 *   Apply a reviewed project-generation plan using Framework filesystem APIs.
 *
 * SAFETY:
 *   Generation is plan-first. Existing files are not overwritten unless the
 *   caller explicitly requests overwrite. No Git commit/push is performed here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_GENERATOR_H
#define UMICOM_DEVELOPER_PROJECT_GENERATOR_H

#include "umicom/developer_project/generation_plan.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer project generator request data shared with callers of this
 * public contract.
 */
typedef struct UmiDeveloperProjectGeneratorRequest {
    const UmiDeveloperProjectGenerationPlan *plan;
    int dry_run;
    int overwrite_existing_files;
} UmiDeveloperProjectGeneratorRequest;

/**
 * Represent the developer project generator report data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperProjectGeneratorReport {
    size_t directories_created;
    size_t files_created;
    size_t files_skipped;
    size_t bytes_written;
    int dry_run;
} UmiDeveloperProjectGeneratorReport;

/**
 * Perform developer project generator through the module contract so client applications
 * do not duplicate its policy. Each supplied report is reset even on admission
 * failure. Report storage must not overlap request/plan storage. All path joins
 * are admitted before effects, but later I/O failure can leave partial files.
 * Existing skip/overwrite policy requires an exclusively controlled destination;
 * the legacy writer does not reserve leaves against concurrent external writers.
 */
UmiStatus umi_developer_project_generator_apply(
    const UmiDeveloperProjectGeneratorRequest *request,
    UmiDeveloperProjectGeneratorReport *out_report);


/** Create an entirely new project below an existing absolute parent.
 * The final project directory and every generated file use exclusive native
 * creation. Existing final names are refused; there is no skip/overwrite mode.
 * All portable paths and joins are admitted before effects. Parent directories
 * below the project use shared no-follow rooted traversal. No operation removes
 * partial results, grants workspace trust or launches a tool.
 *
 * Reports reset on every attempt. Live counts include native creations/accepted
 * bytes before a later failure; files_created does not imply complete writes.
 * Dry runs perform syntax admission only and report predicted files/bytes with
 * zero directories_created. They do not probe existence or reserve a name.
 * Inputs must stay stable and not overlap the report. The caller serializes
 * destination edits; operations do not retain handles between calls, isolate
 * privileged writers or promise a multi-file/power-loss transaction. The
 * executable/optional recipe metadata remains descriptive, as in apply. */
UmiStatus umi_developer_project_generator_create(
    const UmiDeveloperProjectGeneratorRequest *request,
    UmiDeveloperProjectGeneratorReport *out_report);


/** Describe a checked new-project result without filesystem effects.
 * The supplied report must come from the exclusive creator (no skipped files);
 * NULL reports explicitly describe unavailable progress. Live counts can include
 * incomplete files. Preview counts are predictions, never observed effects.
 * Invalid/inconsistent reports or insufficient output capacity leave out
 * unchanged. Inputs must remain stable and separate from writable output. */
UmiStatus UmiDeveloperProjectCreationDescribe(UmiStatus result,
    const UmiDeveloperProjectGeneratorReport *report, char *out, size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
