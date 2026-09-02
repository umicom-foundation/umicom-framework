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
 * do not duplicate its policy.
 */
UmiStatus umi_developer_project_generator_apply(
    const UmiDeveloperProjectGeneratorRequest *request,
    UmiDeveloperProjectGeneratorReport *out_report);

#ifdef __cplusplus
}
#endif

#endif
