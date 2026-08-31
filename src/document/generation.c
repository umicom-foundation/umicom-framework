/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/generation.c
 *
 * PURPOSE:
 *   Implement truthful document plans, bounded text output and provider-neutral
 *   adapter dispatch without storing files or borrowing secret application state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/generation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The registry owns a fixed array of copied adapter descriptors. */
struct UmiDocumentGenerationRegistry {
    UmiDocumentGenerationAdapter *adapters;
    size_t capacity;
    size_t count;
};

/* Confirm a format value is part of this public API revision. */
static bool valid_format(UmiDocumentGenerationFormat format)
{
    return format >= UMI_DOCUMENT_GENERATION_TEXT &&
        format <= UMI_DOCUMENT_GENERATION_STORYBOARD;
}

/* Copy text into fixed plan fields and reject truncation. */
static UmiStatus copy_text(char *destination, size_t capacity, const char *text)
{
    int written;
    if (destination == NULL || capacity == 0U || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(destination, capacity, "%s", text);
    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}

/* Convert the output intent to stable text without allocating memory. */
const char *umi_document_generation_format_text(
    UmiDocumentGenerationFormat format)
{
    switch (format) {
        case UMI_DOCUMENT_GENERATION_TEXT: return "text";
        case UMI_DOCUMENT_GENERATION_PDF: return "pdf";
        case UMI_DOCUMENT_GENERATION_WORD_PROCESSING: return "word-processing";
        case UMI_DOCUMENT_GENERATION_PRESENTATION: return "presentation";
        case UMI_DOCUMENT_GENERATION_VIDEO_SCRIPT: return "video-script";
        case UMI_DOCUMENT_GENERATION_STORYBOARD: return "storyboard";
        default: return "unknown";
    }
}

/* Validate request text and describe the correct extension, MIME and adapter need. */
UmiStatus umi_document_generation_plan(
    const UmiDocumentGenerationRequest *request,
    UmiDocumentGenerationPlan *out_plan)
{
    const char *extension;
    const char *mime_type;
    bool requires_adapter;
    UmiStatus status;
    if (request == NULL || out_plan == NULL || request->title == NULL ||
        request->title[0] == '\0' || request->body == NULL ||
        request->output_name == NULL || request->output_name[0] == '\0' ||
        !valid_format(request->format)) return UMI_STATUS_INVALID_ARGUMENT;

    /* Binary container formats require adapters; text-like formats are built in. */
    switch (request->format) {
        case UMI_DOCUMENT_GENERATION_TEXT:
            extension = "txt";
            mime_type = "text/plain";
            requires_adapter = false;
            break;
        case UMI_DOCUMENT_GENERATION_PDF:
            extension = "pdf";
            mime_type = "application/pdf";
            requires_adapter = true;
            break;
        case UMI_DOCUMENT_GENERATION_WORD_PROCESSING:
            extension = "docx";
            mime_type = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
            requires_adapter = true;
            break;
        case UMI_DOCUMENT_GENERATION_PRESENTATION:
            extension = "pptx";
            mime_type = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
            requires_adapter = true;
            break;
        case UMI_DOCUMENT_GENERATION_VIDEO_SCRIPT:
        case UMI_DOCUMENT_GENERATION_STORYBOARD:
            extension = "txt";
            mime_type = "text/plain";
            requires_adapter = false;
            break;
        default:
            return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_plan, 0, sizeof(*out_plan));
    out_plan->struct_size = (uint32_t)sizeof(*out_plan);
    out_plan->api_version = 1U;
    out_plan->format = request->format;
    out_plan->requires_external_adapter = requires_adapter;
    status = copy_text(out_plan->output_name,
                       sizeof(out_plan->output_name), request->output_name);
    if (status == UMI_STATUS_OK) status = copy_text(
        out_plan->extension, sizeof(out_plan->extension), extension);
    if (status == UMI_STATUS_OK) status = copy_text(
        out_plan->mime_type, sizeof(out_plan->mime_type), mime_type);
    return status;
}

/* Write a deterministic beginner-readable text representation into caller memory. */
UmiStatus umi_document_generation_write_text(
    const UmiDocumentGenerationRequest *request,
    char *buffer,
    size_t capacity,
    size_t *out_required_size)
{
    UmiDocumentGenerationPlan plan;
    const char *section_label = "";
    int written;
    UmiStatus status;
    if (buffer == NULL || capacity == 0U || out_required_size == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_required_size = 0U;
    status = umi_document_generation_plan(request, &plan);
    if (status != UMI_STATUS_OK) return status;
    /* Binary container formats are never disguised as plain text output. */
    if (plan.requires_external_adapter) return UMI_STATUS_NOT_IMPLEMENTED;
    if (request->format == UMI_DOCUMENT_GENERATION_VIDEO_SCRIPT) {
        section_label = "VIDEO SCRIPT\n\n";
    } else if (request->format == UMI_DOCUMENT_GENERATION_STORYBOARD) {
        section_label = "STORYBOARD\n\n";
    }
    written = snprintf(buffer, capacity, "%s\n\n%s%s\n",
                       request->title, section_label, request->body);
    if (written < 0) return UMI_STATUS_INTERNAL_ERROR;
    *out_required_size = (size_t)written + 1U;
    /* The required size includes the terminator so callers can retry exactly. */
    return (size_t)written < capacity
        ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}

/* Allocate checked fixed-capacity descriptor storage for output adapters. */
UmiStatus umi_document_generation_registry_create(
    size_t adapter_capacity,
    UmiDocumentGenerationRegistry **out_registry)
{
    UmiDocumentGenerationRegistry *registry;
    if (out_registry == NULL || adapter_capacity == 0U ||
        adapter_capacity > SIZE_MAX / sizeof(UmiDocumentGenerationAdapter)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_registry = NULL;
    registry = (UmiDocumentGenerationRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->adapters = (UmiDocumentGenerationAdapter *)calloc(
        adapter_capacity, sizeof(*registry->adapters));
    /* Release owner when its adapter array cannot be allocated. */
    if (registry->adapters == NULL) {
        free(registry);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    registry->capacity = adapter_capacity;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

/* Release copied descriptors without assuming ownership of adapter contexts. */
void umi_document_generation_registry_destroy(
    UmiDocumentGenerationRegistry *registry)
{
    if (registry == NULL) return;
    free(registry->adapters);
    free(registry);
}

/* Register an exact unique ID and one adapter per document format. */
UmiStatus umi_document_generation_registry_register(
    UmiDocumentGenerationRegistry *registry,
    const UmiDocumentGenerationAdapter *adapter)
{
    size_t index;
    if (registry == NULL || adapter == NULL || adapter->adapter_id[0] == '\0' ||
        !valid_format(adapter->format) || adapter->generate == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < registry->count; ++index) {
        /* Duplicate ID or format would make dispatch order-dependent. */
        if (strcmp(registry->adapters[index].adapter_id,
                   adapter->adapter_id) == 0 ||
            registry->adapters[index].format == adapter->format) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (registry->count >= registry->capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    registry->adapters[registry->count] = *adapter;
    registry->count += 1U;
    return UMI_STATUS_OK;
}

/* Validate the request and dispatch it to the adapter registered for its format. */
UmiStatus umi_document_generation_registry_generate(
    const UmiDocumentGenerationRegistry *registry,
    const UmiDocumentGenerationRequest *request,
    UmiDocumentGenerationSink sink,
    void *sink_context)
{
    UmiDocumentGenerationPlan plan;
    size_t index;
    UmiStatus status;
    if (registry == NULL || sink == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_document_generation_plan(request, &plan);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < registry->count; ++index) {
        /* Dispatch the normalised format recorded by the validated plan. */
        if (registry->adapters[index].format == plan.format) {
            return registry->adapters[index].generate(
                request, sink, sink_context,
                registry->adapters[index].adapter_context);
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/* Return the safe copied descriptor count, treating a NULL registry as empty. */
size_t umi_document_generation_registry_count(
    const UmiDocumentGenerationRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
