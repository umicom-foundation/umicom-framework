/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build/test_recipe_fixtures.c
 *
 * PURPOSE:
 *   Verify recipe admission, immutable ownership and semantic task projection using independent
 * format fixtures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/base/sha256.h"
#include "umicom/build/recipe.h"
#include "umicom/build/task_graph.h"
#include "umicom/platform/input_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                   \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
static UmiStatus Expected(const char *name) {
#define STATUS(s)                                                                                  \
  if (!strcmp(name, #s))                                                                           \
  return UMI_STATUS_##s
  STATUS(OK);
  STATUS(PARSE_ERROR);
  STATUS(CAPACITY_EXCEEDED);
  STATUS(INVALID_ARGUMENT);
  STATUS(ALREADY_EXISTS);
  STATUS(NOT_FOUND);
  STATUS(INVALID_STATE);
#undef STATUS
  return UMI_STATUS_INTERNAL_ERROR;
}
int main(int argc, char **argv) {
  if (argc != 4)
    return 2;
  UmiBuildRecipe *previous = NULL;
  UmiBuildRecipeReport report;
  CHECK(UmiBuildRecipeReadFile(argv[1], &previous, &report) == UMI_STATUS_OK);
  UmiBuildRecipeInfo before;
  CHECK(UmiBuildRecipeInfoRead(previous, &before) == UMI_STATUS_OK);
  unsigned char *bytes = NULL;
  size_t size = 0U;
  CHECK(UmiInputFileRead(argv[2], UMI_BUILD_RECIPE_MAX_BYTES, &bytes, &size) == UMI_STATUS_OK);
  char digest[65];
  CHECK(UmiSha256Buffer(bytes, size, digest) == UMI_STATUS_OK);
  UmiBuildRecipe *recipe = previous;
  UmiStatus status = UmiBuildRecipeOpen(bytes, size, &recipe, &report);
  CHECK(status == Expected(argv[3]));
  CHECK(report.status == status && report.message[0]);
  /* Accepted state must outlive and not borrow the entire source buffer. */
  memset(bytes, 0, size);
  UmiInputFileFree(bytes);
  if (status != UMI_STATUS_OK) {
    UmiBuildRecipeInfo after;
    CHECK(recipe == previous);
    CHECK(UmiBuildRecipeInfoRead(previous, &after) == UMI_STATUS_OK);
    CHECK(memcmp(&before, &after, sizeof before) == 0);
    UmiBuildRecipeDestroy(previous);
    return 0;
  }
  CHECK(recipe != previous);
  UmiBuildRecipeDestroy(previous);
  UmiBuildRecipeInfo info;
  CHECK(UmiBuildRecipeInfoRead(recipe, &info) == UMI_STATUS_OK);
  CHECK(strcmp(info.recipe_sha256, digest) == 0);
  CHECK(info.stage_count > 0U && info.input_count > 0U);
  CHECK(UmiBuildRecipeInputAt(recipe, info.input_count) == NULL);
  CHECK(UmiBuildRecipeStageAt(recipe, info.stage_count) == NULL);
  UmiBuildTaskRegistry *registry = NULL;
  CHECK(UmiBuildRecipeCreateTasks(recipe, &registry) == UMI_STATUS_OK);
  CHECK(umi_build_task_registry_count(registry) == info.stage_count);
  UmiBuildTaskSnapshot task;
  char final_id[UMI_BUILD_ID_CAPACITY];
  for (size_t i = 0U; i < info.stage_count; ++i) {
    const UmiBuildRecipeStage *stage = UmiBuildRecipeStageAt(recipe, i);
    CHECK(umi_build_task_registry_find(registry, stage->id, &task) == UMI_STATUS_OK);
    CHECK(task.has_build_phase && task.build_phase == stage->phase);
    CHECK(!strcmp(task.label, stage->label) && task.command[0] == '\0');
    CHECK(task.working_directory[0] == '\0' && task.environment_count == 0U);
    CHECK(task.run_on == UMI_BUILD_TASK_RUN_MANUALLY);
    CHECK((task.flags & UMI_BUILD_TASK_REQUIRES_WORKSPACE_TRUST) != 0U);
    CHECK(task.dependency_count == stage->dependency_count &&
          task.input_count == stage->input_count && task.output_count == stage->output_count);
    for (size_t d = 0U; d < stage->dependency_count; ++d)
      CHECK(!strcmp(task.dependencies[d], stage->dependencies[d]));
    for (size_t o = 0U; o < stage->output_count; ++o)
      CHECK(!strcmp(task.outputs[o], stage->outputs[o]));
    for (size_t n = 0U; n < stage->input_count; ++n) {
      size_t j = 0U;
      while (j < info.input_count && strcmp(UmiBuildRecipeInputAt(recipe, j)->id, stage->inputs[n]))
        ++j;
      CHECK(j < info.input_count);
      CHECK(!strcmp(task.inputs[n], UmiBuildRecipeInputAt(recipe, j)->path));
    }
    strcpy(final_id, stage->id);
  }
  UmiBuildRecipeDestroy(recipe);
  CHECK(umi_build_task_registry_find(registry, final_id, &task) == UMI_STATUS_OK);
  UmiBuildTaskGraph *graph = NULL;
  CHECK(umi_build_task_graph_create(&graph) == UMI_STATUS_OK);
  CHECK(umi_build_task_graph_rebuild(graph, registry) == UMI_STATUS_OK);
  umi_build_task_graph_destroy(graph);
  umi_build_task_registry_destroy(registry);
  return 0;
}
