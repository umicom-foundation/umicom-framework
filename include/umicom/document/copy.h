/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/copy.h
 *
 * PURPOSE:
 *   Capture a source draft and write a separate new file without changing its editor owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_COPY_H
#define UMICOM_DOCUMENT_COPY_H
#include "umicom/document/coordinator.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDocumentCopy UmiDocumentCopy;
typedef struct UmiDocumentCopyInfo {
  UmiDocumentId document_id;
  char source_path[UMI_PATH_CAPACITY];
  char display_name[UMI_DOCUMENT_NAME_CAPACITY];
  UmiDocumentTextEncoding encoding;
  size_t text_bytes;
  size_t file_bytes;
} UmiDocumentCopyInfo;
typedef struct UmiDocumentCopyResult {
  size_t bytes_written;
  bool created;
  UmiStatus status;
} UmiDocumentCopyResult;

/** Capture one live draft on the coordinator owner thread. The copy owns its
 * encoded bytes and survives edits, tab changes, closing and owner destruction.
 * Nothing is saved, synchronized, marked clean or added to Undo. Read-only
 * documents may be copied. UTF-8/UTF-8 BOM/UTF-16 formats retain the document's
 * selected encoding; exact visible line endings and final-newline state remain.
 * outCopy is cleared on failure. No provider or filesystem I/O occurs. */
UmiStatus UmiDocumentCoordinatorCaptureCopy(UmiDocumentCoordinator *coordinator,
                                            UmiDocumentId document, UmiDocumentCopy **outCopy);
/** Copy metadata; the output remains unchanged on invalid arguments. */
UmiStatus UmiDocumentCopyInspect(const UmiDocumentCopy *copy, UmiDocumentCopyInfo *outInfo);
/** Borrow immutable encoded bytes until Destroy. Embedded NUL bytes are valid
 * in UTF-16 output; use the returned byte count, never strlen. */
UmiStatus UmiDocumentCopyRead(const UmiDocumentCopy *copy, const unsigned char **outBytes,
                              size_t *outCount);
/** Create an explicitly selected NEW local file in an existing trusted directory.
 * An existing leaf is never replaced, including symlinks. The captured original
 * path is also refused when its file has disappeared. The destination must pass
 * UmiOutputFileValidatePath: absolute ordinary path, no dot segments, and on
 * Windows no UNC/device/alternate-stream paths. Parent aliases may be followed;
 * this is not a workspace confinement boundary.
 *
 * Encoding finishes before creation. A disk failure may leave a partial new
 * file; it is retained for inspection, never removed automatically. A valid
 * result reports the accepted byte count, creation and final status. Successful
 * close is not a power-loss durability guarantee. The operation is synchronous;
 * a native host may dispatch it to its own worker with independently owned input.
 * Copies are immutable and reusable. The caller keeps them alive through calls;
 * different writers still arbitrate each destination through exclusive creation. */
UmiStatus UmiDocumentCopyWriteNew(const UmiDocumentCopy *copy, const char *destination,
                                  UmiDocumentCopyResult *outResult);
void UmiDocumentCopyDestroy(UmiDocumentCopy *copy);
#ifdef __cplusplus
}
#endif
#endif
