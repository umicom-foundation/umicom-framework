/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/browser/reading_document_builder.c
 *
 * PURPOSE:
 *   Project bounded document structure into owned text, semantic ranges and explicit link data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "reading_document_internal.h"
#include <string.h>

typedef struct ReadingFrame {
    char name[32];
    UmiBrowserReadingKind kind;
    unsigned int level;
    size_t start, link;
    int preformatted;
} ReadingFrame;
typedef struct ReadingBuilder {
    UmiBrowserReadingDocument *document;
    ReadingFrame frames[64];
    size_t depth, preformatted;
    int pending_space, row_has_cell;
} ReadingBuilder;
static UmiStatus Append(ReadingBuilder *builder, const char *bytes, size_t count)
{
    UmiBrowserReadingDocument *document = builder->document;
    if (count > UMI_BROWSER_READING_TEXT_MAX - document->summary.text_bytes)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(document->text + document->summary.text_bytes, bytes, count);
    document->summary.text_bytes += count;
    document->text[document->summary.text_bytes] = '\0';
    return UMI_STATUS_OK;
}
static UmiStatus Space(ReadingBuilder *builder)
{
    int pending = builder->pending_space; builder->pending_space = 0;
    size_t count = builder->document->summary.text_bytes;
    return pending && count != 0U && builder->document->text[count - 1U] != '\n'
        ? Append(builder, " ", 1U) : UMI_STATUS_OK;
}
static UmiStatus Break(ReadingBuilder *builder)
{
    builder->pending_space = 0;
    size_t count = builder->document->summary.text_bytes;
    return count != 0U && builder->document->text[count - 1U] != '\n'
        ? Append(builder, "\n", 1U) : UMI_STATUS_OK;
}
static int Block(const char *name)
{
    static const char *const names[] = {"p","div","section","article","header","footer",
        "main","nav","aside","li","tr","h1","h2","h3","h4","h5","h6","pre","blockquote",
        "ul","ol","table","figure","figcaption","dl","dt","dd","hr"};
    for (size_t i = 0U; i < sizeof(names)/sizeof(names[0]); ++i)
        if (strcmp(name, names[i]) == 0) return 1;
    return 0;
}
static int Hidden(const char *name)
{
    static const char *const names[] = {"script","style","head","template","svg","iframe","object"};
    for (size_t i = 0U; i < sizeof(names)/sizeof(names[0]); ++i)
        if (strcmp(name, names[i]) == 0) return 1;
    return 0;
}
static int Void(const char *name)
{
    static const char *const names[] = {"area","base","br","col","embed","hr","img","input","link",
        "meta","param","source","track","wbr"};
    for (size_t i = 0U; i < sizeof(names)/sizeof(names[0]); ++i)
        if (strcmp(name, names[i]) == 0) return 1;
    return 0;
}
static UmiStatus Comment(const char *html, size_t size, size_t *cursor)
{
    size_t p = *cursor + 4U;
    while (p + 2U < size && memcmp(html + p, "-->", 3U) != 0) ++p;
    if (p + 2U >= size) return UMI_STATUS_INVALID_ARGUMENT;
    *cursor = p + 3U; return UMI_STATUS_OK;
}
/* Raw script/style bodies are never tokenised as visible text. Other omitted
 * containers are traversed only to find their closing boundary; nested hidden
 * containers cannot prematurely reveal content through a mismatched close. */
static UmiStatus Skip(const char *html, size_t size, size_t *cursor,
    const char *name, size_t depth)
{
    if (depth == 64U) return UMI_STATUS_CAPACITY_EXCEEDED;
    int raw = strcmp(name, "script") == 0 || strcmp(name, "style") == 0;
    size_t name_size = strlen(name);
    while (*cursor < size) {
        if (html[*cursor] != '<') { ++*cursor; continue; }
        if (raw) {
            size_t start = *cursor + 2U;
            if (size - *cursor < name_size + 3U || html[*cursor + 1U] != '/' ||
                !ReadingEqual(html + start, name_size, name) ||
                (!ReadingSpace((unsigned char)html[start + name_size]) && html[start + name_size] != '>')) {
                ++*cursor; continue;
            }
        }
        if (size - *cursor >= 4U && memcmp(html + *cursor, "<!--", 4U) == 0) {
            UmiStatus status = Comment(html, size, cursor);
            if (status != UMI_STATUS_OK) return status;
            continue;
        }
        ReadingTag tag;
        UmiStatus status = ReadingParseTag(html, size, cursor, &tag);
        if (status != UMI_STATUS_OK) return status;
        if (tag.closing && strcmp(tag.name, name) == 0) return UMI_STATUS_OK;
        if (!tag.closing && !tag.self_closing && (strcmp(tag.name, name) == 0 || Hidden(tag.name))) {
            status = Skip(html, size, cursor, tag.name, depth + 1U);
            if (status != UMI_STATUS_OK) return status;
        }
    }
    return UMI_STATUS_INVALID_ARGUMENT;
}
static UmiStatus Anchor(ReadingBuilder *builder, const ReadingTag *tag)
{
    UmiBrowserReadingDocument *document = builder->document;
    char id[UMI_BROWSER_READING_ANCHOR_CAPACITY]; int found;
    UmiStatus status = ReadingAttribute(tag, "id", id, sizeof(id), &found);
    if (status != UMI_STATUS_OK || !found || id[0] == '\0') return status;
    for (size_t i = 0U; id[i] != '\0'; ++i)
        if ((unsigned char)id[i] <= 0x20U || id[i] == 0x7f) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < document->summary.anchors; ++i)
        if (strcmp(id, document->anchors[i].id) == 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (document->summary.anchors == UMI_BROWSER_READING_ANCHORS_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    ReadingAnchor *anchor = &document->anchors[document->summary.anchors++];
    anchor->offset = document->summary.text_bytes;
    memcpy(anchor->id, id, strlen(id) + 1U);
    return UMI_STATUS_OK;
}
static UmiStatus Span(ReadingBuilder *builder, const ReadingFrame *frame)
{
    UmiBrowserReadingDocument *document = builder->document;
    size_t count = document->summary.text_bytes - frame->start;
    if (frame->link != (size_t)-1) document->links[frame->link].byte_length = count;
    if (count == 0U || frame->kind == 0) return UMI_STATUS_OK;
    if (document->summary.spans == UMI_BROWSER_READING_SPANS_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    document->spans[document->summary.spans++] =
        (UmiBrowserReadingSpan){frame->kind, frame->start, count, frame->level};
    return UMI_STATUS_OK;
}
static UmiStatus Style(ReadingBuilder *builder, const ReadingTag *tag)
{
    UmiBrowserReadingKind kind = 0; unsigned int level = 0U;
    int preformatted = strcmp(tag->name, "pre") == 0;
    int link = strcmp(tag->name, "a") == 0;
    if (tag->name[0] == 'h' && tag->name[1] >= '1' && tag->name[1] <= '6' && tag->name[2] == '\0') {
        kind = UMI_BROWSER_READING_HEADING; level = (unsigned int)(tag->name[1] - '0');
    } else if (preformatted || strcmp(tag->name, "code") == 0) kind = UMI_BROWSER_READING_CODE;
    else if (strcmp(tag->name, "strong") == 0 || strcmp(tag->name, "b") == 0) kind = UMI_BROWSER_READING_STRONG;
    else if (strcmp(tag->name, "em") == 0 || strcmp(tag->name, "i") == 0) kind = UMI_BROWSER_READING_EMPHASIS;
    if (kind == 0 && !link) return UMI_STATUS_OK;
    if (tag->closing) {
        if (builder->depth == 0U || strcmp(builder->frames[builder->depth - 1U].name, tag->name) != 0)
            return UMI_STATUS_INVALID_ARGUMENT;
        ReadingFrame *frame = &builder->frames[--builder->depth];
        if (frame->preformatted) --builder->preformatted;
        return Span(builder, frame);
    }
    if (builder->depth == 64U) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = Space(builder);
    if (status != UMI_STATUS_OK) return status;
    ReadingFrame *frame = &builder->frames[builder->depth++];
    memset(frame, 0, sizeof(*frame));
    memcpy(frame->name, tag->name, strlen(tag->name) + 1U);
    frame->kind = kind; frame->level = level; frame->start = builder->document->summary.text_bytes;
    frame->link = (size_t)-1; frame->preformatted = preformatted;
    if (preformatted) ++builder->preformatted;
    if (link) {
        char target[UMI_BROWSER_READING_TARGET_CAPACITY]; int found;
        status = ReadingAttribute(tag, "href", target, sizeof(target), &found);
        if (status != UMI_STATUS_OK) return status;
        if (found) {
            UmiBrowserReadingDocument *document = builder->document;
            if (document->summary.links == UMI_BROWSER_READING_LINKS_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
            frame->link = document->summary.links++;
            UmiBrowserReadingLink *entry = &document->links[frame->link];
            entry->byte_start = frame->start; memcpy(entry->target, target, strlen(target) + 1U);
        }
    }
    if (tag->self_closing) {
        --builder->depth;
        if (preformatted) --builder->preformatted;
        return Span(builder, frame);
    }
    return UMI_STATUS_OK;
}
/* Retain the text-only image implementation for review. The active version
 * keeps exactly that description and adds inert, bounded source metadata so
 * hosts can offer an explicit illustration action under their own policy. */
#if 0
static UmiStatus Image(ReadingBuilder *builder, const ReadingTag *tag)
{
    char alternative[4096]; int found;
    UmiStatus status = ReadingAttribute(tag, "alt", alternative, sizeof(alternative), &found);
    if (status != UMI_STATUS_OK) return status;
    status = Space(builder);
    if (status != UMI_STATUS_OK) return status;
    ReadingFrame frame = {0}; frame.kind = UMI_BROWSER_READING_IMAGE_DESCRIPTION;
    frame.link = (size_t)-1; frame.start = builder->document->summary.text_bytes;
    ++builder->document->summary.image_descriptions;
    if (!found) ++builder->document->summary.missing_image_descriptions;
    status = Append(builder, "[Image: ", 8U);
    const char *label = !found ? "description missing" : alternative[0] == '\0' ? "decorative" : alternative;
    if (status == UMI_STATUS_OK) status = Append(builder, label, strlen(label));
    if (status == UMI_STATUS_OK) status = Append(builder, "]", 1U);
    if (status == UMI_STATUS_OK) status = Span(builder, &frame);
    return status;
}
#endif
static UmiStatus Image(ReadingBuilder *builder, const ReadingTag *tag)
{
    char alternative[4096]; int found;
    UmiStatus status = ReadingAttribute(tag, "alt", alternative, sizeof(alternative), &found);
    if (status != UMI_STATUS_OK) return status;
    status = Space(builder);
    if (status != UMI_STATUS_OK) return status;
    ReadingFrame frame = {0}; frame.kind = UMI_BROWSER_READING_IMAGE_DESCRIPTION;
    frame.link = (size_t)-1; frame.start = builder->document->summary.text_bytes;
    ++builder->document->summary.image_descriptions;
    if (!found) ++builder->document->summary.missing_image_descriptions;
    status = Append(builder, "[Image: ", 8U);
    const char *label = !found ? "description missing" : alternative[0] == '\0' ? "decorative" : alternative;
    if (status == UMI_STATUS_OK) status = Append(builder, label, strlen(label));
    if (status == UMI_STATUS_OK) status = Append(builder, "]", 1U);
    if (status == UMI_STATUS_OK) status = Span(builder, &frame);
    if (status == UMI_STATUS_OK) {
        /* Metadata is owned by the same immutable projection as its caption.
         * Preserve readable text when src is unusable, but never keep a clipped
         * or first-of-two target that could name a different resource. */
        UmiBrowserReadingImage *image = &builder->document->images[
            builder->document->summary.image_descriptions - 1U];
        image->byte_start = frame.start;
        image->byte_length = builder->document->summary.text_bytes - frame.start;
        image->description_missing = !found;
        image->decorative = found && alternative[0] == '\0';
        int target_found = 0;
        image->target_status = ReadingAttribute(tag, "src", image->target,
            sizeof(image->target), &target_found);
        if (image->target_status == UMI_STATUS_OK && (!target_found || image->target[0] == '\0'))
            image->target_status = UMI_STATUS_NOT_FOUND;
        if (image->target_status != UMI_STATUS_OK) memset(image->target, 0, sizeof(image->target));
    }
    return status;
}
UmiStatus ReadingBuild(const char *html, size_t size, UmiBrowserReadingDocument *document)
{
    ReadingBuilder builder = {0}; builder.document = document;
    size_t p = 0U;
    while (p < size) {
        UmiStatus status;
        if (html[p] != '<') {
            char decoded[4]; size_t consumed, written;
            status = ReadingDecode(html + p, size - p, &consumed, decoded, &written);
            if (status != UMI_STATUS_OK) return status;
            p += consumed;
            if (builder.preformatted) {
                /* Normalise physical CRLF/CR only, without changing indentation.
                 * Entity CR is also a newline but does not consume later text. */
                if (written == 1U && decoded[0] == '\r') {
                    decoded[0] = '\n';
                    if (consumed == 1U && p < size && html[p] == '\n') ++p;
                }
                status = Append(&builder, decoded, written);
            } else if (written == 1U && ReadingSpace((unsigned char)decoded[0])) {
                builder.pending_space = 1; continue;
            } else {
                status = Space(&builder);
                if (status == UMI_STATUS_OK) status = Append(&builder, decoded, written);
            }
            if (status != UMI_STATUS_OK) return status;
            continue;
        }
        if (size - p >= 4U && memcmp(html + p, "<!--", 4U) == 0) {
            status = Comment(html, size, &p);
            if (status != UMI_STATUS_OK) return status;
            continue;
        }
        if (size - p >= 9U && ReadingEqual(html + p, 9U, "<!doctype")) {
            /* Declarations are not resolved: external entities cannot be loaded.
             * This reading profile accepts the ordinary HTML doctype only. */
            size_t end = p + 9U;
            while (end < size && ReadingSpace((unsigned char)html[end])) ++end;
            if (size - end < 4U || !ReadingEqual(html + end, 4U, "html")) return UMI_STATUS_INVALID_ARGUMENT;
            end += 4U;
            while (end < size && ReadingSpace((unsigned char)html[end])) ++end;
            if (end >= size || html[end] != '>') return UMI_STATUS_INVALID_ARGUMENT;
            p = end + 1U; continue;
        }
        ReadingTag tag;
        status = ReadingParseTag(html, size, &p, &tag);
        if (status != UMI_STATUS_OK) return status;
        if (Hidden(tag.name)) {
            if (!tag.closing && !tag.self_closing) status = Skip(html, size, &p, tag.name, 0U);
            if (status != UMI_STATUS_OK) return status;
            continue;
        }
        char hidden[8]; int hidden_found = 0;
        if (!tag.closing) {
            status = ReadingAttribute(&tag, "hidden", hidden, sizeof(hidden), &hidden_found);
            if (status != UMI_STATUS_OK) return status;
            if (hidden_found) {
                if (!tag.self_closing && !Void(tag.name)) status = Skip(html, size, &p, tag.name, 0U);
                if (status != UMI_STATUS_OK) return status;
                continue;
            }
        }
        /* Finish a style before inserting its closing block separator, so
         * headings and code ranges do not accidentally style the next line. */
        if (tag.closing) {
            status = Style(&builder, &tag);
            if (status == UMI_STATUS_OK && Block(tag.name)) status = Break(&builder);
        } else {
            if (Block(tag.name) || strcmp(tag.name, "br") == 0) {
                status = Break(&builder);
                if (strcmp(tag.name, "tr") == 0) builder.row_has_cell = 0;
            }
            if (status == UMI_STATUS_OK) status = Space(&builder);
            if (status == UMI_STATUS_OK) status = Anchor(&builder, &tag);
            if (status == UMI_STATUS_OK && strcmp(tag.name, "li") == 0) status = Append(&builder, "- ", 2U);
            if (status == UMI_STATUS_OK && (strcmp(tag.name, "td") == 0 || strcmp(tag.name, "th") == 0)) {
                if (builder.row_has_cell) status = Append(&builder, " | ", 3U);
                builder.row_has_cell = 1;
            }
            if (status == UMI_STATUS_OK) status = Style(&builder, &tag);
            if (status == UMI_STATUS_OK && strcmp(tag.name, "img") == 0) status = Image(&builder, &tag);
        }
        if (status != UMI_STATUS_OK) return status;
    }
    return builder.depth == 0U ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
