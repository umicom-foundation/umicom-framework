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

/**
 * Return the number of records represented by application presentation workspace runtime
 * policy catalogue without changing their state.
 */
size_t umi_application_presentation_workspace_runtime_policy_catalogue_count(void);
/**
 * Find application presentation workspace runtime policy catalogue while leaving the
 * underlying catalogue or model owned by this module.
 */
const UmiApplicationPresentationWorkspaceRuntimePolicy *
umi_application_presentation_workspace_runtime_policy_catalogue_at(size_t index);
/**
 * Find application presentation workspace runtime policy catalogue while leaving the
 * underlying catalogue or model owned by this module.
 */
const UmiApplicationPresentationWorkspaceRuntimePolicy *
umi_application_presentation_workspace_runtime_policy_catalogue_find(
    const char *recipe_id);
/**
 * Return the number of records represented by application presentation workspace runtime
 * policy catalogue checkpoint without changing their state.
 */
size_t umi_application_presentation_workspace_runtime_policy_catalogue_checkpoint_count(
    UmiApplicationPresentationCheckpointPolicy policy);
/**
 * Check that application presentation workspace runtime policy catalogue satisfies its
 * contract before another service relies on it.
 */
UmiStatus
umi_application_presentation_workspace_runtime_policy_catalogue_validate(void);

#ifdef __cplusplus
}
#endif

#endif
