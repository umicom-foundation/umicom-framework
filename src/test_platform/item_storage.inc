/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/item_storage.inc
 * PURPOSE:
 *   Grow copied test records transactionally and index their stable identifiers.
 *   This private implementation is compiled only through item.c.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static uint64_t UmiItemHash(const char *text)
{
    uint64_t value = UINT64_C(14695981039346656037);
    while (*text != '\0') {
        value ^= (unsigned char)*text++;
        value *= UINT64_C(1099511628211);
    }
    return value;
}

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiTestPlatformItemRegistry *registry, const char *id)
{
    size_t bucket, visited;
    if (registry == NULL || id == NULL || registry->index_capacity == 0U) return SIZE_MAX;
    bucket = (size_t)(UmiItemHash(id) & (uint64_t)(registry->index_capacity - 1U));
    for (visited = 0U; visited < registry->index_capacity; ++visited) {
        size_t slot = registry->index[bucket];
        if (slot == 0U) return SIZE_MAX;
        if (strcmp(registry->items[slot - 1U].id, id) == 0) return slot - 1U;
        bucket = (bucket + 1U) & (registry->index_capacity - 1U);
    }
    return SIZE_MAX;
}

static void UmiItemIndexInsert(UmiTestPlatformItemRegistry *registry, size_t index)
{
    size_t bucket = (size_t)(UmiItemHash(registry->items[index].id) &
        (uint64_t)(registry->index_capacity - 1U));
    while (registry->index[bucket] != 0U && registry->index[bucket] != index + 1U)
        bucket = (bucket + 1U) & (registry->index_capacity - 1U);
    registry->index[bucket] = index + 1U;
}

static void UmiItemReindex(UmiTestPlatformItemRegistry *registry)
{
    size_t index;
    if (registry->index == NULL) return;
    (void)memset(registry->index, 0, registry->index_capacity * sizeof(*registry->index));
    for (index = 0U; index < registry->count; ++index) UmiItemIndexInsert(registry, index);
}

static UmiStatus UmiItemValidate(const UmiTestPlatformItemSnapshot *item)
{
#define UMI_ITEM_CHECK_TEXT(field) \
    if (memchr(item->field, '\0', sizeof(item->field)) == NULL) return UMI_STATUS_CAPACITY_EXCEEDED
    UMI_ITEM_CHECK_TEXT(id); UMI_ITEM_CHECK_TEXT(parent_id);
    UMI_ITEM_CHECK_TEXT(suite_id); UMI_ITEM_CHECK_TEXT(name);
    UMI_ITEM_CHECK_TEXT(uri); UMI_ITEM_CHECK_TEXT(source_uri);
    UMI_ITEM_CHECK_TEXT(framework); UMI_ITEM_CHECK_TEXT(kind);
    UMI_ITEM_CHECK_TEXT(labels); UMI_ITEM_CHECK_TEXT(command);
    UMI_ITEM_CHECK_TEXT(working_directory);
#undef UMI_ITEM_CHECK_TEXT
    return UMI_STATUS_OK;
}

static UmiStatus UmiItemReserve(UmiTestPlatformItemRegistry *registry, size_t required)
{
    size_t capacity, indexCapacity;
    UmiTestPlatformItemSnapshot *items;
    size_t *index;
    if (required <= registry->capacity) return UMI_STATUS_OK;
    if (required > UMI_TEST_PLATFORM_ITEM_MAX_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    capacity = registry->capacity != 0U ? registry->capacity : 32U;
    while (capacity < required) capacity *= 2U;
    indexCapacity = capacity * 2U;
    if (capacity > SIZE_MAX / sizeof(*items) || indexCapacity > SIZE_MAX / sizeof(*index))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Both allocations must succeed before the old ownership is released. */
    items = (UmiTestPlatformItemSnapshot *)malloc(capacity * sizeof(*items));
    if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    index = (size_t *)calloc(indexCapacity, sizeof(*index));
    if (index == NULL) { free(items); return UMI_STATUS_OUT_OF_MEMORY; }
    if (registry->count != 0U)
        (void)memcpy(items, registry->items, registry->count * sizeof(*items));
    free(registry->items); free(registry->index);
    registry->items = items; registry->capacity = capacity;
    registry->index = index; registry->index_capacity = indexCapacity;
    UmiItemReindex(registry);
    return UMI_STATUS_OK;
}
