/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/creation_draft/main.c
 *
 * PURPOSE:
 *   Demonstrate recovery of incomplete source and independently edited creation text through an installed Framework SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/creative_workspace/media_brief_document.h"
#include <stdio.h>
#include <string.h>

static UmiMediaBriefDocument Sample(void)
{
    UmiMediaBriefDocument document = {0};
    document.draft.kind = UMI_MEDIA_BRIEF_VIDEO;
    strcpy(document.draft.title, "Harbour at dawn");
    strcpy(document.draft.source, "A fishing boat returns as the market opens.");
    strcpy(document.draft.direction, "Use quiet camera movement and natural sound.");
    strcpy(document.prompt, "User edit: begin with the harbour bell, then follow the boat.");
    return document;
}
static int Describe(const UmiMediaBriefDocument *document)
{
    printf("Umicom creation draft: %s\nSource: %s\nDirection: %s\nEdited prompt: %s\n",
        document->draft.title, document->draft.source, document->draft.direction, document->prompt);
    puts("Recovered text is a draft. Review it separately before any model handoff.");
    return 0;
}
int main(int argc, char **argv)
{
    UmiMediaBriefDocument document = Sample();
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    if (argc == 2 && strcmp(argv[1], "inspect") == 0) return Describe(&document);
    if (argc == 2 && strcmp(argv[1], "roundtrip") == 0) {
        UmiCreativeAsset *encoded = NULL;
        const void *bytes = NULL; size_t size = 0U;
        UmiMediaBriefDocument restored = {0};
        status = UmiMediaBriefDocumentExport(&document, NULL, &encoded);
        if (status == UMI_STATUS_OK) status = UmiCreativeAssetBytes(encoded, &bytes, &size);
        if (status == UMI_STATUS_OK)
            status = UmiMediaBriefDocumentImport(bytes, size, NULL, &restored);
        if (status == UMI_STATUS_OK && (restored.draft.kind != document.draft.kind ||
            strcmp(restored.draft.source, document.draft.source) != 0 ||
            strcmp(restored.prompt, document.prompt) != 0))
            status = UMI_STATUS_INVALID_STATE;
        UmiCreativeAssetDestroy(encoded);
        if (status == UMI_STATUS_OK) return Describe(&restored);
    } else if (argc == 2 && strcmp(argv[1], "incomplete") == 0) {
        memset(&document.draft, 0, sizeof(document.draft));
        status = UmiMediaBriefDocumentValidate(&document);
        UmiMediaBrief *prepared = NULL;
        UmiStatus preparation = UmiMediaBriefCreate(&document.draft, &prepared);
        UmiMediaBriefDestroy(prepared);
        if (status == UMI_STATUS_OK && preparation == UMI_STATUS_INVALID_ARGUMENT) {
            puts("Unfinished input can be recovered; preparation still requires a title and source.");
            return 0;
        }
        status = UMI_STATUS_INVALID_STATE;
    } else if (argc == 2 && strcmp(argv[1], "refusal") == 0) {
        UmiMediaBriefDocument before;
        memcpy(&before, &document, sizeof(before));
        const char unsupported[] =
            "{\"format\":\"umicom-media-creation-draft\",\"version\":99,"
            "\"draft\":{\"kind\":\"video\",\"title\":\"\",\"source\":\"\",\"direction\":\"\"},\"prompt\":\"\"}";
        status = UmiMediaBriefDocumentImport(unsupported, sizeof(unsupported) - 1U, NULL, &document);
        if (status != UMI_STATUS_OK && memcmp(&before, &document, sizeof(before)) == 0) {
            puts("Unsupported document refused; source and edited prompt remain unchanged.");
            return 0;
        }
        status = UMI_STATUS_INVALID_STATE;
    } else if (argc == 4 && strcmp(argv[1], "save") == 0 && strcmp(argv[3], "--replace") == 0) {
        /* File replacement is explicit in this teaching client's command line.
         * A native application obtains the same decision before a worker save. */
        status = UmiMediaBriefDocumentSave(&document, argv[2], NULL);
        if (status == UMI_STATUS_OK) {
            puts("Creation draft saved at the explicitly selected path.");
            return 0;
        }
    } else if (argc == 3 && strcmp(argv[1], "open") == 0) {
        status = UmiMediaBriefDocumentLoad(argv[2], NULL, &document);
        if (status == UMI_STATUS_OK) return Describe(&document);
    } else {
        fputs("usage: creation-draft inspect|roundtrip|incomplete|refusal\n"
              "       creation-draft save ABSOLUTE_PATH --replace\n"
              "       creation-draft open ABSOLUTE_PATH\n", stderr);
        return 2;
    }
    fprintf(stderr, "Creation draft operation failed: status %d\n", (int)status);
    return 1;
}
