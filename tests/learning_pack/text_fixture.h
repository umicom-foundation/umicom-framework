/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/text_fixture.h
 *
 * PURPOSE:
 *   Prepare verified source, HTML and image metadata for learning-body search regressions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_LEARNING_TEXT_FIXTURE_H
#define UMICOM_LEARNING_TEXT_FIXTURE_H
#include "umicom/teacher/learning_text.h"
#include "umicom/platform/rooted_files.h"
#include <stdio.h>
#include <string.h>

static const char TextGuide[] =
    "<html><head><title>secret-title-token</title></head><body>"
    "<h1>Memory guide</h1><p>Find ownership in a shared <strong>Framework</strong> service.</p>"
    "<script>secret-script-token</script><p>Literal [abc].</p></body></html>";
static const char TextSource[] = "int main(void) {\n    /* ownership belongs to the caller. */\n    return 0;\n}\n";
static const char TextPlain[] = "Ownership can be reviewed.\n";
typedef struct TextFixture {
    const void *bytes[3];
    size_t lengths[3];
    const char *paths[3];
} TextFixture;
static TextFixture TextFixtureDefault(void)
{
    /* Reverse alphabetical source paths expose accidental dependence on index
     * sorting instead of the promised manifest order. */
    return (TextFixture){{TextGuide, TextSource, TextPlain},
        {sizeof(TextGuide)-1U, sizeof(TextSource)-1U, sizeof(TextPlain)-1U},
        {"z-guide.html", "a-example.c", "m-note.txt"}};
}
static UmiStatus TextFixtureAdopt(UmiLearningShelf *shelf, const char *root,
    const char *id, const TextFixture *fixture)
{
    char hashes[3][UMI_SHA256_HEX_CAPACITY], image_hash[UMI_SHA256_HEX_CAPACITY];
    UmiStatus status = UmiSha256Buffer("unused image", 12U, image_hash);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < 3U; ++i) {
        status = UmiSha256Buffer(fixture->bytes[i], fixture->lengths[i], hashes[i]);
        if (status == UMI_STATUS_OK)
            status = UmiRootedFileWrite(root, fixture->paths[i], fixture->bytes[i], fixture->lengths[i]);
    }
    if (status != UMI_STATUS_OK) return status;
    char json[4096];
    /* All substituted metadata are fixed fixture literals. Arbitrary production
     * metadata must go through a JSON writer, not string interpolation. */
    int n = snprintf(json, sizeof(json),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"%s\","
        "\"title\":\"Text search examples\",\"product\":\"studio\",\"locale\":\"en-GB\",\"revision\":7,"
        "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},\"resources\":["
        "{\"id\":\"guide\",\"title\":\"Guide\",\"topic\":\"memory\",\"kind\":\"guide\",\"path\":\"%s\",\"sha256\":\"%s\"},"
        "{\"id\":\"source\",\"title\":\"Source\",\"topic\":\"memory\",\"kind\":\"example\",\"path\":\"%s\",\"sha256\":\"%s\"},"
        "{\"id\":\"note\",\"title\":\"Note\",\"topic\":\"memory\",\"kind\":\"lesson\",\"path\":\"%s\",\"sha256\":\"%s\"},"
        "{\"id\":\"image\",\"title\":\"Image\",\"topic\":\"memory\",\"kind\":\"image\",\"path\":\"not-read.png\",\"sha256\":\"%s\"}]}",
        id, fixture->paths[0], hashes[0], fixture->paths[1], hashes[1],
        fixture->paths[2], hashes[2], image_hash);
    if (n < 0 || (size_t)n >= sizeof(json)) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiLearningPack *pack = NULL;
    status = UmiRootedFileWrite(root, "learning-pack.json", json, (size_t)n);
    if (status == UMI_STATUS_OK)
        status = UmiLearningPackCreate(json, (size_t)n, root, (UmiVersion){0U,9U,0U}, &pack);
    if (status == UMI_STATUS_OK) status = UmiLearningShelfAdopt(shelf, &pack);
    UmiLearningPackDestroy(pack);
    return status;
}
#endif
