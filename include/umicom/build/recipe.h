/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/recipe.h
 *
 * PURPOSE:
 *   Inspect immutable source-build recipe metadata and project validated stages into existing
 * Framework tasks without executing them.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_RECIPE_H
#define UMICOM_BUILD_RECIPE_H
#include "umicom/build/task.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_BUILD_RECIPE_SCHEMA 1U
#define UMI_BUILD_RECIPE_MAX_BYTES 1048576U
#define UMI_BUILD_RECIPE_MAX_INPUTS 64U
#define UMI_BUILD_RECIPE_MAX_STAGES 32U
#define UMI_BUILD_RECIPE_PATH_CAPACITY 512U
#define UMI_BUILD_RECIPE_TRIPLE_CAPACITY 96U
#define UMI_BUILD_RECIPE_DIGEST_CAPACITY 65U

typedef struct UmiBuildRecipe UmiBuildRecipe;

typedef struct UmiBuildRecipeInput {
  char id[UMI_BUILD_ID_CAPACITY];
  char path[UMI_BUILD_RECIPE_PATH_CAPACITY];
  char sha256[UMI_BUILD_RECIPE_DIGEST_CAPACITY];
  uint64_t bytes;
} UmiBuildRecipeInput;

typedef struct UmiBuildRecipeStage {
  char id[UMI_BUILD_ID_CAPACITY];
  char label[UMI_BUILD_NAME_CAPACITY];
  UmiBuildPhase phase;
  char dependencies[UMI_BUILD_TASK_DEPENDENCY_CAPACITY][UMI_BUILD_ID_CAPACITY];
  size_t dependency_count;
  char inputs[UMI_BUILD_TASK_INPUT_CAPACITY][UMI_BUILD_ID_CAPACITY];
  size_t input_count;
  char outputs[UMI_BUILD_TASK_OUTPUT_CAPACITY][UMI_BUILD_RECIPE_PATH_CAPACITY];
  size_t output_count;
} UmiBuildRecipeStage;

typedef struct UmiBuildRecipeInfo {
  char id[UMI_BUILD_ID_CAPACITY];
  char build[UMI_BUILD_RECIPE_TRIPLE_CAPACITY];
  char host[UMI_BUILD_RECIPE_TRIPLE_CAPACITY];
  char target[UMI_BUILD_RECIPE_TRIPLE_CAPACITY];
  char toolchain_id[UMI_BUILD_ID_CAPACITY];
  char compiler[UMI_BUILD_RECIPE_PATH_CAPACITY];
  char toolchain_manifest[UMI_BUILD_RECIPE_PATH_CAPACITY];
  char toolchain_sha256[UMI_BUILD_RECIPE_DIGEST_CAPACITY];
  char recipe_sha256[UMI_BUILD_RECIPE_DIGEST_CAPACITY];
  size_t input_count;
  size_t stage_count;
} UmiBuildRecipeInfo;

typedef struct UmiBuildRecipeReport {
  UmiStatus status;
  /* SIZE_MAX means the diagnostic does not identify one stage. */
  size_t stage_index;
  char message[256];
} UmiBuildRecipeReport;

/* Link Umicom::developer, the existing owner of bounded JSON interpretation.
 * Copy and validate exactly size bytes (no trailing NUL required). This reads
 * no file, starts no process and interprets no shell syntax. Unknown and
 * duplicate decoded members are refused, including command/shell fields.
 * Failure preserves *out_recipe; optional report receives a bounded reason.
 * Success transfers one immutable owner; destroy it once after all borrowers.
 * Raw recipe SHA-256 identifies exact input bytes, not semantic equivalence.
 * Source sizes/digests and toolchain manifest digests remain unverified claims.
 * Triple syntax does not establish an ABI or installed compiler capability. */
UmiStatus UmiBuildRecipeOpen(const void *data, size_t size, UmiBuildRecipe **out_recipe,
                             UmiBuildRecipeReport *report);
/* Read one explicit ordinary absolute file through the shared bounded reader.
 * Final symlinks/devices are refused; parent aliases are not a confinement
 * boundary. No referenced input/toolchain file is opened. Failure preserves
 * *out_recipe; I/O may block and belongs on an interactive host's worker. */
UmiStatus UmiBuildRecipeReadFile(const char *absolute_path, UmiBuildRecipe **out_recipe,
                                 UmiBuildRecipeReport *report);
void UmiBuildRecipeDestroy(UmiBuildRecipe *recipe);
/* Copy info on success; preserve caller output on invalid arguments. */
UmiStatus UmiBuildRecipeInfoRead(const UmiBuildRecipe *recipe, UmiBuildRecipeInfo *out_info);
/* Borrow immutable entries until Destroy. Invalid index/owner returns NULL. */
const UmiBuildRecipeInput *UmiBuildRecipeInputAt(const UmiBuildRecipe *recipe, size_t index);
const UmiBuildRecipeStage *UmiBuildRecipeStageAt(const UmiBuildRecipe *recipe, size_t index);
/* Create a separately owned registry of semantic build-phase tasks, with no
 * command text. Inputs resolve to declared source paths; outputs retain their
 * canonical relative paths. Dependencies were checked by the shared graph.
 * Failure preserves *out_registry. Destroy successful output with the existing
 * task-registry API. Execution still requires an independently configured and
 * authorized provider; accepting a recipe does not authorize any operation. */
UmiStatus UmiBuildRecipeCreateTasks(const UmiBuildRecipe *recipe,
                                    UmiBuildTaskRegistry **out_registry);
#ifdef __cplusplus
}
#endif
#endif
