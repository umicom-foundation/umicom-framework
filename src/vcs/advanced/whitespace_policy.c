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

/*
 * Provide the vcs advanced whitespace normalize operation used by this module and its
 * client applications.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (input == NULL || output == NULL || capacity == 0U ||
        mode > UMI_VCS_WHITESPACE_IGNORE_ALL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (input[input_index] != '\0') {
        unsigned char ch = (unsigned char)input[input_index++];
        /* Apply this branch only when its contract condition is satisfied. */
        if (ch == '\r' && input[input_index] == '\n') {
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (mode == UMI_VCS_WHITESPACE_IGNORE_ALL && isspace(ch)) {
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (mode == UMI_VCS_WHITESPACE_IGNORE_AMOUNT && isspace(ch)) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (in_space) {
                continue;
            }
            ch = (unsigned char)' ';
            in_space = 1;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            in_space = 0;
        }

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (output_index + 1U >= capacity) {
            output[0] = '\0';
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        output[output_index++] = (char)ch;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!isspace(ch)) {
            last_non_space = output_index;
        }
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (mode == UMI_VCS_WHITESPACE_IGNORE_TRAILING) {
        output_index = last_non_space;
    }
    output[output_index] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced whitespace equal operation used by this module and its client
 * applications.
 */
int umi_vcs_advanced_whitespace_equal(
    const char *left,
    const char *right,
    UmiVcsWhitespaceMode mode)
{
    char normalized_left[2048];
    char normalized_right[2048];

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_vcs_advanced_whitespace_normalize(
            left, mode, normalized_left, sizeof(normalized_left)) !=
        UMI_STATUS_OK) {
        return 0;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_vcs_advanced_whitespace_normalize(
            right, mode, normalized_right, sizeof(normalized_right)) !=
        UMI_STATUS_OK) {
        return 0;
    }
    return strcmp(normalized_left, normalized_right) == 0;
}
