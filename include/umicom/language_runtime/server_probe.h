/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/server_probe.h
 * PURPOSE: Report an explicit language-server handshake without keeping a background editor connection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_SERVER_PROBE_H
#define UMICOM_LANGUAGE_RUNTIME_SERVER_PROBE_H
#include "umicom/language_runtime/server_manager.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiLanguageRuntimeProbeResult
    {
        int launched, initialized;
        UmiStatus initializationStatus, shutdownStatus;
        UmiLanguageRuntimeInitializeResult capabilities;
        UmiLanguageRuntimeServerSnapshot connection;
    } UmiLanguageRuntimeProbeResult;
    /**
     * Run the connection probe with a selected child tool directory. This follows
     * Probe's explicit trust, cancellation, cleanup and worker-thread rules.
     * NULL/empty tool_directory retains inherited selection; otherwise use one
     * absolute directory, not a PATH list. The server is closed before return.
     * Nothing activates completion or records a persistent editor connection.
     */
    UmiStatus UmiLanguageRuntimeProbeWithToolDirectory(const UmiLanguageServerProfile *profile,
        const char *root_uri, const char *working_directory, const char *tool_directory,
        uint32_t timeout_ms, const UmiCancellationToken *cancel, UmiLanguageRuntimeProbeResult *out);
    /* Run only after the user selects a trusted local executable. This starts a
 * real child, completes initialization if possible, then shuts it down.
 * It does not activate editor completion, send document text, save settings or
 * install tools. The selected server can itself inspect the working folder.
 * Call on a worker with copied profile/path values and a token kept alive until
 * return. A pre-cancelled call starts nothing. Cancellation bounds read waits,
 * not native launch or synchronous writes; shutdown has its own 250 ms reply
 * budget and still cleans up after a protocol failure. The direct child alone
 * is owned; see process_stream.h for the full lifetime and blocking limits.
 * The result is cleared on entry and records initialization and cleanup
 * separately. A successful handshake with failed shutdown returns that failure. */
    UmiStatus UmiLanguageRuntimeProbe(const UmiLanguageServerProfile *profile, const char *rootUri,
                                      const char *workingDirectory, uint32_t timeoutMs,
                                      const UmiCancellationToken *cancel, UmiLanguageRuntimeProbeResult *out);
#ifdef __cplusplus
}
#endif
#endif
