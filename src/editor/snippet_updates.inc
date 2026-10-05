/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/snippet_updates.inc
 * PURPOSE: Apply linked snippet values atomically while preserving source-independent traversal and exact byte positions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static UmiStatus SnippetUpdateText(const char *text, size_t bytes)
{
    if (text == NULL || memchr(text, '\0', bytes) != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = text;
    view.byte_count = view.capacity = bytes;
    UmiEditorTextPosition end;
    return UmiEditorTextViewPositionAt(&view, bytes, &end);
}
UmiStatus UmiEditorSnippetSessionRead(const UmiEditorSnippetSession *session, const char **out_text,
                                      size_t *out_bytes)
{
    if (session == NULL || out_text == NULL || out_bytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_text = session->expanded;
    *out_bytes = session->expanded_length;
    return UMI_STATUS_OK;
}
UmiStatus UmiEditorSnippetSessionReplace(UmiEditorSnippetSession *session, uint64_t expected_revision,
                                         uint32_t ordinal, const char *text, size_t bytes,
                                         const UmiCancellationToken *cancel)
{
    if (session == NULL || text == NULL || ordinal == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    if (session->state != UMI_EDITOR_SNIPPET_ACTIVE || session->revision != expected_revision)
        return UMI_STATUS_INVALID_STATE;
    if (bytes >= UMI_EDITOR_SNIPPET_DEFAULT_CAPACITY || session->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (primary_for_ordinal(session, ordinal) == SIZE_MAX)
        return UMI_STATUS_NOT_FOUND;
    UmiStatus status = SnippetUpdateText(text, bytes);
    if (status == UMI_STATUS_OK)
        status = SnippetUpdateText(session->expanded, session->expanded_length);
    if (status != UMI_STATUS_OK)
        return status;
    if (session->placeholder_count > SIZE_MAX / sizeof(*session->placeholders))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiEditorSnippetSession *candidate = malloc(sizeof(*candidate));
    if (candidate == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    *candidate = *session;
    candidate->placeholders = NULL;
    candidate->placeholder_capacity = session->placeholder_count;
    if (session->placeholder_count != 0U)
    {
        candidate->placeholders = malloc(session->placeholder_count * sizeof(*candidate->placeholders));
        if (candidate->placeholders == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
        else
            memcpy(candidate->placeholders, session->placeholders,
                   session->placeholder_count * sizeof(*candidate->placeholders));
    }
    candidate->expanded_length = 0U;
    candidate->expanded[0] = '\0';
    size_t consumed = 0U;
    /* Parsed occurrences are in textual order. Rebuild the immutable original
     * segments once, rather than repeatedly shifting a live string. This also
     * assigns coincident empty stops a deterministic order as text is inserted. */
    for (size_t i = 0U; status == UMI_STATUS_OK && i < session->placeholder_count; ++i)
    {
        if (umi_cancellation_token_is_requested(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        const UmiEditorSnippetPlaceholder *old = &session->placeholders[i];
        if (old->start_byte_offset < session->insertion_byte_offset ||
            old->end_byte_offset < old->start_byte_offset ||
            old->end_byte_offset - session->insertion_byte_offset > session->expanded_length)
        {
            status = UMI_STATUS_INVALID_STATE;
            break;
        }
        size_t begin = (size_t)(old->start_byte_offset - session->insertion_byte_offset);
        size_t end = (size_t)(old->end_byte_offset - session->insertion_byte_offset);
        if (begin < consumed)
        {
            status = UMI_STATUS_INVALID_STATE;
            break;
        }
        status = append_text(candidate, session->expanded + consumed, begin - consumed);
        if (status != UMI_STATUS_OK)
            break;
        size_t first = candidate->expanded_length;
        status = append_text(candidate, old->ordinal == ordinal ? text : session->expanded + begin,
                             old->ordinal == ordinal ? bytes : end - begin);
        if (status != UMI_STATUS_OK)
            break;
        if (session->insertion_byte_offset > UINT64_MAX - candidate->expanded_length)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        UmiEditorSnippetPlaceholder *updated = &candidate->placeholders[i];
        updated->start_byte_offset = session->insertion_byte_offset + first;
        updated->end_byte_offset = session->insertion_byte_offset + candidate->expanded_length;
        if (old->ordinal == ordinal)
        {
            memcpy(updated->default_text, text, bytes);
            updated->default_text[bytes] = '\0';
        }
        consumed = end;
    }
    if (status == UMI_STATUS_OK)
        status = append_text(candidate, session->expanded + consumed, session->expanded_length - consumed);
    if (status == UMI_STATUS_OK && session->insertion_byte_offset > UINT64_MAX - candidate->expanded_length)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
    {
        /* Publish only after every occurrence and following range is ready.
         * The original remains readable throughout validation and allocation. */
        candidate->revision = session->revision + 1U;
        free(session->placeholders);
        *session = *candidate;
        free(candidate);
    }
    else
        umi_editor_snippet_session_destroy(candidate);
    return status;
}

UmiStatus UmiEditorSnippetSessionRestart(UmiEditorSnippetSession *session, uint64_t expected_revision,
                                         const UmiEditorSnippetTemplate *snippet,
                                         uint64_t insertion_byte_offset, const UmiCancellationToken *cancel)
{
    if (session == NULL || snippet == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    if (session->revision != expected_revision)
        return UMI_STATUS_INVALID_STATE;
    if (session->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiEditorSnippetSession *candidate = NULL;
    UmiStatus status = umi_editor_snippet_session_create(&candidate);
    if (status == UMI_STATUS_OK)
        status = umi_editor_snippet_session_start(candidate, snippet, insertion_byte_offset);
    if (status == UMI_STATUS_OK)
        status = SnippetUpdateText(candidate->expanded, candidate->expanded_length);
    if (status == UMI_STATUS_OK && insertion_byte_offset > UINT64_MAX - candidate->expanded_length)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
    {
        /* A malformed replacement cannot erase an in-progress template. Move
         * candidate storage only after validation, keeping the session address. */
        candidate->revision = session->revision + 1U;
        free(session->placeholders);
        *session = *candidate;
        free(candidate);
    }
    else
        umi_editor_snippet_session_destroy(candidate);
    return status;
}
