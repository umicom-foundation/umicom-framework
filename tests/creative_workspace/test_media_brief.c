/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_workspace/test_media_brief.c
 *
 * PURPOSE:
 *   Verify source ownership, Unicode boundaries and creation-brief intent.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/creative_workspace/media_brief.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); exit(1); } } while (0)
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1];
    UmiMediaBriefDraft draft = {UMI_MEDIA_BRIEF_STORYBOARD, "Coastal journey", "A train follows the coast.\r\nA station overlooks the sea.", "Quiet documentary style."};
    UmiMediaBrief *brief = NULL;
    if (!strcmp(name, "kinds")) {
        const char *expected[] = {"numbered shots", "Create an image", "Create a video", "not an audio recording"};
        for (unsigned i = 0; i < 4U; ++i) {
            draft.kind = (UmiMediaBriefKind)i;
            CHECK(UmiMediaBriefCreate(&draft, &brief) == UMI_STATUS_OK);
            CHECK(strstr(UmiMediaBriefPrompt(brief), expected[i]) != NULL);
            CHECK(strstr(UmiMediaBriefPrompt(brief), draft.source) != NULL);
            UmiMediaBriefDestroy(brief);
        }
        draft.kind = (UmiMediaBriefKind)4;
        CHECK(UmiMediaBriefCreate(&draft, &brief) == UMI_STATUS_INVALID_ARGUMENT && brief == NULL);
    } else if (!strcmp(name, "immutable")) {
        CHECK(UmiMediaBriefCreate(&draft, &brief) == UMI_STATUS_OK);
        strcpy(draft.source, "Replacement");
        CHECK(strstr(UmiMediaBriefPrompt(brief), "A train follows the coast.") != NULL);
        CHECK(strstr(UmiMediaBriefPrompt(brief), "Replacement") == NULL);
        UmiMediaBriefDestroy(brief);
    } else if (!strcmp(name, "blank")) {
        strcpy(draft.source, " \r\n\t");
        CHECK(UmiMediaBriefCreate(&draft, &brief) == UMI_STATUS_INVALID_ARGUMENT && brief == NULL);
    } else if (!strcmp(name, "control")) {
        draft.direction[2] = '\x1b';
        CHECK(UmiMediaBriefCreate(&draft, &brief) == UMI_STATUS_INVALID_ARGUMENT && brief == NULL);
    } else if (!strcmp(name, "malformed")) {
        draft.source[0] = (char)0xc0; draft.source[1] = (char)0xaf; draft.source[2] = '\0';
        CHECK(UmiMediaBriefCreate(&draft, &brief) != UMI_STATUS_OK && brief == NULL);
    } else if (!strcmp(name, "unterminated")) {
        memset(draft.title, 'x', sizeof(draft.title));
        CHECK(UmiMediaBriefCreate(&draft, &brief) == UMI_STATUS_CAPACITY_EXCEEDED && brief == NULL);
    } else if (!strcmp(name, "limits")) {
        memset(draft.title, 't', sizeof(draft.title) - 1U); draft.title[sizeof(draft.title) - 1U] = '\0';
        memset(draft.source, 's', sizeof(draft.source) - 1U); draft.source[sizeof(draft.source) - 1U] = '\0';
        memset(draft.direction, 'd', sizeof(draft.direction) - 1U); draft.direction[sizeof(draft.direction) - 1U] = '\0';
        CHECK(UmiMediaBriefCreate(&draft, &brief) == UMI_STATUS_OK);
        CHECK(strstr(UmiMediaBriefPrompt(brief), draft.source) != NULL);
        CHECK(strlen(UmiMediaBriefPrompt(brief)) < UMI_MEDIA_BRIEF_PROMPT_CAPACITY);
        UmiMediaBriefDestroy(brief);
        strcpy(draft.source, "Sea \xe6\xb5\xb7 and music \xf0\x9f\x8e\xb5");
        CHECK(UmiMediaBriefCreate(&draft, &brief) == UMI_STATUS_OK);
        UmiMediaBriefDestroy(brief);
    } else CHECK(0);
    return 0;
}
