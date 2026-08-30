/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/presentation/workspace_runtime_policy_catalogue.h
 *
 * PURPOSE:
 *   Discover one live workspace policy for every Framework-owned application
 *   recipe and validate complete recipe coverage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRESENTATION_WORKSPACE_RUNTIME_POLICY_CATALOGUE_H
#define UMICOM_APPLICATION_PRESENTATION_WORKSPACE_RUNTIME_POLICY_CATALOGUE_H

#include "umicom/application/presentation/workspace_runtime_policy.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t umi_application_presentation_workspace_runtime_policy_catalogue_count(void);
const UmiApplicationPresentationWorkspaceRuntimePolicy *
umi_application_presentation_workspace_runtime_policy_catalogue_at(size_t index);
const UmiApplicationPresentationWorkspaceRuntimePolicy *
umi_application_presentation_workspace_runtime_policy_catalogue_find(
    const char *recipe_id);
size_t umi_application_presentation_workspace_runtime_policy_catalogue_checkpoint_count(
    UmiApplicationPresentationCheckpointPolicy policy);
UmiStatus
umi_application_presentation_workspace_runtime_policy_catalogue_validate(void);

#ifdef __cplusplus
}
#endif

#endif
