/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/build.h
 *
 * PURPOSE:
 *   Configure, build, test, clean, install, package, run and deliver native
 *   projects through one validated toolchain and isolated child environment.
 *
 * ARCHITECTURE:
 *   This is the execution boundary used by higher Framework build/task models.
 *   Applications such as Umicom Studio compose this service; they do not spawn
 *   CMake, CTest or application processes directly.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_BUILD_H
#define UMICOM_TOOLCHAIN_BUILD_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/platform/process.h"
#include "umicom/toolchain/environment.h"
#include "umicom/toolchain/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_TEXT_CAPACITY 2048U

/*
 * Existing numeric values are retained. New lifecycle actions are appended so
 * callers compiled against the established configure/build/test/run/make
 * contract keep the same values.
 */
typedef enum UmiBuildAction {
    UMI_BUILD_CONFIGURE = 1,
    UMI_BUILD_COMPILE = 2,
    UMI_BUILD_TEST = 3,
    UMI_BUILD_RUN = 4,
    UMI_BUILD_MAKE = 5,
    UMI_BUILD_CLEAN = 6,
    UMI_BUILD_INSTALL = 7,
    UMI_BUILD_PACKAGE = 8,
    UMI_BUILD_DELIVER = 9,
    UMI_BUILD_COMMAND = 10
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

    /*
     * Appended lifecycle fields preserve the original structure prefix while
     * making install/package execution and supervised cancellation explicit.
     */
    const char *configuration;
    const char *install_prefix;
    const char *package_target;
    uint32_t timeout_ms;
    const UmiCancellationToken *cancellation;
    UmiProcessWindowMode window_mode;
} UmiBuildRequest;

typedef struct UmiBuildReport {
    int configure_exit_code;
    int build_exit_code;
    int test_exit_code;
    int run_exit_code;
    char last_output[UMI_PROCESS_OUTPUT_CAPACITY];

    /* Appended execution evidence for the extended lifecycle. */
    int install_exit_code;
    int package_exit_code;
    UmiBuildAction last_action;
    UmiStatus last_status;
    int cancelled;
    int timed_out;
    int output_truncated;
    uint64_t duration_ms;
} UmiBuildReport;

void umi_build_request_init(UmiBuildRequest *request);
void umi_build_report_init(UmiBuildReport *report);

UmiStatus umi_build_request_validate(UmiBuildAction action,
                                     const UmiBuildRequest *request,
                                     char *out_message,
                                     size_t message_capacity);

const char *umi_build_action_text(UmiBuildAction action);

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
