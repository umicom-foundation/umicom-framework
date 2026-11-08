/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build/test_recipe_boundaries.c
 *
 * PURPOSE:
 *   Verify recipe API output preservation, byte admission and shared file-reader failure
 * boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/recipe.h"
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
static unsigned reads, frees;
static UmiStatus FailedRead(const char *path, size_t maximum, unsigned char **out, size_t *size) {
  (void)path;
  ++reads;
  if (maximum != UMI_BUILD_RECIPE_MAX_BYTES)
    return UMI_STATUS_INTERNAL_ERROR;
  *out = NULL;
  *size = 0U;
  return UMI_STATUS_IO_ERROR;
}
static void CountFree(void *data) {
  ++frees;
  UmiInputFileFree(data);
}
#define UmiInputFileRead FailedRead
#define UmiInputFileFree CountFree
#define UmiBuildRecipeReadFile ControlledRead
#include "../../src/build/recipe_file.c"
#undef UmiInputFileRead
#undef UmiInputFileFree
#undef UmiBuildRecipeReadFile
int main(int argc, char **argv) {
  if (argc != 3)
    return 2;
  const char *mode = argv[1];
  unsigned char *bytes = NULL;
  size_t size = 0U;
  CHECK(UmiInputFileRead(argv[2], UMI_BUILD_RECIPE_MAX_BYTES, &bytes, &size) == UMI_STATUS_OK);
  UmiBuildRecipe *owner = NULL;
  CHECK(UmiBuildRecipeOpen(bytes, size, &owner, NULL) == UMI_STATUS_OK);
  UmiBuildRecipe *output = owner;
  UmiBuildRecipeReport report;
  UmiStatus status = UMI_STATUS_INTERNAL_ERROR, expected = UMI_STATUS_INVALID_ARGUMENT;
  if (!strcmp(mode, "null-bytes"))
    status = UmiBuildRecipeOpen(NULL, size, &output, &report);
  else if (!strcmp(mode, "zero-bytes"))
    status = UmiBuildRecipeOpen(bytes, 0U, &output, &report);
  else if (!strcmp(mode, "null-output"))
    status = UmiBuildRecipeOpen(bytes, size, NULL, &report);
  else if (!strcmp(mode, "null-info")) {
    UmiBuildRecipeInfo value, before;
    memset(&value, 0x3a, sizeof value);
    before = value;
    CHECK(UmiBuildRecipeInfoRead(NULL, &value) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&before, &value, sizeof value) == 0);
    CHECK(UmiBuildRecipeInfoRead(owner, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    goto done;
  } else if (!strcmp(mode, "null-entry")) {
    CHECK(UmiBuildRecipeInputAt(NULL, 0U) == NULL && UmiBuildRecipeStageAt(NULL, 0U) == NULL);
    UmiBuildRecipeDestroy(NULL);
    goto done;
  } else if (!strcmp(mode, "null-registry")) {
    UmiBuildTaskRegistry *registry = NULL;
    CHECK(umi_build_task_registry_create(&registry) == UMI_STATUS_OK);
    UmiBuildTaskRegistry *saved = registry;
    CHECK(UmiBuildRecipeCreateTasks(NULL, &registry) == UMI_STATUS_INVALID_ARGUMENT &&
          registry == saved);
    CHECK(UmiBuildRecipeCreateTasks(owner, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    umi_build_task_registry_destroy(registry);
    goto done;
  } else if (!strcmp(mode, "byte-limit") || !strcmp(mode, "byte-excess")) {
    size_t length = UMI_BUILD_RECIPE_MAX_BYTES + (!strcmp(mode, "byte-excess") ? 1U : 0U);
    unsigned char *large = malloc(length);
    CHECK(large != NULL && size < length);
    memset(large, ' ', length);
    memcpy(large, bytes, size);
    expected = length > UMI_BUILD_RECIPE_MAX_BYTES ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
    status = UmiBuildRecipeOpen(large, length, &output, &report);
    free(large);
  } else if (!strcmp(mode, "invalid-utf8") || !strcmp(mode, "literal-nul")) {
    unsigned char *p = (unsigned char *)strstr((char *)bytes, "source-demo");
    CHECK(p != NULL);
    p[0] = !strcmp(mode, "invalid-utf8") ? 0xffU : 0U;
    expected = UMI_STATUS_PARSE_ERROR;
    status = UmiBuildRecipeOpen(bytes, size, &output, &report);
  } else if (!strcmp(mode, "file-error")) {
    expected = UMI_STATUS_IO_ERROR;
    status = ControlledRead("chosen-file", &output, &report);
    CHECK(reads == 1U && frees == 1U);
  } else if (!strcmp(mode, "file-null-output")) {
    status = ControlledRead("chosen-file", NULL, &report);
    CHECK(reads == 0U && frees == 0U);
  } else
    return 2;
  CHECK(status == expected && report.status == status && report.message[0]);
  if (status == UMI_STATUS_OK) {
    CHECK(output != owner);
    UmiBuildRecipeDestroy(output);
  } else
    CHECK(output == owner);
done:
  UmiBuildRecipeDestroy(owner);
  UmiInputFileFree(bytes);
  return 0;
}
