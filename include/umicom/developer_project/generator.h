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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_GENERATOR_H
#define UMICOM_DEVELOPER_PROJECT_GENERATOR_H

#include "umicom/developer_project/generation_plan.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeveloperProjectGeneratorRequest {
    const UmiDeveloperProjectGenerationPlan *plan;
    int dry_run;
    int overwrite_existing_files;
} UmiDeveloperProjectGeneratorRequest;

typedef struct UmiDeveloperProjectGeneratorReport {
    size_t directories_created;
    size_t files_created;
    size_t files_skipped;
    size_t bytes_written;
    int dry_run;
} UmiDeveloperProjectGeneratorReport;

UmiStatus umi_developer_project_generator_apply(
    const UmiDeveloperProjectGeneratorRequest *request,
    UmiDeveloperProjectGeneratorReport *out_report);

#ifdef __cplusplus
}
#endif

#endif
