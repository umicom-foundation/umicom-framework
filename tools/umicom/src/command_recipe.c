/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_recipe.c
 *
 * PURPOSE:
 *   Present read-only source-build recipe inspection through Framework without stage execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "cli.h"
#include "umicom/build/recipe.h"
#include "umicom/platform/output_file.h"
#include <stdio.h>
#include <string.h>
int umi_cli_command_recipe(int argc, char **argv) {
  if (argc < 0 || (argc != 0 && argv == NULL))
    return 2;
  for (int i = 0; i < argc; ++i)
    if (argv[i] == NULL || !argv[i][0])
      return 2;
  if (argc == 1 && (!strcmp(argv[0], "--help") || !strcmp(argv[0], "-h"))) {
    puts("Usage: umicom recipe inspect --file ABSOLUTE_JSON_FILE\n"
         "Inspect bounded recipe metadata. No stage is executed; file/toolchain claims are "
         "unverified.");
    return 0;
  }
  /* Complete admission precedes the single shared file read. No arbitrary
   * command, shell, network, publication or execution option is accepted. */
  if (argc != 3 || strcmp(argv[0], "inspect") || strcmp(argv[1], "--file") ||
      UmiOutputFileValidatePath(argv[2]) != UMI_STATUS_OK)
    return 2;
  UmiBuildRecipe *recipe = NULL;
  UmiBuildRecipeReport report;
  UmiStatus status = UmiBuildRecipeReadFile(argv[2], &recipe, &report);
  if (status != UMI_STATUS_OK) {
    fprintf(stderr, "Recipe refused: %s (%s)\n", report.message, umi_status_text(status));
    return 1;
  }
  UmiBuildRecipeInfo info;
  status = UmiBuildRecipeInfoRead(recipe, &info);
  if (status == UMI_STATUS_OK) {
    printf("Recipe: %s\nSHA-256: %s\nBuild: %s\nHost: %s\nTarget: %s\n"
           "Toolchain: %s\nInputs: %zu | Stages: %zu\n",
           info.id, info.recipe_sha256, info.build, info.host, info.target, info.toolchain_id,
           info.input_count, info.stage_count);
    for (size_t i = 0U; i < info.stage_count; ++i) {
      const UmiBuildRecipeStage *stage = UmiBuildRecipeStageAt(recipe, i);
      printf("Stage %s: %s | phase %u | dependencies %zu | inputs %zu | outputs %zu\n", stage->id,
             stage->label, (unsigned)stage->phase, stage->dependency_count, stage->input_count,
             stage->output_count);
    }
    puts(
        "Metadata accepted. Sources, toolchain and outputs remain unverified; no stages executed.");
    if (ferror(stdout))
      status = UMI_STATUS_IO_ERROR;
  }
  UmiBuildRecipeDestroy(recipe);
  return status == UMI_STATUS_OK ? 0 : 1;
}
