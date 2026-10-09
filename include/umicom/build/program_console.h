/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/program_console.h
 * PURPOSE: Prepare the saved project launch settings for a console that accepts input.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_PROGRAM_CONSOLE_H
#define UMICOM_BUILD_PROGRAM_CONSOLE_H
#include "umicom/build/profile.h"
#include "umicom/terminal/program_console.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C"
{
#endif
    /** Copy the selected existing program, launch folder, arguments and child
 * environment into a new console. The caller supplies the current project trust
 * decision. No save, configure, build or process launch occurs here. The native
 * channel checks executable support when Run starts. Failure clears out.
 */
    UmiStatus UmiBuildProgramConsoleCreate(const UmiBuildProfile *profile, bool trusted,
                                           UmiProgramConsole **out);
#ifdef __cplusplus
}
#endif
#endif
