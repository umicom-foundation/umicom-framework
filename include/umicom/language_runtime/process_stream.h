/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/process_stream.h
 *
 * PURPOSE:
 *   Own a persistent bidirectional child process for LSP/DAP/interactive tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_PROCESS_STREAM_H
#define UMICOM_LANGUAGE_RUNTIME_PROCESS_STREAM_H
#include "umicom/language_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the language runtime process stream data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimeProcessStream UmiLanguageRuntimeProcessStream;
/**
 * Represent the language runtime process stream config data shared with callers of this
 * public contract.
 */
/* Use one calling thread at a time. Start borrows configuration strings only
 * until it returns; it inherits the environment and does not change the parent
 * directory. Prefer absolute executable and working-directory paths. Bare
 * programs retain native search rules. Empty arguments are preserved.
 *
 * Windows inputs are UTF-8, converted strictly to UTF-16. Output remains raw
 * bytes, with stderr inherited separately unless merge_stderr is requested.
 * A GUI host without stderr uses a null sink. The Windows command line is
 * limited to 32,767 UTF-16 units including NUL. POSIX launch setup failures
 * are returned before a stream is published. Serialise environment changes
 * with launch; leave child reaping to this owner.
 *
 * Read returns NOT_FOUND for either timeout or exhausted stdout; use
 * is_running to inspect child lifetime. Write is synchronous and may block if
 * the server stops reading. Keep all operations on a persistent worker.
 * Stop/destroy own only the direct child, not descendants or a security
 * sandbox. POSIX retains SIGTERM then SIGKILL; Windows closes stdin, waits,
 * then terminates a child that did not exit. Destruction may wait briefly.
 * This low-level stream does not initialize the language protocol itself. */
typedef struct UmiLanguageRuntimeProcessStreamConfig{const char*program;const char*const*arguments;size_t argument_count;const char*working_directory;int merge_stderr;}UmiLanguageRuntimeProcessStreamConfig;
/**
 * Provide the language runtime process stream start operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_process_stream_start(const UmiLanguageRuntimeProcessStreamConfig*c,UmiLanguageRuntimeProcessStream**out);
/**
 * Start a persistent child with one project-selected tool directory prepended to
 * its PATH. NULL or empty directory retains the ordinary inherited launch.
 * The directory must be absolute and contain one path, not a PATH list. A simple
 * ASCII tool name resolves inside that folder (with .exe on Windows); an absolute
 * executable remains authoritative. Relative executable paths are rejected.
 * No fallback is attempted when the selected tool is absent. All strings are
 * borrowed only until return; failure clears out. This grants no execution
 * permission: callers must approve the project and selected tools beforehand.
 * The direct-child lifetime, raw protocol streams and threading rules above apply.
 */
UmiStatus UmiLanguageRuntimeProcessStreamStartWithToolDirectory(
    const UmiLanguageRuntimeProcessStreamConfig *config, const char *tool_directory,
    UmiLanguageRuntimeProcessStream **out);

/**
 * Release or reset state held by language runtime process stream so the same storage can
 * be reused safely.
 */
void umi_language_runtime_process_stream_destroy(UmiLanguageRuntimeProcessStream*s);
/**
 * Write language runtime process stream in its stable representation and report capacity
 * or input failures to the caller.
 */
UmiStatus umi_language_runtime_process_stream_write(UmiLanguageRuntimeProcessStream*s,const void*b,size_t n);
/**
 * Read language runtime process stream into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_language_runtime_process_stream_read(UmiLanguageRuntimeProcessStream*s,void*out,size_t cap,uint32_t timeout_ms,size_t*n);
/**
 * Provide the language runtime process stream stop operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_process_stream_stop(UmiLanguageRuntimeProcessStream*s,uint32_t timeout_ms);
/**
 * Provide the language runtime process stream is running operation used by this module and
 * its client applications.
 */
int umi_language_runtime_process_stream_is_running(UmiLanguageRuntimeProcessStream*s);

/** Start with reviewed NAME=VALUE overrides and an optional tools PATH prefix.
 * Uses UmiProcessEnvironmentValidate syntax and never changes the host environment.
 * The native child inherits other variables; input strings need live only for this
 * call. Program selection and child-only lifetime follow StartWithToolDirectory.
 * This may execute arbitrary configured code; the caller must authorize it. */
UmiStatus UmiLanguageRuntimeProcessStreamStartWithEnvironment(
    const UmiLanguageRuntimeProcessStreamConfig *config, const char *tool_directory,
    const char *definitions, UmiLanguageRuntimeProcessStream **out);

#ifdef __cplusplus
}
#endif
#endif
