/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/editor_config_inspection.h
 *
 * PURPOSE:
 *   Inspect project-bounded editor settings through checked file reads with source digests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_EDITOR_CONFIG_INSPECTION_H
#define UMICOM_DOCUMENT_EDITOR_CONFIG_INSPECTION_H
#include "umicom/base/sha256.h"
#include "umicom/editor/editor_config_result.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_CONFIG_INSPECTION_PATH 2048U
#define UMI_EDITOR_CONFIG_INSPECTION_BUDGET (8U * 1024U * 1024U)
typedef struct UmiEditorConfigInspection UmiEditorConfigInspection;
typedef struct UmiEditorConfigInspectionFile {
  char relative_path[UMI_EDITOR_CONFIG_INSPECTION_PATH];
  char sha256[UMI_SHA256_HEX_CAPACITY];
  size_t bytes;
  int root;
} UmiEditorConfigInspectionFile;
typedef struct UmiEditorConfigInspectionDiagnostic {
  char relative_path[UMI_EDITOR_CONFIG_INSPECTION_PATH];
  size_t line;
  UmiStatus status;
} UmiEditorConfigInspectionDiagnostic;
/* Inspect configurations from a chosen project boundary down to a target file.
 * The root is an ordinary absolute directory; the file is a portable relative
 * path. Search begins in the target's parent and stops at root=true or the
 * explicit boundary. Never read above that boundary, execute project commands,
 * modify settings or write files. The target file need not exist.
 * Reuse rooted-file admission: roots/descendants must not be links/reparse
 * points; ancestors above the chosen root remain trusted. Access can block, so
 * call on a worker for interactive clients. This is a series of observations,
 * not a consistent filesystem snapshot or a file lock.
 * At most 32 directory levels, 1 MiB per config and 8 MiB total are read.
 * Missing configs are ignored; denied, malformed and changed reads fail rather
 * than presenting incomplete settings. Cancellation is checked between reads
 * and before publishing. NULL cancellation means no cancellation requested.
 * Failure clears out_inspection and optionally reports the failing config.
 * A successful inspection owns copied properties and digests. */
UmiStatus UmiEditorConfigInspectionLoad(const char *project_root, const char *relative_file,
                                        const UmiCancellationToken *cancel,
                                        UmiEditorConfigInspection **out_inspection,
                                        UmiEditorConfigInspectionDiagnostic *diagnostic);
void UmiEditorConfigInspectionDestroy(UmiEditorConfigInspection *inspection);
const UmiEditorConfigResult *
UmiEditorConfigInspectionResult(const UmiEditorConfigInspection *inspection);
size_t UmiEditorConfigInspectionFileCount(const UmiEditorConfigInspection *inspection);
/* Found files are reported outermost to innermost; absent candidates are not
 * listed. Metadata copies survive Destroy. Errors leave output unchanged. */
UmiStatus UmiEditorConfigInspectionFileAt(const UmiEditorConfigInspection *inspection, size_t index,
                                          UmiEditorConfigInspectionFile *out_file);
#ifdef __cplusplus
}
#endif
#endif
