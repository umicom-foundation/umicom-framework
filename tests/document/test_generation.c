/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_generation.c
 *
 * PURPOSE:
 *   Verify text generation and replaceable binary-format adapter dispatch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/document/generation.h"

/* Count bytes received from an adapter without writing a test file. */
static UmiStatus count_sink(
    const void *data,
    size_t data_size,
    void *sink_context)
{
    size_t *byte_count = (size_t *)sink_context;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (data == NULL || byte_count == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *byte_count += data_size;
    return UMI_STATUS_OK;
}

/* Simulate a binary adapter to verify registry dispatch and sink ownership. */
static UmiStatus fake_pdf_adapter(
    const UmiDocumentGenerationRequest *request,
    UmiDocumentGenerationSink sink,
    void *sink_context,
    void *adapter_context)
{
    const char bytes[] = "PDF";
    (void)adapter_context;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL || sink == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return sink(bytes, sizeof(bytes) - 1U, sink_context);
}

/* Exercise built-in text and a separately registered container adapter. */
int main(void)
{
    UmiDocumentGenerationRegistry *registry = NULL;
    UmiDocumentGenerationRequest request = {
        "First Lesson", "Write a small program.", "lesson",
        UMI_DOCUMENT_GENERATION_STORYBOARD};
    UmiDocumentGenerationAdapter adapter = {0};
    UmiDocumentGenerationPlan plan;
    char output[256];
    size_t required_size = 0U;
    size_t byte_count = 0U;

    /* Storyboard text uses the built-in bounded output path. */
    assert(umi_document_generation_plan(&request, &plan) == UMI_STATUS_OK);
    assert(!plan.requires_external_adapter);
    assert(umi_document_generation_write_text(
        &request, output, sizeof(output), &required_size) == UMI_STATUS_OK);
    assert(strstr(output, "STORYBOARD") != NULL);
    assert(required_size > 0U);

    /* PDF intent is dispatched only after an explicit adapter is registered. */
    assert(umi_document_generation_registry_create(2U, &registry) ==
           UMI_STATUS_OK);
    (void)snprintf(adapter.adapter_id, sizeof(adapter.adapter_id), "%s",
                   "test.pdf");
    adapter.format = UMI_DOCUMENT_GENERATION_PDF;
    adapter.generate = fake_pdf_adapter;
    assert(umi_document_generation_registry_register(registry, &adapter) ==
           UMI_STATUS_OK);
    request.format = UMI_DOCUMENT_GENERATION_PDF;
    assert(umi_document_generation_registry_generate(
        registry, &request, count_sink, &byte_count) == UMI_STATUS_OK);
    assert(byte_count == 3U);

    umi_document_generation_registry_destroy(registry);
    return 0;
}
