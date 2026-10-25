/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/kanban/location.h
 *
 * PURPOSE:
 *   Describe explicit board locations and deterministic project-specific filenames.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_KANBAN_LOCATION_H
#define UMICOM_KANBAN_LOCATION_H
#include "umicom/kanban/document.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiKanbanProjectLocation {
  char project_root[UMI_PATH_CAPACITY];
  char filename[UMI_KANBAN_FILENAME_CAPACITY];
} UmiKanbanProjectLocation;
/** Check the same directory, archive leaf and sibling guard syntax used by
 * board storage, without opening files or creating directories. Existence,
 * links and access rights remain checks for the later document operation. */
UmiStatus UmiKanbanLocationValidate(const char *directory, const char *filename);
/** Prepare a stable suggested filename for an explicitly chosen project root.
 * Validate before normalising: traversal is not silently resolved.
 * The full SHA-256 of a domain tag and normalised root distinguishes projects;
 * the filename contains neither the project basename nor its directory path.
 * This is a naming convention, not encryption or filesystem identity. Moving
 * a project or changing its path's case can select a different name. Symlink
 * aliases are not resolved. No board or directory is created or opened.
 * The user still chooses a separate private board directory. Output stays
 * unchanged on failure; input must not overlap output. */
UmiStatus UmiKanbanProjectLocationCreate(const char *project_root, UmiKanbanProjectLocation *out);
#ifdef __cplusplus
}
#endif
#endif
