/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/process_windows_arguments.inc
 * PURPOSE: Encode literal argv using Windows C-runtime quoting rules.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/*
 * Provide the windows needs quotes operation used by this module and its client
 * applications.
 */
static int umi_windows_needs_quotes(const char *text)
{
    const char *cursor = text;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || text[0] == '\0') return 1;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == ' ' || *cursor == '\t' || *cursor == '"') return 1;
        ++cursor;
    }
    return 0;
}

/*
 * Provide the windows append quoted operation used by this module and its client
 * applications.
 */
static UmiStatus umi_windows_append_quoted(char *buffer,
                                           size_t capacity,
                                           size_t *used,
                                           const char *text)
{
    size_t backslashes = 0U;
    const char *cursor;
    int quoted;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer == NULL || used == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    quoted = umi_windows_needs_quotes(text);
    /* Apply this branch only when its contract condition is satisfied. */
    if (quoted) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (*used + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        buffer[(*used)++] = '"';
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (cursor = text; ; ++cursor) {
        char value = *cursor;
        /* Apply this branch only when its contract condition is satisfied. */
        if (value == '\\') {
            ++backslashes;
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (value == '"') {
            size_t index;
            /* Visit each bounded item once so every record receives the same rule. */
            for (index = 0U; index < backslashes * 2U + 1U; ++index) {
                /* Keep the operation inside its valid bounds before reading, writing or adding data. */
                if (*used + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
                buffer[(*used)++] = '\\';
            }
            backslashes = 0U;
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (*used + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
            buffer[(*used)++] = '"';
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (value == '\0') {
            size_t index;
            size_t count = quoted ? backslashes * 2U : backslashes;
            /* Visit each bounded item once so every record receives the same rule. */
            for (index = 0U; index < count; ++index) {
                /* Keep the operation inside its valid bounds before reading, writing or adding data. */
                if (*used + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
                buffer[(*used)++] = '\\';
            }
            break;
        }
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (backslashes > 0U) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (*used + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
            buffer[(*used)++] = '\\';
            --backslashes;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (*used + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        buffer[(*used)++] = value;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (quoted) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (*used + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        buffer[(*used)++] = '"';
    }
    buffer[*used] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Provide the windows command line operation used by this module and its client
 * applications.
 */
static UmiStatus umi_windows_command_line(const UmiProcessRequest *request,
                                          char *buffer,
                                          size_t capacity)
{
    size_t used = 0U;
    size_t index;
    UmiStatus status;
    status = umi_windows_append_quoted(buffer, capacity, &used, request->program);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < request->argument_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (used + 2U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        buffer[used++] = ' ';
        buffer[used] = '\0';
        status = umi_windows_append_quoted(buffer,
                                           capacity,
                                           &used,
                                           request->arguments[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

