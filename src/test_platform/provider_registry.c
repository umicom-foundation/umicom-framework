/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/provider_registry.c
 *
 * PURPOSE:
 *   Implement deterministic test-provider registration and capability-based
 *   selection. The implementation uses bounded storage so provider discovery
 *   remains predictable in desktop, CI and embedded hosts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/provider_registry.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct UmiTestPlatformProviderRegistry {
    UmiTestPlatformProviderDescriptor entries[UMI_TEST_PLATFORM_PROVIDER_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the bounded length operation used by this module and its client applications. */
static size_t bounded_length(const char *text, size_t capacity)
{
    size_t length = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (length < capacity && text[length] != '\0') ++length;
    return length;
}

/* Check that text satisfies its contract before another service relies on it. */
static int text_is_valid(const char *text, size_t capacity, int required)
{
    const size_t length = bounded_length(text, capacity);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required && length == 0U) return 0;
    return length < capacity;
}

/*
 * Provide the ascii equal insensitive operation used by this module and its client
 * applications.
 */
static int ascii_equal_insensitive(const char *left, const char *right)
{
    size_t index = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL || right == NULL) return 0;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (left[index] != '\0' && right[index] != '\0') {
        const unsigned char left_character = (unsigned char)left[index];
        const unsigned char right_character = (unsigned char)right[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (tolower(left_character) != tolower(right_character)) return 0;
        ++index;
    }
    return left[index] == '\0' && right[index] == '\0';
}

/*
 * Provide the framework list contains operation used by this module and its client
 * applications.
 */
static int framework_list_contains(const char *frameworks, const char *framework)
{
    char token[128];
    size_t token_length = 0U;
    size_t index = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (framework == NULL || framework[0] == '\0') return 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (frameworks == NULL || frameworks[0] == '\0') return 0;

    /* Visit each bounded item once so every record receives the same rule. */
    for (;;) {
        const unsigned char character = (unsigned char)frameworks[index];
        const int separator = character == 0U || character == (unsigned char)',' ||
                              character == (unsigned char)';' ||
                              isspace(character) != 0;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!separator) {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (token_length + 1U < sizeof(token)) {
                token[token_length++] = (char)character;
            }
        } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (token_length > 0U) {
            token[token_length] = '\0';
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (ascii_equal_insensitive(token, framework) ||
                strcmp(token, "*") == 0) {
                return 1;
            }
            token_length = 0U;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (character == 0U) break;
        ++index;
    }
    return 0;
}

/*
 * Provide the validate descriptor operation used by this module and its client
 * applications.
 */
static UmiStatus validate_descriptor(
    const UmiTestPlatformProviderDescriptor *descriptor)
{
    const uint32_t known_capabilities =
        (uint32_t)UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_DISCOVERY |
        (uint32_t)UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_INCREMENTAL_DISCOVERY |
        (uint32_t)UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_EXECUTION |
        (uint32_t)UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_DEBUG |
        (uint32_t)UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_COVERAGE |
        (uint32_t)UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_BENCHMARK |
        (uint32_t)UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_ATTACHMENTS |
        (uint32_t)UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_STRUCTURED_REPORTS |
        (uint32_t)UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_CONTINUOUS_TESTING |
        (uint32_t)UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_SHARDING |
        (uint32_t)UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_RETRY;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL ||
        descriptor->struct_size != (uint32_t)sizeof(*descriptor) ||
        descriptor->api_version != UMI_TEST_PLATFORM_PROVIDER_API_VERSION ||
        !text_is_valid(descriptor->id, sizeof(descriptor->id), 1) ||
        !text_is_valid(descriptor->label, sizeof(descriptor->label), 1) ||
        !text_is_valid(descriptor->frameworks, sizeof(descriptor->frameworks), 0) ||
        !text_is_valid(descriptor->executable, sizeof(descriptor->executable), 0) ||
        !text_is_valid(descriptor->default_arguments,
                       sizeof(descriptor->default_arguments), 0) ||
        !text_is_valid(descriptor->working_directory,
                       sizeof(descriptor->working_directory), 0)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (descriptor->kind < UMI_TEST_PLATFORM_PROVIDER_UNKNOWN ||
        descriptor->kind > UMI_TEST_PLATFORM_PROVIDER_EXTERNAL ||
        (descriptor->capabilities & ~known_capabilities) != 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiTestPlatformProviderRegistry *registry,
                         const char *provider_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL) return (size_t)-1;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->entries[index].id, provider_id) == 0) return index;
    }
    return (size_t)-1;
}

/*
 * Initialise test platform provider registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_test_platform_provider_registry_create(
    UmiTestPlatformProviderRegistry **out_registry)
{
    UmiTestPlatformProviderRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiTestPlatformProviderRegistry *)calloc(1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by test platform provider registry so the same storage can
 * be reused safely.
 */
void umi_test_platform_provider_registry_destroy(
    UmiTestPlatformProviderRegistry *registry)
{
    free(registry);
}

/*
 * Provide the test platform provider registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_platform_provider_registry_upsert(
    UmiTestPlatformProviderRegistry *registry,
    const UmiTestPlatformProviderDescriptor *descriptor)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = validate_descriptor(descriptor);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    index = find_index(registry, descriptor->id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == (size_t)-1) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (registry->count >= UMI_TEST_PLATFORM_PROVIDER_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = registry->count++;
    }
    registry->entries[index] = *descriptor;
    registry->entries[index].revision = ++registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Remove test platform provider registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_test_platform_provider_registry_remove(
    UmiTestPlatformProviderRegistry *registry,
    const char *provider_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL || provider_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(registry, provider_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == (size_t)-1) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < registry->count) {
        (void)memmove(&registry->entries[index], &registry->entries[index + 1U],
                      (registry->count - index - 1U) * sizeof(registry->entries[0]));
    }
    --registry->count;
    (void)memset(&registry->entries[registry->count], 0,
                 sizeof(registry->entries[registry->count]));
    ++registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Find test platform provider registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_test_platform_provider_registry_find(
    const UmiTestPlatformProviderRegistry *registry,
    const char *provider_id,
    UmiTestPlatformProviderDescriptor *out_descriptor)
{
    const size_t index = find_index(registry, provider_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == (size_t)-1) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = registry->entries[index];
    return UMI_STATUS_OK;
}

/*
 * Find test platform provider registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_test_platform_provider_registry_at(
    const UmiTestPlatformProviderRegistry *registry,
    size_t position,
    UmiTestPlatformProviderDescriptor *out_descriptor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = registry->entries[position];
    return UMI_STATUS_OK;
}

/*
 * Initialise test platform provider query from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_platform_provider_query_init(UmiTestPlatformProviderQuery *query)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL) return;
    (void)memset(query, 0, sizeof(*query));
    query->struct_size = (uint32_t)sizeof(*query);
    query->api_version = UMI_TEST_PLATFORM_PROVIDER_API_VERSION;
    query->kind = UMI_TEST_PLATFORM_PROVIDER_UNKNOWN;
    query->require_trusted = 1;
}

/*
 * Provide the descriptor matches operation used by this module and its client
 * applications.
 */
static int descriptor_matches(const UmiTestPlatformProviderDescriptor *descriptor,
                              const UmiTestPlatformProviderQuery *query)
{
    /* Apply this operation only while the related capability or state is available. */
    if (!query->include_disabled && !descriptor->enabled) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!query->include_unavailable && !descriptor->available) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (query->require_trusted && !descriptor->trusted) return 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query->provider_id != NULL && query->provider_id[0] != '\0' &&
        strcmp(descriptor->id, query->provider_id) != 0) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (query->kind != UMI_TEST_PLATFORM_PROVIDER_UNKNOWN &&
        descriptor->kind != query->kind) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((descriptor->capabilities & query->required_capabilities) !=
        query->required_capabilities) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!framework_list_contains(descriptor->frameworks, query->framework)) return 0;
    return 1;
}

/* Provide the descriptor score operation used by this module and its client applications. */
static int32_t descriptor_score(const UmiTestPlatformProviderDescriptor *descriptor,
                                const UmiTestPlatformProviderQuery *query)
{
    uint32_t capabilities = descriptor->capabilities & query->required_capabilities;
    int32_t score = descriptor->priority * 100;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capabilities != 0U) {
        score += (int32_t)(capabilities & 1U);
        capabilities >>= 1U;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (descriptor->built_in) score += 20;
    /* Apply this branch only when its contract condition is satisfied. */
    if (descriptor->trusted) score += 10;
    /* Apply this branch only when its contract condition is satisfied. */
    if (descriptor->available) score += 5;
    return score;
}

/*
 * Provide the test platform provider registry select operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_platform_provider_registry_select(
    const UmiTestPlatformProviderRegistry *registry,
    const UmiTestPlatformProviderQuery *query,
    UmiTestPlatformProviderSelection *out_selection)
{
    size_t index;
    size_t best_index = (size_t)-1;
    int32_t best_score = 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || query == NULL || out_selection == NULL ||
        query->struct_size != (uint32_t)sizeof(*query) ||
        query->api_version != UMI_TEST_PLATFORM_PROVIDER_API_VERSION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_selection, 0, sizeof(*out_selection));
    out_selection->struct_size = (uint32_t)sizeof(*out_selection);
    out_selection->api_version = UMI_TEST_PLATFORM_PROVIDER_API_VERSION;
    out_selection->registry_revision = registry->revision;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        int32_t score;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!descriptor_matches(&registry->entries[index], query)) continue;
        score = descriptor_score(&registry->entries[index], query);
        /* Apply this branch only when its contract condition is satisfied. */
        if (best_index == (size_t)-1 || score > best_score ||
            (score == best_score &&
             strcmp(registry->entries[index].id,
                    registry->entries[best_index].id) < 0)) {
            best_index = index;
            best_score = score;
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (best_index == (size_t)-1) return UMI_STATUS_NOT_FOUND;
    out_selection->provider = registry->entries[best_index];
    out_selection->matched_capabilities =
        registry->entries[best_index].capabilities & query->required_capabilities;
    out_selection->score = best_score;
    out_selection->found = 1;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by test platform provider registry without
 * changing their state.
 */
size_t umi_test_platform_provider_registry_count(
    const UmiTestPlatformProviderRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the test platform provider registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_test_platform_provider_registry_revision(
    const UmiTestPlatformProviderRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}

/*
 * Provide the test platform provider kind text operation used by this module and its
 * client applications.
 */
const char *umi_test_platform_provider_kind_text(UmiTestPlatformProviderKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_TEST_PLATFORM_PROVIDER_CTEST: return "CTest";
        case UMI_TEST_PLATFORM_PROVIDER_NATIVE: return "Native";
        case UMI_TEST_PLATFORM_PROVIDER_PYTHON: return "Python";
        case UMI_TEST_PLATFORM_PROVIDER_JAVA: return "Java";
        case UMI_TEST_PLATFORM_PROVIDER_JAVASCRIPT: return "JavaScript";
        case UMI_TEST_PLATFORM_PROVIDER_EXTERNAL: return "External";
        case UMI_TEST_PLATFORM_PROVIDER_UNKNOWN:
        default: return "Unknown";
    }
}
