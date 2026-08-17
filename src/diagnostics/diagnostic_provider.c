/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/diagnostic_provider.c
 *
 * PURPOSE:
 *   Implement provider registration, stable finding fingerprints and bounded
 *   dynamic batches for compiler, linter, language, test, security and runtime
 *   diagnostics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

static uint64_t next_revision(uint64_t value)
{
    return value == UINT64_MAX ? 1U : value + 1U;
}

static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static size_t registry_find_index(const UmiDiagnosticProviderRegistry *registry,
                                  const char *provider_id)
{
    size_t position;
    if (registry == NULL || provider_id == NULL) return SIZE_MAX;
    for (position = 0U; position < registry->count; ++position) {
        if (strcmp(registry->items[position].descriptor.id, provider_id) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

static size_t batch_find_index(const UmiDiagnosticProviderBatch *batch,
                               const char *provider_id,
                               const char *diagnostic_id)
{
    size_t position;
    if (batch == NULL || provider_id == NULL || diagnostic_id == NULL) {
        return SIZE_MAX;
    }
    for (position = 0U; position < batch->count; ++position) {
        if (strcmp(batch->items[position].provider_id, provider_id) == 0 &&
            strcmp(batch->items[position].diagnostic.id, diagnostic_id) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

static UmiStatus reserve_registry(UmiDiagnosticProviderRegistry *registry,
                                  size_t required)
{
    size_t capacity;
    ProviderEntry *replacement;
    if (required <= registry->capacity) return UMI_STATUS_OK;
    capacity = registry->capacity > 0U
        ? registry->capacity : UMI_DIAGNOSTIC_PROVIDER_INITIAL_CAPACITY;
    while (capacity < required) {
        if (capacity >= UMI_DIAGNOSTIC_PROVIDER_MAX_CAPACITY ||
            capacity > SIZE_MAX / 2U) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        capacity *= 2U;
        if (capacity > UMI_DIAGNOSTIC_PROVIDER_MAX_CAPACITY) {
            capacity = UMI_DIAGNOSTIC_PROVIDER_MAX_CAPACITY;
        }
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (ProviderEntry *)realloc(
        registry->items, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->items = replacement;
    registry->capacity = capacity;
    return UMI_STATUS_OK;
}

static UmiStatus reserve_batch(UmiDiagnosticProviderBatch *batch,
                               size_t required)
{
    size_t capacity;
    UmiDiagnosticProviderFinding *replacement;
    if (required <= batch->capacity) return UMI_STATUS_OK;
    capacity = batch->capacity > 0U
        ? batch->capacity : UMI_DIAGNOSTIC_PROVIDER_INITIAL_CAPACITY;
    while (capacity < required) {
        if (capacity >= UMI_DIAGNOSTIC_PROVIDER_MAX_CAPACITY ||
            capacity > SIZE_MAX / 2U) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        capacity *= 2U;
        if (capacity > UMI_DIAGNOSTIC_PROVIDER_MAX_CAPACITY) {
            capacity = UMI_DIAGNOSTIC_PROVIDER_MAX_CAPACITY;
        }
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiDiagnosticProviderFinding *)realloc(
        batch->items, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    batch->items = replacement;
    batch->capacity = capacity;
    return UMI_STATUS_OK;
}

static uint64_t hash_bytes(uint64_t hash, const void *bytes, size_t size)
{
    const unsigned char *cursor = (const unsigned char *)bytes;
    size_t position;
    for (position = 0U; position < size; ++position) {
        hash ^= (uint64_t)cursor[position];
        hash *= FNV_PRIME;
    }
    return hash;
}

static uint64_t hash_text(uint64_t hash, const char *text)
{
    static const unsigned char separator = 0U;
    if (text != NULL) hash = hash_bytes(hash, text, strlen(text));
    return hash_bytes(hash, &separator, sizeof(separator));
}

static uint64_t hash_u32(uint64_t hash, uint32_t value)
{
    unsigned char bytes[4];
    bytes[0] = (unsigned char)(value & 0xffU);
    bytes[1] = (unsigned char)((value >> 8U) & 0xffU);
    bytes[2] = (unsigned char)((value >> 16U) & 0xffU);
    bytes[3] = (unsigned char)((value >> 24U) & 0xffU);
    return hash_bytes(hash, bytes, sizeof(bytes));
}

UmiStatus umi_diagnostic_provider_descriptor_validate(
    const UmiDiagnosticProviderDescriptor *descriptor)
{
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

UmiStatus umi_diagnostic_provider_registry_create(
    UmiDiagnosticProviderRegistry **out_registry)
{
    UmiDiagnosticProviderRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiDiagnosticProviderRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_diagnostic_provider_registry_destroy(
    UmiDiagnosticProviderRegistry *registry)
{
    if (registry == NULL) return;
    free(registry->items);
    registry->items = NULL;
    free(registry);
}

UmiStatus umi_diagnostic_provider_registry_clear(
    UmiDiagnosticProviderRegistry *registry)
{
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    registry->count = 0U;
    registry->revision = next_revision(registry->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_provider_registry_upsert(
    UmiDiagnosticProviderRegistry *registry,
    const UmiDiagnosticProviderDescriptor *descriptor)
{
    size_t position;
    UmiStatus status;
    if (registry == NULL ||
        umi_diagnostic_provider_descriptor_validate(descriptor) !=
            UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = registry_find_index(registry, descriptor->id);
    if (position == SIZE_MAX) {
        status = reserve_registry(registry, registry->count + 1U);
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

UmiStatus umi_diagnostic_provider_registry_remove(
    UmiDiagnosticProviderRegistry *registry,
    const char *provider_id)
{
    size_t position;
    if (registry == NULL || provider_id == NULL || provider_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = registry_find_index(registry, provider_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
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

UmiStatus umi_diagnostic_provider_registry_find(
    const UmiDiagnosticProviderRegistry *registry,
    const char *provider_id,
    UmiDiagnosticProviderDescriptor *out_descriptor)
{
    size_t position;
    if (registry == NULL || provider_id == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = registry_find_index(registry, provider_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = registry->items[position].descriptor;
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_provider_registry_at(
    const UmiDiagnosticProviderRegistry *registry,
    size_t position,
    UmiDiagnosticProviderDescriptor *out_descriptor)
{
    if (registry == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (position >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = registry->items[position].descriptor;
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_provider_registry_snapshot(
    const UmiDiagnosticProviderRegistry *registry,
    UmiDiagnosticProviderRegistrySnapshot *out_snapshot)
{
    size_t position;
    if (registry == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DIAGNOSTIC_PROVIDER_API_VERSION;
    out_snapshot->provider_count = registry->count;
    out_snapshot->revision = registry->revision;
    for (position = 0U; position < registry->count; ++position) {
        const UmiDiagnosticProviderDescriptor *descriptor =
            &registry->items[position].descriptor;
        if (descriptor->enabled) ++out_snapshot->enabled_count;
        if (descriptor->incremental) ++out_snapshot->incremental_count;
        if (descriptor->supports_fixes) ++out_snapshot->fix_provider_count;
        if (descriptor->supports_sarif) ++out_snapshot->sarif_provider_count;
    }
    return UMI_STATUS_OK;
}

size_t umi_diagnostic_provider_registry_count(
    const UmiDiagnosticProviderRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_diagnostic_provider_registry_revision(
    const UmiDiagnosticProviderRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}

uint64_t umi_diagnostic_provider_fingerprint(
    const UmiDiagnosticProviderFinding *finding)
{
    uint64_t hash = FNV_OFFSET;
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

uint64_t umi_diagnostic_provider_content_fingerprint(
    const UmiDiagnosticProviderFinding *finding)
{
    uint64_t hash;
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

UmiStatus umi_diagnostic_provider_finding_init(
    UmiDiagnosticProviderFinding *finding,
    const char *provider_id,
    const UmiDiagnosticSnapshot *diagnostic)
{
    size_t length;
    if (finding == NULL || provider_id == NULL || provider_id[0] == '\0' ||
        diagnostic == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(provider_id);
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

UmiStatus umi_diagnostic_provider_finding_validate(
    const UmiDiagnosticProviderFinding *finding)
{
    char message[128];
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
    if (finding->has_fix && finding->fix_description[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (finding->has_fix &&
        (finding->replacement_end_line < finding->replacement_start_line ||
         (finding->replacement_end_line == finding->replacement_start_line &&
          finding->replacement_end_column <
              finding->replacement_start_column))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_provider_batch_create(
    UmiDiagnosticProviderBatch **out_batch)
{
    UmiDiagnosticProviderBatch *batch;
    if (out_batch == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_batch = NULL;
    batch = (UmiDiagnosticProviderBatch *)calloc(1U, sizeof(*batch));
    if (batch == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    batch->revision = 1U;
    *out_batch = batch;
    return UMI_STATUS_OK;
}

void umi_diagnostic_provider_batch_destroy(UmiDiagnosticProviderBatch *batch)
{
    if (batch == NULL) return;
    free(batch->items);
    batch->items = NULL;
    free(batch);
}

UmiStatus umi_diagnostic_provider_batch_clear(UmiDiagnosticProviderBatch *batch)
{
    if (batch == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    batch->count = 0U;
    batch->revision = next_revision(batch->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_provider_batch_upsert(
    UmiDiagnosticProviderBatch *batch,
    const UmiDiagnosticProviderFinding *finding)
{
    UmiDiagnosticProviderFinding normalized;
    size_t position;
    UmiStatus status;
    if (batch == NULL || finding == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    normalized = *finding;
    normalized.has_fix = finding->has_fix != 0;
    normalized.suppressed = finding->suppressed != 0;
    if (normalized.fingerprint == 0U) {
        normalized.fingerprint = umi_diagnostic_provider_fingerprint(&normalized);
    }
    if (normalized.content_fingerprint == 0U) {
        normalized.content_fingerprint =
            umi_diagnostic_provider_content_fingerprint(&normalized);
    }
    status = umi_diagnostic_provider_finding_validate(&normalized);
    if (status != UMI_STATUS_OK) return status;
    position = batch_find_index(batch, normalized.provider_id,
                                normalized.diagnostic.id);
    if (position == SIZE_MAX) {
        status = reserve_batch(batch, batch->count + 1U);
        if (status != UMI_STATUS_OK) return status;
        position = batch->count++;
    }
    batch->items[position] = normalized;
    batch->revision = next_revision(batch->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_provider_batch_remove(
    UmiDiagnosticProviderBatch *batch,
    const char *provider_id,
    const char *diagnostic_id)
{
    size_t position;
    if (batch == NULL || provider_id == NULL || diagnostic_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = batch_find_index(batch, provider_id, diagnostic_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (position + 1U < batch->count) {
        (void)memmove(&batch->items[position],
                      &batch->items[position + 1U],
                      (batch->count - position - 1U) * sizeof(*batch->items));
    }
    --batch->count;
    batch->revision = next_revision(batch->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_provider_batch_find(
    const UmiDiagnosticProviderBatch *batch,
    const char *provider_id,
    const char *diagnostic_id,
    UmiDiagnosticProviderFinding *out_finding)
{
    size_t position;
    if (batch == NULL || provider_id == NULL || diagnostic_id == NULL ||
        out_finding == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = batch_find_index(batch, provider_id, diagnostic_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_finding = batch->items[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_provider_batch_at(
    const UmiDiagnosticProviderBatch *batch,
    size_t position,
    UmiDiagnosticProviderFinding *out_finding)
{
    if (batch == NULL || out_finding == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (position >= batch->count) return UMI_STATUS_NOT_FOUND;
    *out_finding = batch->items[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_provider_batch_snapshot(
    const UmiDiagnosticProviderBatch *batch,
    UmiDiagnosticProviderBatchSnapshot *out_snapshot)
{
    size_t position;
    if (batch == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DIAGNOSTIC_PROVIDER_API_VERSION;
    out_snapshot->finding_count = batch->count;
    out_snapshot->revision = batch->revision;
    for (position = 0U; position < batch->count; ++position) {
        const UmiDiagnosticProviderFinding *finding = &batch->items[position];
        size_t comparison;
        int first_provider = 1;
        if (finding->has_fix) ++out_snapshot->fixable_count;
        if ((size_t)finding->diagnostic.severity <
            UMI_DIAGNOSTIC_SEVERITY_COUNT) {
            ++out_snapshot->by_severity[finding->diagnostic.severity];
        }
        if (position == 0U || finding->run_id < out_snapshot->minimum_run_id) {
            out_snapshot->minimum_run_id = finding->run_id;
        }
        if (finding->run_id > out_snapshot->maximum_run_id) {
            out_snapshot->maximum_run_id = finding->run_id;
        }
        for (comparison = 0U; comparison < position; ++comparison) {
            if (strcmp(batch->items[comparison].provider_id,
                       finding->provider_id) == 0) {
                first_provider = 0;
                break;
            }
        }
        if (first_provider) ++out_snapshot->provider_count;
    }
    return UMI_STATUS_OK;
}

size_t umi_diagnostic_provider_batch_count(
    const UmiDiagnosticProviderBatch *batch)
{
    return batch != NULL ? batch->count : 0U;
}

uint64_t umi_diagnostic_provider_batch_revision(
    const UmiDiagnosticProviderBatch *batch)
{
    return batch != NULL ? batch->revision : 0U;
}
