/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/kanban/project_binding.h
 *
 * PURPOSE:
 *   Represent a private project-to-board location without opening either location.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_KANBAN_PROJECT_BINDING_H
#define UMICOM_KANBAN_PROJECT_BINDING_H
#include "umicom/base/value_archive.h"
#include "umicom/kanban/location.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiKanbanProjectBinding {
  char project_root[UMI_PATH_CAPACITY];
  char directory[UMI_PATH_CAPACITY];
  char filename[UMI_KANBAN_FILENAME_CAPACITY];
} UmiKanbanProjectBinding;
#define UMI_KANBAN_BINDING_MAXIMUM_BYTES                                                           \
  (UMI_VALUE_ARCHIVE_HEADER_SIZE + 24U + 2U * UMI_PATH_CAPACITY + UMI_KANBAN_FILENAME_CAPACITY)
/** Copy explicitly selected paths after syntax admission and normalisation.
 * Neither the project nor board must exist yet. No filesystem identity, access
 * permission, archive integrity or authorization is established here. Outputs
 * stay unchanged on failure. Inputs may alias fields of out; they are copied
 * before publication. Directory and project use platform path bytes; native
 * text widgets additionally require UTF-8. No path is copied to public cards. */
UmiStatus UmiKanbanProjectBindingCreate(const char *project_root, const char *directory,
                                        const char *filename, UmiKanbanProjectBinding *out);
/** Require complete terminated fields and canonical normalised paths. */
UmiStatus UmiKanbanProjectBindingValidate(const UmiKanbanProjectBinding *binding);
/** Same bounded value-archive ownership contract as base/value_archive.h:
 * NULL/zero measures; short output reports size and writes no bytes. Decode
 * validates an independent candidate. No struct padding or unused tails are
 * serialized. Unknown schemas are refused. Buffers must not overlap.
 * Checksums detect accidental damage; paths are not encrypted or authenticated. */
UmiStatus UmiKanbanProjectBindingEncode(const UmiKanbanProjectBinding *binding, void *bytes,
                                        size_t capacity, size_t *out_size);
UmiStatus UmiKanbanProjectBindingDecode(const void *bytes, size_t size,
                                        UmiKanbanProjectBinding *out);
#ifdef __cplusplus
}
#endif
#endif
