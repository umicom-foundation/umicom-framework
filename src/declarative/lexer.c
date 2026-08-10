/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/lexer.c
 *
 * PURPOSE:
 *   Implement a deterministic line tokenizer supporting whitespace, comments and quoted values.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/lexer.h"

#include <ctype.h>
#include <string.h>

UmiStatus umi_decl_lexer_split_line(const char *line, UmiDeclTokenLine *out_tokens)
{
    size_t i = 0U;
    if (line == NULL || out_tokens == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_tokens, 0, sizeof(*out_tokens));

    /* The grammar is intentionally small so templates remain readable in ordinary editors. */
    while (line[i] != '\0') {
        size_t length = 0U;
        char quote = '\0';
        while (isspace((unsigned char)line[i])) i += 1U;
        if (line[i] == '\0' || line[i] == '#') break;
        if (out_tokens->count >= UMI_DECL_MAX_TOKENS_PER_LINE) return UMI_STATUS_CAPACITY_EXCEEDED;
        if (line[i] == '"' || line[i] == '\'') quote = line[i++];
        while (line[i] != '\0') {
            if (quote != '\0') {
                if (line[i] == quote) { i += 1U; break; }
            } else if (isspace((unsigned char)line[i]) || line[i] == '#') {
                break;
            }
            if (length + 1U >= UMI_DECL_TEXT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
            out_tokens->tokens[out_tokens->count][length++] = line[i++];
        }
        out_tokens->tokens[out_tokens->count][length] = '\0';
        out_tokens->count += 1U;
        if (quote == '\0' && line[i] == '#') break;
    }
    return UMI_STATUS_OK;
}
