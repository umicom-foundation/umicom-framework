/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/whitespace_policy.c
 *
 * PURPOSE:
 *   Implement text normalization under explicit whitespace comparison policy.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/advanced/whitespace_policy.h"

#include <ctype.h>
#include <string.h>

UmiStatus umi_vcs_advanced_whitespace_normalize(
    const char *input,
    UmiVcsWhitespaceMode mode,
    char *output,
    size_t capacity)
{
    size_t input_index = 0U;
    size_t output_index = 0U;
    size_t last_non_space = 0U;
    int in_space = 0;

    if (input == NULL || output == NULL || capacity == 0U ||
        mode > UMI_VCS_WHITESPACE_IGNORE_ALL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    while (input[input_index] != '\0') {
        unsigned char ch = (unsigned char)input[input_index++];
        if (ch == '\r' && input[input_index] == '\n') {
            continue;
        }
        if (mode == UMI_VCS_WHITESPACE_IGNORE_ALL && isspace(ch)) {
            continue;
        }
        if (mode == UMI_VCS_WHITESPACE_IGNORE_AMOUNT && isspace(ch)) {
            if (in_space) {
                continue;
            }
            ch = (unsigned char)' ';
            in_space = 1;
        } else {
            in_space = 0;
        }

        if (output_index + 1U >= capacity) {
            output[0] = '\0';
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        output[output_index++] = (char)ch;
        if (!isspace(ch)) {
            last_non_space = output_index;
        }
    }

    if (mode == UMI_VCS_WHITESPACE_IGNORE_TRAILING) {
        output_index = last_non_space;
    }
    output[output_index] = '\0';
    return UMI_STATUS_OK;
}

int umi_vcs_advanced_whitespace_equal(
    const char *left,
    const char *right,
    UmiVcsWhitespaceMode mode)
{
    char normalized_left[2048];
    char normalized_right[2048];

    if (umi_vcs_advanced_whitespace_normalize(
            left, mode, normalized_left, sizeof(normalized_left)) !=
        UMI_STATUS_OK) {
        return 0;
    }
    if (umi_vcs_advanced_whitespace_normalize(
            right, mode, normalized_right, sizeof(normalized_right)) !=
        UMI_STATUS_OK) {
        return 0;
    }
    return strcmp(normalized_left, normalized_right) == 0;
}
