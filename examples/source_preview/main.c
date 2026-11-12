/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/source_preview/main.c
 *
 * PURPOSE:
 *   Build a read-only source preview through Framework and check Unicode limits and refusal recovery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/browser/source_preview.h"

/* The structured model owns exactly the accepted text; clients do not parse
 * it again or retain an address into the original source span. */
static int show_markdown(void)
{
    UmiBrowserSourcePreviewService *service = NULL;
    UmiBrowserSourcePreviewResult *result = malloc(sizeof(*result));
    UmiBrowserReadingDocument *reading = NULL;
    UmiBrowserSourcePreviewRequest request;
    UmiBrowserReadingSummary summary;
    if (result == NULL) return 1;
    UmiStatus status = umi_browser_source_preview_service_create(&service);
    if (status == UMI_STATUS_OK) status = umi_browser_source_preview_service_install_builtins(service);
    umi_browser_source_preview_request_init(&request);
    request.source = "# Umicom practice\n\nRead **owned** text.";
    request.source_length = strlen(request.source);
    request.source_revision = 1U; request.path = "practice.md";
    if (status == UMI_STATUS_OK)
        status = umi_browser_source_preview_service_render_reading(service, &request, result, &reading);
    int good = status == UMI_STATUS_OK && reading != NULL &&
        UmiBrowserReadingDocumentSummary(reading, &summary) == UMI_STATUS_OK &&
        summary.spans == 2U &&
        strcmp(result->content, UmiBrowserReadingDocumentText(reading)) == 0;
    if (good) printf("Provider: %s; complete reading spans: %zu; no source executed.\n",
        result->provider_id, summary.spans);
    else fprintf(stderr, "Structured preview unavailable (status %d).\n", (int)status);
    UmiBrowserReadingDocumentDestroy(reading);
    umi_browser_source_preview_service_destroy(service);
    free(result);
    return good ? 0 : 1;
}

int main(int argc, char **argv)
{
    const char *mode = argc == 2 ? argv[1] : "";
    if (strcmp(mode, "markdown") == 0) return show_markdown();
    UmiBrowserSourcePreviewService *service = NULL;
    UmiBrowserSourcePreviewResult *result = malloc(sizeof(*result));
    UmiBrowserSourcePreviewResult *before = malloc(sizeof(*before));
    UmiBrowserSourcePreviewRequest request;
    UmiStatus status;
    int good = 0;
    if (argc != 2 || (strcmp(mode, "unicode") != 0 &&
        strcmp(mode, "htm") != 0 && strcmp(mode, "refusal") != 0)) {
        /* Keep the original usage for review; the structured mode is additive. */
#if 0
        fprintf(stderr, "usage: source-preview unicode|htm|refusal\n");
#else
        fprintf(stderr, "usage: source-preview unicode|htm|refusal|markdown\n");
#endif
        free(result); free(before); return 2;
    }
    if (result == NULL || before == NULL) {
        free(result); free(before); return 1;
    }
    status = umi_browser_source_preview_service_create(&service);
    if (status == UMI_STATUS_OK)
        status = umi_browser_source_preview_service_install_builtins(service);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Could not prepare preview service: %d\n", (int)status);
        free(result); free(before);
        umi_browser_source_preview_service_destroy(service); return 1;
    }
    umi_browser_source_preview_request_init(&request);
    request.path = "practice.txt";
    request.source = "A\xE2\x82\xAC" "B";
    request.source_length = 5U;
    request.source_revision = 1U;
    request.maximum_output_bytes = 3U;
    umi_browser_source_preview_result_init(result);
    strcpy(result->content, "Keep this preview if the next request is invalid.");
    result->content_bytes = strlen(result->content);
    memcpy(before, result, sizeof(*before));
    if (strcmp(mode, "htm") == 0) {
        request.path = "practice.htm";
        request.source = "<p>Umicom practice</p>";
        request.source_length = strlen(request.source);
        request.maximum_output_bytes = 100U;
    } else if (strcmp(mode, "refusal") == 0) {
        /* The middle byte is part of the declared span, not a terminator. */
        request.source = "a\0b"; request.source_length = 3U;
    }
    status = umi_browser_source_preview_service_render(service, &request, result);
    if (strcmp(mode, "refusal") == 0) {
        good = status == UMI_STATUS_INVALID_ARGUMENT &&
            memcmp(before, result, sizeof(*result)) == 0;
        if (good) puts("Invalid source refused; previous preview unchanged.");
    } else if (status == UMI_STATUS_OK) {
        if (strcmp(mode, "unicode") == 0)
            good = strcmp(result->content, "A") == 0 && result->truncated == 1;
        else good = strcmp(result->provider_id, "umicom.preview.readable-htm") == 0 &&
            strstr(result->content, "Umicom practice") != NULL;
        if (good) printf("Provider: %s; output bytes: %zu; no source executed.\n",
                         result->provider_id, result->content_bytes);
    }
    if (!good) fprintf(stderr, "Preview expectation failed (status %d).\n", (int)status);
    free(result); free(before);
    umi_browser_source_preview_service_destroy(service);
    return good ? 0 : 1;
}
