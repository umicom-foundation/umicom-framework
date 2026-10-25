/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_storage/test_binding_archive.c
 *
 * PURPOSE:
 *   Check canonical binding bytes and reject damaged or mismatched records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../../src/base/value_archive_internal.h"
#include "binding_fixture.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  UmiKanbanProjectBinding value, decoded, sentinel;
  CHECK(UmiKanbanProjectBindingCreate(PROJECT, BOARDS, "private.umikb", &value) == UMI_STATUS_OK);
  unsigned char bytes[UMI_KANBAN_BINDING_MAXIMUM_BYTES], before[UMI_KANBAN_BINDING_MAXIMUM_BYTES];
  memset(bytes, 0x47, sizeof bytes);
  memcpy(before, bytes, sizeof bytes);
  size_t size = 0U;
  CHECK(UmiKanbanProjectBindingEncode(&value, NULL, 0U, &size) == UMI_STATUS_OK &&
        size < sizeof bytes);
  if (!strcmp(name, "measure"))
    return 0;
  if (!strcmp(name, "short")) {
    size_t need = 0U;
    CHECK(UmiKanbanProjectBindingEncode(&value, bytes, size - 1U, &need) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(need == size && !memcmp(bytes, before, sizeof bytes));
    return 0;
  }
  CHECK(UmiKanbanProjectBindingEncode(&value, bytes, sizeof bytes, &size) == UMI_STATUS_OK);
  if (!strcmp(name, "unused-tail")) {
    value.project_root[strlen(value.project_root) + 1U] = 'X';
    size_t size2 = 0U;
    CHECK(UmiKanbanProjectBindingEncode(&value, before, sizeof before, &size2) == UMI_STATUS_OK);
    CHECK(size2 == size && !memcmp(bytes, before, size));
    return 0;
  }
  if (!strcmp(name, "roundtrip") || !strcmp(name, "ownership")) {
    CHECK(UmiKanbanProjectBindingDecode(bytes, size, &decoded) == UMI_STATUS_OK);
    CHECK(BindingEqual(&value, &decoded));
    memset(bytes, 'x', size);
    CHECK(BindingEqual(&value, &decoded));
    return 0;
  }
  memset(&decoded, 0x51, sizeof decoded);
  sentinel = decoded;
  if (!strcmp(name, "truncated"))
    --size;
  else if (!strcmp(name, "checksum"))
    bytes[size - 1U] ^= 1U;
  else if (!strcmp(name, "schema"))
    UmiArchiveFinish(bytes, size, UINT64_C(0x554d494b4c4f4302));
  else if (!strcmp(name, "trailing")) {
    bytes[size++] = 'x';
    UmiArchiveFinish(bytes, size, UINT64_C(0x554d494b4c4f4301));
  } else if (!strcmp(name, "invalid-root")) {
    bytes[UMI_VALUE_ARCHIVE_HEADER_SIZE + 8U] = '.';
    UmiArchiveFinish(bytes, size, UINT64_C(0x554d494b4c4f4301));
  } else if (!strcmp(name, "embedded-nul")) {
    bytes[UMI_VALUE_ARCHIVE_HEADER_SIZE + 8U + 3U] = '\0';
    UmiArchiveFinish(bytes, size, UINT64_C(0x554d494b4c4f4301));
  } else if (!strcmp(name, "oversized")) {
    CHECK(UmiKanbanProjectBindingDecode(bytes, UMI_KANBAN_BINDING_MAXIMUM_BYTES + 1U, &decoded) ==
          UMI_STATUS_PARSE_ERROR);
    CHECK(!memcmp(&decoded, &sentinel, sizeof decoded));
    return 0;
  } else
    return 2;
  CHECK(UmiKanbanProjectBindingDecode(bytes, size, &decoded) != UMI_STATUS_OK);
  CHECK(!memcmp(&decoded, &sentinel, sizeof decoded));
  return 0;
}
