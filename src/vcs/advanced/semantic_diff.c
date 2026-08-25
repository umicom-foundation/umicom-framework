/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/semantic_diff.c
 *
 * PURPOSE:
 *   Implement conservative token-based structural similarity estimation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/advanced/semantic_diff.h"

#include <ctype.h>
#include <string.h>

static size_t tokenize(const char *text, uint64_t *tokens, size_t capacity)
{
    size_t count = 0U;
    char token[128];
    size_t token_length = 0U;
    const unsigned char *cursor = (const unsigned char *)text;

    while (*cursor != 0U) {
        if (isalnum(*cursor) || *cursor == (unsigned char)'_') {
            if (token_length + 1U < sizeof(token)) {
                token[token_length++] = (char)tolower(*cursor);
            }
        } else if (token_length > 0U) {
            token[token_length] = '\0';
            if (count < capacity) {
                tokens[count] = umi_vcs_advanced_hash_text(token);
            }
            count += 1U;
            token_length = 0U;
        }
        cursor += 1;
    }

    if (token_length > 0U) {
        token[token_length] = '\0';
        if (count < capacity) {
            tokens[count] = umi_vcs_advanced_hash_text(token);
        }
        count += 1U;
    }
    return count;
}

UmiStatus umi_vcs_advanced_semantic_diff_compare(
    const char *left,
    const char *right,
    UmiVcsAdvancedSemanticDiff *out_result)
{
    uint64_t left_tokens[256];
    uint64_t right_tokens[256];
    int used[256] = {0};
    size_t left_count;
    size_t right_count;
    size_t left_limit;
    size_t right_limit;
    size_t common = 0U;
    size_t i;
    size_t j;

    if (left == NULL || right == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_result, 0, sizeof(*out_result));
    out_result->struct_size = (uint32_t)sizeof(*out_result);
    out_result->api_version = UMI_VCS_ADVANCED_API_VERSION;
    out_result->left_fingerprint = umi_vcs_advanced_hash_text(left);
    out_result->right_fingerprint = umi_vcs_advanced_hash_text(right);

    left_count = tokenize(left, left_tokens, 256U);
    right_count = tokenize(right, right_tokens, 256U);
    left_limit = left_count < 256U ? left_count : 256U;
    right_limit = right_count < 256U ? right_count : 256U;

    for (i = 0U; i < left_limit; ++i) {
        for (j = 0U; j < right_limit; ++j) {
            if (!used[j] && left_tokens[i] == right_tokens[j]) {
                used[j] = 1;
                common += 1U;
                break;
            }
        }
    }

    out_result->left_token_count = left_count;
    out_result->right_token_count = right_count;
    out_result->common_token_count = common;
    if (left_count == 0U && right_count == 0U) {
        out_result->similarity_percent = 100U;
    } else {
        out_result->similarity_percent =
            (uint32_t)((200U * common) / (left_count + right_count));
    }
    out_result->equivalent = out_result->similarity_percent == 100U;
    return UMI_STATUS_OK;
}
