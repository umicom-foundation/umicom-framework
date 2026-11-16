/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media_generation/comfy_node_pages.h
 *
 * PURPOSE:
 *   Expose captured, filtered node pages through shared native file and presentation contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_MEDIA_GENERATION_COMFY_NODE_PAGES_H
#define UMICOM_MEDIA_GENERATION_COMFY_NODE_PAGES_H
#include "umicom/media_generation/comfy_palette.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_COMFY_NODE_PAGE_LIMIT 64U
#define UMI_COMFY_NODE_PAGE_JSON_CAPACITY (512U * 1024U)
/* Admit absolute ordinary filenames through the shared connection/file owners,
 * then select copied metadata with the existing query contract. Catalogue bytes
 * are bounded by UMI_COMFY_NODES_MAX_BYTES; connection documents are below 8192.
 * Final symlinks, directories and devices are refused. Parent aliases can still
 * resolve elsewhere; this is not confinement or an atomic two-file snapshot.
 * Initialize *out=NULL. Failure preserves it. Query must stay immutable; it is
 * validated by the selector after capture. Cancellation cannot interrupt a
 * blocking read. No network, custom-node import, activation or persistence. */
UmiStatus UmiComfyNodeSelectionFileRead(const char *connection_document,
    const char *object_info_json,const UmiComfyNodeQuery *query,
    const UmiCancellationToken *cancel,UmiComfyNodeSelection **out);
/* Emit a complete page only on success. Capacity includes NUL; length excludes
 * it. Failure leaves both outputs unchanged. Do not overlap inputs/outputs;
 * serialize access/destruction. Offset may equal selected_count for an empty
 * terminal page; greater offsets are NOT_FOUND. Limit must be 1..64.
 *
 * Identity contains an exact decimal-string revision, connection fingerprint,
 * original response digest and selection_sha256. The selection digest hashes
 * UTF-8 "Umicom.ComfySelection.v1" including NUL, then every ordered class ID
 * including NUL. Join pages only when all four identities match. Equal queries
 * need not have distinct identities if their ordered result is identical.
 * Digests are consistency labels, not authorization or server authenticity.
 * Unknown node flags are null; counts/masks are bounded numbers. Descriptors,
 * defaults and opaque extensions are omitted, not generally redacted. Named
 * metadata may itself be sensitive. No readiness or generation is established. */
UmiStatus UmiComfyNodeSelectionJson(const UmiComfyNodeSelection *selection,
    size_t offset,size_t limit,char *out,size_t capacity,size_t *out_length);
/* Arguments start with nodes. Shared command used by CLI, Media and SDK hosts.
 * Exit 0: page emitted; 2: syntax/options; 1: file, selection or output failure. */
int UmiComfyNodeSelectionFileCommand(int argc,char **argv);
#ifdef __cplusplus
}
#endif
#endif
