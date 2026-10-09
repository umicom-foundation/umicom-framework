/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/configuration_lifecycle.inc
 * PURPOSE: Rename and remove saved configurations through the owning library transaction.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Catalogue edits validate every affected profile before writing. Removal keeps
 * names in their visible order and erases only the final vacated storage slot. */
static UmiStatus ConfigurationReadProfile(UmiDataServer *server, const char *prefix,
                                          const char *root, size_t slot,
                                          uint64_t catalogue_revision, UmiBuildProfile *out)
{
    char entry[128];
    uint64_t revision = 0U;
    UmiStatus status = ConfigurationSlot(prefix, slot, entry);
    if (status == UMI_STATUS_OK)
        status = UmiBuildProfileStorageRead(server, entry, root, out, &revision);
    if (status == UMI_STATUS_NOT_FOUND)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK && revision > catalogue_revision)
        status = UMI_STATUS_PARSE_ERROR;
    return status;
}
static UmiStatus ConfigurationPublishRevision(UmiDataServer *server, const char *prefix,
                                              uint64_t revision, uint64_t *out)
{
    char number[32];
    snprintf(number, sizeof number, "%" PRIu64, revision);
    UmiStatus status = ConfigurationSet(server, prefix, "revision", number);
    if (status == UMI_STATUS_OK)
        status = umi_data_server_commit(server);
    if (status == UMI_STATUS_OK)
        *out = revision;
    else
        status = ConfigurationFinishRead(server, status);
    return status;
}
UmiStatus UmiBuildConfigurationRename(UmiDataServer *server, const char *source, const char *name,
                                      const char *replacement, uint64_t expected_revision,
                                      uint64_t *out_revision)
{
    if (server == NULL || out_revision == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiBuildConfigurationNameValidate(name);
    if (status == UMI_STATUS_OK)
        status = UmiBuildConfigurationNameValidate(replacement);
    char root[UMI_BUILD_PATH_CAPACITY], prefix[128], entry[128];
    if (status == UMI_STATUS_OK)
        status = ConfigurationIdentity(source, root, prefix);
    if (status != UMI_STATUS_OK)
        return status;
    UmiBuildProfile *profile = malloc(sizeof *profile);
    if (profile == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_data_server_begin(server);
    if (status != UMI_STATUS_OK)
    {
        free(profile);
        return status;
    }
    UmiBuildConfigurationCatalogue catalogue;
    status = ConfigurationReadCatalogue(server, prefix, root, &catalogue);
    if (status == UMI_STATUS_OK && catalogue.revision != expected_revision)
        status = UMI_STATUS_INVALID_STATE;
    size_t slot = status == UMI_STATUS_OK ? ConfigurationFind(&catalogue, name) : 0U;
    if (status == UMI_STATUS_OK && slot == catalogue.count)
        status = UMI_STATUS_NOT_FOUND;
    if (status == UMI_STATUS_OK)
        status = ConfigurationReadProfile(server, prefix, root, slot, catalogue.revision, profile);
    if (status == UMI_STATUS_OK && strcmp(name, replacement) == 0)
    {
        status = ConfigurationFinishRead(server, status);
        if (status == UMI_STATUS_OK)
            *out_revision = catalogue.revision;
        free(profile);
        return status;
    }
    if (status == UMI_STATUS_OK && ConfigurationFind(&catalogue, replacement) != catalogue.count)
        status = UMI_STATUS_ALREADY_EXISTS;
    if (status == UMI_STATUS_OK && catalogue.revision == UINT64_MAX)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK)
        status = ConfigurationSlot(prefix, slot, entry);
    if (status == UMI_STATUS_OK)
        status = ConfigurationSet(server, entry, "name", replacement);
    status =
        status == UMI_STATUS_OK
            ? ConfigurationPublishRevision(server, prefix, catalogue.revision + 1U, out_revision)
            : ConfigurationFinishRead(server, status);
    free(profile);
    return status;
}
UmiStatus UmiBuildConfigurationRemove(UmiDataServer *server, const char *source, const char *name,
                                      uint64_t expected_revision, uint64_t *out_revision)
{
    if (server == NULL || out_revision == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiBuildConfigurationNameValidate(name);
    char root[UMI_BUILD_PATH_CAPACITY], prefix[128], entry[128], key[192], number[32];
    if (status == UMI_STATUS_OK)
        status = ConfigurationIdentity(source, root, prefix);
    if (status != UMI_STATUS_OK)
        return status;
    status = umi_data_server_begin(server);
    if (status != UMI_STATUS_OK)
        return status;
    UmiBuildConfigurationCatalogue catalogue;
    status = ConfigurationReadCatalogue(server, prefix, root, &catalogue);
    if (status == UMI_STATUS_OK && catalogue.revision != expected_revision)
        status = UMI_STATUS_INVALID_STATE;
    size_t slot = status == UMI_STATUS_OK ? ConfigurationFind(&catalogue, name) : 0U;
    if (status == UMI_STATUS_OK && slot == catalogue.count)
        status = UMI_STATUS_NOT_FOUND;
    if (status == UMI_STATUS_OK && catalogue.revision == UINT64_MAX)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    UmiBuildProfile *profiles = NULL;
    if (status == UMI_STATUS_OK)
    {
        profiles = calloc(catalogue.count - slot, sizeof *profiles);
        if (profiles == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t index = slot; status == UMI_STATUS_OK && index < catalogue.count; ++index)
        status = ConfigurationReadProfile(server, prefix, root, index, catalogue.revision,
                                          &profiles[index - slot]);
    /* Erase recognized records before moving successors. An extension field in
     * any affected slot refuses removal and rolls back earlier deletions. */
    for (size_t index = slot; status == UMI_STATUS_OK && index < catalogue.count; ++index)
    {
        status = ConfigurationSlot(prefix, index, entry);
        if (status == UMI_STATUS_OK)
            status = ConfigurationKey(entry, "name", key);
        if (status == UMI_STATUS_OK)
            status = umi_data_server_delete(server, key);
        if (status == UMI_STATUS_OK)
            status = UmiBuildProfileStorageRemove(server, entry);
    }
    for (size_t index = slot; status == UMI_STATUS_OK && index + 1U < catalogue.count; ++index)
    {
        status = ConfigurationSlot(prefix, index, entry);
        if (status == UMI_STATUS_OK)
            status = UmiBuildProfileStorageWrite(server, entry, &profiles[index + 1U - slot],
                                                 catalogue.revision + 1U);
        if (status == UMI_STATUS_OK)
            status = ConfigurationSet(server, entry, "name", catalogue.names[index + 1U]);
    }
    if (status == UMI_STATUS_OK)
    {
        snprintf(number, sizeof number, "%zu", catalogue.count - 1U);
        status = ConfigurationSet(server, prefix, "count", number);
    }
    status =
        status == UMI_STATUS_OK
            ? ConfigurationPublishRevision(server, prefix, catalogue.revision + 1U, out_revision)
            : ConfigurationFinishRead(server, status);
    free(profiles);
    return status;
}
