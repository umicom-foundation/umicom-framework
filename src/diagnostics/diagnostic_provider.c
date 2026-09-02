/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/diagnostic_provider.c
 *
 * PURPOSE:
 *   Implement provider registration, stable finding fingerprints and bounded
 *   dynamic batches for compiler, linter, language, test, security and runtime
 *   diagnostics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/diagnostic_provider.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define UMI_DIAGNOSTIC_PROVIDER_INITIAL_CAPACITY 16U
#define UMI_DIAGNOSTIC_PROVIDER_MAX_CAPACITY 65536U
#define FNV_OFFSET UINT64_C(14695981039346656037)
#define FNV_PRIME UINT64_C(1099511628211)

typedef struct ProviderEntry {
    UmiDiagnosticProviderDescriptor descriptor;
    uint64_t sequence;
} ProviderEntry;

struct UmiDiagnosticProviderRegistry {
    ProviderEntry *items;
    size_t count;
    size_t capacity;
    uint64_t sequence;
    uint64_t revision;
};

struct UmiDiagnosticProviderBatch {
    UmiDiagnosticProviderFinding *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t value)
{
    return value == UINT64_MAX ? 1U : value + 1U;
}

/* Provide the terminated operation used by this module and its client applications. */
static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

/*
 * Provide the registry find index operation used by this module and its client
 * applications.
 */
static size_t registry_find_index(const UmiDiagnosticProviderRegistry *registry,
                                  const char *provider_id)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < registry->count; ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(registry->items[position].descriptor.id, provider_id) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

/* Provide the batch find index operation used by this module and its client applications. */
static size_t batch_find_index(const UmiDiagnosticProviderBatch *batch,
                               const char *provider_id,
                               const char *diagnostic_id)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (batch == NULL || provider_id == NULL || diagnostic_id == NULL) {
        return SIZE_MAX;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < batch->count; ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(batch->items[position].provider_id, provider_id) == 0 &&
            strcmp(batch->items[position].diagnostic.id, diagnostic_id) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

/* Provide the reserve registry operation used by this module and its client applications. */
static UmiStatus reserve_registry(UmiDiagnosticProviderRegistry *registry,
                                  size_t required)
{
    size_t capacity;
    ProviderEntry *replacement;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= registry->capacity) return UMI_STATUS_OK;
    capacity = registry->capacity > 0U
        ? registry->capacity : UMI_DIAGNOSTIC_PROVIDER_INITIAL_CAPACITY;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity >= UMI_DIAGNOSTIC_PROVIDER_MAX_CAPACITY ||
            capacity > SIZE_MAX / 2U) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        capacity *= 2U;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > UMI_DIAGNOSTIC_PROVIDER_MAX_CAPACITY) {
            capacity = UMI_DIAGNOSTIC_PROVIDER_MAX_CAPACITY;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (ProviderEntry *)realloc(
        registry->items, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->items = replacement;
    registry->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the reserve batch operation used by this module and its client applications. */
static UmiStatus reserve_batch(UmiDiagnosticProviderBatch *batch,
                               size_t required)
{
    size_t capacity;
    UmiDiagnosticProviderFinding *replacement;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= batch->capacity) return UMI_STATUS_OK;
    capacity = batch->capacity > 0U
        ? batch->capacity : UMI_DIAGNOSTIC_PROVIDER_INITIAL_CAPACITY;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity >= UMI_DIAGNOSTIC_PROVIDER_MAX_CAPACITY ||
            capacity > SIZE_MAX / 2U) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        capacity *= 2U;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > UMI_DIAGNOSTIC_PROVIDER_MAX_CAPACITY) {
            capacity = UMI_DIAGNOSTIC_PROVIDER_MAX_CAPACITY;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiDiagnosticProviderFinding *)realloc(
        batch->items, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    batch->items = replacement;
    batch->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the hash bytes operation used by this module and its client applications. */
static uint64_t hash_bytes(uint64_t hash, const void *bytes, size_t size)
{
    const unsigned char *cursor = (const unsigned char *)bytes;
    size_t position;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < size; ++position) {
        hash ^= (uint64_t)cursor[position];
        hash *= FNV_PRIME;
    }
    return hash;
}

/* Provide the hash text operation used by this module and its client applications. */
static uint64_t hash_text(uint64_t hash, const char *text)
{
    static const unsigned char separator = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text != NULL) hash = hash_bytes(hash, text, strlen(text));
    return hash_bytes(hash, &separator, sizeof(separator));
}

/* Provide the hash u32 operation used by this module and its client applications. */
static uint64_t hash_u32(uint64_t hash, uint32_t value)
{
    unsigned char bytes[4];
    bytes[0] = (unsigned char)(value & 0xffU);
    bytes[1] = (unsigned char)((value >> 8U) & 0xffU);
    bytes[2] = (unsigned char)((value >> 16U) & 0xffU);
    bytes[3] = (unsigned char)((value >> 24U) & 0xffU);
    return hash_bytes(hash, bytes, sizeof(bytes));
}

/*
 * Check that diagnostic provider descriptor satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_diagnostic_provider_descriptor_validate(
    const UmiDiagnosticProviderDescriptor *descriptor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL ||
        descriptor->struct_size != (uint32_t)sizeof(*descriptor) ||
        descriptor->api_version != UMI_DIAGNOSTIC_PROVIDER_API_VERSION ||
        !terminated(descriptor->id, sizeof(descriptor->id)) ||
        descriptor->id[0] == '\0' ||
        !terminated(descriptor->label, sizeof(descriptor->label)) ||
        descriptor->label[0] == '\0' ||
        !terminated(descriptor->tool_name, sizeof(descriptor->tool_name)) ||
        !terminated(descriptor->tool_version, sizeof(descriptor->tool_version)) ||
        !terminated(descriptor->information_uri,
                    sizeof(descriptor->information_uri)) ||
        descriptor->category < UMI_DIAGNOSTIC_PROVIDER_OTHER ||
        descriptor->category > UMI_DIAGNOSTIC_PROVIDER_VALIDATOR ||
        descriptor->default_kind < UMI_DIAGNOSTIC_KIND_GENERAL ||
        descriptor->default_kind >= (UmiDiagnosticKind)UMI_DIAGNOSTIC_KIND_COUNT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Initialise diagnostic provider registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_diagnostic_provider_registry_create(
    UmiDiagnosticProviderRegistry **out_registry)
{
    UmiDiagnosticProviderRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiDiagnosticProviderRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by diagnostic provider registry so the same storage can be
 * reused safely.
 */
void umi_diagnostic_provider_registry_destroy(
    UmiDiagnosticProviderRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    free(registry->items);
    registry->items = NULL;
    free(registry);
}

/*
 * Release or reset state held by diagnostic provider registry so the same storage can be
 * reused safely.
 */
UmiStatus umi_diagnostic_provider_registry_clear(
    UmiDiagnosticProviderRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    registry->count = 0U;
    registry->revision = next_revision(registry->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the diagnostic provider registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_provider_registry_upsert(
    UmiDiagnosticProviderRegistry *registry,
    const UmiDiagnosticProviderDescriptor *descriptor)
{
    size_t position;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL ||
        umi_diagnostic_provider_descriptor_validate(descriptor) !=
            UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = registry_find_index(registry, descriptor->id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (position == SIZE_MAX) {
        status = reserve_registry(registry, registry->count + 1U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        position = registry->count++;
    }
    registry->items[position].descriptor = *descriptor;
    registry->items[position].descriptor.enabled = descriptor->enabled != 0;
    registry->items[position].descriptor.incremental =
        descriptor->incremental != 0;
    registry->items[position].descriptor.supports_fixes =
        descriptor->supports_fixes != 0;
    registry->items[position].descriptor.supports_sarif =
        descriptor->supports_sarif != 0;
    registry->sequence = next_revision(registry->sequence);
    registry->items[position].sequence = registry->sequence;
    registry->revision = next_revision(registry->revision);
    return UMI_STATUS_OK;
}

/*
 * Remove diagnostic provider registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_diagnostic_provider_registry_remove(
    UmiDiagnosticProviderRegistry *registry,
    const char *provider_id)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL || provider_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = registry_find_index(registry, provider_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position + 1U < registry->count) {
        (void)memmove(&registry->items[position],
                      &registry->items[position + 1U],
                      (registry->count - position - 1U) *
                          sizeof(*registry->items));
    }
    --registry->count;
    registry->revision = next_revision(registry->revision);
    return UMI_STATUS_OK;
}

/*
 * Find diagnostic provider registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_diagnostic_provider_registry_find(
    const UmiDiagnosticProviderRegistry *registry,
    const char *provider_id,
    UmiDiagnosticProviderDescriptor *out_descriptor)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = registry_find_index(registry, provider_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = registry->items[position].descriptor;
    return UMI_STATUS_OK;
}

/*
 * Find diagnostic provider registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_diagnostic_provider_registry_at(
    const UmiDiagnosticProviderRegistry *registry,
    size_t position,
    UmiDiagnosticProviderDescriptor *out_descriptor)
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
    *out_descriptor = registry->items[position].descriptor;
    return UMI_STATUS_OK;
}

/*
 * Provide the diagnostic provider registry snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_provider_registry_snapshot(
    const UmiDiagnosticProviderRegistry *registry,
    UmiDiagnosticProviderRegistrySnapshot *out_snapshot)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DIAGNOSTIC_PROVIDER_API_VERSION;
    out_snapshot->provider_count = registry->count;
    out_snapshot->revision = registry->revision;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < registry->count; ++position) {
        const UmiDiagnosticProviderDescriptor *descriptor =
            &registry->items[position].descriptor;
        /* Apply this operation only while the related capability or state is available. */
        if (descriptor->enabled) ++out_snapshot->enabled_count;
        /* Apply this branch only when its contract condition is satisfied. */
        if (descriptor->incremental) ++out_snapshot->incremental_count;
        /* Apply this branch only when its contract condition is satisfied. */
        if (descriptor->supports_fixes) ++out_snapshot->fix_provider_count;
        /* Apply this branch only when its contract condition is satisfied. */
        if (descriptor->supports_sarif) ++out_snapshot->sarif_provider_count;
    }
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by diagnostic provider registry without
 * changing their state.
 */
size_t umi_diagnostic_provider_registry_count(
    const UmiDiagnosticProviderRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the diagnostic provider registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_diagnostic_provider_registry_revision(
    const UmiDiagnosticProviderRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}

/*
 * Provide the diagnostic provider fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_diagnostic_provider_fingerprint(
    const UmiDiagnosticProviderFinding *finding)
{
    uint64_t hash = FNV_OFFSET;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (finding == NULL) return 0U;
    hash = hash_text(hash, finding->provider_id);
    hash = hash_text(hash, finding->diagnostic.source);
    hash = hash_text(hash, finding->diagnostic.code);
    hash = hash_text(hash, finding->diagnostic.uri);
    hash = hash_text(hash, finding->rule_name);
    hash = hash_u32(hash, (uint32_t)finding->diagnostic.kind);
    hash = hash_u32(hash, finding->diagnostic.line);
    hash = hash_u32(hash, finding->diagnostic.column);
    hash = hash_u32(hash, finding->diagnostic.end_line);
    hash = hash_u32(hash, finding->diagnostic.end_column);
    return hash != 0U ? hash : 1U;
}

/*
 * Provide the diagnostic provider content fingerprint operation used by this module and
 * its client applications.
 */
uint64_t umi_diagnostic_provider_content_fingerprint(
    const UmiDiagnosticProviderFinding *finding)
{
    uint64_t hash;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (finding == NULL) return 0U;
    hash = umi_diagnostic_provider_fingerprint(finding);
    hash = hash_text(hash, finding->diagnostic.message);
    hash = hash_text(hash, finding->diagnostic.detail);
    hash = hash_text(hash, finding->diagnostic.tags);
    hash = hash_text(hash, finding->snippet);
    hash = hash_text(hash, finding->replacement_text);
    hash = hash_u32(hash, (uint32_t)finding->diagnostic.severity);
    return hash != 0U ? hash : 1U;
}

/*
 * Initialise diagnostic provider finding from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_diagnostic_provider_finding_init(
    UmiDiagnosticProviderFinding *finding,
    const char *provider_id,
    const UmiDiagnosticSnapshot *diagnostic)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (finding == NULL || provider_id == NULL || provider_id[0] == '\0' ||
        diagnostic == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(provider_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= sizeof(finding->provider_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memset(finding, 0, sizeof(*finding));
    finding->struct_size = (uint32_t)sizeof(*finding);
    finding->api_version = UMI_DIAGNOSTIC_PROVIDER_API_VERSION;
    finding->diagnostic = *diagnostic;
    (void)memcpy(finding->provider_id, provider_id, length + 1U);
    finding->fingerprint = umi_diagnostic_provider_fingerprint(finding);
    finding->content_fingerprint =
        umi_diagnostic_provider_content_fingerprint(finding);
    return umi_diagnostic_provider_finding_validate(finding);
}

/*
 * Check that diagnostic provider finding satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_diagnostic_provider_finding_validate(
    const UmiDiagnosticProviderFinding *finding)
{
    char message[128];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (finding == NULL ||
        finding->struct_size != (uint32_t)sizeof(*finding) ||
        finding->api_version != UMI_DIAGNOSTIC_PROVIDER_API_VERSION ||
        !terminated(finding->provider_id, sizeof(finding->provider_id)) ||
        finding->provider_id[0] == '\0' ||
        !terminated(finding->rule_name, sizeof(finding->rule_name)) ||
        !terminated(finding->help_uri, sizeof(finding->help_uri)) ||
        !terminated(finding->logical_location,
                    sizeof(finding->logical_location)) ||
        !terminated(finding->snippet, sizeof(finding->snippet)) ||
        !terminated(finding->fix_description,
                    sizeof(finding->fix_description)) ||
        !terminated(finding->replacement_text,
                    sizeof(finding->replacement_text)) ||
        !terminated(finding->suppression_justification,
                    sizeof(finding->suppression_justification)) ||
        umi_diagnostic_snapshot_validate(&finding->diagnostic,
                                         message,
                                         sizeof(message)) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (finding->baseline_hint < UMI_DIAGNOSTIC_PROVIDER_BASELINE_NONE ||
        finding->baseline_hint > UMI_DIAGNOSTIC_PROVIDER_BASELINE_ABSENT ||
        finding->suppression_kind < UMI_DIAGNOSTIC_PROVIDER_SUPPRESSION_NONE ||
        finding->suppression_kind > UMI_DIAGNOSTIC_PROVIDER_SUPPRESSION_IN_SOURCE ||
        finding->suppression_status <
            UMI_DIAGNOSTIC_PROVIDER_SUPPRESSION_STATUS_NONE ||
        finding->suppression_status > UMI_DIAGNOSTIC_PROVIDER_SUPPRESSION_REJECTED ||
        (finding->suppressed &&
         finding->suppression_kind == UMI_DIAGNOSTIC_PROVIDER_SUPPRESSION_NONE)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (finding->has_fix && finding->fix_description[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (finding->has_fix &&
        (finding->replacement_end_line < finding->replacement_start_line ||
         (finding->replacement_end_line == finding->replacement_start_line &&
          finding->replacement_end_column <
              finding->replacement_start_column))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Initialise diagnostic provider batch from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_diagnostic_provider_batch_create(
    UmiDiagnosticProviderBatch **out_batch)
{
    UmiDiagnosticProviderBatch *batch;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_batch == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_batch = NULL;
    batch = (UmiDiagnosticProviderBatch *)calloc(1U, sizeof(*batch));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (batch == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    batch->revision = 1U;
    *out_batch = batch;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by diagnostic provider batch so the same storage can be
 * reused safely.
 */
void umi_diagnostic_provider_batch_destroy(UmiDiagnosticProviderBatch *batch)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (batch == NULL) return;
    free(batch->items);
    batch->items = NULL;
    free(batch);
}

/*
 * Release or reset state held by diagnostic provider batch so the same storage can be
 * reused safely.
 */
UmiStatus umi_diagnostic_provider_batch_clear(UmiDiagnosticProviderBatch *batch)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (batch == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    batch->count = 0U;
    batch->revision = next_revision(batch->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the diagnostic provider batch upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_provider_batch_upsert(
    UmiDiagnosticProviderBatch *batch,
    const UmiDiagnosticProviderFinding *finding)
{
    UmiDiagnosticProviderFinding normalized;
    size_t position;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (batch == NULL || finding == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    normalized = *finding;
    normalized.has_fix = finding->has_fix != 0;
    normalized.suppressed = finding->suppressed != 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (normalized.fingerprint == 0U) {
        normalized.fingerprint = umi_diagnostic_provider_fingerprint(&normalized);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (normalized.content_fingerprint == 0U) {
        normalized.content_fingerprint =
            umi_diagnostic_provider_content_fingerprint(&normalized);
    }
    status = umi_diagnostic_provider_finding_validate(&normalized);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    position = batch_find_index(batch, normalized.provider_id,
                                normalized.diagnostic.id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (position == SIZE_MAX) {
        status = reserve_batch(batch, batch->count + 1U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        position = batch->count++;
    }
    batch->items[position] = normalized;
    batch->revision = next_revision(batch->revision);
    return UMI_STATUS_OK;
}

/*
 * Remove diagnostic provider batch while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_diagnostic_provider_batch_remove(
    UmiDiagnosticProviderBatch *batch,
    const char *provider_id,
    const char *diagnostic_id)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (batch == NULL || provider_id == NULL || diagnostic_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = batch_find_index(batch, provider_id, diagnostic_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position + 1U < batch->count) {
        (void)memmove(&batch->items[position],
                      &batch->items[position + 1U],
                      (batch->count - position - 1U) * sizeof(*batch->items));
    }
    --batch->count;
    batch->revision = next_revision(batch->revision);
    return UMI_STATUS_OK;
}

/*
 * Find diagnostic provider batch while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_diagnostic_provider_batch_find(
    const UmiDiagnosticProviderBatch *batch,
    const char *provider_id,
    const char *diagnostic_id,
    UmiDiagnosticProviderFinding *out_finding)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (batch == NULL || provider_id == NULL || diagnostic_id == NULL ||
        out_finding == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = batch_find_index(batch, provider_id, diagnostic_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_finding = batch->items[position];
    return UMI_STATUS_OK;
}

/*
 * Find diagnostic provider batch while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_diagnostic_provider_batch_at(
    const UmiDiagnosticProviderBatch *batch,
    size_t position,
    UmiDiagnosticProviderFinding *out_finding)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (batch == NULL || out_finding == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= batch->count) return UMI_STATUS_NOT_FOUND;
    *out_finding = batch->items[position];
    return UMI_STATUS_OK;
}

/*
 * Provide the diagnostic provider batch snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_provider_batch_snapshot(
    const UmiDiagnosticProviderBatch *batch,
    UmiDiagnosticProviderBatchSnapshot *out_snapshot)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (batch == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DIAGNOSTIC_PROVIDER_API_VERSION;
    out_snapshot->finding_count = batch->count;
    out_snapshot->revision = batch->revision;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < batch->count; ++position) {
        const UmiDiagnosticProviderFinding *finding = &batch->items[position];
        size_t comparison;
        int first_provider = 1;
        /* Apply this branch only when its contract condition is satisfied. */
        if (finding->has_fix) ++out_snapshot->fixable_count;
        /* Apply this branch only when its contract condition is satisfied. */
        if ((size_t)finding->diagnostic.severity <
            UMI_DIAGNOSTIC_SEVERITY_COUNT) {
            ++out_snapshot->by_severity[finding->diagnostic.severity];
        }
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (position == 0U || finding->run_id < out_snapshot->minimum_run_id) {
            out_snapshot->minimum_run_id = finding->run_id;
        }
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (finding->run_id > out_snapshot->maximum_run_id) {
            out_snapshot->maximum_run_id = finding->run_id;
        }
        /* Visit each bounded item once so every record receives the same rule. */
        for (comparison = 0U; comparison < position; ++comparison) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(batch->items[comparison].provider_id,
                       finding->provider_id) == 0) {
                first_provider = 0;
                break;
            }
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (first_provider) ++out_snapshot->provider_count;
    }
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by diagnostic provider batch without changing
 * their state.
 */
size_t umi_diagnostic_provider_batch_count(
    const UmiDiagnosticProviderBatch *batch)
{
    return batch != NULL ? batch->count : 0U;
}

/*
 * Provide the diagnostic provider batch revision operation used by this module and its
 * client applications.
 */
uint64_t umi_diagnostic_provider_batch_revision(
    const UmiDiagnosticProviderBatch *batch)
{
    return batch != NULL ? batch->revision : 0U;
}
