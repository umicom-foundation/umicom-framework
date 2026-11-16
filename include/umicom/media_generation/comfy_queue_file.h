/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media_generation/comfy_queue_file.h
 *
 * PURPOSE:
 *   Compose captured queue files and bounded metadata pages for native products and independent SDK clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_MEDIA_GENERATION_COMFY_QUEUE_FILE_H
#define UMICOM_MEDIA_GENERATION_COMFY_QUEUE_FILE_H
#include "umicom/media_generation/comfy_queue.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_COMFY_QUEUE_PAGE_LIMIT 256U
#define UMI_COMFY_QUEUE_JSON_CAPACITY (512U * 1024U)
/* Validate both ordinary absolute UTF-8 filenames before I/O. Reuse the
 * provider document reader, regular-file owner and immutable copied queue.
 * Final links/devices/directories are refused; parent aliases may be followed.
 * This is neither filesystem confinement nor a snapshot across both files.
 * Document bytes must be below 8192; queue bytes may be at most 4 MiB.
 *
 * Saved settings must meet the existing enabled ComfyUI loopback-origin
 * contract. Historical revision and hashes are observations, not live identity.
 * Initialize *out=NULL; failures preserve it. Clear temporary input contents
 * and staged settings. GUI callers use a worker: local reads may block.
 * Cancellation is checked between operations and before publication; it
 * cannot interrupt a blocking read. Borrow inputs/token until return.
 * No store, vault, provider request, queue mutation or generation occurs. */
UmiStatus UmiComfyQueueFileRead(const char *connection_document,
    const char *queue_json,const UmiCancellationToken *cancel,UmiComfyQueue **out);
/* Serialize one page of copied queue metadata. offset may equal Count for an
 * empty final page; larger values return NOT_FOUND. limit is 1..256. Preserve
 * output bytes and length on every failure; capacity includes the final NUL.
 * Inputs/outputs must not overlap. Stage at most JSON_CAPACITY bytes.
 *
 * Running entries precede pending entries, preserving response order within
 * each list. Scheduling numbers and revision are lossless JSON strings; do
 * not interpret a number as execution rank or progress. IDs may be sensitive.
 * Workflow, extra data and output-node lists are omitted, not redacted.
 * Compare response hash, connection hash and revision before combining pages
 * obtained from separate owners/reads. No authentication or freshness claim. */
UmiStatus UmiComfyQueueJson(const UmiComfyQueue *queue,size_t offset,size_t limit,
    char *out,size_t capacity,size_t *out_length);
/* Arguments include queue: queue CONNECTION QUEUE_JSON [--offset N] [--limit N].
 * Either option may occur once, in any order. Defaults 0 and 256. Unsigned
 * decimal bounds: offset<=4096, limit 1..256. queue --help performs no read.
 * Exit 0: complete page emitted, 2: usage, 1: read/parse/page/output failure.
 * Output wrapper labels offline-files. Absence is never a completion receipt. */
int UmiComfyQueueFileCommand(int argc,char **argv);
#ifdef __cplusplus
}
#endif
#endif
