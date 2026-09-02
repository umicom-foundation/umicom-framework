/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/parser.c
 *
 * PURPOSE:
 *   Parse application, version, component and property statements into the semantic document model.
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

#include "umicom/declarative/parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/declarative/lexer.h"

/* Provide the infer kind operation used by this module and its client applications. */
static UmiDeclValueKind infer_kind(const char *value)
{
    char *end = NULL;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(value, "true") == 0 || strcmp(value, "false") == 0) return UMI_DECL_VALUE_BOOLEAN;
    (void)strtoll(value, &end, 10);
    /* Apply this branch only when its contract condition is satisfied. */
    if (end != value && *end == '\0') return UMI_DECL_VALUE_INTEGER;
    (void)strtod(value, &end);
    /* Apply this branch only when its contract condition is satisfied. */
    if (end != value && *end == '\0') return UMI_DECL_VALUE_REAL;
    return UMI_DECL_VALUE_STRING;
}

/* Provide the parse version operation used by this module and its client applications. */
static UmiStatus parse_version(const char *text, UmiDeclVersion *out_version)
{
    unsigned int major = 0U, minor = 0U, patch = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_version == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (sscanf(text, "%u.%u.%u", &major, &minor, &patch) != 3 || major > 65535U || minor > 65535U || patch > 65535U) return UMI_STATUS_PARSE_ERROR;
    out_version->major = (uint16_t)major;
    out_version->minor = (uint16_t)minor;
    out_version->patch = (uint16_t)patch;
    return UMI_STATUS_OK;
}

/* Provide the join tokens operation used by this module and its client applications. */
static UmiStatus join_tokens(const UmiDeclTokenLine *tokens, size_t start, char *out_text, size_t capacity)
{
    size_t used = 0U;
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tokens == NULL || out_text == NULL || capacity == 0U || start >= tokens->count) return UMI_STATUS_INVALID_ARGUMENT;
    out_text[0] = '\0';
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = start; i < tokens->count; ++i) {
        size_t length = strlen(tokens->tokens[i]);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (used + length + (i > start ? 1U : 0U) + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        /* Apply this branch only when its contract condition is satisfied. */
        if (i > start) out_text[used++] = ' ';
        (void)memcpy(out_text + used, tokens->tokens[i], length);
        used += length;
        out_text[used] = '\0';
    }
    return UMI_STATUS_OK;
}

/* Provide the decl parse text operation used by this module and its client applications. */
UmiStatus umi_decl_parse_text(const char *text, UmiDeclDocument **out_document, UmiDeclDiagnosticList *diagnostics)
{
    char *copy;
    char *cursor;
    char application_id[UMI_DECL_ID_CAPACITY] = {0};
    UmiDeclDocument *document = NULL;
    size_t line_number = 0U;
    UmiStatus overall = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_document == NULL || diagnostics == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_document = NULL;
    umi_decl_diagnostics_clear(diagnostics);
    copy = (char *)malloc(strlen(text) + 1U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (void)strcpy(copy, text);
    cursor = copy;

    /* Parsing is line oriented: each statement is independently diagnosable and beginner friendly. */
    while (cursor != NULL && *cursor != '\0') {
        char *next = strchr(cursor, '\n');
        UmiDeclTokenLine tokens;
        UmiStatus status;
        line_number += 1U;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (next != NULL) *next = '\0';
        status = umi_decl_lexer_split_line(cursor, &tokens);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            (void)umi_decl_diagnostics_add(diagnostics, UMI_DECL_DIAGNOSTIC_ERROR, line_number, 1U, "lexer", "Unable to tokenize source line");
            overall = UMI_STATUS_PARSE_ERROR;
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (tokens.count > 0U) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (strcmp(tokens.tokens[0], "application") == 0 && tokens.count == 2U) {
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (!umi_decl_id_is_valid(tokens.tokens[1])) {
                    (void)umi_decl_diagnostics_add(diagnostics, UMI_DECL_DIAGNOSTIC_ERROR, line_number, 1U, "application-id", "Invalid application identifier");
                    overall = UMI_STATUS_PARSE_ERROR;
                } else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (document != NULL) {
                    (void)umi_decl_diagnostics_add(diagnostics, UMI_DECL_DIAGNOSTIC_ERROR, line_number, 1U, "duplicate-application", "Application statement may appear only once");
                    overall = UMI_STATUS_PARSE_ERROR;
                } /* Use this fallback path when the earlier condition does not apply. */ else {
                    (void)umi_decl_copy_text(application_id, sizeof(application_id), tokens.tokens[1]);
                    status = umi_decl_document_create(application_id, &document);
                    /* Preserve the original failure result so the caller can respond to the correct cause. */
                    if (status != UMI_STATUS_OK) overall = status;
                }
            } else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strcmp(tokens.tokens[0], "version") == 0 && tokens.count == 2U && document != NULL) {
                UmiDeclVersion version;
                status = parse_version(tokens.tokens[1], &version);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status == UMI_STATUS_OK) status = umi_decl_document_set_version(document, version);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) {
                    (void)umi_decl_diagnostics_add(diagnostics, UMI_DECL_DIAGNOSTIC_ERROR, line_number, 1U, "version", "Invalid template version");
                    overall = UMI_STATUS_PARSE_ERROR;
                }
            } else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strcmp(tokens.tokens[0], "component") == 0 && tokens.count >= 3U && document != NULL) {
                UmiDeclNode node;
                const char *parent = tokens.count >= 4U ? tokens.tokens[3] : "-";
                status = umi_decl_node_init(&node, tokens.tokens[1], tokens.tokens[2], parent);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status == UMI_STATUS_OK) status = umi_decl_document_add_node(document, &node);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) {
                    (void)umi_decl_diagnostics_add(diagnostics, UMI_DECL_DIAGNOSTIC_ERROR, line_number, 1U, "component", "Unable to add component");
                    overall = UMI_STATUS_PARSE_ERROR;
                }
            } else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strcmp(tokens.tokens[0], "property") == 0 && tokens.count >= 4U && document != NULL) {
                UmiDeclNode node;
                char value[UMI_DECL_TEXT_CAPACITY];
                status = umi_decl_document_find_node(document, tokens.tokens[1], &node);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status == UMI_STATUS_OK) status = join_tokens(&tokens, 3U, value, sizeof(value));
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status == UMI_STATUS_OK) status = umi_decl_node_set_attribute(&node, tokens.tokens[2], infer_kind(value), value);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status == UMI_STATUS_OK) status = umi_decl_document_update_node(document, &node);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) {
                    (void)umi_decl_diagnostics_add(diagnostics, UMI_DECL_DIAGNOSTIC_ERROR, line_number, 1U, "property", "Unable to set component property");
                    overall = UMI_STATUS_PARSE_ERROR;
                }
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                (void)umi_decl_diagnostics_add(diagnostics, UMI_DECL_DIAGNOSTIC_ERROR, line_number, 1U, "statement", "Unknown or incomplete declarative statement");
                overall = UMI_STATUS_PARSE_ERROR;
            }
        }
        cursor = next != NULL ? next + 1 : NULL;
    }
    free(copy);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL) {
        (void)umi_decl_diagnostics_add(diagnostics, UMI_DECL_DIAGNOSTIC_ERROR, 1U, 1U, "application", "Template must begin with an application statement");
        return UMI_STATUS_PARSE_ERROR;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (overall != UMI_STATUS_OK) {
        umi_decl_document_destroy(document);
        return overall;
    }
    *out_document = document;
    return UMI_STATUS_OK;
}

/* Provide the decl parse file operation used by this module and its client applications. */
UmiStatus umi_decl_parse_file(const char *path, UmiDeclDocument **out_document, UmiDeclDiagnosticList *diagnostics)
{
    FILE *file;
    long length;
    char *text;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || out_document == NULL || diagnostics == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    file = fopen(path, "rb");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (file == NULL) return UMI_STATUS_IO_ERROR;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (fseek(file, 0L, SEEK_END) != 0) { (void)fclose(file); return UMI_STATUS_IO_ERROR; }
    length = ftell(file);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length < 0L || fseek(file, 0L, SEEK_SET) != 0) { (void)fclose(file); return UMI_STATUS_IO_ERROR; }
    text = (char *)malloc((size_t)length + 1U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) { (void)fclose(file); return UMI_STATUS_OUT_OF_MEMORY; }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0L && fread(text, 1U, (size_t)length, file) != (size_t)length) { free(text); (void)fclose(file); return UMI_STATUS_IO_ERROR; }
    text[(size_t)length] = '\0';
    (void)fclose(file);
    status = umi_decl_parse_text(text, out_document, diagnostics);
    free(text);
    return status;
}
