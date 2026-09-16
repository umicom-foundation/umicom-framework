/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_foundations_resources.c
 *
 * PURPOSE:
 *   Verify every lesson points at its own complete, readable HTML resource.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/teacher/foundations_curriculum.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* This is a test of Umicom-authored lesson HTML, not a browser or a general
 * HTML5 validator. It deliberately accepts only quoted local href attributes
 * and plain title/h1 text (with the five named XML/HTML entities). It skips
 * comments and raw script/style bodies instead of mistaking example markup
 * for a real lesson identity. Nothing in a document is executed or rewritten. */
#define MAX_DOCUMENT_BYTES (2U * 1024U * 1024U)
#define MAX_REFERENCES 512U
#define MAX_ATTRIBUTES 64U

typedef struct TextSpan { const char *text; size_t length; } TextSpan;
typedef struct Attribute { TextSpan name, value; } Attribute;
typedef struct HtmlTag {
    TextSpan name;
    Attribute attributes[MAX_ATTRIBUTES];
    size_t attributeCount;
    const char *after;
    int closing;
} HtmlTag;
typedef struct LessonDocument {
    TextSpan identity, title, heading;
    TextSpan links[MAX_REFERENCES], anchors[MAX_REFERENCES];
    size_t identityCount, titleCount, headingCount, linkCount, anchorCount;
    size_t headCount, htmlCount, htmlCloseCount;
} LessonDocument;

static int IsSpace(char value)
{ return value == ' ' || value == '\t' || value == '\r' || value == '\n' || value == '\f'; }
static unsigned char LowerAscii(unsigned char value)
{ return value >= 'A' && value <= 'Z' ? (unsigned char)(value + ('a' - 'A')) : value; }
static int SpanIs(TextSpan span, const char *value)
{
    size_t length = strlen(value);
    if (span.length != length) return 0;
    for (size_t index = 0U; index < length; ++index)
        if (LowerAscii((unsigned char)span.text[index]) != LowerAscii((unsigned char)value[index])) return 0;
    return 1;
}
static int SpanSame(TextSpan a, TextSpan b)
{ return a.length == b.length && memcmp(a.text, b.text, a.length) == 0; }
static int Starts(const char *cursor, const char *end, const char *text)
{
    size_t length = strlen(text);
    return (size_t)(end - cursor) >= length && memcmp(cursor, text, length) == 0;
}
static int NameChar(char value)
{
    unsigned char c = (unsigned char)value;
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') || value == '-' || value == '_' || value == ':';
}

/* Read one tag with bounded slices. Large embedded image values are borrowed,
 * not copied into small buffers. A repeated attribute is ambiguous and fails. */
static int ReadTag(const char **position, const char *end, HtmlTag *tag)
{
    const char *cursor = *position;
    if (cursor == end || *cursor++ != '<') return 0;
    memset(tag, 0, sizeof(*tag));
    if (cursor < end && *cursor == '/') { tag->closing = 1; ++cursor; }
    const char *name = cursor;
    while (cursor < end && NameChar(*cursor)) ++cursor;
    if (cursor == name) return 0;
    tag->name = (TextSpan){name, (size_t)(cursor - name)};
    while (cursor < end) {
        int separated = 0;
        while (cursor < end && IsSpace(*cursor)) { ++cursor; separated = 1; }
        if (cursor == end) return 0;
        if (*cursor == '>' || (*cursor == '/' && cursor + 1 < end && cursor[1] == '>')) {
            cursor += *cursor == '/' ? 2 : 1;
            tag->after = cursor; *position = cursor; return 1;
        }
        if (tag->closing || !separated || tag->attributeCount == MAX_ATTRIBUTES) return 0;
        Attribute attribute = {0};
        name = cursor;
        while (cursor < end && NameChar(*cursor)) ++cursor;
        if (cursor == name) return 0;
        attribute.name = (TextSpan){name, (size_t)(cursor - name)};
        for (size_t index = 0U; index < tag->attributeCount; ++index) {
            TextSpan prior = tag->attributes[index].name;
            if (prior.length != attribute.name.length) continue;
            size_t same = 0U;
            for (; same < prior.length; ++same)
                if (LowerAscii((unsigned char)prior.text[same]) !=
                    LowerAscii((unsigned char)attribute.name.text[same])) break;
            if (same == prior.length) return 0;
        }
        const char *afterName = cursor;
        while (cursor < end && IsSpace(*cursor)) ++cursor;
        if (cursor < end && *cursor == '=') {
            ++cursor;
            while (cursor < end && IsSpace(*cursor)) ++cursor;
            if (cursor == end || (*cursor != '"' && *cursor != '\'')) return 0;
            char quote = *cursor++;
            const char *value = cursor;
            while (cursor < end && *cursor != quote) ++cursor;
            if (cursor == end) return 0;
            attribute.value = (TextSpan){value, (size_t)(cursor - value)}; ++cursor;
        } else {
            /* Boolean attributes, such as open on details, have no value. */
            cursor = afterName;
        }
        tag->attributes[tag->attributeCount++] = attribute;
    }
    return 0;
}
static TextSpan GetAttribute(const HtmlTag *tag, const char *name)
{
    for (size_t index = 0U; index < tag->attributeCount; ++index)
        if (SpanIs(tag->attributes[index].name, name)) return tag->attributes[index].value;
    return (TextSpan){NULL, 0U};
}
static int TextEquals(TextSpan span, const char *expected)
{
    while (span.length != 0U && IsSpace(span.text[0])) { ++span.text; --span.length; }
    while (span.length != 0U && IsSpace(span.text[span.length - 1U])) --span.length;
    size_t offset = 0U;
    for (size_t index = 0U; index < span.length; ++index) {
        char value = span.text[index];
        if (value == '&') {
            const char *const names[] = {"&amp;", "&lt;", "&gt;", "&quot;", "&apos;"};
            const char values[] = {'&', '<', '>', '"', '\''};
            size_t entity = 0U;
            for (; entity < sizeof(values); ++entity) {
                size_t length = strlen(names[entity]);
                if (span.length - index >= length && memcmp(span.text + index, names[entity], length) == 0) {
                    value = values[entity]; index += length - 1U; break;
                }
            }
            if (entity == sizeof(values)) return 0;
        }
        if (expected[offset] == '\0' || value != expected[offset++]) return 0;
    }
    return expected[offset] == '\0';
}

/* Tokenise actual tags. Raw script/style examples cannot satisfy metadata,
 * document-end, local-link or anchor checks. This does not interpret JavaScript. */
static int InspectDocument(const char *text, size_t length, LessonDocument *document)
{
    const char *cursor = text, *end = text + length;
    const char *titleStart = NULL, *headingStart = NULL;
    int inHead = 0, inHtml = 0;
    memset(document, 0, sizeof(*document));
    if (memchr(text, '\0', length) != NULL) return 0;
    while (cursor < end) {
        if (*cursor != '<') { ++cursor; continue; }
        if (Starts(cursor, end, "<!--")) {
            cursor += 4;
            while (cursor < end && !Starts(cursor, end, "-->")) ++cursor;
            if (cursor == end) return 0;
            cursor += 3; continue;
        }
        if (Starts(cursor, end, "<!DOCTYPE") || Starts(cursor, end, "<!doctype")) {
            while (cursor < end && *cursor != '>') ++cursor;
            if (cursor == end) return 0;
            ++cursor; continue;
        }
        const char *tagStart = cursor;
        HtmlTag tag;
        if (!ReadTag(&cursor, end, &tag)) return 0;
        if (SpanIs(tag.name, "html")) {
            if (tag.closing) { if (!inHtml || inHead) return 0; inHtml = 0; ++document->htmlCloseCount; }
            else { if (inHtml || document->htmlCount != 0U) return 0; inHtml = 1; ++document->htmlCount; }
        } else if (!inHtml) return 0;
        if (SpanIs(tag.name, "head")) {
            if (tag.closing) { if (!inHead) return 0; inHead = 0; }
            else { if (inHead || document->headCount != 0U) return 0; inHead = 1; ++document->headCount; }
        }
        if (SpanIs(tag.name, "title")) {
            if (!inHead) return 0;
            if (!tag.closing) { if (titleStart != NULL) return 0; titleStart = cursor; ++document->titleCount; }
            else { if (titleStart == NULL) return 0; document->title = (TextSpan){titleStart, (size_t)(tagStart-titleStart)}; titleStart = NULL; }
        }
        if (SpanIs(tag.name, "h1")) {
            if (inHead) return 0;
            if (!tag.closing) { if (headingStart != NULL) return 0; headingStart = cursor; ++document->headingCount; }
            else { if (headingStart == NULL) return 0; document->heading = (TextSpan){headingStart, (size_t)(tagStart-headingStart)}; headingStart = NULL; }
        }
        if (!tag.closing && SpanIs(tag.name, "meta") && SpanIs(GetAttribute(&tag, "name"), "umicom-lesson-id")) {
            if (!inHead) return 0;
            document->identity = GetAttribute(&tag, "content"); ++document->identityCount;
        }
        if (!tag.closing) {
            TextSpan id = GetAttribute(&tag, "id");
            if (id.text != NULL) {
                if (id.length == 0U || document->anchorCount == MAX_REFERENCES) return 0;
                for (size_t index = 0U; index < document->anchorCount; ++index)
                    if (SpanSame(id, document->anchors[index])) return 0;
                document->anchors[document->anchorCount++] = id;
            }
            TextSpan href = GetAttribute(&tag, "href");
            if ((SpanIs(tag.name, "a") || (SpanIs(tag.name, "link") && SpanIs(GetAttribute(&tag, "rel"), "canonical"))) && href.text != NULL) {
                if (document->linkCount == MAX_REFERENCES) return 0;
                document->links[document->linkCount++] = href;
            }
        }
        if (!tag.closing && (SpanIs(tag.name, "script") || SpanIs(tag.name, "style"))) {
            const char *rawName = SpanIs(tag.name, "script") ? "script" : "style";
            int found = 0;
            while (cursor < end) {
                if (Starts(cursor, end, "</")) {
                    const char *probe = cursor;
                    HtmlTag close;
                    if (ReadTag(&probe, end, &close) && close.closing && SpanIs(close.name, rawName)) {
                        cursor = probe; found = 1; break;
                    }
                }
                ++cursor;
            }
            if (!found) return 0;
        }
    }
    return !inHtml && !inHead && titleStart == NULL && headingStart == NULL &&
        document->htmlCount == 1U && document->htmlCloseCount == 1U && document->headCount == 1U;
}
static int MatchesLesson(const LessonDocument *document, const char *id, const char *title)
{
    char pageTitle[UMI_TEACHER_TEXT_CAPACITY + 16U];
    int written = snprintf(pageTitle, sizeof(pageTitle), "%s | Umicom", title);
    return written >= 0 && (size_t)written < sizeof(pageTitle) &&
        document->identityCount == 1U && TextEquals(document->identity, id) &&
        document->titleCount == 1U && TextEquals(document->title, pageTitle) &&
        document->headingCount == 1U && TextEquals(document->heading, title);
}

static char *ReadDocument(const char *path, size_t *length)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) return NULL;
    if (fseek(file, 0L, SEEK_END) != 0) { (void)fclose(file); return NULL; }
    long size = ftell(file);
    if (size <= 0L || (unsigned long)size > MAX_DOCUMENT_BYTES || fseek(file, 0L, SEEK_SET) != 0) {
        (void)fclose(file); return NULL;
    }
    char *data = malloc((size_t)size + 1U);
    if (data == NULL) { (void)fclose(file); return NULL; }
    size_t got = fread(data, 1U, (size_t)size, file);
    int closed = fclose(file);
    if (got != (size_t)size || closed != 0 || memchr(data, '\0', got) != NULL) { free(data); return NULL; }
    data[got] = '\0'; *length = got; return data;
}

/* Lesson navigation uses filenames in one installed directory. Reject remote
 * executable schemes and path traversal rather than silently following them.
 * Ordinary HTTPS/HTTP/mailto links are identified but never fetched. */
static int ClassifyLink(TextSpan span, char *file, size_t capacity, TextSpan *fragment)
{
    if (span.length >= 8U && memcmp(span.text,"https://",8U)==0) return 0;
    if (span.length >= 7U && memcmp(span.text,"http://",7U)==0) return 0;
    if (span.length >= 7U && memcmp(span.text,"mailto:",7U)==0) return 0;
    size_t pathLength = 0U;
    while (pathLength < span.length && span.text[pathLength] != '#') ++pathLength;
    if (pathLength >= capacity) return -1;
    for (size_t index = 0U; index < pathLength; ++index) {
        char c = span.text[index];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.')) return -1;
    }
    if ((pathLength == 1U && span.text[0] == '.') ||
        (pathLength == 2U && span.text[0] == '.' && span.text[1] == '.')) return -1;
    if (pathLength != 0U) memcpy(file, span.text, pathLength);
    file[pathLength] = '\0';
    *fragment = pathLength < span.length ?
        (TextSpan){span.text + pathLength + 1U, span.length - pathLength - 1U} : (TextSpan){NULL,0U};
    return 1;
}
static int HasAnchor(const LessonDocument *document, TextSpan anchor)
{
    if (anchor.length == 0U) return 1;
    for (size_t index = 0U; index < document->anchorCount; ++index)
        if (SpanSame(document->anchors[index], anchor)) return 1;
    return 0;
}
static int CheckLinks(const char *directory, const char *ownPath, const LessonDocument *document, size_t *checked)
{
    for (size_t index = 0U; index < document->linkCount; ++index) {
        char name[256], path[4096]; TextSpan fragment;
        int kind = ClassifyLink(document->links[index], name, sizeof(name), &fragment);
        if (kind < 0) { (void)fprintf(stderr,"Unsupported or unsafe lesson link in %s\n",ownPath); return 0; }
        if (kind == 0) continue;
        if (name[0] == '\0') {
            if (!HasAnchor(document,fragment)) return 0;
        } else {
            int written = snprintf(path,sizeof(path),"%s/%s",directory,name);
            if (written < 0 || (size_t)written >= sizeof(path)) return 0;
            size_t length = 0U; char *data = ReadDocument(path,&length);
            if (data == NULL) { (void)fprintf(stderr,"Missing/unreadable link from %s to %s\n",ownPath,path); return 0; }
            if (fragment.length != 0U) {
                LessonDocument *target = calloc(1U,sizeof(*target));
                int valid = target != NULL && InspectDocument(data,length,target) && HasAnchor(target,fragment);
                free(target); free(data);
                if (!valid) { (void)fprintf(stderr,"Missing/invalid anchor in %s\n",path); return 0; }
            } else free(data);
        }
        ++*checked;
    }
    return 1;
}

/* Negative cases execute without assert(), including in Release. These cases
 * prove that comments, attributes, escaped examples and script text cannot
 * replace real page metadata or hide broken navigation. */
static int SelfTest(const char *group)
{
    const char *const documents[] = {
        "<!doctype html><html><head><title>Example | Umicom</title><meta name='umicom-lesson-id' content='lesson.test'></head><body><h1>Example</h1></body></html>",
        "<html><head><title>Example | Umicom</title><!-- <meta name=\"umicom-lesson-id\" content=\"lesson.test\"> --></head><h1>Example</h1></html>",
        "<html><head><title>Example | Umicom</title><script>const x='<meta name=\"umicom-lesson-id\" content=\"lesson.test\">';</script></head><h1>Example</h1></html>",
        "<html><head><title>Wrong | Umicom</title><meta name='umicom-lesson-id' content='lesson.test'></head><h1>Example</h1></html>",
        "<html><head><title>Example | Umicom</title><meta name='umicom-lesson-id' content='other'></head><h1>Example</h1></html>",
        "<html><head><title>Example | Umicom</title><meta name='umicom-lesson-id' content='lesson.test'><meta name='umicom-lesson-id' content='lesson.test'></head><h1>Example</h1></html>",
        "<html><head><title>Example | Umicom</title></head><meta name='umicom-lesson-id' content='lesson.test'><h1>Example</h1></html>",
        "<html><head><title>Example | Umicom</title><meta name='umicom-lesson-id' content='lesson.test'></head><h1>Example</h1><!-- </html> -->",
        "<html><head><title>Example | Umicom</title><meta name='umicom-lesson-id' content='lesson.test'></head><h1>Wrong</h1></html>",
        "<html><head><title>Example | Umicom</title><meta name='umicom-lesson-id' content='lesson.test' content='other'></head><h1>Example</h1></html>",
        "<html><head><title>Example | Umicom</title><meta name='umicom-lesson-id' content='lesson.test'></head><h1 id='x'>Example</h1><p id='x'>duplicate</p></html>",
        "<HTML><HEAD><TITLE>Example | Umicom</TITLE><META content='lesson.test' name='umicom-lesson-id'></HEAD><H1>Example</H1></HTML>"
    };
    LessonDocument *document = calloc(1U,sizeof(*document));
    if (document == NULL) return EXIT_FAILURE;
    int result = EXIT_SUCCESS;
    if (strcmp(group,"markup") == 0) {
        for (size_t index = 0U; index < sizeof(documents)/sizeof(documents[0]); ++index) {
            int valid = InspectDocument(documents[index],strlen(documents[index]),document) && MatchesLesson(document,"lesson.test","Example");
            if (valid != (index == 0U || index == 11U)) { (void)fprintf(stderr,"Markup case %zu failed\n",index); result=EXIT_FAILURE; break; }
        }
        char embedded[] = "<html>\0</html>";
        if (InspectDocument(embedded,sizeof(embedded)-1U,document)) result=EXIT_FAILURE;
        if (InspectDocument("<html><!-- unfinished",20U,document)) result=EXIT_FAILURE;
    } else if (strcmp(group,"bounds") == 0) {
        /* Every strict prefix is incomplete. Parsing uses the supplied length,
         * not a strlen() over a potentially unterminated caller-owned slice. */
        for (size_t index = 0U; index < sizeof(documents)/sizeof(documents[0]); ++index) {
            size_t length = strlen(documents[index]);
            char *copy = malloc(length);
            if (copy == NULL) { result = EXIT_FAILURE; break; }
            memcpy(copy, documents[index], length);
            for (size_t prefix = 0U; prefix < length; ++prefix)
                if (InspectDocument(copy, prefix, document)) result = EXIT_FAILURE;
            /* Malformed byte mutations need not all be rejected as HTML, but
             * every one must remain a bounded read under sanitizers. */
            for (size_t byte = 0U; byte < length; ++byte) {
                char saved = copy[byte];
                copy[byte] = (byte % 2U == 0U) ? '<' : '\'';
                (void)InspectDocument(copy, length, document);
                copy[byte] = saved;
            }
            free(copy);
        }
    } else if (strcmp(group,"links") == 0) {
        const char *const bad[] = {"../secret.txt","/absolute.html","C:\\file.html","javascript:alert(1)","%2e%2e/file.html","x.html?query=1","//remote/path"};
        char file[256];TextSpan fragment;
        for(size_t index=0U;index<sizeof(bad)/sizeof(bad[0]);++index)
            if(ClassifyLink((TextSpan){bad[index],strlen(bad[index])},file,sizeof(file),&fragment)!=-1) result=EXIT_FAILURE;
        const char link[]="git-first-commit.html#top";
        if(ClassifyLink((TextSpan){link,sizeof(link)-1U},file,sizeof(file),&fragment)!=1 || strcmp(file,"git-first-commit.html")!=0 || !SpanIs(fragment,"top")) result=EXIT_FAILURE;
        if(ClassifyLink((TextSpan){link,sizeof(link)-1U},file,3U,&fragment)!=-1) result=EXIT_FAILURE;
        const char external[]="https://example.invalid/never-requested";
        if(ClassifyLink((TextSpan){external,sizeof(external)-1U},file,sizeof(file),&fragment)!=0) result=EXIT_FAILURE;
        const char anchors[]="<html><head><title>Anchors</title></head><body id='top'><a href='#missing'>test</a></body></html>";
        if(!InspectDocument(anchors,sizeof(anchors)-1U,document) || !HasAnchor(document,(TextSpan){"top",3U}) || HasAnchor(document,(TextSpan){"missing",7U})) result=EXIT_FAILURE;
    } else result=EXIT_FAILURE;
    free(document);
    if(result==EXIT_SUCCESS) (void)printf("Lesson resource %s negative/positive checks passed.\n",group);
    return result;
}

/* Paths in the legacy public catalogue are Applications-root-relative.
 * This test receives the canonical Framework checkout, so only the exact
 * fixed prefix is removed; no application-owned content is copied. */
int main(int argc, char **argv)
{
    const char prefix[] = "framework/docs/learning/";
    size_t checked = 0U, links = 0U;
    if(argc==3 && strcmp(argv[1],"--self-test")==0) return SelfTest(argv[2]);
    if(argc!=2 || argv[1][0]=='\0') { (void)fprintf(stderr,"Expected one Framework source directory.\n"); return EXIT_FAILURE; }
    char directory[4096];
    int written=snprintf(directory,sizeof(directory),"%s/docs/learning",argv[1]);
    if(written<0 || (size_t)written>=sizeof(directory)) return EXIT_FAILURE;
    LessonDocument *document=calloc(1U,sizeof(*document));
    if(document==NULL) return EXIT_FAILURE;
    int result=EXIT_SUCCESS;
    size_t count=umi_teacher_foundations_curriculum_count();
    for(size_t index=0U; index<count; ++index) {
        const UmiTeacherFoundationsLesson *lesson=umi_teacher_foundations_curriculum_at(index);
        if(lesson==NULL || strncmp(lesson->resource_path,prefix,sizeof(prefix)-1U)!=0) { result=EXIT_FAILURE; break; }
        /* A second catalogue row cannot claim the same identity or resource. */
        for(size_t prior=0U;prior<index;++prior) {
            const UmiTeacherFoundationsLesson *other=umi_teacher_foundations_curriculum_at(prior);
            if(other==NULL || strcmp(lesson->id,other->id)==0 || strcmp(lesson->resource_path,other->resource_path)==0) result=EXIT_FAILURE;
        }
        const char *name=lesson->resource_path+sizeof(prefix)-1U;
        char safeName[256];TextSpan fragment;
        if(ClassifyLink((TextSpan){name,strlen(name)},safeName,sizeof(safeName),&fragment)!=1 || safeName[0]=='\0' || fragment.text!=NULL || result!=EXIT_SUCCESS) { result=EXIT_FAILURE; break; }
        char path[4096];
        written=snprintf(path,sizeof(path),"%s/%s",directory,safeName);
        if(written<0 || (size_t)written>=sizeof(path)) { result=EXIT_FAILURE; break; }
        size_t length=0U;char *text=ReadDocument(path,&length);
        int valid=text!=NULL && InspectDocument(text,length,document) && MatchesLesson(document,lesson->id,lesson->title) && CheckLinks(directory,path,document,&links);
        free(text);
        if(!valid) { (void)fprintf(stderr,"%s: lesson identity/title/markup/link check failed in %s\n",lesson->id,path); result=EXIT_FAILURE; break; }
        ++checked;
    }
    /* Reference and compatibility pages are not assessed lessons. Check their
     * envelope and links too, without assigning new progress identities. */
    const char *const references[] = {
        "index.html", "workshop-build.html", "naming-and-ownership.html",
        "git-first-comment.html", "programming-workshop.html", "application-development.html"
    };
    size_t referencesChecked = 0U;
    for (size_t index = 0U; result == EXIT_SUCCESS && index < sizeof(references)/sizeof(references[0]); ++index) {
        char path[4096];
        written = snprintf(path, sizeof(path), "%s/%s", directory, references[index]);
        if (written < 0 || (size_t)written >= sizeof(path)) { result = EXIT_FAILURE; break; }
        size_t length = 0U;
        char *text = ReadDocument(path, &length);
        int valid = text != NULL && InspectDocument(text, length, document) &&
            document->identityCount == 0U && document->titleCount == 1U &&
            document->headingCount == 1U && CheckLinks(directory, path, document, &links);
        free(text);
        if (!valid) { (void)fprintf(stderr, "Reference page check failed: %s\n", path); result = EXIT_FAILURE; break; }
        ++referencesChecked;
    }
    free(document);
    if(checked==0U) result=EXIT_FAILURE;
    if(result==EXIT_SUCCESS) (void)printf("Verified %zu catalogue identities and titles/headings, %zu reference pages and %zu local links.\n",checked,referencesChecked,links);
    return result;
}
