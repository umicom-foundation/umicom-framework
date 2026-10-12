/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/launch_review/test_inspection.c
 * PURPOSE: Verify launch-file checks never search PATH or approve executable contents.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/build/launch_inspection.h"

int main(int argc, char **argv) {
  CHECK(argc == 2);
  char directory[UMI_PATH_CAPACITY], program[UMI_PATH_CAPACITY];
  FixtureDirectory(directory);
  FixturePath(program, directory, "caf\xc3\xa9 application.exe");
  UmiOutputFile *file = NULL;
  CHECK(UmiOutputFileCreate(program, &file) == UMI_STATUS_OK);
  /* The contents deliberately are not an executable. This operation checks
   * existence and kind, and must never imply that a loader accepted the file. */
  CHECK(UmiOutputFileWrite(file, "plain text", 10U) == UMI_STATUS_OK);
  CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK);
  UmiOutputFileDestroy(file);
  UmiBuildProfile profile;
  umi_build_profile_init(&profile);
  strcpy(profile.source_directory, directory);
  strcpy(profile.run_program, program);
  UmiBuildLaunchInspection result;
  memset(&result, 0x5a, sizeof(result));
  unsigned char previous[sizeof(result)];
  memcpy(previous, &result, sizeof(result));
  UmiCancellationToken *cancel = NULL;
  if (strcmp(argv[1], "invalid") == 0) {
    profile.run_program[0] = '\0';
    CHECK(UmiBuildLaunchInspect(&profile, directory, NULL, &result) != UMI_STATUS_OK);
    CHECK(memcmp(previous, &result, sizeof(result)) == 0);
  } else if (strcmp(argv[1], "cancelled") == 0) {
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    umi_cancellation_token_request(cancel);
    CHECK(UmiBuildLaunchInspect(&profile, directory, cancel, &result) == UMI_STATUS_CANCELLED);
    CHECK(memcmp(previous, &result, sizeof(result)) == 0);
  } else {
    if (strcmp(argv[1], "missing") == 0)
      strcpy(profile.run_program, "./not-created.exe");
    else if (strcmp(argv[1], "wrong-kind") == 0) {
      strcpy(profile.run_program, directory);
      strcpy(profile.run_working_directory, program);
    } else if (strcmp(argv[1], "bare") == 0)
      strcpy(profile.run_program, "not-created.exe");
    else if (strcmp(argv[1], "relative") == 0)
      strcpy(profile.run_program, "./caf\xc3\xa9 application.exe");
    else
      CHECK(strcmp(argv[1], "regular") == 0);
    CHECK(UmiBuildLaunchInspect(&profile, directory, NULL, &result) == UMI_STATUS_OK);
    if (strcmp(argv[1], "missing") == 0) {
      CHECK(result.run_program.inspected && result.debug_program.inspected);
      CHECK(result.run_program.status == UMI_STATUS_NOT_FOUND);
      CHECK(result.debug_program.status == UMI_STATUS_NOT_FOUND);
      CHECK(result.working_directory.expected_kind);
    } else if (strcmp(argv[1], "bare") == 0) {
      CHECK(result.run_uses_program_lookup && !result.run_program.inspected);
      CHECK(result.run_program.status == UMI_STATUS_UNAVAILABLE);
      CHECK(result.debug_program.inspected && result.debug_program.status == UMI_STATUS_NOT_FOUND);
    } else if (strcmp(argv[1], "wrong-kind") == 0) {
      CHECK(result.run_program.kind == UMI_FILE_KIND_DIRECTORY &&
            !result.run_program.expected_kind);
      CHECK(result.working_directory.kind == UMI_FILE_KIND_REGULAR &&
            !result.working_directory.expected_kind);
    } else {
      CHECK(!result.run_uses_program_lookup && result.run_program.inspected);
      CHECK(result.run_program.status == UMI_STATUS_OK && result.run_program.expected_kind);
      CHECK(result.run_program.size == 10U && result.debug_program.expected_kind);
      CHECK(result.working_directory.expected_kind);
      CHECK(umi_path_equal(result.run_program.path, program));
    }
  }
  umi_cancellation_token_destroy(cancel);
  size_t length = 0U;
  unsigned char *bytes = FixtureRead(program, &length);
  CHECK(length == 10U && memcmp(bytes, "plain text", length) == 0);
  free(bytes);
  return 0;
}
