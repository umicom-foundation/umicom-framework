/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/launch_inspection.h
 * PURPOSE: Report launch-file observations without launching or adopting a project.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_LAUNCH_INSPECTION_H
#define UMICOM_BUILD_LAUNCH_INSPECTION_H
#include "umicom/build/profile.h"
#include "umicom/platform/directory.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiBuildLaunchFile {
  char path[UMI_BUILD_PATH_CAPACITY];
  UmiStatus status;
  UmiFileKind kind;
  uint64_t size;
  bool inspected;
  bool expected_kind;
} UmiBuildLaunchFile;

typedef struct UmiBuildLaunchInspection {
  UmiBuildLaunchFile run_program;
  UmiBuildLaunchFile debug_program;
  UmiBuildLaunchFile working_directory;
  bool run_uses_program_lookup;
} UmiBuildLaunchInspection;

/** @brief Inspect resolved launch paths on the calling worker, without executing.
 * An OK return means the observations were collected, not that launching will
 * succeed. Each entry retains its own filesystem status and file kind.
 * Bare Run names are deliberately not searched through PATH; their entry is
 * uninspected with UNAVAILABLE status. Native Debug still checks its explicit
 * project-relative path. Symbolic links retain the platform's reported kind.
 * No permission, executable format, dependency, trust or debugger check occurs.
 * Files can change after inspection. Cancellation is cooperative between stat
 * calls; an operating-system call cannot be interrupted. Failure retains out.
 * The caller must provide the same absolute project root used for launch. */
UmiStatus UmiBuildLaunchInspect(const UmiBuildProfile *profile,
                                const char *absoluteProjectDirectory,
                                const UmiCancellationToken *cancel, UmiBuildLaunchInspection *out);
#ifdef __cplusplus
}
#endif
#endif
