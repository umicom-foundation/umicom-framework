/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_submission.c
 *
 * PURPOSE:
 *   Confirm accepted ComfyUI submissions without converting HTTP success into execution success.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "comfy_response_internal.h"
#include <string.h>
UmiStatus UmiComfySubmissionDecode(unsigned http_status, const void *bytes, size_t length,
    const UmiCancellationToken *cancel, UmiComfySubmission *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (http_status != 200U) return UMI_STATUS_UNAVAILABLE;
    UmiJsonTree *tree = NULL;
    UmiStatus status = UmiComfyResponseOpen(bytes, length, UMI_COMFY_EVENT_MAX_BYTES, cancel, &tree);
    if (status != UMI_STATUS_OK) return status;
    UmiComfySubmission staged = {0};
    int error = -1, number = -1, nodes = -1;
    status = UmiJsonTreeMember(tree, 0, "error", &error);
    if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_OK;
    else if (status == UMI_STATUS_OK && !UmiJsonTreeIsNull(tree, error)) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) status = UmiComfyResponseText(tree, 0, "prompt_id",
        staged.prompt_id, sizeof(staged.prompt_id), 0);
    if (status == UMI_STATUS_OK) status = UmiComfyResponseMember(tree, 0, "number", &number);
/* The shared JSON API groups numbers, booleans and null as primitives. Use its existing enum and check the validated source spelling below; retain the initial incompatible enum check for review. The former implementation is retained for owner review. */
#if 0
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, number) != UMI_LANGUAGE_RUNTIME_JSON_NUMBER)
        status = UMI_STATUS_PARSE_ERROR;
#endif
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, number) != UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) {
        const char *span = NULL; size_t amount = 0;
        status = UmiJsonTreeSourceSpan(tree, number, &span, &amount);
        /* Complete tree validation has already checked numeric grammar. Only
         * a sign or decimal digit distinguishes a number from true/false/null. */
        if (status == UMI_STATUS_OK && (amount == 0 ||
            (span[0] != '-' && (span[0] < '0' || span[0] > '9')))) status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK && amount >= sizeof(staged.queue_number)) status = UMI_STATUS_CAPACITY_EXCEEDED;
        if (status == UMI_STATUS_OK) { memcpy(staged.queue_number, span, amount); staged.queue_number[amount] = '\0'; }
    }
    if (status == UMI_STATUS_OK) status = UmiComfyResponseMember(tree, 0, "node_errors", &nodes);
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, nodes) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) staged.node_error_count = UmiJsonTreeCount(tree, nodes);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel)) status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK) *out = staged;
    UmiJsonTreeDestroy(tree);
    return status;
}
UmiStatus UmiComfyTaskConfirmJson(const UmiProviderTaskReceipt *receipt,
    uint64_t expected_revision, unsigned http_status, const void *bytes, size_t length,
    const UmiCancellationToken *cancel, UmiProviderTaskReceipt *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiComfyReceiptCheck(receipt, expected_revision);
    if (status != UMI_STATUS_OK) return status;
    UmiComfySubmission submission;
    status = UmiComfySubmissionDecode(http_status, bytes, length, cancel, &submission);
    if (status != UMI_STATUS_OK) return status;
    return UmiProviderTaskConfirm(receipt, expected_revision, submission.prompt_id, out);
}
