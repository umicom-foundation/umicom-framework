/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/build.h
 *
 * PURPOSE:
 *   Configure, build, test, run, repair, and open prepared shells through one
 *   native service that uses a validated toolchain and isolated child process
 *   environment.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_BUILD_H
#define UMICOM_TOOLCHAIN_BUILD_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/toolchain/environment.h"
#include "umicom/toolchain/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_TEXT_CAPACITY 2048U

typedef enum UmiBuildAction {
    UMI_BUILD_CONFIGURE = 1,
    UMI_BUILD_COMPILE = 2,
    UMI_BUILD_TEST = 3,
    UMI_BUILD_RUN = 4,
    UMI_BUILD_MAKE = 5
} UmiBuildAction;

typedef struct UmiBuildRequest {
    const char *source_root;
    const char *build_directory;
    const char *preset;
    const char *target;
    const char *executable;
    const char *const *run_arguments;
    size_t run_argument_count;
    int jobs;
    int clean;
} UmiBuildRequest;

typedef struct UmiBuildReport {
    int configure_exit_code;
    int build_exit_code;
    int test_exit_code;
    int run_exit_code;
    char last_output[UMI_PROCESS_OUTPUT_CAPACITY];
} UmiBuildReport;

UmiStatus umi_build_execute(const UmiToolchainProfile *profile,
                            UmiEnvironmentPlan *environment,
                            UmiBuildAction action,
                            const UmiBuildRequest *request,
                            UmiBuildReport *out_report);
UmiStatus umi_build_repair_cache(const UmiToolchainProfile *profile,
                                 const char *build_directory,
                                 char *out_recovery_path,
                                 size_t capacity,
                                 int dry_run);
UmiStatus umi_build_write_user_presets(const UmiToolchainProfile *profile,
                                       const char *project_root,
                                       const char *path);
UmiStatus umi_build_open_shell(const UmiToolchainProfile *profile,
                               UmiEnvironmentPlan *environment,
                               const char *working_directory,
                               const char *shell_program);

#ifdef __cplusplus
}
#endif

#endif
