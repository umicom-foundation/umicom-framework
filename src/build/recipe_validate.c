/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/recipe_validate.c
 *
 * PURPOSE:
 *   Admit portable recipe identities and file claims, then delegate dependency validation to the
 * shared task graph.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "recipe_internal.h"
#include "umicom/build/task_graph.h"
#include "umicom/repository/path.h"
#include <string.h>

static int Identifier(const char *text) {
  if (text[0] < 'a' || text[0] > 'z')
    return 0;
  for (const char *p = text + 1; *p; ++p)
    if (!((*p >= 'a' && *p <= 'z') || (*p >= '0' && *p <= '9') || *p == '_' || *p == '-' ||
          *p == '.'))
      return 0;
  return 1;
}
static int Triple(const char *text) {
  size_t separators = 0U, component = 0U;
  for (const char *p = text; *p; ++p) {
    if (*p == '-') {
      if (component == 0U)
        return 0;
      ++separators;
      component = 0U;
    } else {
      if (!((*p >= 'a' && *p <= 'z') || (*p >= '0' && *p <= '9') || *p == '_' || *p == '.'))
        return 0;
      ++component;
    }
  }
  return separators >= 2U && component > 0U;
}
static int Digest(const char *text) {
  if (strlen(text) != 64U)
    return 0;
  for (size_t i = 0U; i < 64U; ++i)
    if (!((text[i] >= '0' && text[i] <= '9') || (text[i] >= 'a' && text[i] <= 'f')))
      return 0;
  return 1;
}
static char Fold(char c) { return c >= 'A' && c <= 'Z' ? (char)(c + ('a' - 'A')) : c; }
static int Reserved(const char *start, size_t length) {
  char stem[9];
  size_t n = 0U;
  while (n < length && start[n] != '.') {
    if (n >= sizeof stem - 1U)
      return 0;
    stem[n] = Fold(start[n]);
    ++n;
  }
  stem[n] = '\0';
  return strcmp(stem, "con") == 0 || strcmp(stem, "prn") == 0 || strcmp(stem, "aux") == 0 ||
         strcmp(stem, "nul") == 0 ||
         (n == 4U && (memcmp(stem, "com", 3U) == 0 || memcmp(stem, "lpt", 3U) == 0) &&
          stem[3] >= '1' && stem[3] <= '9');
}
static int PortablePath(const char *text) {
  if (!umi_repository_control_path_is_safe_relative(text))
    return 0;
  /* The general repository policy permits native separators and text. Recipe
   * file claims use one stricter portable spelling on every host instead of
   * allowing host-dependent aliases to name competing outputs. */
  const char *segment = text;
  for (const char *p = text;; ++p) {
    const unsigned char c = (unsigned char)*p;
    if (c == '/' || c == 0U) {
      const size_t n = (size_t)(p - segment);
      if (n == 0U || segment[n - 1U] == '.' || segment[n - 1U] == ' ' || Reserved(segment, n))
        return 0;
      if (c == 0U)
        return 1;
      segment = p + 1;
    } else if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ||
                 c == '.' || c == '_' || c == '-' || c == ' '))
      return 0;
  }
}
static int Collides(const char *a, const char *b) {
  size_t i = 0U;
  while (a[i] && b[i] && Fold(a[i]) == Fold(b[i]))
    ++i;
  return (a[i] == '\0' && (b[i] == '\0' || b[i] == '/')) || (b[i] == '\0' && a[i] == '/');
}
static int Label(const char *label) {
  if (!label[0])
    return 0;
  for (const unsigned char *p = (const unsigned char *)label; *p; ++p)
    if (*p < 32U || *p == 127U)
      return 0;
  return 1;
}
UmiStatus UmiRecipeValidate(const UmiBuildRecipe *recipe, UmiBuildRecipeReport *report) {
  if (!Identifier(recipe->info.id) || !Identifier(recipe->info.toolchain_id) ||
      !Triple(recipe->info.build) || !Triple(recipe->info.host) || !Triple(recipe->info.target) ||
      !Digest(recipe->info.toolchain_sha256))
    return UmiRecipeReport(
        report, UMI_STATUS_INVALID_ARGUMENT, SIZE_MAX,
        "Recipe identity, target triples or toolchain manifest digest are invalid.");
  const char *paths[2U + UMI_BUILD_RECIPE_MAX_INPUTS +
                    UMI_BUILD_RECIPE_MAX_STAGES * UMI_BUILD_TASK_OUTPUT_CAPACITY];
  size_t path_count = 0U;
  paths[path_count++] = recipe->info.compiler;
  paths[path_count++] = recipe->info.toolchain_manifest;
  for (size_t i = 0U; i < recipe->info.input_count; ++i) {
    const UmiBuildRecipeInput *input = &recipe->inputs[i];
    if (!Identifier(input->id) || !Digest(input->sha256))
      return UmiRecipeReport(report, UMI_STATUS_INVALID_ARGUMENT, SIZE_MAX,
                             "Source input ID or SHA-256 claim is invalid.");
    for (size_t j = 0U; j < i; ++j)
      if (strcmp(input->id, recipe->inputs[j].id) == 0)
        return UmiRecipeReport(report, UMI_STATUS_ALREADY_EXISTS, SIZE_MAX,
                               "Source input IDs must be distinct.");
    paths[path_count++] = input->path;
  }
  for (size_t i = 0U; i < recipe->info.stage_count; ++i) {
    const UmiBuildRecipeStage *stage = &recipe->stages[i];
    if (!Identifier(stage->id) || !Label(stage->label))
      return UmiRecipeReport(report, UMI_STATUS_INVALID_ARGUMENT, i,
                             "Stage ID or display label is invalid.");
    for (size_t j = 0U; j < i; ++j)
      if (strcmp(stage->id, recipe->stages[j].id) == 0)
        return UmiRecipeReport(report, UMI_STATUS_ALREADY_EXISTS, i, "Stage IDs must be distinct.");
    for (size_t d = 0U; d < stage->dependency_count; ++d)
      if (!Identifier(stage->dependencies[d]))
        return UmiRecipeReport(report, UMI_STATUS_INVALID_ARGUMENT, i,
                               "Stage dependency ID is invalid.");
    for (size_t input = 0U; input < stage->input_count; ++input)
      if (!Identifier(stage->inputs[input]))
        return UmiRecipeReport(report, UMI_STATUS_INVALID_ARGUMENT, i,
                               "Stage source input reference is invalid.");
    for (size_t output = 0U; output < stage->output_count; ++output)
      paths[path_count++] = stage->outputs[output];
  }
  for (size_t i = 0U; i < path_count; ++i) {
    if (!PortablePath(paths[i]))
      return UmiRecipeReport(report, UMI_STATUS_INVALID_ARGUMENT, SIZE_MAX,
                             "File claims require canonical portable relative paths.");
    for (size_t j = 0U; j < i; ++j)
      if (Collides(paths[i], paths[j]))
        return UmiRecipeReport(report, UMI_STATUS_ALREADY_EXISTS, SIZE_MAX,
                               "File claims collide by case, equality or ancestor path.");
  }
  /* Every recipe stage is enabled. The canonical registry rejects malformed
   * dependencies/references, and the graph resolves missing IDs and cycles. */
  UmiBuildTaskRegistry *registry = NULL;
  UmiBuildTaskGraph *graph = NULL;
  UmiStatus status = UmiBuildRecipeCreateTasks(recipe, &registry);
  if (status == UMI_STATUS_OK)
    status = umi_build_task_graph_create(&graph);
  if (status == UMI_STATUS_OK)
    status = umi_build_task_graph_rebuild(graph, registry);
  umi_build_task_graph_destroy(graph);
  umi_build_task_registry_destroy(registry);
  return status == UMI_STATUS_OK ? status
                                 : UmiRecipeReport(report, status, SIZE_MAX,
                                                   "Stage tasks contain invalid, duplicate or "
                                                   "unresolved references, or a dependency cycle.");
}
