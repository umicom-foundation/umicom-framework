/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/model_registry.h
 *
 * PURPOSE:
 *   Maintain IDE-visible projects and active project selection by stable ID.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_MODEL_REGISTRY_H
#define UMICOM_DEVELOPER_PROJECT_MODEL_REGISTRY_H

#include "umicom/developer_project/model.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer project model registry data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperProjectModelRegistry
    UmiDeveloperProjectModelRegistry;

/**
 * Initialise developer project model registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_developer_project_model_registry_create(
    UmiDeveloperProjectModelRegistry **out_registry);

/**
 * Release or reset state held by developer project model registry so the same storage can
 * be reused safely.
 */
void umi_developer_project_model_registry_destroy(
    UmiDeveloperProjectModelRegistry *registry);

/**
 * Provide the developer project model registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_project_model_registry_upsert(
    UmiDeveloperProjectModelRegistry *registry,
    const UmiDeveloperProjectModel *model);

/**
 * Remove developer project model registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_developer_project_model_registry_remove(
    UmiDeveloperProjectModelRegistry *registry,
    const char *project_id);

/**
 * Find developer project model registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_developer_project_model_registry_find(
    const UmiDeveloperProjectModelRegistry *registry,
    const char *project_id,
    UmiDeveloperProjectModel *out_model);

/**
 * Find developer project model registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_developer_project_model_registry_at(
    const UmiDeveloperProjectModelRegistry *registry,
    size_t index,
    UmiDeveloperProjectModel *out_model);

/**
 * Provide the developer project model registry activate operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_project_model_registry_activate(
    UmiDeveloperProjectModelRegistry *registry,
    const char *project_id);

/**
 * Provide the developer project model registry active operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_project_model_registry_active(
    const UmiDeveloperProjectModelRegistry *registry,
    UmiDeveloperProjectModel *out_model);

/**
 * Return the number of records represented by developer project model registry without
 * changing their state.
 */
size_t umi_developer_project_model_registry_count(
    const UmiDeveloperProjectModelRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
