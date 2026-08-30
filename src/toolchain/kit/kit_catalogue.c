/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/kit/kit_catalogue.c
 * PURPOSE: Implement developer-kit catalogue and consumer bindings.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/kit_catalogue.h"

#include <stdlib.h>
#include <string.h>

struct UmiToolchainKitCatalogue {
    UmiToolchainKitSnapshot items[UMI_TOOLCHAIN_KIT_CATALOGUE_CAPACITY];
    size_t count;
    UmiToolchainKitBindingSnapshot bindings[UMI_TOOLCHAIN_KIT_BINDING_CAPACITY];
    size_t binding_count;
    uint64_t revision;
};

static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

static size_t find_kit(const UmiToolchainKitCatalogue *catalogue,
                       const char *kit_id)
{
    size_t index;
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->items[index].id, kit_id) == 0) return index;
    }
    return SIZE_MAX;
}

static size_t find_binding(const UmiToolchainKitCatalogue *catalogue,
                           UmiToolchainKitBindingKind kind,
                           const char *consumer_id)
{
    size_t index;
    for (index = 0U; index < catalogue->binding_count; ++index) {
        if (catalogue->bindings[index].kind == kind &&
            strcmp(catalogue->bindings[index].consumer_id, consumer_id) == 0)
            return index;
    }
    return SIZE_MAX;
}

UmiStatus umi_toolchain_kit_catalogue_create(
    UmiToolchainKitCatalogue **out_catalogue)
{
    UmiToolchainKitCatalogue *catalogue;
    if (out_catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_catalogue = NULL;
    catalogue = (UmiToolchainKitCatalogue *)calloc(1U, sizeof(*catalogue));
    if (catalogue == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    catalogue->revision = 1U;
    *out_catalogue = catalogue;
    return UMI_STATUS_OK;
}

void umi_toolchain_kit_catalogue_destroy(UmiToolchainKitCatalogue *catalogue)
{
    free(catalogue);
}

UmiStatus umi_toolchain_kit_catalogue_upsert(
    UmiToolchainKitCatalogue *catalogue,
    const UmiToolchainKitSnapshot *kit)
{
    size_t index;
    UmiToolchainKitSnapshot copy;
    UmiStatus status;
    if (catalogue == NULL || kit == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_toolchain_kit_validate(kit);
    if (status != UMI_STATUS_OK) return status;
    index = find_kit(catalogue, kit->id);
    if (index == SIZE_MAX) {
        if (catalogue->count >= UMI_TOOLCHAIN_KIT_CATALOGUE_CAPACITY)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        index = catalogue->count++;
    }
    copy = *kit;
    copy.struct_size = (uint32_t)sizeof(copy);
    copy.api_version = UMI_TOOLCHAIN_KIT_API_VERSION;
    copy.revision = catalogue->items[index].revision + 1U;
    if (copy.revision == 0U) copy.revision = 1U;
    catalogue->items[index] = copy;
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_toolchain_kit_catalogue_remove(
    UmiToolchainKitCatalogue *catalogue,
    const char *kit_id)
{
    size_t index;
    size_t binding_index = 0U;
    if (catalogue == NULL || kit_id == NULL || kit_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_kit(catalogue, kit_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < catalogue->count) {
        (void)memmove(&catalogue->items[index], &catalogue->items[index + 1U],
                      (catalogue->count - index - 1U) * sizeof(catalogue->items[0]));
    }
    catalogue->count -= 1U;
    (void)memset(&catalogue->items[catalogue->count], 0,
                 sizeof(catalogue->items[0]));

    while (binding_index < catalogue->binding_count) {
        if (strcmp(catalogue->bindings[binding_index].kit_id, kit_id) == 0) {
            if (binding_index + 1U < catalogue->binding_count) {
                (void)memmove(&catalogue->bindings[binding_index],
                              &catalogue->bindings[binding_index + 1U],
                              (catalogue->binding_count - binding_index - 1U) *
                                  sizeof(catalogue->bindings[0]));
            }
            catalogue->binding_count -= 1U;
            (void)memset(&catalogue->bindings[catalogue->binding_count], 0,
                         sizeof(catalogue->bindings[0]));
        } else {
            binding_index += 1U;
        }
    }
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_toolchain_kit_catalogue_find(
    const UmiToolchainKitCatalogue *catalogue,
    const char *kit_id,
    UmiToolchainKitSnapshot *out_kit)
{
    size_t index;
    if (catalogue == NULL || kit_id == NULL || out_kit == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_kit(catalogue, kit_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_kit = catalogue->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_toolchain_kit_catalogue_at(
    const UmiToolchainKitCatalogue *catalogue,
    size_t index,
    UmiToolchainKitSnapshot *out_kit)
{
    if (catalogue == NULL || out_kit == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= catalogue->count) return UMI_STATUS_NOT_FOUND;
    *out_kit = catalogue->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_toolchain_kit_catalogue_bind(
    UmiToolchainKitCatalogue *catalogue,
    UmiToolchainKitBindingKind kind,
    const char *consumer_id,
    const char *kit_id)
{
    size_t index;
    UmiToolchainKitBindingSnapshot *binding;
    UmiStatus status;
    if (catalogue == NULL || consumer_id == NULL || consumer_id[0] == '\0' ||
        kit_id == NULL || kit_id[0] == '\0' ||
        kind == UMI_TOOLCHAIN_KIT_BINDING_UNKNOWN)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (find_kit(catalogue, kit_id) == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    index = find_binding(catalogue, kind, consumer_id);
    if (index == SIZE_MAX) {
        if (catalogue->binding_count >= UMI_TOOLCHAIN_KIT_BINDING_CAPACITY)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        index = catalogue->binding_count++;
        (void)memset(&catalogue->bindings[index], 0,
                     sizeof(catalogue->bindings[index]));
        catalogue->bindings[index].struct_size =
            (uint32_t)sizeof(catalogue->bindings[index]);
        catalogue->bindings[index].api_version =
            UMI_TOOLCHAIN_KIT_CATALOGUE_API_VERSION;
        catalogue->bindings[index].kind = kind;
        status = copy_text(catalogue->bindings[index].consumer_id,
                           sizeof(catalogue->bindings[index].consumer_id),
                           consumer_id);
        if (status != UMI_STATUS_OK) {
            catalogue->binding_count -= 1U;
            return status;
        }
    }
    binding = &catalogue->bindings[index];
    status = copy_text(binding->kit_id, sizeof(binding->kit_id), kit_id);
    if (status != UMI_STATUS_OK) return status;
    binding->revision += 1U;
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_toolchain_kit_catalogue_unbind(
    UmiToolchainKitCatalogue *catalogue,
    UmiToolchainKitBindingKind kind,
    const char *consumer_id)
{
    size_t index;
    if (catalogue == NULL || consumer_id == NULL || consumer_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_binding(catalogue, kind, consumer_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < catalogue->binding_count) {
        (void)memmove(&catalogue->bindings[index],
                      &catalogue->bindings[index + 1U],
                      (catalogue->binding_count - index - 1U) *
                          sizeof(catalogue->bindings[0]));
    }
    catalogue->binding_count -= 1U;
    (void)memset(&catalogue->bindings[catalogue->binding_count], 0,
                 sizeof(catalogue->bindings[0]));
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_toolchain_kit_catalogue_resolve_binding(
    const UmiToolchainKitCatalogue *catalogue,
    UmiToolchainKitBindingKind kind,
    const char *consumer_id,
    UmiToolchainKitSnapshot *out_kit)
{
    size_t binding_index;
    if (catalogue == NULL || consumer_id == NULL || out_kit == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    binding_index = find_binding(catalogue, kind, consumer_id);
    if (binding_index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    return umi_toolchain_kit_catalogue_find(
        catalogue, catalogue->bindings[binding_index].kit_id, out_kit);
}

UmiStatus umi_toolchain_kit_catalogue_binding_at(
    const UmiToolchainKitCatalogue *catalogue,
    size_t index,
    UmiToolchainKitBindingSnapshot *out_binding)
{
    if (catalogue == NULL || out_binding == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= catalogue->binding_count) return UMI_STATUS_NOT_FOUND;
    *out_binding = catalogue->bindings[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_toolchain_kit_catalogue_snapshot(
    const UmiToolchainKitCatalogue *catalogue,
    UmiToolchainKitCatalogueSnapshot *out_snapshot)
{
    size_t index;
    if (catalogue == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_TOOLCHAIN_KIT_CATALOGUE_API_VERSION;
    out_snapshot->kit_count = catalogue->count;
    out_snapshot->binding_count = catalogue->binding_count;
    out_snapshot->revision = catalogue->revision;
    for (index = 0U; index < catalogue->count; ++index) {
        if (catalogue->items[index].state == UMI_TOOLCHAIN_KIT_READY)
            out_snapshot->ready_count += 1U;
        if (catalogue->items[index].state == UMI_TOOLCHAIN_KIT_DEGRADED)
            out_snapshot->degraded_count += 1U;
        if (umi_toolchain_kit_is_cross_compile(&catalogue->items[index]) != 0)
            out_snapshot->cross_compile_count += 1U;
    }
    return UMI_STATUS_OK;
}

size_t umi_toolchain_kit_catalogue_count(
    const UmiToolchainKitCatalogue *catalogue)
{
    return catalogue != NULL ? catalogue->count : 0U;
}
