/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/process_search_path.h
 * PURPOSE: Prepare an owned child search path without changing the host environment.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_PROCESS_SEARCH_PATH_H
#define UMICOM_PLATFORM_PROCESS_SEARCH_PATH_H
#include "umicom/base/status.h"
#include "umicom/platform/process.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /**
     * Check one absolute directory for use at the front of PATH. Empty, relative,
     * control-containing or separator-containing values are rejected. A directory
     * is not a list of search paths. This is syntax validation, not a filesystem
     * check or permission to execute tools from that directory.
     */
    UmiStatus UmiProcessSearchDirectoryValidate(const char *directory);
    /**
     * Allocate directory followed by the supplied inherited PATH. NULL or empty
     * inheritedPath means no inherited entries. The host separator is used and
     * a trailing empty search entry is not introduced. Retain existing inherited
     * entries exactly, including their order. Total storage is bounded to 128 KiB.
     * Failure clears a valid out pointer. Free the result with SearchPathFree.
     */
    UmiStatus UmiProcessSearchPathJoin(const char *directory, const char *inheritedPath,
                                       char **out);
    /**
     * Read an owned snapshot of the host PATH without adding or changing entries.
     * Windows reads the native Unicode value and converts strictly to UTF-8.
     * Missing or empty PATH succeeds with an allocated empty string. Storage is
     * bounded to 128 KiB; failure clears out. Free with SearchPathFree.
     * Serialise host environment mutation with this read, as with child launch.
     */
    UmiStatus UmiProcessSearchPathRead(char **out);
    /**
     * Copy the host PATH and prepend one explicit directory for a child request.
     * Windows reads the native Unicode environment; POSIX copies native bytes.
     * This never sets PATH in the parent. As with process launch, callers must
     * serialise unrelated process-wide environment mutations with this capture.
     */
    UmiStatus UmiProcessSearchPathCapture(const char *directory, char **out);
    /**
     * Resolve a simple tool name in one explicit directory. Names contain only
     * ASCII letters, digits, underscores and hyphens; Windows adds .exe. An empty
     * or NULL directory keeps the bare tool name for normal PATH lookup. No file
     * is opened and no fallback is attempted for an explicit folder. Output is
     * unchanged on failure. Input strings must be terminated and not overlap out.
     */
    UmiStatus UmiProcessToolProgram(const char *directory, const char *name, char *out,
                                    size_t capacity);
    /**
     * Execute a tool with a child-only PATH prefix and an absolute program path.
     * request.program is the simple tool name. Other arguments, working directory,
     * cancellation and capture choices are preserved. If request already overrides
     * PATH, that value is the inherited suffix; other variables are retained.
     * Empty/NULL directory uses the ordinary runner unchanged. Result is initialised
     * on rejection with launched false and exit_code -1. Nothing grants trust:
     * applications must authorise the project and tools before calling this worker
     * operation. Borrowed storage and observers stay alive until the call returns.
     */
    UmiStatus UmiProcessExecuteTool(const UmiProcessRequest *request, const char *directory,
                                    UmiProcessLifetime lifetime, UmiProcessOutputObserver observer,
                                    void *context, UmiProcessResult *out);
    /** Release an owned search-path string; NULL is accepted. */
    void UmiProcessSearchPathFree(char *path);
#ifdef __cplusplus
}
#endif
#endif
