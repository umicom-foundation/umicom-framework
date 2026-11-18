/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/local_models/memory_command.h
 *
 * PURPOSE:
 *   Expose one offline dense KV planning command for native CLI and product consumers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_LOCAL_MODELS_MEMORY_COMMAND_H
#define UMICOM_LOCAL_MODELS_MEMORY_COMMAND_H
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Parse ten decimal arguments (excluding the command name):
 * cache bytes, requested tokens, sequences, layers, key heads, value heads,
 * key dimensions, value dimensions, key element bytes, value element bytes.
 * A sole --help prints usage. No files, model weights or host memory are read.
 * Streams stay owned by the caller and must be distinct, live writable streams.
 * Return 0 for a completed report (including zero affordable tokens), 2 for
 * invalid input/geometry, and 1 for arithmetic or output failure. A report is
 * planning arithmetic only and must never be interpreted as launch admission. */
int UmiModelDenseKvCommand(int argc, const char *const *argv,
    FILE *output, FILE *diagnostics);
#ifdef __cplusplus
}
#endif
#endif
