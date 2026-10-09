/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/native_attach.h
 * PURPOSE: Prepare an explicit local process attachment without launch arguments or process-name guessing.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_RUNTIME_NATIVE_ATTACH_H
#define UMICOM_DEBUG_RUNTIME_NATIVE_ATTACH_H
#include "umicom/debug_runtime/platform.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiDebugNativeAttachOptions
    {
        const char *kind;
        const char *executable;
        const char *program;
        const char *working_directory;
        const char *tool_directory;
        uint64_t process_id;
    } UmiDebugNativeAttachOptions;
    typedef struct UmiDebugNativeAttachPlan
    {
        UmiDebugAdapterProfile profile;
        UmiDebugLaunchConfigurationSnapshot configuration;
        char arguments[8192];
        char tool_directory[1024];
        uint64_t process_id;
    } UmiDebugNativeAttachPlan;
    /** Read a decimal process ID between 1 and INT32_MAX. Whitespace, signs,
 * prefixes, empty text and overflow are refused; failure preserves output.
 * This parser does not look up processes or grant permission to attach. */
    UmiStatus UmiDebugNativeProcessIdRead(const char *text, uint64_t *out);
    /** Copy and validate native GDB/LLDB attachment settings. The selected working
 * directory must be absolute and exist; optional program is an absolute existing
 * symbols/executable file. An explicit adapter executable must be absolute.
 * Empty executable selects the installed builtin adapter, optionally from an
 * explicit tool folder. No process is started, enumerated or attached here.
 * The plan emits only pid and optional program, never launch args, target env,
 * remote endpoints or debugger commands. Failure preserves output. */
    UmiStatus UmiDebugNativeAttachPlanCreate(const UmiDebugNativeAttachOptions *options,
                                             UmiDebugNativeAttachPlan *out);
    /** Attach the selected local adapter to an explicitly approved running process.
 * The caller authorizes both target and adapter. This synchronous owner-thread
 * call does not build, save, launch a target, change its environment or elevate
 * privileges. Rejects attaching the current host and requires a nonzero timeout.
 * A PID is not durable identity: the target can exit or be reused before attach.
 * OS permissions and adapter support remain authoritative.
 *
 * Uses the normal initialization, breakpoints and configurationDone sequence.
 * Owner destruction requests detach, not target termination. Explicit Stop
 * callers should pass terminate_debuggee=0 for an attachment; an unacknowledged disconnect cannot prove the target resumed.
 * Never retry an uncertain attachment automatically. Inspect the target before
 * another explicit request. Native process support is Windows and POSIX hosts. */
    UmiStatus UmiDebugRuntimePlatformAttachNative(UmiDebugRuntimePlatform *platform,
                                                  const UmiDebugNativeAttachOptions *options,
                                                  uint32_t timeout_ms);
#ifdef __cplusplus
}
#endif
#endif
