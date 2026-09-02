/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/lexer.c
 *
 * PURPOSE:
 *   Implement a deterministic line tokenizer supporting whitespace, comments and quoted values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/lexer.h"

#include <ctype.h>
#include <string.h>

/*
 * Provide the decl lexer split line operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_lexer_split_line(const char *line, UmiDeclTokenLine *out_tokens)
{
    size_t i = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (line == NULL || out_tokens == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_tokens, 0, sizeof(*out_tokens));

    /* The grammar is intentionally small so templates remain readable in ordinary editors. */
    while (line[i] != '\0') {
        size_t length = 0U;
        char quote = '\0';
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (isspace((unsigned char)line[i])) i += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (line[i] == '\0' || line[i] == '#') break;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (out_tokens->count >= UMI_DECL_MAX_TOKENS_PER_LINE) return UMI_STATUS_CAPACITY_EXCEEDED;
        /* Apply this branch only when its contract condition is satisfied. */
        if (line[i] == '"' || line[i] == '\'') quote = line[i++];
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (line[i] != '\0') {
            /* Apply this branch only when its contract condition is satisfied. */
            if (quote != '\0') {
                /* Apply this branch only when its contract condition is satisfied. */
                if (line[i] == quote) { i += 1U; break; }
            } else /* Apply this branch only when its contract condition is satisfied. */ if (isspace((unsigned char)line[i]) || line[i] == '#') {
                break;
            }
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (length + 1U >= UMI_DECL_TEXT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
            out_tokens->tokens[out_tokens->count][length++] = line[i++];
        }
        out_tokens->tokens[out_tokens->count][length] = '\0';
        out_tokens->count += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (quote == '\0' && line[i] == '#') break;
    }
    return UMI_STATUS_OK;
}
