/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build/test_task_registry.c
 *
 * PURPOSE:
 *   Verify registry ownership, capacity and revision exhaustion without production counter hooks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/task.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Direct inclusion makes private revision limits reachable without exposing
 * counter setters in the public SDK. All operations still use the real owner. */
#include "../../src/build/task.c"
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                   \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
static int Is(const char *a, const char *b) { return strcmp(a, b) == 0; }
static UmiStatus Add(UmiBuildTaskRegistry *owner, const char *id) {
  UmiBuildTaskSnapshot task;
  umi_build_task_init(&task, id, id, UMI_BUILD_TASK_BUILD);
  return umi_build_task_registry_upsert(owner, &task);
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *mode = argv[1];
  if (Is(mode, "create-null"))
    return umi_build_task_registry_create(NULL) == UMI_STATUS_INVALID_ARGUMENT ? 0 : 1;
  if (Is(mode, "null-reset")) {
    CHECK(umi_build_task_registry_reset(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    umi_build_task_registry_clear(NULL);
    return 0;
  }
  UmiBuildTaskRegistry *owner = NULL;
  CHECK(umi_build_task_registry_create(&owner) == UMI_STATUS_OK && owner != NULL);
  UmiBuildTaskRegistry *before = malloc(sizeof(*before));
  CHECK(before != NULL);
  UmiBuildTaskSnapshot task, found;
  umi_build_task_init(&task, "build", "Build", UMI_BUILD_TASK_BUILD);
  if (Is(mode, "new-owner")) {
    CHECK(umi_build_task_registry_count(owner) == 0U &&
          umi_build_task_registry_revision(owner) == 1U);
  } else if (Is(mode, "owned-copy")) {
    CHECK(umi_build_task_registry_upsert(owner, &task) == UMI_STATUS_OK);
    memset(&task, 0, sizeof task);
    CHECK(umi_build_task_registry_find(owner, "build", &found) == UMI_STATUS_OK);
    CHECK(strcmp(found.label, "Build") == 0 && found.has_build_phase == 1);
  } else if (Is(mode, "zero-task-revision")) {
    task.revision = 0U;
    CHECK(umi_build_task_registry_upsert(owner, &task) == UMI_STATUS_OK);
    CHECK(umi_build_task_registry_find(owner, "build", &found) == UMI_STATUS_OK &&
          found.revision == 1U);
    CHECK(task.revision == 0U);
  } else if (Is(mode, "replace-order") || Is(mode, "remove-middle")) {
    CHECK(Add(owner, "first") == UMI_STATUS_OK && Add(owner, "build") == UMI_STATUS_OK &&
          Add(owner, "last") == UMI_STATUS_OK);
    const uint64_t revision = umi_build_task_registry_revision(owner);
    if (Is(mode, "replace-order")) {
      strcpy(task.label, "Replacement");
      task.revision = 47U;
      CHECK(umi_build_task_registry_upsert(owner, &task) == UMI_STATUS_OK);
      CHECK(umi_build_task_registry_at(owner, 1U, &found) == UMI_STATUS_OK);
      CHECK(strcmp(found.label, "Replacement") == 0 && found.revision == 2U);
      CHECK(umi_build_task_registry_count(owner) == 3U);
    } else {
      CHECK(umi_build_task_registry_remove(owner, "build") == UMI_STATUS_OK);
      CHECK(umi_build_task_registry_count(owner) == 2U);
      CHECK(umi_build_task_registry_at(owner, 1U, &found) == UMI_STATUS_OK &&
            strcmp(found.task_id, "last") == 0);
    }
    CHECK(umi_build_task_registry_revision(owner) == revision + 1U);
    CHECK(umi_build_task_registry_at(owner, 0U, &found) == UMI_STATUS_OK &&
          strcmp(found.task_id, "first") == 0);
  } else if (Is(mode, "capacity-preserves")) {
    for (size_t i = 0U; i < UMI_BUILD_TASK_CAPACITY; ++i) {
      char id[32];
      snprintf(id, sizeof id, "task-%zu", i);
      CHECK(Add(owner, id) == UMI_STATUS_OK);
    }
    *before = *owner;
    CHECK(umi_build_task_registry_upsert(owner, &task) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(before, owner, sizeof *owner) == 0);
    strcpy(task.task_id, "task-0");
    CHECK(umi_build_task_registry_upsert(owner, &task) == UMI_STATUS_OK);
    CHECK(umi_build_task_registry_count(owner) == UMI_BUILD_TASK_CAPACITY);
  } else if (Is(mode, "missing-preserves")) {
    CHECK(Add(owner, "build") == UMI_STATUS_OK);
    *before = *owner;
    CHECK(umi_build_task_registry_remove(owner, "missing") == UMI_STATUS_NOT_FOUND);
    memset(&found, 0xa5, sizeof found);
    UmiBuildTaskSnapshot saved = found;
    CHECK(umi_build_task_registry_find(owner, "missing", &found) == UMI_STATUS_NOT_FOUND);
    CHECK(umi_build_task_registry_at(owner, 1U, &found) == UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(&saved, &found, sizeof found) == 0 && memcmp(before, owner, sizeof *owner) == 0);
  } else if (Is(mode, "reset-normal")) {
    CHECK(Add(owner, "build") == UMI_STATUS_OK);
    const uint64_t revision = owner->revision;
    CHECK(umi_build_task_registry_reset(owner) == UMI_STATUS_OK);
    CHECK(owner->count == 0U && owner->revision == revision + 1U);
    CHECK(Add(owner, "other") == UMI_STATUS_OK);
    umi_build_task_registry_clear(owner);
    CHECK(owner->count == 0U && owner->revision == revision + 3U);
  } else {
    CHECK(Add(owner, "build") == UMI_STATUS_OK);
    if (Is(mode, "replacement-task-limit"))
      owner->tasks[0].revision = UINT64_MAX;
    else
      owner->revision = UINT64_MAX;
    *before = *owner;
    UmiStatus status = UMI_STATUS_OK;
    if (Is(mode, "insert-revision-limit")) {
      strcpy(task.task_id, "new");
      status = umi_build_task_registry_upsert(owner, &task);
    } else if (Is(mode, "replace-revision-limit") || Is(mode, "replacement-task-limit"))
      status = umi_build_task_registry_upsert(owner, &task);
    else if (Is(mode, "remove-revision-limit"))
      status = umi_build_task_registry_remove(owner, "build");
    else if (Is(mode, "reset-revision-limit"))
      status = umi_build_task_registry_reset(owner);
    else if (Is(mode, "clear-revision-limit")) {
      umi_build_task_registry_clear(owner);
      status = UMI_STATUS_CAPACITY_EXCEEDED;
    } else {
      free(before);
      umi_build_task_registry_destroy(owner);
      return 2;
    }
    CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && memcmp(before, owner, sizeof *owner) == 0);
  }
  free(before);
  umi_build_task_registry_destroy(owner);
  return 0;
}
