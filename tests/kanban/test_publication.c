/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban/test_publication.c
 * PURPOSE: Prevent private planning data and active markup from crossing the public HTML boundary.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/kanban/publication.h"
static char *Html(UmiKanbanBoard *board, const UmiKanbanPublicCard *cards, size_t count)
{
    size_t size = 0U; uint64_t revision = umi_kanban_board_revision(board);
    REQUIRE(umi_kanban_publication_html(board, revision, "Public roadmap", cards, count, NULL, 0U, &size) == UMI_STATUS_OK);
    char *bytes = malloc(size); REQUIRE(bytes != NULL);
    REQUIRE(umi_kanban_publication_html(board, revision, "Public roadmap", cards, count, bytes, size, &size) == UMI_STATUS_OK);
    REQUIRE(strlen(bytes) + 1U == size); return bytes;
}
static void Refuse(UmiKanbanBoard *board, const UmiKanbanPublicCard *cards,
    size_t count, uint64_t revision, UmiStatus expected)
{
    char bytes[32]; memset(bytes, 'z', sizeof(bytes)); char before[32]; memcpy(before, bytes, sizeof(bytes));
    size_t size = 912U;
    REQUIRE(umi_kanban_publication_html(board, revision, "Public title", cards, count, bytes, sizeof(bytes), &size) == expected);
    REQUIRE(memcmp(bytes, before, sizeof(bytes)) == 0 && size == 912U);
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2); UmiKanbanBoard *board = NULL;
    REQUIRE(umi_kanban_board_create_workflow("secret.board", "PRIVATE_BOARD_TITLE", &board) == UMI_STATUS_OK);
    UmiKanbanEdit *edit = Edits(1U); Create(edit, "secret.card");
    strcpy(edit->card.title, "PRIVATE_CARD_TITLE"); strcpy(edit->card.description, "PRIVATE_CHAT_AND_PATH");
    strcpy(edit->card.assignee, "private.developer.id");
    Apply(board, edit, 1U);
    UmiKanbanPublicCard cards[2] = {0}; strcpy(cards[0].card_id, "secret.card"); strcpy(cards[0].title, "Reviewed feature");
    uint64_t revision = umi_kanban_board_revision(board);
    if (strcmp(argv[1], "projection") == 0 || strcmp(argv[1], "empty") == 0) {
        char *html = Html(board, cards, strcmp(argv[1], "empty") == 0 ? 0U : 1U);
        REQUIRE(strstr(html, "PRIVATE_") == NULL && strstr(html, "secret.") == NULL);
        REQUIRE(strstr(html, "private.developer.id") == NULL && strstr(html, "local-owner") == NULL);
        REQUIRE(strstr(html, "<script") == NULL && strstr(html, "<form") == NULL);
        REQUIRE(strstr(html, "Assigned to ") == NULL);
        REQUIRE((strstr(html, "Reviewed feature") != NULL) == (strcmp(argv[1], "projection") == 0));
        free(html);
    } else if (strcmp(argv[1], "escaping") == 0) {
        strcpy(cards[0].title, "<script>alert('x')</script> & \"review\"");
        strcpy(cards[0].summary, "<img src=x onerror=alert(1)>"); strcpy(cards[0].developer, "A & B");
        char *html = Html(board, cards, 1U);
        REQUIRE(strstr(html, "&lt;script&gt;alert(&#39;x&#39;)&lt;/script&gt; &amp; &quot;review&quot;") != NULL);
        REQUIRE(strstr(html, "<script") == NULL && strstr(html, "<img") == NULL);
        REQUIRE(strstr(html, "Assigned to A &amp; B") != NULL); free(html);
    } else if (strcmp(argv[1], "utf8") == 0) {
        strcpy(cards[0].title, "Fran\xc3\xa7ois \xe6\x97\xa5\xe6\x9c\xac");
        char *html = Html(board, cards, 1U); REQUIRE(strstr(html, cards[0].title) != NULL); free(html);
        const char *invalid[] = {"\xc0\xaf", "\xed\xa0\x80", "\xf4\x90\x80\x80", "\xe2\x82", "\x80", "\x01", "\x7f"};
        for (size_t i = 0U; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
            strcpy(cards[0].title, invalid[i]); Refuse(board, cards, 1U, revision, UMI_STATUS_INVALID_ARGUMENT);
        }
    } else if (strcmp(argv[1], "short-buffer") == 0) {
        size_t required = 0U;
        REQUIRE(umi_kanban_publication_html(board, revision, "Title", cards, 1U, NULL, 0U, &required) == UMI_STATUS_OK);
        char *bytes = malloc(required); REQUIRE(bytes != NULL); memset(bytes, 'x', required); size_t result = 0U;
        REQUIRE(umi_kanban_publication_html(board, revision, "Title", cards, 1U, bytes, required - 1U, &result) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(result == required);
        for (size_t i = 0U; i < required; ++i) REQUIRE(bytes[i] == 'x');
        free(bytes);
    } else if (strcmp(argv[1], "stale") == 0) {
        Move(board, edit, "secret.card", "doing"); Refuse(board, cards, 1U, revision, UMI_STATUS_INVALID_STATE);
    } else if (strcmp(argv[1], "selection") == 0) {
        cards[1] = cards[0]; Refuse(board, cards, 2U, revision, UMI_STATUS_ALREADY_EXISTS);
        strcpy(cards[0].card_id, "missing"); Refuse(board, cards, 1U, revision, UMI_STATUS_NOT_FOUND);
        strcpy(cards[0].card_id, "secret.card");
        Action(edit, UMI_KANBAN_ARCHIVE_CARD, "secret.card", NULL); Apply(board, edit, 1U);
        Refuse(board, cards, 1U, umi_kanban_board_revision(board), UMI_STATUS_INVALID_STATE);
    } else if (strcmp(argv[1], "bounded-fields") == 0) {
        memset(cards[0].summary, 'x', sizeof(cards[0].summary)); Refuse(board, cards, 1U, revision, UMI_STATUS_INVALID_ARGUMENT);
        cards[0].summary[0] = '\0'; memset(cards[0].developer, 'x', sizeof(cards[0].developer));
        Refuse(board, cards, 1U, revision, UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(argv[1], "byte-limit") == 0) {
        UmiKanbanPublicCard *selection = calloc(UMI_KANBAN_MAX_CARDS, sizeof(*selection));
        UmiKanbanEdit *batch = Edits(UMI_KANBAN_MAX_EDITS); REQUIRE(selection != NULL);
        strcpy(selection[0].card_id, "secret.card"); size_t pending = 0U;
        for (size_t i = 1U; i < UMI_KANBAN_MAX_CARDS; ++i) {
            char id[64]; (void)snprintf(id, sizeof(id), "public.%zu", i);
            Create(&batch[pending++], id); strcpy(selection[i].card_id, id);
            if (pending == UMI_KANBAN_MAX_EDITS || i + 1U == UMI_KANBAN_MAX_CARDS) {
                Apply(board, batch, pending); pending = 0U;
            }
        }
        /* Quotes expand to six HTML bytes each. A valid maximum-size board
         * can therefore exceed the website byte budget without invalid input. */
        for (size_t i = 0U; i < UMI_KANBAN_MAX_CARDS; ++i) {
            memset(selection[i].title, '"', sizeof(selection[i].title) - 1U);
            memset(selection[i].summary, '"', sizeof(selection[i].summary) - 1U);
            memset(selection[i].developer, '"', sizeof(selection[i].developer) - 1U);
        }
        Refuse(board, selection, UMI_KANBAN_MAX_CARDS, umi_kanban_board_revision(board), UMI_STATUS_CAPACITY_EXCEEDED);
        free(selection); free(batch);
    } else if (strcmp(argv[1], "stage-placement") == 0) {
        const char *columns[] = {"todo", "doing", "testing", "blocked", "done"};
        for (size_t i = 0U; i < 5U; ++i) {
            Move(board, edit, "secret.card", columns[i]); char *html = Html(board, cards, 1U);
            char section[64]; (void)snprintf(section, sizeof(section), "<section id=\"%s\">", columns[i]);
            const char *begin = strstr(html, section), *card = strstr(html, "Reviewed feature");
            REQUIRE(begin != NULL && card != NULL && card > begin && card < strstr(begin, "</section>")); free(html);
        }
    } else REQUIRE(0);
    free(edit); umi_kanban_board_destroy(board); return 0;
}
