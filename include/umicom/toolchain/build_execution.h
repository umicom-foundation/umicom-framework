/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/build_execution.h
 *
 * PURPOSE:
 *   Stream, preview and record native development commands without changing legacy request layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TOOLCHAIN_BUILD_EXECUTION_H
#define UMICOM_TOOLCHAIN_BUILD_EXECUTION_H
#include "umicom/toolchain/build.h"
#include "umicom/platform/output_file.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Observers borrow arguments only for the call. A preview reports commands but
 * never calls the executor. Keep the request, environment and observer alive
 * until execution returns; there is no global callback or process cwd change. */
typedef void (*UmiBuildCommandObserver)(UmiBuildAction action,
    const UmiProcessRequest *command, void *context);
typedef UmiStatus (*UmiBuildProcessExecutor)(const UmiProcessRequest *command,
    UmiProcessOutputObserver observer, void *observer_context,
    UmiProcessResult *result, void *executor_context);

typedef struct UmiBuildExecutionOptions {
    int preview;
    /* An explicitly selected binary directory overrides preset binaryDir for
     * configure, build, test and package. Without this flag CMake owns presets. */
    int explicit_build_directory;
    const char *const *configure_definitions; /* NAME=VALUE, without -D. */
    size_t configure_definition_count;
    UmiProcessOutputObserver output;
    UmiBuildCommandObserver command;
    void *observer_context;
    /* NULL uses the native process runner. Hosts can supply a test transport;
     * it must stream all output and preserve cancellation and exit evidence. */
    UmiBuildProcessExecutor executor;
    void *executor_context;
    /* Optional literal native-build arguments follow CMake's -- separator.
     * The caller chooses arguments appropriate to its configured generator.
     * These apply to build/package phases only, with at most 16 values. */
    const char *const *build_arguments;
    size_t build_argument_count;
    /* Absolute, distinct CTest-owned output paths. CTest may overwrite them;
     * use a new qualification directory to retain earlier evidence. These are
     * separate from the exclusively created raw stream log. */
    const char *test_output_log;
    const char *test_output_junit;
} UmiBuildExecutionOptions;

typedef struct UmiBuildExecutionLog {
    UmiStatus execution_status;
    UmiStatus log_status;
    int created;
    int capture_complete;
    UmiOutputFileSnapshot file;
} UmiBuildExecutionLog;

/** Validate request and execution options without creating logs or invoking
 * observers, tools or filesystem operations. CLI and native clients share this
 * admission boundary. Configure definitions apply only to configure/make/deliver.
 * Borrowed argument strings must remain alive until execution returns. */
UmiStatus UmiBuildExecutionValidate(UmiBuildAction action,
    const UmiBuildRequest *request, const UmiBuildExecutionOptions *options,
    char *message, size_t message_capacity);

/** Validate the complete action sequence before side effects. Reuses request/
 * option admission, checks profile compatibility, child-window mode, the full
 * optional environment and every CMake/CTest slot the sequence will consume.
 * Execution requires nonempty validated tool paths; preview permits missing
 * tools with descriptive default names but refuses malformed supplied fields.
 * No allocation, filesystem access, observer, tool probe or process runs here.
 * Relative program names retain their existing PATH-resolution semantics.
 * Metadata admission is not executable discovery or a successful build. Inputs
 * must remain stable through the subsequent call, including observer callbacks. */
UmiStatus UmiBuildExecutionPreflight(const UmiToolchainProfile *profile,
    const UmiEnvironmentPlan *environment, UmiBuildAction action,
    const UmiBuildRequest *request, const UmiBuildExecutionOptions *options,
    char *message, size_t message_capacity);

/** Execute or preview the established action sequence. Stop at the first failed
 * phase; dry-run exit fields stay -1 because no child ran. No Git operation is
 * implied. Existing umi_build_execute callers keep their source and ABI layout. */
UmiStatus UmiBuildExecuteWithOptions(const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment, UmiBuildAction action,
    const UmiBuildRequest *request, const UmiBuildExecutionOptions *options,
    UmiBuildReport *report);

/** Reserve a new absolute log before execution and retain every raw output byte.
 * Existing logs are never overwritten. Parents must exist. A failed log open
 * prevents execution; later write errors retain evidence and are returned even
 * if the child succeeded. The current phase finishes, then later phases stop.
 * NULL log_path disables file capture. Preview never creates a file. Logging
 * cannot guarantee power-loss durability. Caller owns retention of private logs. */
UmiStatus UmiBuildExecuteLogged(const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment, UmiBuildAction action,
    const UmiBuildRequest *request, const UmiBuildExecutionOptions *options,
    const char *log_path, UmiBuildExecutionLog *log, UmiBuildReport *report);
#ifdef __cplusplus
}
#endif
#endif
