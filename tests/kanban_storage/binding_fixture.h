/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_storage/binding_fixture.h
 *
 * PURPOSE:
 *   Share synthetic project identities for private board binding regressions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_KANBAN_BINDING_FIXTURE_H
#define UMICOM_TEST_KANBAN_BINDING_FIXTURE_H
#include "umicom/kanban/binding_storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
#ifdef _WIN32
#define PROJECT "C:/Projects/editor"
#define BOARDS "C:/Private/boards"
#define SETTINGS "C:/Private/settings"
#else
#define PROJECT "/projects/editor"
#define BOARDS "/private/boards"
#define SETTINGS "/private/settings"
#endif
static inline int BindingEqual(const UmiKanbanProjectBinding *a, const UmiKanbanProjectBinding *b) {
  return !strcmp(a->project_root, b->project_root) && !strcmp(a->directory, b->directory) &&
         !strcmp(a->filename, b->filename);
}
#endif
