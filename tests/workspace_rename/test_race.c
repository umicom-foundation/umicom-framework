/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workspace_rename/test_race.c
 *
 * PURPOSE:
 *   Exercise competing no-replacement renames and prove the losing source and winning bytes
 * survive.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/platform/threading.h"
typedef struct Worker {
  const char *root;
  const char *name;
  UmiStatus status;
  UmiWorkspaceRenameResult result;
} Worker;
static int Run(void *data) {
  Worker *worker = data;
  UmiWorkspaceGraph *graph = NULL;
  CHECK(umi_workspace_graph_create(&graph) == UMI_STATUS_OK);
  CHECK(umi_workspace_graph_open(graph, worker->root, 1) == UMI_STATUS_OK);
  worker->status = UmiWorkspaceRenameEntry(graph, 0U, worker->name, "winner.c", &worker->result);
  umi_workspace_graph_destroy(graph);
  return 0;
}
int main(void) {
  RenameFixture f = {0};
  CHECK(StartRename(&f) == 0);
  CHECK(PutRename(&f, "first.c", "first") == 0);
  CHECK(PutRename(&f, "second.c", "second") == 0);
  Worker first = {.root = f.root, .name = "first.c"}, second = {.root = f.root, .name = "second.c"};
  UmiThread *a = NULL, *b = NULL;
  int ea = 1, eb = 1;
  CHECK(umi_thread_start(Run, &first, &a) == UMI_STATUS_OK);
  CHECK(umi_thread_start(Run, &second, &b) == UMI_STATUS_OK);
  CHECK(umi_thread_join(a, &ea) == UMI_STATUS_OK && umi_thread_join(b, &eb) == UMI_STATUS_OK);
  umi_thread_destroy(a);
  umi_thread_destroy(b);
  CHECK(ea == 0 && eb == 0);
  if (first.status == UMI_STATUS_NOT_IMPLEMENTED && second.status == UMI_STATUS_NOT_IMPLEMENTED) {
    CHECK(StopRename(&f) == 0);
    return 77;
  }
  CHECK((first.status == UMI_STATUS_OK && second.status == UMI_STATUS_ALREADY_EXISTS) ||
        (second.status == UMI_STATUS_OK && first.status == UMI_STATUS_ALREADY_EXISTS));
  Worker *winner = first.result.renamed ? &first : &second,
         *loser = first.result.renamed ? &second : &first;
  CHECK(RenameText(winner->result.destination, winner == &first ? "first" : "second") == 0);
  CHECK(RenameText(loser->result.source, loser == &first ? "first" : "second") == 0);
  CHECK(!umi_fs_exists(winner->result.source));
  CHECK(StopRename(&f) == 0);
  return 0;
}
