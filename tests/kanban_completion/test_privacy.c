/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_completion/test_privacy.c
 *
 * PURPOSE:
 *   Keep private completion evidence separate from portable board and public roadmap output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
#include "umicom/kanban/publication.h"
static int Contains(const unsigned char *bytes, size_t size, const char *text)
{
    size_t count = strlen(text);
    for (size_t i = 0; i <= size && count <= size - i; ++i)
        if (!memcmp(bytes + i, text, count)) return 1;
    return 0;
}
int main(int argc, char **argv)
{
    const char *cases[] = {"board-archive","public-html","private-archive"};
    REQUIRE(argc == 2 && Known(argv[1], cases, sizeof cases / sizeof cases[0]));
    UmiKanbanBoard *board = Ready();
    UmiKanbanCompletionRecord record; Record(&record, "alpha");
    const char *private_note = "PRIVATE-REVIEW-ACCEPTANCE-NOTE";
    strcpy(record.rationale, private_note);
    UmiKanbanCompletionPlan *plan = NULL;
    REQUIRE(UmiKanbanCompletionPlanCreate(board, umi_kanban_board_revision(board), "done",
        "private-reviewer", "PRIVATE-SOURCE-IDENTITY", "Debug/native", &record, 1, &plan) == UMI_STATUS_OK);
    REQUIRE(UmiKanbanCompletionPlanApply(plan, board, umi_kanban_board_revision(board)) == UMI_STATUS_OK);
    size_t size = 0; unsigned char *bytes = NULL;
    if (!strcmp(argv[1], "board-archive")) bytes = Snapshot(board, &size);
    else if (!strcmp(argv[1], "public-html")) {
        UmiKanbanPublicCard card = {0};
        strcpy(card.card_id, "alpha"); strcpy(card.title, "Public feature");
        strcpy(card.summary, "Reviewed public description."); strcpy(card.developer, "Contributor");
        REQUIRE(umi_kanban_publication_html(board, umi_kanban_board_revision(board), "Public board",
            &card, 1, NULL, 0, &size) == UMI_STATUS_OK);
        bytes = calloc(size + 1U, 1U); REQUIRE(bytes);
        REQUIRE(umi_kanban_publication_html(board, umi_kanban_board_revision(board), "Public board",
            &card, 1, (char *)bytes, size + 1U, &size) == UMI_STATUS_OK);
    } else {
        REQUIRE(UmiKanbanCompletionPlanEncode(plan, NULL, 0, &size) == UMI_STATUS_OK);
        bytes = malloc(size); REQUIRE(bytes);
        REQUIRE(UmiKanbanCompletionPlanEncode(plan, bytes, size, &size) == UMI_STATUS_OK);
    }
    int private_archive = !strcmp(argv[1], "private-archive");
    REQUIRE(Contains(bytes, size, private_note) == private_archive);
    REQUIRE(Contains(bytes, size, "PRIVATE-SOURCE-IDENTITY") == private_archive);
    REQUIRE(Contains(bytes, size, "private-reviewer") == private_archive);
    free(bytes); UmiKanbanCompletionPlanDestroy(plan); umi_kanban_board_destroy(board); return 0;
}
