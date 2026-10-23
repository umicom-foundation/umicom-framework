/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/publication.c
 * PURPOSE: Keep private board content behind an explicit publication projection shared by native hosts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/kanban/publication.h"
#include "board_internal.h"
#include <string.h>

typedef struct HtmlWriter { char *bytes; size_t used; UmiStatus status; } HtmlWriter;
static void Put(HtmlWriter *writer, const char *text, size_t length)
{
    if (writer->status != UMI_STATUS_OK) return;
    if (length > UMI_KANBAN_PUBLICATION_BYTE_LIMIT - writer->used) {
        writer->status = UMI_STATUS_CAPACITY_EXCEEDED; return;
    }
    if (writer->bytes != NULL) memcpy(writer->bytes + writer->used, text, length);
    writer->used += length;
}
static void Literal(HtmlWriter *writer, const char *text) { Put(writer, text, strlen(text)); }
static void Escape(HtmlWriter *writer, const char *text)
{
    for (const unsigned char *p = (const unsigned char *)text; *p != 0U; ++p) {
        switch (*p) {
        case '&': Literal(writer, "&amp;"); break;
        case '<': Literal(writer, "&lt;"); break;
        case '>': Literal(writer, "&gt;"); break;
        case '"': Literal(writer, "&quot;"); break;
        case '\'': Literal(writer, "&#39;"); break;
        default: Put(writer, (const char *)p, 1U); break;
        }
    }
}
/* Validate canonical UTF-8 before emitting the document. This rejects embedded
 * controls and invalid/overlong encodings rather than publishing ambiguous text. */
static bool PublicText(const char *text, size_t capacity, bool optional)
{
    if (text == NULL) return false;
    size_t length = 0U;
    while (length < capacity && text[length] != '\0') ++length;
    if (length == capacity || (!optional && length == 0U)) return false;
    const unsigned char *p = (const unsigned char *)text;
    for (size_t i = 0U; i < length;) {
        unsigned char first = p[i];
        if (first < 0x80U) {
            if ((first < 0x20U && first != '\n' && first != '\t' && first != '\r') || first == 0x7fU) return false;
            ++i; continue;
        }
        size_t width = first >= 0xc2U && first <= 0xdfU ? 2U :
            first >= 0xe0U && first <= 0xefU ? 3U : first >= 0xf0U && first <= 0xf4U ? 4U : 0U;
        if (width == 0U || width > length - i) return false;
        for (size_t j = 1U; j < width; ++j) if ((p[i + j] & 0xc0U) != 0x80U) return false;
        if ((first == 0xe0U && p[i + 1U] < 0xa0U) || (first == 0xedU && p[i + 1U] > 0x9fU) ||
            (first == 0xf0U && p[i + 1U] < 0x90U) || (first == 0xf4U && p[i + 1U] > 0x8fU)) return false;
        i += width;
    }
    return true;
}
static const UmiKanbanStage PublicStages[] = {UMI_KANBAN_TODO, UMI_KANBAN_DOING,
    UMI_KANBAN_TESTING, UMI_KANBAN_BLOCKED, UMI_KANBAN_DONE};
static const char *const PublicTitles[] = {"To do", "Doing", "Testing", "Blocked", "Done"};
static const char *const PublicSlugs[] = {"todo", "doing", "testing", "blocked", "done"};
static void Render(HtmlWriter *writer, const UmiKanbanBoard *board,
    const char *title, const UmiKanbanPublicCard *cards, size_t count)
{
    Literal(writer, "<!doctype html><html lang=\"en\"><head><meta charset=\"utf-8\">"
        "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
        "<meta name=\"referrer\" content=\"no-referrer\"><title>");
    Escape(writer, title);
    Literal(writer, "</title><style>"
        ":root{color-scheme:light;--ink:#173247;--muted:#596c7c;--line:#dce5eb;--blue:#216b99}"
        "*{box-sizing:border-box}body{margin:0;background:#eff4f7;color:var(--ink);font:16px/1.55 system-ui,sans-serif}"
        "header{background:#122e43;color:white;padding:30px clamp(20px,4vw,64px)}"
        "header small{letter-spacing:.12em;color:#adcddd}h1{font-size:clamp(27px,4vw,42px);margin:8px 0}"
        "header p{max-width:850px;color:#d6e5ee}nav{display:flex;gap:12px;flex-wrap:wrap;padding:16px clamp(20px,4vw,64px);background:white;border-bottom:1px solid var(--line)}"
        "a{color:var(--blue)}nav a{padding:6px 12px;border:1px solid var(--line);border-radius:20px;text-decoration:none}"
        "main{padding:24px;display:grid;grid-template-columns:repeat(5,minmax(240px,1fr));gap:16px;overflow:auto;align-items:start}"
        "section{background:#e7eef3;padding:14px;border-radius:12px;scroll-margin-top:20px;min-height:200px}"
        "section h2{font-size:17px;margin:0 0 18px;border-top:4px solid #7d97a9;padding-top:12px}"
        "#doing h2{border-color:#2a84bd}#testing h2{border-color:#9277b8}#blocked h2{border-color:#d38b3c}#done h2{border-color:#408f73}"
        "article{background:white;border:1px solid var(--line);border-radius:9px;padding:17px;margin-bottom:12px;box-shadow:0 2px 3px #17324705}"
        "h3{font-size:17px;line-height:1.4;margin:0 0 10px;overflow-wrap:anywhere}p{margin:8px 0;overflow-wrap:anywhere;white-space:pre-wrap}"
        ".owner{color:#25634e;font-size:14px;margin-top:16px}.empty,footer{color:var(--muted)}footer{padding:20px 24px;font-size:13px}"
        ":focus-visible{outline:3px solid #1d76a7;outline-offset:3px}"
        "@media(max-width:700px){main{display:block;padding:16px}section{margin-bottom:20px;min-height:100px}nav{position:static}}"
        "@media print{main{display:block;overflow:visible}section,article{break-inside:avoid}nav{display:none}header{background:white;color:#173247}header p{color:#596c7c}}"
        "</style></head><body><header><small>UMICOM · CONTRIBUTOR BOARD</small><h1>");
    Escape(writer, title);
    Literal(writer, "</h1><p>A published view of planned work. The board owner manages assignments and updates this snapshot."
        " A list position describes planning status, not verified build or test results.</p></header><nav aria-label=\"Board lists\">");
    for (size_t lane = 0U; lane < 5U; ++lane) {
        Literal(writer, "<a href=\"#"); Literal(writer, PublicSlugs[lane]); Literal(writer, "\">");
        Literal(writer, PublicTitles[lane]); Literal(writer, "</a>");
    }
    Literal(writer, "</nav><main>");
    for (size_t lane = 0U; lane < 5U; ++lane) {
        Literal(writer, "<section id=\""); Literal(writer, PublicSlugs[lane]); Literal(writer, "\"><h2>");
        Literal(writer, PublicTitles[lane]); Literal(writer, "</h2>");
        size_t in_lane = 0U;
        for (size_t i = 0U; i < count; ++i) {
            size_t index = UmiKanbanFind(board, cards[i].card_id);
            if (board->config.columns[board->rows[index].column].stage != PublicStages[lane]) continue;
            ++in_lane;
            Literal(writer, "<article><h3>"); Escape(writer, cards[i].title); Literal(writer, "</h3>");
            if (cards[i].summary[0] != '\0') { Literal(writer, "<p>"); Escape(writer, cards[i].summary); Literal(writer, "</p>"); }
            if (cards[i].developer[0] != '\0') { Literal(writer, "<p class=\"owner\">Assigned to "); Escape(writer, cards[i].developer); Literal(writer, "</p>"); }
            Literal(writer, "</article>");
        }
        if (in_lane == 0U) Literal(writer, "<p class=\"empty\">No published cards in this list.</p>");
        Literal(writer, "</section>");
    }
    Literal(writer, "</main><footer>Read-only snapshot · Generated by Umicom Framework · No account, cookies or tracking required.</footer></body></html>");
}
UmiStatus umi_kanban_publication_html(const UmiKanbanBoard *board,
    uint64_t expected_revision, const char *public_title,
    const UmiKanbanPublicCard *cards, size_t count,
    char *bytes, size_t capacity, size_t *out_size)
{
    if (board == NULL || out_size == NULL || (cards == NULL && count != 0U) ||
        count > UMI_KANBAN_MAX_CARDS || (bytes == NULL && capacity != 0U) ||
        !PublicText(public_title, 256U, false)) return UMI_STATUS_INVALID_ARGUMENT;
    if (expected_revision != board->revision) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < count; ++i) {
        if (!UmiKanbanId(cards[i].card_id, false) || !PublicText(cards[i].title, sizeof(cards[i].title), false) ||
            !PublicText(cards[i].summary, sizeof(cards[i].summary), true) ||
            !PublicText(cards[i].developer, sizeof(cards[i].developer), true)) return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(cards[i].card_id, cards[j].card_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
        size_t index = UmiKanbanFind(board, cards[i].card_id);
        if (index == UMI_KANBAN_MISSING) return UMI_STATUS_NOT_FOUND;
        if (board->rows[index].archived) return UMI_STATUS_INVALID_STATE;
    }
    HtmlWriter measure = {NULL, 0U, UMI_STATUS_OK};
    Render(&measure, board, public_title, cards, count);
    if (measure.status != UMI_STATUS_OK) return measure.status;
    if (measure.used == UMI_KANBAN_PUBLICATION_BYTE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    *out_size = measure.used + 1U;
    if (bytes == NULL) return UMI_STATUS_OK;
    if (capacity < *out_size) return UMI_STATUS_CAPACITY_EXCEEDED;
    HtmlWriter writer = {bytes, 0U, UMI_STATUS_OK};
    Render(&writer, board, public_title, cards, count);
    bytes[writer.used] = '\0';
    return writer.status;
}
