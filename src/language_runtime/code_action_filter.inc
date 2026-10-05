/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/code_action_filter.inc
 * PURPOSE: Select complete action kinds without projecting away required edit or command metadata.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

UmiStatus UmiLanguageCodeActionFilterName(UmiLanguageCodeActionFilter filter, const char **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    switch (filter)
    {
    case UMI_LANGUAGE_ACTION_FILTER_ALL:
        *out = "";
        break;
    case UMI_LANGUAGE_ACTION_FILTER_QUICK_FIX:
        *out = "quickfix";
        break;
    case UMI_LANGUAGE_ACTION_FILTER_REFACTOR:
        *out = "refactor";
        break;
    case UMI_LANGUAGE_ACTION_FILTER_ORGANIZE_IMPORTS:
        *out = "source.organizeImports";
        break;
    case UMI_LANGUAGE_ACTION_FILTER_FIX_ALL:
        *out = "source.fixAll";
        break;
    default:
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
/* Kind families use dot boundaries. A textual prefix alone would incorrectly
 * treat unrelated server actions as belonging to the requested family. */
static int ActionKindMatches(const char *kind, const char *family)
{
    if (family[0] == '\0')
        return 1;
    if (kind == NULL)
        return 0;
    size_t bytes = strlen(family);
    return strncmp(kind, family, bytes) == 0 && (kind[bytes] == '\0' || kind[bytes] == '.');
}
UmiStatus UmiLanguageCodeActionCatalogueFilter(const UmiLanguageCodeActionCatalogue *catalogue,
                                               UmiLanguageCodeActionFilter filter,
                                               const UmiCancellationToken *cancel,
                                               UmiLanguageCodeActionCatalogue **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (catalogue == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *family = NULL;
    UmiStatus status = UmiLanguageCodeActionFilterName(filter, &family);
    if (status != UMI_STATUS_OK)
        return status;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    /* Measure complete original rows before allocating. This bounds temporary
     * storage and preserves unknown fields instead of reconstructing actions. */
    size_t bytes = 2U, count = 0U;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < catalogue->count; ++i)
    {
        if (CompletionCancelled(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        if (!ActionKindMatches(catalogue->rows[i].kind, family))
            continue;
        const char *span = NULL;
        size_t length = 0U;
        status = UmiLanguageCodeActionCatalogueItemJson(catalogue, i, &span, &length);
        size_t separator = count != 0U ? 1U : 0U;
        if (status == UMI_STATUS_OK &&
            (separator > 1024U * 1024U - bytes || length > 1024U * 1024U - bytes - separator))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        if (status == UMI_STATUS_OK)
        {
            bytes += length + separator;
            ++count;
        }
    }
    if (status != UMI_STATUS_OK)
        return status;
    char *json = malloc(bytes + 1U);
    if (json == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    size_t used = 1U, written = 0U;
    json[0] = '[';
    for (size_t i = 0U; status == UMI_STATUS_OK && i < catalogue->count; ++i)
    {
        if (CompletionCancelled(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        if (!ActionKindMatches(catalogue->rows[i].kind, family))
            continue;
        const char *span = NULL;
        size_t length = 0U;
        status = UmiLanguageCodeActionCatalogueItemJson(catalogue, i, &span, &length);
        if (status != UMI_STATUS_OK)
            break;
        if (written != 0U)
            json[used++] = ',';
        memcpy(json + used, span, length);
        used += length;
        ++written;
    }
    if (status == UMI_STATUS_OK)
    {
        json[used++] = ']';
        json[used] = '\0';
        status = UmiLanguageCodeActionCatalogueCreate(json, used, cancel, out);
    }
    free(json);
    return status;
}
