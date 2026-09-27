/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/native_project.h
 *
 * PURPOSE:
 *   Create inspectable C23 projects from the canonical declarative document.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_NATIVE_PROJECT_H
#define UMICOM_DESIGNER_NATIVE_PROJECT_H

#include "umicom/declarative/document.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_NATIVE_NODE_LIMIT 128U
#define UMI_DESIGNER_NATIVE_DEPTH_LIMIT 32U
#define UMI_DESIGNER_NATIVE_FILE_COUNT 6U
#define UMI_DESIGNER_NATIVE_SOURCE_LIMIT (4U * 1024U * 1024U)
#define UMI_DESIGNER_NATIVE_PATH_LIMIT 4096U

/* Plans own generated text, never the supplied document or a toolkit object.
 * Create on the document owner's thread. Completed plans are immutable and
 * may be read concurrently while their owner keeps them alive. Destroy only
 * after all readers and publication have finished. No callbacks are invoked. */
typedef struct UmiDesignerNativeProject UmiDesignerNativeProject;

typedef struct UmiDesignerNativeProjectSummary {
    uint64_t sourceRevision;
    size_t nodeCount;
    size_t fileCount;
    size_t totalBytes;
} UmiDesignerNativeProjectSummary;

typedef struct UmiDesignerNativeFileView {
    const char *path;
    const char *text;
    size_t length;
} UmiDesignerNativeFileView;

typedef struct UmiDesignerNativePublishResult {
    size_t filesWritten;
    size_t bytesWritten;
    int directoryCreated;
    int complete;
} UmiDesignerNativePublishResult;

/* Check the bounded native controls profile before code generation or GTK
 * construction. Unknown components/properties are reported, never ignored.
 * Validation does not edit, serialise, save or execute the document. An optional
 * explanation is always terminated when its capacity is nonzero. */
UmiStatus UmiDesignerNativeValidate(const UmiDeclDocument *document,
    char *explanation, size_t capacity);

/* projectName is a portable lower-case CMake target: [a-z][a-z0-9_]{0,62}.
 * The generated constructor preserves node order, identifiers and typed values.
 * Output ownership is set to NULL on failure. No file or process is opened. */
UmiStatus UmiDesignerNativeProjectCreate(const UmiDeclDocument *document,
    const char *projectName, UmiDesignerNativeProject **outProject,
    char *explanation, size_t capacity);
void UmiDesignerNativeProjectDestroy(UmiDesignerNativeProject *project);
UmiStatus UmiDesignerNativeProjectGetSummary(const UmiDesignerNativeProject *project,
    UmiDesignerNativeProjectSummary *outSummary);
/* Borrowed strings last until project destruction. Outputs are cleared first. */
UmiStatus UmiDesignerNativeProjectFile(const UmiDesignerNativeProject *project,
    size_t index, UmiDesignerNativeFileView *outFile);

/* Publish only into a NEW absolute directory whose parent already exists.
 * Existing files, directories and links are refused, even if empty. On error,
 * a partially created directory is retained for inspection; it is never removed
 * recursively. Inspect both the status and outResult->complete. The completion
 * record is written last. This is not a multi-file filesystem transaction,
 * a hostile-directory sandbox, a signature, or a tested power-loss guarantee.
 * POSIX uses a held directory descriptor; Windows uses a held directory handle.
 * Select a directory you own. No shell, compiler, Git or installer is invoked. */
UmiStatus UmiDesignerNativeProjectPublish(const UmiDesignerNativeProject *project,
    const char *newDirectory, UmiDesignerNativePublishResult *outResult);

/* Complete practice document: a notes editor and explicit character-count
 * action. Its text is temporary; it has no filesystem or database permission. */
UmiStatus UmiDesignerNativeNotesDocument(UmiDeclDocument **outDocument);

#ifdef __cplusplus
}
#endif
#endif
