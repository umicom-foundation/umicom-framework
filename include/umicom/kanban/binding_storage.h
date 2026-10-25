/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/kanban/binding_storage.h
 *
 * PURPOSE:
 *   Remember an explicitly selected board location in a separate private settings folder.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_KANBAN_BINDING_STORAGE_H
#define UMICOM_KANBAN_BINDING_STORAGE_H
#include "umicom/kanban/project_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Derive a private settings leaf from the project's existing deterministic
 * SHA-256 naming convention. No directory is created and no file is opened.
 * settings_directory must be an absolute, existing private folder for Load/
 * Save. This helper checks syntax only. Output is unchanged on failure. */
UmiStatus UmiKanbanBindingStoragePath(const char *settings_directory, const char *project_root,
                                      char *out_path, size_t capacity);
/** Read one complete bounded binding and require its stored project identity
 * to equal the requested normalised root. A file under another project's name
 * cannot silently redirect that project. Missing/invalid input is an error;
 * out stays unchanged. Does not open the remembered board or grant trust. */
UmiStatus UmiKanbanBindingStorageLoad(const char *settings_directory, const char *project_root,
                                      UmiKanbanProjectBinding *out);
/** Atomically replace one private binding through rooted-file storage.
 * Caller serializes writers for this project; this is last-writer-wins local
 * preference storage, not a shared board transaction. No board is saved, moved,
 * opened or published. Existing directories are required. Link/reparse rules
 * and replacement limits are those of platform/rooted_files.h.
 * The record contains local project and board paths: keep it outside public
 * repositories. A saved location is a convenience, not an access credential.
 * Load/Save may block; native hosts must call them from a worker. */
UmiStatus UmiKanbanBindingStorageSave(const char *settings_directory,
                                      const UmiKanbanProjectBinding *binding);
#ifdef __cplusplus
}
#endif
#endif
