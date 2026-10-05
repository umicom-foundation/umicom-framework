/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/audio_arrangement/command.h
 * PURPOSE: Keep command dispatch separate from native argument conversion for source-based acceptance coverage.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AUDIO_ARRANGEMENT_COMMAND_H
#define UMICOM_AUDIO_ARRANGEMENT_COMMAND_H
/* Arguments are complete UTF-8 values, never a command string for a shell.
 * 0 means success, 1 an operation failed, and 2 an invalid command shape. */
int UmiAudioArrangementCommand(int argc, char **argv);
#endif
