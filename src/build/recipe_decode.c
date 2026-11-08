/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/recipe_decode.c
 *
 * PURPOSE:
 *   Decode the closed source-build recipe schema through the shared owned JSON tree service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "recipe_internal.h"
#include <string.h>

/* Schema policy is deliberately closed: recognizing selected fields while
 * ignoring command-like extensions would make inspection disagree with later
 * execution. Decoded-key checks also catch escaped duplicate spellings. */
static UmiStatus Keys(const UmiJsonTree *tree, int object, const char *const *names, size_t count) {
  if (UmiJsonTreeKind(tree, object) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    return UMI_STATUS_PARSE_ERROR;
  uint32_t seen = 0U;
  for (int key = UmiJsonTreeFirst(tree, object); key >= 0;) {
    char name[64];
    UmiStatus status = UmiJsonTreeText(tree, key, name, sizeof name);
    if (status != UMI_STATUS_OK)
      return status;
    size_t index = 0U;
    while (index < count && strcmp(name, names[index]) != 0)
      ++index;
    if (index == count)
      return UMI_STATUS_PARSE_ERROR;
    uint32_t bit = UINT32_C(1) << index;
    if ((seen & bit) != 0U)
      return UMI_STATUS_ALREADY_EXISTS;
    seen |= bit;
    int value = UmiJsonTreeNext(tree, key);
    if (value < 0)
      return UMI_STATUS_PARSE_ERROR;
    key = UmiJsonTreeNext(tree, value);
  }
  return seen == (UINT32_C(1) << count) - 1U ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}
static UmiStatus Member(const UmiJsonTree *tree, int object, const char *name, int *value) {
  return UmiJsonTreeMember(tree, object, name, value);
}
static UmiStatus Text(const UmiJsonTree *tree, int object, const char *name, char *out,
                      size_t capacity) {
  int node;
  UmiStatus status = Member(tree, object, name, &node);
  if (status != UMI_STATUS_OK)
    return status;
  if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
    return UMI_STATUS_PARSE_ERROR;
  return UmiJsonTreeText(tree, node, out, capacity);
}
static UmiStatus Number(const UmiJsonTree *tree, int object, const char *name, int64_t *out) {
  int node;
  UmiStatus status = Member(tree, object, name, &node);
  return status == UMI_STATUS_OK ? UmiJsonTreeInteger(tree, node, out) : status;
}
static UmiStatus Array(const UmiJsonTree *tree, int object, const char *name, size_t minimum,
                       size_t maximum, int *out, size_t *count) {
  UmiStatus status = Member(tree, object, name, out);
  if (status != UMI_STATUS_OK)
    return status;
  if (UmiJsonTreeKind(tree, *out) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
    return UMI_STATUS_PARSE_ERROR;
  *count = UmiJsonTreeCount(tree, *out);
  if (*count > maximum)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  return *count >= minimum ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}
static UmiStatus TextArray(const UmiJsonTree *tree, int object, const char *name, size_t minimum,
                           size_t maximum, char *out, size_t stride, size_t *count) {
  int array;
  UmiStatus status = Array(tree, object, name, minimum, maximum, &array, count);
  if (status != UMI_STATUS_OK)
    return status;
  size_t i = 0U;
  for (int node = UmiJsonTreeFirst(tree, array); node >= 0; node = UmiJsonTreeNext(tree, node)) {
    if (i >= *count || UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
      return UMI_STATUS_PARSE_ERROR;
    status = UmiJsonTreeText(tree, node, out + i * stride, stride);
    if (status != UMI_STATUS_OK)
      return status;
    ++i;
  }
  return i == *count ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}
static UmiStatus Phase(const char *text, UmiBuildPhase *phase) {
  static const struct {
    const char *name;
    UmiBuildPhase value;
  } phases[] = {{"configure", UMI_BUILD_PHASE_CONFIGURE}, {"build", UMI_BUILD_PHASE_BUILD},
                {"test", UMI_BUILD_PHASE_TEST},           {"install", UMI_BUILD_PHASE_INSTALL},
                {"package", UMI_BUILD_PHASE_PACKAGE},     {"deploy", UMI_BUILD_PHASE_DEPLOY}};
  for (size_t i = 0U; i < sizeof phases / sizeof phases[0]; ++i)
    if (strcmp(text, phases[i].name) == 0) {
      *phase = phases[i].value;
      return UMI_STATUS_OK;
    }
  return UMI_STATUS_PARSE_ERROR;
}
UmiStatus UmiRecipeDecode(const UmiJsonTree *tree, UmiBuildRecipe *recipe,
                          UmiBuildRecipeReport *report) {
  static const char *const root_keys[] = {"schema", "id",        "build",  "host",
                                          "target", "toolchain", "inputs", "stages"};
  static const char *const tool_keys[] = {"id", "compiler", "manifest", "sha256"};
  static const char *const input_keys[] = {"id", "path", "sha256", "bytes"};
  static const char *const stage_keys[] = {"id",           "label",  "phase",
                                           "dependencies", "inputs", "outputs"};
  UmiStatus status;
  size_t stage_index = SIZE_MAX;
  const char *reason = "Recipe root members or schema are invalid.";
#define READ(call)                                                                                 \
  do {                                                                                             \
    status = (call);                                                                               \
    if (status != UMI_STATUS_OK)                                                                   \
      goto failed;                                                                                 \
  } while (0)
  READ(Keys(tree, 0, root_keys, sizeof root_keys / sizeof root_keys[0]));
  int64_t schema;
  READ(Number(tree, 0, "schema", &schema));
  if (schema != UMI_BUILD_RECIPE_SCHEMA) {
    status = UMI_STATUS_PARSE_ERROR;
    goto failed;
  }
  READ(Text(tree, 0, "id", recipe->info.id, sizeof recipe->info.id));
  READ(Text(tree, 0, "build", recipe->info.build, sizeof recipe->info.build));
  READ(Text(tree, 0, "host", recipe->info.host, sizeof recipe->info.host));
  READ(Text(tree, 0, "target", recipe->info.target, sizeof recipe->info.target));
  reason = "Toolchain members or text values are invalid.";
  int tool;
  READ(Member(tree, 0, "toolchain", &tool));
  READ(Keys(tree, tool, tool_keys, sizeof tool_keys / sizeof tool_keys[0]));
  READ(Text(tree, tool, "id", recipe->info.toolchain_id, sizeof recipe->info.toolchain_id));
  READ(Text(tree, tool, "compiler", recipe->info.compiler, sizeof recipe->info.compiler));
  READ(Text(tree, tool, "manifest", recipe->info.toolchain_manifest,
            sizeof recipe->info.toolchain_manifest));
  READ(Text(tree, tool, "sha256", recipe->info.toolchain_sha256,
            sizeof recipe->info.toolchain_sha256));
  reason =
      "Source inputs require bounded IDs, paths, SHA-256 claims and nonnegative integer sizes.";
  int inputs;
  READ(Array(tree, 0, "inputs", 1U, UMI_BUILD_RECIPE_MAX_INPUTS, &inputs,
             &recipe->info.input_count));
  size_t i = 0U;
  for (int node = UmiJsonTreeFirst(tree, inputs); node >= 0; node = UmiJsonTreeNext(tree, node)) {
    if (i >= recipe->info.input_count) {
      status = UMI_STATUS_PARSE_ERROR;
      goto failed;
    }
    UmiBuildRecipeInput *input = &recipe->inputs[i++];
    READ(Keys(tree, node, input_keys, sizeof input_keys / sizeof input_keys[0]));
    READ(Text(tree, node, "id", input->id, sizeof input->id));
    READ(Text(tree, node, "path", input->path, sizeof input->path));
    READ(Text(tree, node, "sha256", input->sha256, sizeof input->sha256));
    int64_t bytes;
    READ(Number(tree, node, "bytes", &bytes));
    if (bytes < 0) {
      status = UMI_STATUS_PARSE_ERROR;
      goto failed;
    }
    input->bytes = (uint64_t)bytes;
  }
  if (i != recipe->info.input_count) {
    status = UMI_STATUS_PARSE_ERROR;
    goto failed;
  }
  reason = "Stages require bounded semantic phases, dependencies, source references and outputs.";
  int stages;
  READ(Array(tree, 0, "stages", 1U, UMI_BUILD_RECIPE_MAX_STAGES, &stages,
             &recipe->info.stage_count));
  i = 0U;
  for (int node = UmiJsonTreeFirst(tree, stages); node >= 0; node = UmiJsonTreeNext(tree, node)) {
    if (i >= recipe->info.stage_count) {
      status = UMI_STATUS_PARSE_ERROR;
      goto failed;
    }
    stage_index = i;
    UmiBuildRecipeStage *stage = &recipe->stages[i++];
    char phase[32];
    READ(Keys(tree, node, stage_keys, sizeof stage_keys / sizeof stage_keys[0]));
    READ(Text(tree, node, "id", stage->id, sizeof stage->id));
    READ(Text(tree, node, "label", stage->label, sizeof stage->label));
    READ(Text(tree, node, "phase", phase, sizeof phase));
    READ(Phase(phase, &stage->phase));
    READ(TextArray(tree, node, "dependencies", 0U, UMI_BUILD_TASK_DEPENDENCY_CAPACITY,
                   &stage->dependencies[0][0], sizeof stage->dependencies[0],
                   &stage->dependency_count));
    READ(TextArray(tree, node, "inputs", 0U, UMI_BUILD_TASK_INPUT_CAPACITY, &stage->inputs[0][0],
                   sizeof stage->inputs[0], &stage->input_count));
    READ(TextArray(tree, node, "outputs", 1U, UMI_BUILD_TASK_OUTPUT_CAPACITY, &stage->outputs[0][0],
                   sizeof stage->outputs[0], &stage->output_count));
  }
  if (i != recipe->info.stage_count) {
    status = UMI_STATUS_PARSE_ERROR;
    goto failed;
  }
#undef READ
  return UMI_STATUS_OK;
failed:
  return UmiRecipeReport(report, status, stage_index, reason);
}
