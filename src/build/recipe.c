/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/recipe.c
 *
 * PURPOSE:
 *   Own validated source-build recipes and project semantic phases into the canonical task
 * registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "recipe_internal.h"
#include "umicom/base/sha256.h"
#include <stdlib.h>
#include <string.h>

UmiStatus UmiRecipeReport(UmiBuildRecipeReport *report, UmiStatus status, size_t stage,
                          const char *message) {
  if (report != NULL) {
    UmiBuildRecipeReport value = {0};
    value.status = status;
    value.stage_index = stage;
    if (message != NULL) {
      size_t length = strlen(message);
      if (length >= sizeof value.message)
        length = sizeof value.message - 1U;
      memcpy(value.message, message, length);
    }
    *report = value;
  }
  return status;
}
UmiStatus UmiBuildRecipeOpen(const void *data, size_t size, UmiBuildRecipe **out_recipe,
                             UmiBuildRecipeReport *report) {
  if (data == NULL || size == 0U || out_recipe == NULL)
    return UmiRecipeReport(report, UMI_STATUS_INVALID_ARGUMENT, SIZE_MAX,
                           "Recipe bytes and output owner are required.");
  if (size > UMI_BUILD_RECIPE_MAX_BYTES)
    return UmiRecipeReport(report, UMI_STATUS_CAPACITY_EXCEEDED, SIZE_MAX,
                           "Recipe exceeds the bounded byte limit.");
  UmiJsonTree *tree = NULL;
  const UmiJsonTreeLimits limits = {UMI_BUILD_RECIPE_MAX_BYTES, 16384U, 16U};
  UmiStatus status = UmiJsonTreeCreate(data, size, &limits, NULL, &tree);
  if (status != UMI_STATUS_OK)
    return UmiRecipeReport(report, status, SIZE_MAX,
                           "Recipe is not a complete bounded JSON document.");
  UmiBuildRecipe *candidate = calloc(1U, sizeof *candidate);
  if (candidate == NULL) {
    UmiJsonTreeDestroy(tree);
    return UmiRecipeReport(report, UMI_STATUS_OUT_OF_MEMORY, SIZE_MAX,
                           "Recipe storage could not be allocated.");
  }
  status = UmiRecipeDecode(tree, candidate, report);
  UmiJsonTreeDestroy(tree);
  if (status == UMI_STATUS_OK)
    status = UmiRecipeValidate(candidate, report);
  if (status == UMI_STATUS_OK) {
    status = UmiSha256Buffer(data, size, candidate->info.recipe_sha256);
    if (status != UMI_STATUS_OK)
      UmiRecipeReport(report, status, SIZE_MAX, "Recipe byte identity could not be calculated.");
  }
  if (status != UMI_STATUS_OK) {
    free(candidate);
    return status;
  }
  *out_recipe = candidate;
  return UmiRecipeReport(
      report, UMI_STATUS_OK, SIZE_MAX,
      "Recipe metadata accepted; source and toolchain claims remain unverified.");
}
void UmiBuildRecipeDestroy(UmiBuildRecipe *recipe) { free(recipe); }
UmiStatus UmiBuildRecipeInfoRead(const UmiBuildRecipe *recipe, UmiBuildRecipeInfo *out_info) {
  if (recipe == NULL || out_info == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out_info = recipe->info;
  return UMI_STATUS_OK;
}
const UmiBuildRecipeInput *UmiBuildRecipeInputAt(const UmiBuildRecipe *recipe, size_t index) {
  return recipe != NULL && index < recipe->info.input_count ? &recipe->inputs[index] : NULL;
}
const UmiBuildRecipeStage *UmiBuildRecipeStageAt(const UmiBuildRecipe *recipe, size_t index) {
  return recipe != NULL && index < recipe->info.stage_count ? &recipe->stages[index] : NULL;
}
static UmiBuildTaskKind TaskKind(UmiBuildPhase phase) {
  switch (phase) {
  case UMI_BUILD_PHASE_CONFIGURE:
    return UMI_BUILD_TASK_CONFIGURE;
  case UMI_BUILD_PHASE_BUILD:
    return UMI_BUILD_TASK_BUILD;
  case UMI_BUILD_PHASE_TEST:
    return UMI_BUILD_TASK_TEST;
  case UMI_BUILD_PHASE_INSTALL:
    return UMI_BUILD_TASK_INSTALL;
  case UMI_BUILD_PHASE_PACKAGE:
    return UMI_BUILD_TASK_PACKAGE;
  case UMI_BUILD_PHASE_DEPLOY:
    return UMI_BUILD_TASK_DEPLOY;
  default:
    return UMI_BUILD_TASK_COMMAND;
  }
}
UmiStatus UmiBuildRecipeCreateTasks(const UmiBuildRecipe *recipe,
                                    UmiBuildTaskRegistry **out_registry) {
  if (recipe == NULL || out_registry == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiBuildTaskRegistry *registry = NULL;
  UmiStatus status = umi_build_task_registry_create(&registry);
  if (status != UMI_STATUS_OK)
    return status;
  /* Reuse the existing descriptor builder and registry owner. No command is
   * generated from recipe text; a separately configured provider must supply
   * execution details and enforce workspace trust before any real work. */
  UmiBuildTaskSnapshot task;
  for (size_t i = 0U; i < recipe->info.stage_count && status == UMI_STATUS_OK; ++i) {
    const UmiBuildRecipeStage *stage = &recipe->stages[i];
    umi_build_task_init(&task, stage->id, stage->label, TaskKind(stage->phase));
    task.flags |= UMI_BUILD_TASK_REQUIRES_WORKSPACE_TRUST;
    for (size_t d = 0U; d < stage->dependency_count && status == UMI_STATUS_OK; ++d)
      status = umi_build_task_add_dependency(&task, stage->dependencies[d]);
    for (size_t input = 0U; input < stage->input_count && status == UMI_STATUS_OK; ++input) {
      size_t found = 0U;
      while (found < recipe->info.input_count &&
             strcmp(stage->inputs[input], recipe->inputs[found].id) != 0)
        ++found;
      status = found == recipe->info.input_count
                   ? UMI_STATUS_NOT_FOUND
                   : umi_build_task_add_input(&task, recipe->inputs[found].path);
    }
    for (size_t output = 0U; output < stage->output_count && status == UMI_STATUS_OK; ++output)
      status = umi_build_task_add_output(&task, stage->outputs[output]);
    if (status == UMI_STATUS_OK)
      status = umi_build_task_registry_upsert(registry, &task);
  }
  if (status != UMI_STATUS_OK) {
    umi_build_task_registry_destroy(registry);
    return status;
  }
  *out_registry = registry;
  return UMI_STATUS_OK;
}
