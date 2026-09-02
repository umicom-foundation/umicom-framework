/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/loader.c
 *
 * PURPOSE:
 *   Load provider bytes, enforce size/binary policy, decode Unicode and analyse
 *   or normalise line endings before a working copy observes the content.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/loader.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/document/fingerprint.h"
#include "umicom/document/line_endings.h"
#include "umicom/document/text_encoding.h"

/*
 * Provide the document load options default operation used by this module and its client
 * applications.
 */
UmiDocumentLoadOptions umi_document_load_options_default(void)
{
    return (UmiDocumentLoadOptions){
        UMI_DOCUMENT_DEFAULT_MAXIMUM_BYTES,
        UMI_DOCUMENT_ENCODING_UTF8,
        UMI_DOCUMENT_LINE_ENDING_NONE,
        1,
        1
    };
}

/* Read document into validated module state and return a status when input cannot be used. */
UmiStatus umi_document_load(const UmiDocumentProvider *provider,
                            const char *resource,
                            const UmiDocumentLoadOptions *options,
                            UmiDocumentLoadResult *out_result)
{
    UmiDocumentLoadOptions effective = umi_document_load_options_default();
    unsigned char *bytes = NULL;
    size_t byte_count = 0U;
    char *decoded = NULL;
    size_t decoded_length = 0U;
    UmiDocumentLineEndingAnalysis analysis;
    int had_bom = 0;
    int binary = 0;
    UmiDocumentTextEncoding detected;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider == NULL || resource == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_result, 0, sizeof(*out_result));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (options != NULL) effective = *options;
    /* Apply this branch only when its contract condition is satisfied. */
    if (effective.maximum_bytes == 0U) effective.maximum_bytes = UMI_DOCUMENT_DEFAULT_MAXIMUM_BYTES;
    status = umi_document_provider_read(provider, resource, &bytes, &byte_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (byte_count > effective.maximum_bytes) {
        umi_document_provider_release_bytes(provider, bytes);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    detected = umi_document_encoding_detect(bytes, byte_count, &had_bom, &binary);
    /* Apply this branch only when its contract condition is satisfied. */
    if (binary && effective.reject_binary) {
        umi_document_provider_release_bytes(provider, bytes);
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    status = umi_document_decode_text(bytes, byte_count, effective.fallback_encoding,
                                      &decoded, &decoded_length, &detected, &had_bom);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_document_provider_release_bytes(provider, bytes);
        return status;
    }
    status = umi_document_line_endings_analyse(decoded, decoded_length, &analysis);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK &&
        effective.normalise_to != UMI_DOCUMENT_LINE_ENDING_NONE) {
        char *normalised = NULL;
        size_t normalised_length = 0U;
        status = umi_document_line_endings_normalise(
            decoded, decoded_length, effective.normalise_to, 0,
            &normalised, &normalised_length);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            free(decoded);
            decoded = normalised;
            decoded_length = normalised_length;
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        out_result->text = decoded;
        out_result->text_length = decoded_length;
        out_result->detected_encoding = detected;
        out_result->detected_line_ending = analysis.dominant;
        out_result->lf_count = analysis.lf_count;
        out_result->crlf_count = analysis.crlf_count;
        out_result->cr_count = analysis.cr_count;
        out_result->fingerprint.content_hash = umi_document_hash_bytes(bytes, byte_count);
        out_result->fingerprint.byte_count = byte_count;
        out_result->fingerprint.valid = 1;
        out_result->had_bom = had_bom;
        out_result->binary = binary;
        decoded = NULL;
    }
    free(decoded);
    umi_document_provider_release_bytes(provider, bytes);
    return status;
}

/*
 * Release or reset state held by document load result so the same storage can be reused
 * safely.
 */
void umi_document_load_result_dispose(UmiDocumentLoadResult *result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == NULL) return;
    free(result->text);
    (void)memset(result, 0, sizeof(*result));
}
