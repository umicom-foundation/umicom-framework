/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/process_channel.h
 * PURPOSE: Own a live child process and its pipes while protocol clients own framing.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A live, argument-based child channel. Framework owns the child and its pipes;
 * protocol clients own framing. The synchronous process runner is unchanged.
 * No shell, socket listener or process-global signal handler is installed.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_PROCESS_CHANNEL_H
#define UMICOM_PLATFORM_PROCESS_CHANNEL_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/process.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CHANNEL_MAX_ARGUMENTS 64U
#define UMI_CHANNEL_DIAGNOSTIC_CAPACITY 4096U
    typedef struct UmiProcessChannel UmiProcessChannel;
    typedef struct UmiProcessChannelRequest {
        const char *program;
        /* Absolute native executable. */
        const char *const *arguments;
        /* Does not include argv[0]. */
        size_t argumentCount;
        const char *workingDirectory;
        /* Absolute; existing directory. */
    }
    UmiProcessChannelRequest;
    typedef struct UmiProcessChannelSnapshot {
        int running;
        int exitCode;
        int terminated;
        /* Host requested forced termination. */
        int diagnosticsTruncated;
        uint64_t processId;
        char diagnostics[UMI_CHANNEL_DIAGNOSTIC_CAPACITY];
    }
    UmiProcessChannelSnapshot;
    /** One calling thread at a time. Open borrows inputs only until return. A new
                         * process starts with dedicated stdin/stdout and separately drained stderr.
                         * Environment is a small host-compatible allowlist, never the caller's full
                         * environment. Linux uses a process group; Windows uses an owned job. Neither
                         * is a security sandbox. Do not run untrusted host executables.
     * Linux parent-death signalling is tied to the thread that calls Open:
     * keep that thread alive until the child ends, using a persistent worker
     * rather than a one-shot thread that exits immediately after launch. */
    UmiStatus UmiProcessChannelOpen(const UmiProcessChannelRequest *request,     UmiProcessChannel **outChannel);
    /** Open an explicitly selected trusted native program with the parent's
     * environment plus optional overrides. No shell or PATH executable search
     * occurs. This differs deliberately from Open's restricted environment.
     * Overrides use portable ASCII names and UTF-8 values; duplicates are
     * refused case-insensitively. Inputs are borrowed until return. Serialize
     * host environment changes with launch preparation. Linux still requires
     * a native ELF executable and an existing non-symlink directory leaf.
     * Keep the opening thread alive until the child ends. Failure clears out.
     */
    UmiStatus UmiProcessChannelOpenProgram(const UmiProcessChannelRequest *request,
        const UmiEnvironmentVariable *environment, size_t environmentCount,
        UmiProcessChannel **outChannel);
    /** Close the parent's stdin writer so the child can observe end-of-file.
     * This is idempotent and does not stop the child or close either output.
     * Serialize with Read, Write, Poll and destruction; writes after closure
     * return INVALID_STATE. Queued application input must be sent first.
     */
    UmiStatus UmiProcessChannelCloseInput(UmiProcessChannel *channel);
    /** Read some stdout, draining diagnostics while waiting. timeoutMs <= 60000.
                         * OK with *outRead==0 means EOF. TIMEOUT is not EOF. Inputs/outputs are bytes.
                         * Read never merges stderr into a protocol stream. */
    UmiStatus UmiProcessChannelRead(UmiProcessChannel *channel, void *buffer,     size_t capacity, size_t *outRead, unsigned timeoutMs);
    /** A small bounded frame (maximum 4096 bytes). A timeout can mean partial
                         * delivery; the caller must stop issuing protocol commands after that result. */
    UmiStatus UmiProcessChannelWrite(UmiProcessChannel *channel, const void *bytes,     size_t length, unsigned timeoutMs);
    UmiStatus UmiProcessChannelPoll(UmiProcessChannel *channel,     UmiProcessChannelSnapshot *outSnapshot);
    /** Explicit force-stop, not guest shutdown. Targets only the owned process
                         * scope; never a PID supplied by the caller. */
    UmiStatus UmiProcessChannelTerminate(UmiProcessChannel *channel);
    /** Destruction force-stops a still-running child and closes its pipes. A GUI
                         * must obtain confirmation BEFORE calling this on an active machine. */
    void UmiProcessChannelDestroy(UmiProcessChannel *channel);
#ifdef __cplusplus
}
#endif
#endif
