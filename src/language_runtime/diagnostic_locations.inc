/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/diagnostic_locations.inc
 * PURPOSE: Expose primary and related diagnostic ranges through one complete navigation view.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

UmiStatus UmiLanguageDiagnosticCatalogueLocation(const UmiLanguageDiagnosticCatalogue *catalogue,
                                                 size_t diagnostic_index, size_t location_index,
                                                 UmiLanguageSourceLocation *out)
{
    if (catalogue == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (diagnostic_index >= catalogue->count)
        return UMI_STATUS_NOT_FOUND;
    const DiagnosticRow *row = &catalogue->rows[diagnostic_index];
    /* Keep indexing in the shared owner, not in application widgets. The
     * subtraction occurs only after excluding the primary position, so even
     * a retained invalid UI selection cannot wrap into an unrelated row. */
    if (location_index == 0U)
    {
        *out = (UmiLanguageSourceLocation){
            .uri = catalogue->uri, .target = row->value.range, .selection = row->value.range};
        return UMI_STATUS_OK;
    }
    if (location_index - 1U >= row->value.related_count)
        return UMI_STATUS_NOT_FOUND;
    *out = row->related[location_index - 1U].value.location;
    return UMI_STATUS_OK;
}
