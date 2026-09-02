/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/normalizer.c
 *
 * PURPOSE:
 *   Calculate raw and whitespace/comment-normalised hashes for duplicate
 *   detection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */

#include "umicom/codeguard/normalizer.h"

#include <ctype.h>
#include <stdio.h>

#define UMI_CODEGUARD_HASH_OFFSET UINT64_C(1469598103934665603)
#define UMI_CODEGUARD_HASH_PRIME UINT64_C(1099511628211)

typedef enum UmiCodeGuardNormalizerState {
    UMI_CODEGUARD_NORMALIZER_CODE = 0,
    UMI_CODEGUARD_NORMALIZER_PENDING_SLASH,
    UMI_CODEGUARD_NORMALIZER_LINE_COMMENT,
    UMI_CODEGUARD_NORMALIZER_BLOCK_COMMENT,
    UMI_CODEGUARD_NORMALIZER_BLOCK_COMMENT_STAR,
    UMI_CODEGUARD_NORMALIZER_STRING,
    UMI_CODEGUARD_NORMALIZER_STRING_ESCAPE,
    UMI_CODEGUARD_NORMALIZER_CHARACTER,
    UMI_CODEGUARD_NORMALIZER_CHARACTER_ESCAPE
} UmiCodeGuardNormalizerState;

/* FNV-1a is used consistently by raw and normalised fingerprints so results
 * remain deterministic across supported operating systems. */
static void umi_codeguard_hash_append(uint64_t *hash, unsigned char value)
{
    *hash ^= (uint64_t)value;
    *hash *= UMI_CODEGUARD_HASH_PRIME;
}

/*
 * Provide the codeguard hash bytes operation used by this module and its client
 * applications.
 */
uint64_t umi_codeguard_hash_bytes(const void *data, size_t size)
{
    const unsigned char *bytes = (const unsigned char *)data;
    uint64_t hash = UMI_CODEGUARD_HASH_OFFSET;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bytes == NULL && size > 0U) {
        return 0U;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < size; ++index) {
        umi_codeguard_hash_append(&hash, bytes[index]);
    }
    return hash;
}

/* Process one byte. The pending-slash state delays hashing '/' until the next
 * byte proves whether it is an operator or the start of a comment. */
static void umi_codeguard_normalize_byte(unsigned char value,
                                         UmiCodeGuardNormalizerState *state,
                                         uint64_t *hash)
{
    int process_again = 1;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (process_again) {
        process_again = 0;
        /* Select the behaviour associated with the requested command or state value. */
        switch (*state) {
            case UMI_CODEGUARD_NORMALIZER_CODE:
                /* Apply this branch only when its contract condition is satisfied. */
                if (isspace(value) != 0) {
                    break;
                }
                /* Apply this branch only when its contract condition is satisfied. */
                if (value == '/') {
                    *state = UMI_CODEGUARD_NORMALIZER_PENDING_SLASH;
                    break;
                }
                umi_codeguard_hash_append(hash, value);
                /* Apply this branch only when its contract condition is satisfied. */
                if (value == '"') {
                    *state = UMI_CODEGUARD_NORMALIZER_STRING;
                } else /* Apply this branch only when its contract condition is satisfied. */ if (value == '\'') {
                    *state = UMI_CODEGUARD_NORMALIZER_CHARACTER;
                }
                break;

            case UMI_CODEGUARD_NORMALIZER_PENDING_SLASH:
                /* Apply this branch only when its contract condition is satisfied. */
                if (value == '/') {
                    *state = UMI_CODEGUARD_NORMALIZER_LINE_COMMENT;
                } else /* Apply this branch only when its contract condition is satisfied. */ if (value == '*') {
                    *state = UMI_CODEGUARD_NORMALIZER_BLOCK_COMMENT;
                } /* Use this fallback path when the earlier condition does not apply. */ else {
                    /* The previous slash is an operator. Hash it, then handle
                     * the current byte normally without reading another byte. */
                    umi_codeguard_hash_append(hash, '/');
                    *state = UMI_CODEGUARD_NORMALIZER_CODE;
                    process_again = 1;
                }
                break;

            case UMI_CODEGUARD_NORMALIZER_LINE_COMMENT:
                /* Apply this branch only when its contract condition is satisfied. */
                if (value == '\n' || value == '\r') {
                    *state = UMI_CODEGUARD_NORMALIZER_CODE;
                }
                break;

            case UMI_CODEGUARD_NORMALIZER_BLOCK_COMMENT:
                /* Apply this branch only when its contract condition is satisfied. */
                if (value == '*') {
                    *state = UMI_CODEGUARD_NORMALIZER_BLOCK_COMMENT_STAR;
                }
                break;

            case UMI_CODEGUARD_NORMALIZER_BLOCK_COMMENT_STAR:
                /* Apply this branch only when its contract condition is satisfied. */
                if (value == '/') {
                    *state = UMI_CODEGUARD_NORMALIZER_CODE;
                } else /* Apply this branch only when its contract condition is satisfied. */ if (value != '*') {
                    *state = UMI_CODEGUARD_NORMALIZER_BLOCK_COMMENT;
                }
                break;

            case UMI_CODEGUARD_NORMALIZER_STRING:
                umi_codeguard_hash_append(hash, value);
                /* Apply this branch only when its contract condition is satisfied. */
                if (value == '\\') {
                    *state = UMI_CODEGUARD_NORMALIZER_STRING_ESCAPE;
                } else /* Apply this branch only when its contract condition is satisfied. */ if (value == '"') {
                    *state = UMI_CODEGUARD_NORMALIZER_CODE;
                }
                break;

            case UMI_CODEGUARD_NORMALIZER_STRING_ESCAPE:
                umi_codeguard_hash_append(hash, value);
                *state = UMI_CODEGUARD_NORMALIZER_STRING;
                break;

            case UMI_CODEGUARD_NORMALIZER_CHARACTER:
                umi_codeguard_hash_append(hash, value);
                /* Apply this branch only when its contract condition is satisfied. */
                if (value == '\\') {
                    *state = UMI_CODEGUARD_NORMALIZER_CHARACTER_ESCAPE;
                } else /* Apply this branch only when its contract condition is satisfied. */ if (value == '\'') {
                    *state = UMI_CODEGUARD_NORMALIZER_CODE;
                }
                break;

            case UMI_CODEGUARD_NORMALIZER_CHARACTER_ESCAPE:
                umi_codeguard_hash_append(hash, value);
                *state = UMI_CODEGUARD_NORMALIZER_CHARACTER;
                break;
        }
    }
}

/*
 * Provide the codeguard hash file operation used by this module and its client
 * applications.
 */
UmiStatus umi_codeguard_hash_file(const char *path,
                                  uint64_t *out_raw_hash,
                                  uint64_t *out_normalized_hash,
                                  size_t *out_size)
{
    FILE *file;
    unsigned char buffer[4096];
    size_t bytes_read;
    size_t total_size = 0U;
    uint64_t raw_hash = UMI_CODEGUARD_HASH_OFFSET;
    uint64_t normalized_hash = UMI_CODEGUARD_HASH_OFFSET;
    UmiCodeGuardNormalizerState state = UMI_CODEGUARD_NORMALIZER_CODE;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || out_raw_hash == NULL || out_normalized_hash == NULL ||
        out_size == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    file = fopen(path, "rb");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (file == NULL) {
        return UMI_STATUS_IO_ERROR;
    }

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while ((bytes_read = fread(buffer, 1U, sizeof(buffer), file)) > 0U) {
        size_t index;

        total_size += bytes_read;
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < bytes_read; ++index) {
            umi_codeguard_hash_append(&raw_hash, buffer[index]);
            umi_codeguard_normalize_byte(buffer[index], &state,
                                          &normalized_hash);
        }
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (ferror(file) != 0) {
        (void)fclose(file);
        return UMI_STATUS_IO_ERROR;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (fclose(file) != 0) {
        return UMI_STATUS_IO_ERROR;
    }

    /* A slash at end-of-file cannot begin a comment, so it is an operator. */
    if (state == UMI_CODEGUARD_NORMALIZER_PENDING_SLASH) {
        umi_codeguard_hash_append(&normalized_hash, '/');
    }

    *out_raw_hash = raw_hash;
    *out_normalized_hash = normalized_hash;
    *out_size = total_size;
    return UMI_STATUS_OK;
}
