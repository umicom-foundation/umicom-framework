/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/snippet_storage.h
 * PURPOSE: Persist explicitly named local snippet templates independently of editor insertion and provider credentials.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEVELOPER_PROJECT_SNIPPET_STORAGE_H
#define UMICOM_DEVELOPER_PROJECT_SNIPPET_STORAGE_H
#include "umicom/editor/snippet_session.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* Validate terminated metadata, visible names and UTF-8 body bytes. Metadata
 * cannot contain control characters. Body may contain tabs and line endings.
 * This checks storage safety, not full language-server snippet syntax. The
 * parser and insertion review determine supported expansion behaviour. */
    UmiStatus UmiSnippetTemplateValidate(const UmiEditorSnippetTemplate *snippet);
    /* Exact JSON fields: format, id, language, name and body. Missing, duplicate,
 * unknown or mistyped fields reject the complete record. Encode needs room for
 * NUL; its returned byte count excludes NUL. Decode accepts at most 65536 bytes.
 * Caller outputs remain unchanged on failure. Inputs must not overlap outputs.
 * These functions belong to Umicom::developer; no filesystem is accessed. */
    UmiStatus UmiSnippetTemplateEncode(const UmiEditorSnippetTemplate *snippet, char *out, size_t capacity,
                                       size_t *out_bytes);
    UmiStatus UmiSnippetTemplateDecode(const void *bytes, size_t size, UmiEditorSnippetTemplate *out);
    /* Resolve a case-preserving named file in the application's local config path.
 * Language and ID are UTF-8 metadata, never path components; hexadecimal
 * encoding prevents traversal and case folding. No directories are created.
 * An absolute base_override permits portable hosts and isolated tests. */
    UmiStatus UmiSnippetTemplatePath(const char *application_directory, const char *base_override,
                                     const char *language_id, const char *template_id, char *out,
                                     size_t capacity);
    /* Explicit regular-file load or atomic replacement through the shared native
 * file services. Load verifies exact expected language and ID before returning
 * a draft template. Save replaces a named template; its trusted parent must
 * already exist. The file contains ordinary local text, not encrypted secrets.
 * Save inherits UmiLocalFileReplace's last-writer, metadata and durability limits.
 * Run I/O on a worker and discard stale load results. Neither call edits source,
 * expands a template, starts a process or contacts a service. */
    UmiStatus UmiSnippetTemplateLoad(const char *path, const char *expected_language, const char *expected_id,
                                     UmiEditorSnippetTemplate *out);
    UmiStatus UmiSnippetTemplateSave(const char *path, const UmiEditorSnippetTemplate *snippet);
#ifdef __cplusplus
}
#endif
#endif
