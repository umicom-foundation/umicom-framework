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

typedef struct UmiDeveloperProjectModelRegistry
    UmiDeveloperProjectModelRegistry;

UmiStatus umi_developer_project_model_registry_create(
    UmiDeveloperProjectModelRegistry **out_registry);

void umi_developer_project_model_registry_destroy(
    UmiDeveloperProjectModelRegistry *registry);

UmiStatus umi_developer_project_model_registry_upsert(
    UmiDeveloperProjectModelRegistry *registry,
    const UmiDeveloperProjectModel *model);

UmiStatus umi_developer_project_model_registry_remove(
    UmiDeveloperProjectModelRegistry *registry,
    const char *project_id);

UmiStatus umi_developer_project_model_registry_find(
    const UmiDeveloperProjectModelRegistry *registry,
    const char *project_id,
    UmiDeveloperProjectModel *out_model);

UmiStatus umi_developer_project_model_registry_at(
    const UmiDeveloperProjectModelRegistry *registry,
    size_t index,
    UmiDeveloperProjectModel *out_model);

UmiStatus umi_developer_project_model_registry_activate(
    UmiDeveloperProjectModelRegistry *registry,
    const char *project_id);

UmiStatus umi_developer_project_model_registry_active(
    const UmiDeveloperProjectModelRegistry *registry,
    UmiDeveloperProjectModel *out_model);

size_t umi_developer_project_model_registry_count(
    const UmiDeveloperProjectModelRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
