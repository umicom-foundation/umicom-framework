/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/program_console_internal.h
 * PURPOSE: Keep the console queue and native execution state under one owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROGRAM_CONSOLE_INTERNAL_H
#define UMICOM_PROGRAM_CONSOLE_INTERNAL_H
#include "umicom/platform/process_environment.h"
#include "umicom/platform/threading.h"
#include "umicom/terminal/program_console.h"
typedef struct ConsoleInput
{
    size_t length;
    unsigned char bytes[UMI_PROGRAM_CONSOLE_INPUT_CAPACITY];
} ConsoleInput;
struct UmiProgramConsole
{
    UmiMutex *mutex;
    UmiProcessChannelRequest command;
    char *program;
    char *directory;
    char *arguments[UMI_CHANNEL_MAX_ARGUMENTS];
    UmiProcessEnvironmentPlan *environment;
    uint32_t timeout_ms;
    bool entered;
    size_t head, count;
    ConsoleInput queue[UMI_PROGRAM_CONSOLE_QUEUE_CAPACITY];
    UmiProgramConsoleSnapshot snapshot;
};
/* Private worker helpers always acquire the mutex; no native I/O holds it. */
void ConsoleOutput(UmiProgramConsole *console, const char *bytes, size_t length);
void ConsolePublishChannel(UmiProgramConsole *console, const UmiProcessChannelSnapshot *channel);
#endif
