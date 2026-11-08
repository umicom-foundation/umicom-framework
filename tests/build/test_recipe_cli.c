/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build/test_recipe_cli.c
 *
 * PURPOSE:
 *   Verify complete recipe command admission before its shared file reader is invoked.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/recipe.h"
#include <stdio.h>
#include <string.h>
static unsigned reads;
static UmiStatus Read(const char *path, UmiBuildRecipe **out, UmiBuildRecipeReport *report) {
  (void)path;
  (void)out;
  ++reads;
  memset(report, 0, sizeof *report);
  report->status = UMI_STATUS_IO_ERROR;
  strcpy(report->message, "Controlled reader refusal");
  return UMI_STATUS_IO_ERROR;
}
#define UmiBuildRecipeReadFile Read
#include "../../tools/umicom/src/command_recipe.c"
#undef UmiBuildRecipeReadFile
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                   \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
#ifdef _WIN32
  char file[] = "C:/explicit/recipe.json";
#else
  char file[] = "/explicit/recipe.json";
#endif
  char *args[] = {"inspect", "--file", file, "--apply"};
  int length = 3, expected = 2;
  const char *mode = argv[1];
  if (!strcmp(mode, "valid"))
    expected = 1;
  else if (!strcmp(mode, "help")) {
    args[0] = "--help";
    length = 1;
    expected = 0;
  } else if (!strcmp(mode, "short-help")) {
    args[0] = "-h";
    length = 1;
    expected = 0;
  } else if (!strcmp(mode, "empty"))
    length = 0;
  else if (!strcmp(mode, "negative"))
    length = -1;
  else if (!strcmp(mode, "null-vector")) {
    CHECK(umi_cli_command_recipe(3, NULL) == 2 && reads == 0U);
    return 0;
  } else if (!strcmp(mode, "null-item"))
    args[2] = NULL;
  else if (!strcmp(mode, "empty-item"))
    args[2] = "";
  else if (!strcmp(mode, "unknown-action"))
    args[0] = "execute";
  else if (!strcmp(mode, "unknown-option"))
    args[1] = "--command";
  else if (!strcmp(mode, "relative-file"))
    args[2] = "recipe.json";
  else if (!strcmp(mode, "missing-file"))
    length = 2;
  else if (!strcmp(mode, "extra-option"))
    length = 4;
  else if (!strcmp(mode, "help-mixed"))
    args[0] = "--help";
  else
    return 2;
  CHECK(umi_cli_command_recipe(length, args) == expected);
  CHECK(reads == (expected == 1 ? 1U : 0U));
  return 0;
}
