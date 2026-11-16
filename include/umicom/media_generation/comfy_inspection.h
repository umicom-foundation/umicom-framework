/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media_generation/comfy_inspection.h
 *
 * PURPOSE:
 *   Inspect explicit local ComfyUI observations through shared bounded file, settings and workflow owners.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_MEDIA_GENERATION_COMFY_INSPECTION_H
#define UMICOM_MEDIA_GENERATION_COMFY_INSPECTION_H
#include "umicom/media_generation/comfy_preflight.h"
#include "umicom/media_generation/comfy_references.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_COMFY_INSPECTION_JSON_CAPACITY (1024U * 1024U)
typedef struct UmiComfyInspectionPaths {
    const char *connection_document;
    const char *workflow_json;
    const char *node_catalogue_json;
    char client_id[129];
} UmiComfyInspectionPaths;
/* Read three explicitly chosen ordinary absolute local file paths. Validate
 * all path syntax and the terminated nonempty UTF-8 client ID before I/O.
 * Reuse UmiInputFileRead's regular-file, final-link and observable-change
 * checks. Parent aliases may be followed: choose trusted directories. This
 * is not a filesystem sandbox or an atomic snapshot across the three files.
 *
 * Limits: connection document below 8192 bytes, workflow 1 MiB, catalogue
 * 4 MiB. The imported connection must satisfy the existing ComfyUI loopback
 * observation contract. Its revision is historical, not current-store proof.
 * These files and the client ID define a hypothetical request for review;
 * no store, vault, HTTP, model, Python node or submission action occurs.
 *
 * Initialize *out=NULL. Preserve it on failure; successful report owns copies.
 * Clear and free temporary input bytes and owners. Calls may block on local
 * reads; GUI hosts use an owned worker. Cancellation is checked between reads
 * and parse phases, not during the blocking file operation. Borrow all inputs
 * until return and serialize access/destruction. */
UmiStatus UmiComfyInspectionRead(const UmiComfyInspectionPaths *paths,
    const UmiCancellationToken *cancel,UmiComfyPreflightReport **out);
/* Format a metadata-only report with JSON-escaped names and exact digests.
 * Revision is a decimal string to avoid floating-point client rounding.
 * The report has no authenticated origin; the host must label file versus
 * live observations separately. No prompt values or file paths are emitted.
 * capacity includes NUL; output length excludes it. Preserve both outputs
 * on refusal. Inputs and outputs must not overlap; temporary storage is bounded
 * by JSON_CAPACITY and cleared before release. No filesystem or network I/O. */
UmiStatus UmiComfyInspectionJson(const UmiComfyPreflightReport *report,
    char *out,size_t capacity,size_t *out_length);
/* Shared synchronous console command. Arguments exclude the 'comfy' word.
 * inspect CONNECTION WORKFLOW OBJECT_INFO [--client-id ID]
 * All paths are absolute. JSON output labels its origin as offline-files.
 * Exit 0: observed class/required-field presence; 3: missing requirements;
 * 2: usage error; 1: read/parse/identity/output error. No generation occurs.
 * A missing-input report remains complete JSON, even with exit 3. */
int UmiComfyInspectionCommand(int argc,char **argv);

#define UMI_COMFY_REFERENCE_PAGE_LIMIT 256U
#define UMI_COMFY_REFERENCE_JSON_CAPACITY (2U * 1024U * 1024U)
/* Reuse exactly the file, cancellation and identity contract of InspectionRead,
 * then inventory reference candidates. Legacy presence inspection does not
 * acquire the inventory's candidate/text limits. Initialize *out=NULL; preserve
 * it on refusal. Result owns copies after all file buffers are retired. */
UmiStatus UmiComfyReferenceInspectionRead(const UmiComfyInspectionPaths *paths,
    const UmiCancellationToken *cancel,UmiComfyReferenceInventory **out);
/* Serialize a bounded page with complete aggregate counts and digests. offset
 * may equal candidate_count for an empty final page; larger offsets return
 * NOT_FOUND. limit is 1..PAGE_LIMIT. Preserve output bytes and length on refusal;
 * capacity includes NUL. Inputs/outputs do not overlap. No I/O or logging.
 *
 * Candidate node/input/target/class/type text is emitted, including candidate
 * strings that a custom node might treat as data. Unrelated input values are
 * omitted. Unknown slots are null; known slots and revision are decimal strings
 * so JSON consumers cannot silently round 64-bit values. This is an observation,
 * never graph validity, privacy filtering or execution authorization. */
UmiStatus UmiComfyReferenceInspectionJson(const UmiComfyReferenceInventory *inventory,
    size_t offset,size_t limit,char *out,size_t capacity,size_t *out_length);
/* Arguments include the references word: references CONNECTION WORKFLOW
 * OBJECT_INFO [--client-id ID] [--offset N] [--limit N]. Options may occur once
 * in any order. N is unsigned decimal, offset<=REFERENCE_LIMIT, limit 1..256.
 * Exit 0 means a page was emitted, including unresolved candidates. Exit 2 is
 * usage; 1 is read/parse/identity/page/output failure. No generation occurs. */
int UmiComfyReferenceInspectionCommand(int argc,char **argv);

#ifdef __cplusplus
}
#endif
#endif
