/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_operations/test_publication_scope_cli.c
 *
 * PURPOSE:
 *   Verify explicit publication exclusions reach preview and fresh apply without any Git
 * operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../../tools/umicom/src/cli.h"
#include "umicom/repository/publication.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static const char *mode;
static unsigned prepares, previews, applies;
static int valid = 1;
UmiStatus umi_cli_context_prepare_operation(UmiCliContext *context, const char *root,
                                            UmiToolchainOperationKind operation) {
  (void)context;
  (void)operation;
  ++prepares;
  if (strcmp(root, "/work"))
    valid = 0;
  return UMI_STATUS_OK;
}
static UmiStatus Scoped(const UmiToolchainProfile *, UmiEnvironmentPlan *,
                        const UmiRepositoryPublicationRequest *,
                        const UmiRepositoryPublicationOptions *, size_t, const char *const *,
                        size_t, UmiRepositoryPublicationReport *, size_t);
#define umi_repository_publication_run_scoped_sized Scoped
#include "../../tools/umicom/src/command_repository_publication.c"
#undef umi_repository_publication_run_scoped_sized
static UmiStatus Scoped(const UmiToolchainProfile *profile, UmiEnvironmentPlan *env,
                        const UmiRepositoryPublicationRequest *request,
                        const UmiRepositoryPublicationOptions *options, size_t option_size,
                        const char *const *names, size_t count,
                        UmiRepositoryPublicationReport *report, size_t report_size) {
  (void)profile;
  (void)env;
  if (option_size != sizeof *options || report_size != sizeof *report || count != 1U ||
      strcmp(names[0], "external"))
    valid = 0;
  if (request->apply) {
    ++applies;
    if (previews != 1U)
      valid = 0;
  } else
    ++previews;
  if (!strcmp(mode, "watch")) {
    if (request->refresh || request->apply)
      valid = 0;
    publication_stop = 1;
  }
  memset(report, 0, sizeof *report);
  report->status = !strcmp(mode, "unmatched-preview") ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK;
  return report->status;
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  mode = argv[1];
  UmiCliContext *context = calloc(1U, sizeof *context);
  if (!context)
    return 1;
  char *args[40] = {"publish-all", "/work", "--exclude", "external"};
  int count = 4, expected = 2;
  unsigned expected_prepares = 0U, expected_previews = 0U, expected_applies = 0U;
  if (!strcmp(mode, "preview") || !strcmp(mode, "apply") || !strcmp(mode, "watch") ||
      !strcmp(mode, "unmatched-preview")) {
    expected = 0;
    expected_prepares = expected_previews = 1U;
    if (!strcmp(mode, "apply")) {
      args[count++] = "--apply";
      expected_applies = 1U;
    }
    if (!strcmp(mode, "watch"))
      args[0] = "watch";
    if (!strcmp(mode, "unmatched-preview")) {
      args[count++] = "--apply";
      expected = 1;
    }
  } else if (!strcmp(mode, "missing-name"))
    count = 3;
  else if (!strcmp(mode, "duplicate")) {
    args[4] = "--exclude";
    args[5] = "external";
    count = 6;
  } else if (!strcmp(mode, "nested"))
    args[3] = "Applications/external";
  else if (!strcmp(mode, "option-name"))
    args[3] = "--apply";
  else if (!strcmp(mode, "too-many")) {
    count = 36;
    for (int i = 2; i < count; i += 2) {
      args[i] = "--exclude";
      args[i + 1] = "external";
    }
  } else if (!strcmp(mode, "unknown"))
    args[2] = "--omit";
  else {
    free(context);
    return 2;
  }
  int actual = umi_cli_command_repository_publication(context, count, args);
  int failed = actual != expected || prepares != expected_prepares ||
               previews != expected_previews || applies != expected_applies || !valid;
  if (failed)
    fprintf(stderr, "scope %s: result %d prepares %u previews %u applies %u valid %d\n", mode,
            actual, prepares, previews, applies, valid);
  free(context);
  return failed;
}
