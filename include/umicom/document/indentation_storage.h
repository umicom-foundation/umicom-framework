/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/indentation_storage.h
 *
 * PURPOSE:
 *   Store a small editor preference record using shared user-local paths and checked file
 * operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_INDENTATION_STORAGE_H
#define UMICOM_DOCUMENT_INDENTATION_STORAGE_H
#include "umicom/editor/indentation_record.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Resolve config/editor-indentation.txt under a product's user-local paths.
 * base_override, when provided, is absolute. No directory is created. */
UmiStatus UmiDocumentIndentationStoragePath(const char *application_directory,
                                            const char *base_override, char *out_path,
                                            size_t capacity);
/** Read a bounded regular absolute file. Output is unchanged on failure. */
UmiStatus UmiDocumentIndentationStorageLoad(const char *path, UmiEditorIndentation *out_options);
/** Encode and replace an owned regular local preference file. Its trusted
 * parent must exist. Uses UmiLocalFileReplace: independent writes are
 * last-writer-wins and replacement is not a power-loss durability guarantee.
 * No document, application settings or directory creation is performed.
 * File I/O can block; interactive hosts should use a worker. */
UmiStatus UmiDocumentIndentationStorageSave(const char *path, const UmiEditorIndentation *options);
#ifdef __cplusplus
}
#endif
#endif
