/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_publication_plan/test_archive.c
 *
 * PURPOSE:
 *   Verify full archive admission, deterministic encoding and rejected damaged plans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../../src/base/value_archive_internal.h"
#include "fixture.h"
int main(int argc, char **argv) {
  REQUIRE(argc == 2);
  const char *name = argv[1],
             *cases[] = {"roundtrip", "short",    "measure",      "truncated",    "checksum",
                         "schema",    "trailing", "embedded-nul", "invalid-utf8", "oversized-count",
                         "duplicate", "empty",    "ownership",    "unused-tail",  "null"};
  CaseKnown(name, cases, sizeof cases / sizeof cases[0]);
  UmiKanbanBoard *board = PlanBoard();
  UmiKanbanPublicationPlan *plan = Plan(board), *decoded = NULL;
  if (!strcmp(name, "empty")) {
    UmiKanbanPublicationPlanDestroy(plan);
    REQUIRE(UmiKanbanPublicationPlanCreate(board, umi_kanban_board_revision(board), "Empty", NULL,
                                           0U, &plan) == UMI_STATUS_OK);
  }
  size_t n;
  unsigned char *bytes = PlanBytes(plan, &n);
  if (!strcmp(name, "measure")) {
    size_t count = 0U;
    REQUIRE(UmiKanbanPublicationPlanEncode(plan, NULL, 0U, &count) == UMI_STATUS_OK && count == n);
  } else if (!strcmp(name, "short")) {
    unsigned char *short_out = malloc(n);
    REQUIRE(short_out != NULL);
    memset(short_out, 0xa5, n);
    size_t count = 0U;
    REQUIRE(UmiKanbanPublicationPlanEncode(plan, short_out, n - 1U, &count) ==
                UMI_STATUS_CAPACITY_EXCEEDED &&
            count == n);
    for (size_t i = 0U; i < n; ++i)
      REQUIRE(short_out[i] == 0xa5);
    free(short_out);
  } else if (!strcmp(name, "truncated")) {
    for (size_t i = 0U; i < n; ++i)
      REQUIRE(UmiKanbanPublicationPlanDecode(bytes, i, &decoded) != UMI_STATUS_OK &&
              decoded == NULL);
  } else if (!strcmp(name, "null")) {
    REQUIRE(UmiKanbanPublicationPlanDecode(NULL, 0U, &decoded) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(UmiKanbanPublicationPlanDecode(bytes, n, NULL) == UMI_STATUS_INVALID_ARGUMENT);
  } else if (!strcmp(name, "unused-tail")) {
    UmiKanbanPublicCard cards[2] = {PublicCard("card.first"), PublicCard("card.other")};
    memset(cards[0].summary + strlen(cards[0].summary) + 1U, 0xa5,
           sizeof cards[0].summary - strlen(cards[0].summary) - 1U);
    UmiKanbanPublicationPlan *other = NULL;
    REQUIRE(UmiKanbanPublicationPlanCreate(board, umi_kanban_board_revision(board),
                                           "Public roadmap", cards, 2U, &other) == UMI_STATUS_OK);
    size_t size;
    unsigned char *copy = PlanBytes(other, &size);
    REQUIRE(size == n && !memcmp(copy, bytes, n));
    free(copy);
    UmiKanbanPublicationPlanDestroy(other);
  } else {
    UmiStatus expected = UMI_STATUS_OK;
    if (!strcmp(name, "checksum")) {
      bytes[n - 1U] ^= 1U;
      expected = UMI_STATUS_PARSE_ERROR;
    }
    if (!strcmp(name, "schema")) {
      UmiArchiveFinish(bytes, n, UINT64_C(1));
      expected = UMI_STATUS_UNAVAILABLE;
    }
    if (!strcmp(name, "trailing")) {
      bytes = realloc(bytes, n + 1U);
      REQUIRE(bytes != NULL);
      bytes[n++] = 0U;
      UmiArchiveFinish(bytes, n, UINT64_C(0x554d494b50554201));
      expected = UMI_STATUS_PARSE_ERROR;
    }
    if (!strcmp(name, "embedded-nul") || !strcmp(name, "invalid-utf8")) {
      bytes[FindBytes(bytes, n, "Public roadmap")] =
          (unsigned char)(!strcmp(name, "embedded-nul") ? 0U : 0xffU);
      UmiArchiveFinish(bytes, n, UINT64_C(0x554d494b50554201));
      expected =
          !strcmp(name, "embedded-nul") ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!strcmp(name, "oversized-count")) {
      size_t offset = 32U + 8U + strlen("private.board") + 8U + strlen("Public roadmap") + 8U;
      UmiArchiveStoreInteger(bytes + offset, UMI_KANBAN_MAX_CARDS + 1U, 8U);
      UmiArchiveFinish(bytes, n, UINT64_C(0x554d494b50554201));
      expected = UMI_STATUS_PARSE_ERROR;
    }
    if (!strcmp(name, "duplicate")) {
      memcpy(bytes + FindBytes(bytes, n, "card.other"), "card.first", 10U);
      UmiArchiveFinish(bytes, n, UINT64_C(0x554d494b50554201));
      expected = UMI_STATUS_ALREADY_EXISTS;
    }
    REQUIRE(UmiKanbanPublicationPlanDecode(bytes, n, &decoded) == expected);
    if (expected == UMI_STATUS_OK) {
      if (!strcmp(name, "ownership")) {
        memset(bytes, 0, n);
        UmiKanbanPublicationPlanDestroy(plan);
        plan = NULL;
      }
      UmiKanbanPublicationPlanInfo info;
      REQUIRE(UmiKanbanPublicationPlanRead(decoded, &info) == UMI_STATUS_OK);
      REQUIRE(info.card_count == (!strcmp(name, "empty") ? 0U : 2U));
    } else
      REQUIRE(decoded == NULL);
  }
  free(bytes);
  UmiKanbanPublicationPlanDestroy(decoded);
  UmiKanbanPublicationPlanDestroy(plan);
  umi_kanban_board_destroy(board);
  return 0;
}
