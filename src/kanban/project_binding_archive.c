/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/project_binding_archive.c
 *
 * PURPOSE:
 *   Reuse the shared bounded value codec for private board location records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../base/value_archive_internal.h"
#include "umicom/kanban/project_binding.h"
static uint64_t BindingSchema(void) { return UINT64_C(0x554d494b4c4f4301); }
static size_t BindingBound(void) { return UMI_KANBAN_BINDING_MAXIMUM_BYTES; }
static void BindingWrite(UmiArchiveWriter *writer, const UmiKanbanProjectBinding *value) {
  UmiArchiveWriteText(writer, value->project_root, sizeof value->project_root);
  UmiArchiveWriteText(writer, value->directory, sizeof value->directory);
  UmiArchiveWriteText(writer, value->filename, sizeof value->filename);
}
static void BindingRead(UmiArchiveReader *reader, UmiKanbanProjectBinding *value) {
  UmiArchiveReadText(reader, value->project_root, sizeof value->project_root);
  UmiArchiveReadText(reader, value->directory, sizeof value->directory);
  UmiArchiveReadText(reader, value->filename, sizeof value->filename);
}
UMI_DEFINE_VALUE_ARCHIVE(UmiKanbanProjectBindingEncode, UmiKanbanProjectBindingDecode,
                         UmiKanbanProjectBinding, BindingSchema, BindingBound, BindingWrite,
                         BindingRead, UmiKanbanProjectBindingValidate)
