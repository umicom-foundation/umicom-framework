/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/storyboard_document/main.c
 *
 * PURPOSE:
 *   Use the installed Framework API to round-trip, inspect and explicitly save editable storyboards.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/creative_workspace/media_storyboard.h"
#include <stdio.h>
#include <string.h>

static UmiMediaStoryboardDraft Sample(void)
{
    UmiMediaStoryboardDraft draft = {0};
    draft.count = 2U;
    strcpy(draft.scenes[0].title, "Arrival");
    strcpy(draft.scenes[0].story, "Show @{harbour} in the morning.");
    draft.scenes[0].duration_seconds = 12U;
    strcpy(draft.scenes[1].title, "Departure");
    strcpy(draft.scenes[1].story, "Follow the boat into open water.");
    draft.scenes[1].duration_seconds = 18U;
    return draft;
}
static int Describe(const UmiMediaStoryboardDraft *draft)
{
    printf("Umicom Media storyboard: %zu editable scenes\n", draft->count);
    for (size_t i = 0U; i < draft->count; ++i)
        printf("%zu. %s (%u requested seconds)\n%s\n", i + 1U,
            draft->scenes[i].title, (unsigned)draft->scenes[i].duration_seconds,
            draft->scenes[i].story);
    puts("References remain text until a separate library-backed preparation.");
    return 0;
}
int main(int argc, char **argv)
{
    UmiMediaStoryboardDraft draft = Sample();
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    if (argc == 2 && strcmp(argv[1], "inspect") == 0) return Describe(&draft);
    if (argc == 2 && strcmp(argv[1], "roundtrip") == 0) {
        UmiCreativeAsset *document = NULL;
        const void *bytes = NULL;
        size_t size = 0U;
        UmiMediaStoryboardDraft restored = {0};
        status = UmiMediaStoryboardDraftExport(&draft, NULL, &document);
        if (status == UMI_STATUS_OK)
            status = UmiCreativeAssetBytes(document, &bytes, &size);
        if (status == UMI_STATUS_OK)
            status = UmiMediaStoryboardDraftImport(bytes, size, NULL, &restored);
        if (status == UMI_STATUS_OK &&
            (restored.count != draft.count ||
             strcmp(restored.scenes[0].story, draft.scenes[0].story) != 0))
            status = UMI_STATUS_INVALID_STATE;
        UmiCreativeAssetDestroy(document);
        if (status == UMI_STATUS_OK) return Describe(&restored);
    } else if (argc == 2 && strcmp(argv[1], "refusal") == 0) {
        UmiMediaStoryboardDraft before;
        memcpy(&before, &draft, sizeof(before));
        const char unsupported[] =
            "{\"format\":\"umicom-media-storyboard\",\"version\":99,\"scenes\":[]}";
        status = UmiMediaStoryboardDraftImport(unsupported, sizeof(unsupported) - 1U, NULL, &draft);
        if (status != UMI_STATUS_OK && memcmp(&draft, &before, sizeof(draft)) == 0) {
            puts("Unsupported version refused; the existing draft is unchanged.");
            return 0;
        }
        status = UMI_STATUS_INVALID_STATE;
    } else if (argc == 4 && strcmp(argv[1], "save") == 0 && strcmp(argv[3], "--replace") == 0) {
        /* The command line explicitly selects replacement. UI callers should
         * obtain that decision before running this same service on a worker. */
        status = UmiMediaStoryboardDraftSave(&draft, argv[2], NULL);
        if (status == UMI_STATUS_OK) {
            puts("Storyboard saved at the explicitly selected path.");
            return 0;
        }
    } else if (argc == 3 && strcmp(argv[1], "open") == 0) {
        status = UmiMediaStoryboardDraftLoad(argv[2], NULL, &draft);
        if (status == UMI_STATUS_OK) return Describe(&draft);
    } else {
        fputs("usage: storyboard-document inspect|roundtrip|refusal\n"
              "       storyboard-document save ABSOLUTE_PATH --replace\n"
              "       storyboard-document open ABSOLUTE_PATH\n", stderr);
        return 2;
    }
    fprintf(stderr, "Storyboard operation failed: status %d\n", (int)status);
    return 1;
}
