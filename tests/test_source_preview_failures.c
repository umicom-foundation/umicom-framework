/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_source_preview_failures.c
 *
 * PURPOSE:
 *   Inject render allocation failure into the real preview implementation and preserve prior output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "support/assertions.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
static int reject_allocation;
static void *preview_test_malloc(size_t size)
{
    return reject_allocation ? NULL : malloc(size);
}
/* Rename only public symbols so this fixture owns the real implementation.
 * The allocation boundary is substituted; Unicode and HTML services stay real. */
#define umi_browser_source_preview_request_init fixture_umi_browser_source_preview_request_init
#define umi_browser_source_preview_result_init fixture_umi_browser_source_preview_result_init
#define umi_browser_source_preview_provider_init fixture_umi_browser_source_preview_provider_init
#define umi_browser_source_preview_service_create fixture_umi_browser_source_preview_service_create
#define umi_browser_source_preview_service_destroy fixture_umi_browser_source_preview_service_destroy
#define umi_browser_source_preview_service_register fixture_umi_browser_source_preview_service_register
#define umi_browser_source_preview_service_unregister fixture_umi_browser_source_preview_service_unregister
#define umi_browser_source_preview_service_install_builtins fixture_umi_browser_source_preview_service_install_builtins
#define umi_browser_source_preview_service_render fixture_umi_browser_source_preview_service_render
#define umi_browser_source_preview_service_count fixture_umi_browser_source_preview_service_count
#define umi_browser_source_preview_service_at fixture_umi_browser_source_preview_service_at
#define umi_browser_source_preview_kind_text fixture_umi_browser_source_preview_kind_text
#define umi_browser_source_preview_service_render_reading fixture_umi_browser_source_preview_service_render_reading
#define malloc preview_test_malloc
#include "../src/browser/source_preview.c"
#undef malloc

int main(void)
{
    UmiBrowserSourcePreviewService *service = NULL;
    UmiBrowserSourcePreviewResult *result = malloc(sizeof(*result));
    UmiBrowserSourcePreviewResult *before = malloc(sizeof(*before));
    UmiBrowserSourcePreviewRequest request;
    assert(result != NULL && before != NULL);
    assert(umi_browser_source_preview_service_create(&service) == UMI_STATUS_OK);
    reject_allocation = 1;
    assert(umi_browser_source_preview_service_install_builtins(service) == UMI_STATUS_OUT_OF_MEMORY);
    assert(umi_browser_source_preview_service_count(service) == 0U);
    reject_allocation = 0;
    assert(umi_browser_source_preview_service_install_builtins(service) == UMI_STATUS_OK);
    umi_browser_source_preview_request_init(&request);
    request.source = "retained"; request.source_length = 8U; request.source_revision = 1U;
    memset(result, 0xA5, sizeof(*result));
    memcpy(before, result, sizeof(*before));
    reject_allocation = 1;
    assert(umi_browser_source_preview_service_render(service, &request, result) == UMI_STATUS_OUT_OF_MEMORY);
    assert(memcmp(result, before, sizeof(*result)) == 0);
    reject_allocation = 0;
    assert(umi_browser_source_preview_service_render(service, &request, result) == UMI_STATUS_OK);
    assert(strcmp(result->content, "retained") == 0);
    free(before); free(result);
    umi_browser_source_preview_service_destroy(service);
    return 0;
}
