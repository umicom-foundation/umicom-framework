/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/program_console.h
 * PURPOSE: Own bounded interactive input and captured output for one trusted native program.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TERMINAL_PROGRAM_CONSOLE_H
#define UMICOM_TERMINAL_PROGRAM_CONSOLE_H
#include "umicom/platform/process_channel.h"
#include <stdbool.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_PROGRAM_CONSOLE_INPUT_CAPACITY 4096U
#define UMI_PROGRAM_CONSOLE_QUEUE_CAPACITY 16U
#define UMI_PROGRAM_CONSOLE_OUTPUT_CAPACITY 65536U
    typedef struct UmiProgramConsole UmiProgramConsole;
    typedef struct UmiProgramConsoleConfig
    {
        UmiProcessChannelRequest command;
        const char *environment;    /* Reviewed NAME=VALUE list; never a secret store. */
        const char *tool_directory; /* Optional absolute child PATH prefix. */
        uint32_t timeout_ms;        /* Zero waits until exit or an explicit stop. */
    } UmiProgramConsoleConfig;
    typedef struct UmiProgramConsoleSnapshot
    {
        bool started;
        bool completed;
        bool stop_requested;
        bool input_ended;
        bool output_truncated;
        bool diagnostics_truncated;
        size_t queued_frames;
        uint64_t input_bytes_sent;
        uint64_t output_bytes_seen;
        int exit_code;
        UmiStatus status;
        char output[UMI_PROGRAM_CONSOLE_OUTPUT_CAPACITY];
        char diagnostics[UMI_CHANNEL_DIAGNOSTIC_CAPACITY];
    } UmiProgramConsoleSnapshot;
    /** Copy one absolute program, folder, argument vector and environment plan.
 * Nothing executes here. Failure clears out. Input is limited to sixteen
 * 4,096-byte frames; stdout and stderr retain bounded tails independently.
 * This is a pipe console, not a terminal emulator or a security sandbox.
 * Only Windows and Linux currently supply the native channel.
 */
    UmiStatus UmiProgramConsoleCreate(const UmiProgramConsoleConfig *config,
                                      UmiProgramConsole **out);
    /** Run once on a dedicated worker that remains alive throughout execution.
 * Other threads may Send, EndInput, Stop and Read concurrently. Never call Run
 * twice, destroy while it is active, or mutate the host environment during
 * launch preparation. All channel operations stay on this worker. A failed or
 * partially delivered input write stops the owned child rather than retrying.
 */
    UmiStatus UmiProgramConsoleRun(UmiProgramConsole *console);
    /** Copy one raw input frame into the bounded queue, without logging its bytes.
 * OK means queued, not consumed by the child. BUSY leaves the queue unchanged.
 * Zero length, an ended input stream or a stopped/completed job are refused.
 * No newline, shell interpretation or text encoding conversion is added.
 */
    UmiStatus UmiProgramConsoleSend(UmiProgramConsole *console, const void *bytes, size_t length);
    /** Queue end-of-file after all accepted input. Idempotent; future Send calls
 * are refused. The program may continue producing output after observing EOF.
 */
    UmiStatus UmiProgramConsoleEndInput(UmiProgramConsole *console);
    /** Request cancellation without blocking for the process. Pending input may
 * be discarded. The worker terminates only the process scope that it owns.
 */
    UmiStatus UmiProgramConsoleStop(UmiProgramConsole *console);
    /** Copy a coherent bounded snapshot. Counts do not prove that the program read
 * input or that output has been saved. NUL output bytes appear as question marks.
 */
    UmiStatus UmiProgramConsoleRead(UmiProgramConsole *console, UmiProgramConsoleSnapshot *out);
    /** Release storage only after Run has returned, or before it starts. The owner
 * must join its worker before destruction. BUSY preserves the pointer while Run
 * is active; success clears it. Other controller calls must be quiescent.
 */
    UmiStatus UmiProgramConsoleDestroy(UmiProgramConsole **console);
#ifdef __cplusplus
}
#endif
#endif
