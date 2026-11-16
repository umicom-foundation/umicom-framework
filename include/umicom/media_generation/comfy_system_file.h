/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media_generation/comfy_system_file.h
 *
 * PURPOSE:
 *   Share captured system-report files, lossless JSON and console behavior across native products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_MEDIA_GENERATION_COMFY_SYSTEM_FILE_H
#define UMICOM_MEDIA_GENERATION_COMFY_SYSTEM_FILE_H
#include "umicom/media_generation/comfy_system.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_COMFY_SYSTEM_JSON_CAPACITY (512U * 1024U)
/* Validate both absolute ordinary UTF-8 filenames before I/O; reuse provider
 * document admission and bounded regular-file ownership. Document bytes must
 * be below 8192; response bytes at most 1 MiB. Final links, devices and directories
 * are refused; trusted parent aliases may still be followed. This is not file
 * confinement or an atomic capture across the two files.
 *
 * Initialize *out=NULL; every failure preserves it. Clear temporary response
 * and connection storage. Cancellation is checked between operations and before
 * publication, but cannot interrupt blocking reads. Use a worker in a GUI.
 * No provider request, vault access, persistence, model activation or generation. */
UmiStatus UmiComfySystemStatsFileRead(const char *connection_document,
    const char *system_json,const UmiCancellationToken *cancel,UmiComfySystemStats **out);
/* Copy a complete escaped report only after successful staging. Capacity includes
 * the NUL; output length excludes it. Failure preserves output bytes and length.
 * Inputs/outputs do not overlap and the owner remains alive throughout.
 *
 * Unknown metadata is JSON null. Known counters, indices and connection revision
 * are decimal strings, preserving integers beyond floating-point precision.
 * Counts, response indices and known masks are bounded JSON numbers. Keep raw
 * reported counters, inconsistency flags and identity together. Nothing here
 * establishes source authenticity, freshness, model fit or memory reservation.
 * No argv, opaque package/deployment fields or saved endpoint is emitted.
 * This is an explicit metadata projection, not a general redaction guarantee. */
UmiStatus UmiComfySystemStatsJson(const UmiComfySystemStats *stats,
    char *out,size_t capacity,size_t *out_length);
/* Arguments include system: system CONNECTION SYSTEM_JSON, or system --help.
 * Exit 0: complete observation emitted; 2: usage; 1: read/parse/output failure.
 * The command labels output origin as offline-files and performs no provider I/O. */
int UmiComfySystemStatsFileCommand(int argc,char **argv);
#ifdef __cplusplus
}
#endif
#endif
