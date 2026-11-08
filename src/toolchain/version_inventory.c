/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/version_inventory.c
 *
 * PURPOSE:
 *   Serialize version-only discovery through shared Unicode, JSON and exclusive-output services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/version_inventory.h"
#include "umicom/base/text.h"
#include "umicom/base/utf8.h"
#include "umicom/platform/output_file.h"
#include "umicom/protocol/json.h"
#include "umicom/protocol/json_writer.h"
#include <stdlib.h>
#include <string.h>

/* Retain the private writer layout for review. Protocol now owns bounded JSON
 * emission for every evidence client and the language-runtime compatibility API. */
#if 0
typedef struct InventoryWriter {
    char *bytes;
    size_t capacity, length;
    UmiStatus status;
} InventoryWriter;

#endif
typedef UmiJsonWriter InventoryWriter;

/* Validate fixed ABI fields before any unbounded string helper reads them. */
static int valid_text(const char *text, size_t capacity)
{
    const char *end = memchr(text, '\0', capacity);
    return end != NULL && UmiUtf8Validate((const unsigned char *)text,
        (size_t)(end - text), NULL);
}
static UmiStatus validate(const UmiToolchainDiscoveryReport *r, size_t size, UmiStatus result)
{
    if (r == NULL || size != sizeof(*r) ||
        !umi_toolchain_profile_storage_compatible(&r->profile) ||
        result < UMI_STATUS_OK || result > UMI_STATUS_BUSY)
        return UMI_STATUS_INVALID_ARGUMENT;
    const UmiToolchainProfile *p = &r->profile;
    if (r->compile_probe_passed || r->link_probe_passed || r->runtime_probe_passed ||
        r->c23_probe_passed || p->c23_capable) return UMI_STATUS_INVALID_STATE;
    if (p->family < UMI_TOOLCHAIN_UNKNOWN || p->family > UMI_TOOLCHAIN_MSYS2_MINGW64 ||
        p->selected_c_compiler < 0 || p->selected_c_compiler > UMI_TOOL_COUNT ||
        p->selected_cpp_compiler < 0 || p->selected_cpp_compiler > UMI_TOOL_COUNT ||
        p->tool_count > UMI_TOOL_COUNT) return UMI_STATUS_INVALID_ARGUMENT;
#define CHECK_TEXT(field) if (!valid_text(p->field, sizeof(p->field))) return UMI_STATUS_INVALID_ARGUMENT
    CHECK_TEXT(profile_id); CHECK_TEXT(display_name); CHECK_TEXT(root);
    CHECK_TEXT(bin_directory); CHECK_TEXT(prefix_directory);
    CHECK_TEXT(generator); CHECK_TEXT(target_triple);
#undef CHECK_TEXT
    if (r->tools_found > p->tool_count || r->required_tools > p->tool_count + 1U ||
        r->required_tools_missing > r->required_tools) return UMI_STATUS_INVALID_ARGUMENT;
    unsigned char seen[UMI_TOOL_COUNT] = {0};
    for (size_t i = 0U; i < p->tool_count; ++i) {
        const UmiToolInfo *t = &p->tools[i];
        if (t->kind < 0 || t->kind >= UMI_TOOL_COUNT ||
            t->state < UMI_TOOL_MISSING || t->state > UMI_TOOL_FAILED ||
            seen[(size_t)t->kind] || !valid_text(t->path, sizeof(t->path)) ||
            !valid_text(t->version, sizeof(t->version))) return UMI_STATUS_INVALID_ARGUMENT;
        seen[(size_t)t->kind] = 1U;
    }
    return UMI_STATUS_OK;
}
/* These original append/escape helpers remain for review. The shared writer
 * additionally makes each append atomic and keeps argument failures sticky. */
#if 0
static void raw(InventoryWriter *w, const char *text)
{
    if (w->status != UMI_STATUS_OK) return;
    size_t n = strlen(text);
    /* Subtraction avoids wrapping when callers provide very large capacities. */
    if (w->length >= w->capacity || n >= w->capacity - w->length) {
        w->status = UMI_STATUS_CAPACITY_EXCEEDED;
        return;
    }
    memcpy(w->bytes + w->length, text, n + 1U);
    w->length += n;
}
static void text(InventoryWriter *w, const char *value)
{
    if (w->status != UMI_STATUS_OK) return;
    /* Paths are the largest emitted field. JSON controls can expand sixfold. */
    char escaped[UMI_TOOL_PATH_CAPACITY * 6U];
    w->status = umi_json_escape(value, escaped, sizeof(escaped));
    raw(w, "\""); raw(w, escaped); raw(w, "\"");
}
static void number(InventoryWriter *w, uint64_t value)
{
    if (w->status != UMI_STATUS_OK) return;
    char digits[32];
    w->status = umi_text_format(digits, sizeof(digits), "%llu", (unsigned long long)value);
    raw(w, digits);
}
#endif
static void raw(InventoryWriter *w, const char *value) { (void)UmiJsonWriterRaw(w, value); }
static void text(InventoryWriter *w, const char *value) { (void)UmiJsonWriterString(w, value); }
static void number(InventoryWriter *w, uint64_t value) { (void)UmiJsonWriterUInt64(w, value); }
static void property(InventoryWriter *w, const char *name, const char *value)
{ raw(w, ",\""); raw(w, name); raw(w, "\":"); text(w, value); }

UmiStatus umi_toolchain_version_inventory_format_sized(
    const UmiToolchainDiscoveryReport *report, size_t report_size,
    UmiStatus discovery_status, char *out_json, size_t capacity, size_t *out_size)
{
    if (out_size != NULL) *out_size = 0U;
    if (out_json == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    out_json[0] = '\0';
    UmiStatus status = validate(report, report_size, discovery_status);
    if (status != UMI_STATUS_OK) return status;
    InventoryWriter w = {out_json, capacity, 0U, UMI_STATUS_OK};
    const UmiToolchainProfile *p = &report->profile;
    raw(&w, "{\"schema\":\"umicom.tool-version-inventory\",\"version\":1,\"mode\":\"versions-only\",\"discoveryStatus\":");
    number(&w, (uint64_t)discovery_status);
    property(&w, "discoveryStatusText", umi_status_text(discovery_status));
    raw(&w, ",\"qualificationPassed\":false,\"probes\":{\"compile\":\"not-run\",\"link\":\"not-run\",\"runtime\":\"not-run\",\"c23\":\"not-run\"}");
    raw(&w, ",\"toolsFound\":"); number(&w, report->tools_found);
    raw(&w, ",\"requiredTools\":"); number(&w, report->required_tools);
    raw(&w, ",\"requiredToolsMissing\":"); number(&w, report->required_tools_missing);
    raw(&w, ",\"profile\":{\"id\":"); text(&w, p->profile_id);
    property(&w, "displayName", p->display_name);
    property(&w, "family", umi_toolchain_family_text(p->family));
    property(&w, "root", p->root); property(&w, "binDirectory", p->bin_directory);
    property(&w, "prefixDirectory", p->prefix_directory);
    property(&w, "generator", p->generator); property(&w, "targetTriple", p->target_triple);
    property(&w, "selectedCCompiler", p->selected_c_compiler == UMI_TOOL_COUNT ? "none" : umi_tool_kind_name(p->selected_c_compiler));
    property(&w, "selectedCppCompiler", p->selected_cpp_compiler == UMI_TOOL_COUNT ? "none" : umi_tool_kind_name(p->selected_cpp_compiler));
    raw(&w, ",\"discoveryComplete\":"); raw(&w, p->complete ? "true" : "false");
    raw(&w, "},\"tools\":[");
    for (size_t i = 0U; i < p->tool_count; ++i) {
        const UmiToolInfo *t = &p->tools[i];
        if (i != 0U) raw(&w, ",");
        raw(&w, "{\"kind\":"); text(&w, umi_tool_kind_name(t->kind));
        property(&w, "state", umi_tool_state_text(t->state));
        property(&w, "path", t->path); property(&w, "version", t->version);
        raw(&w, ",\"required\":"); raw(&w, t->required ? "true" : "false"); raw(&w, "}");
    }
    raw(&w, "]}\n");
    if (w.status != UMI_STATUS_OK) out_json[0] = '\0';
    else if (out_size != NULL) *out_size = w.length;
    return w.status;
}

UmiStatus umi_toolchain_version_inventory_save_sized(
    const UmiToolchainDiscoveryReport *report, size_t report_size,
    UmiStatus discovery_status, const char *path)
{
    UmiStatus status = UmiOutputFileValidatePath(path);
    if (status != UMI_STATUS_OK) return status;
    char *json = malloc(UMI_TOOLCHAIN_VERSION_INVENTORY_CAPACITY);
    if (json == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    size_t length = 0U;
    status = umi_toolchain_version_inventory_format_sized(report, report_size,
        discovery_status, json, UMI_TOOLCHAIN_VERSION_INVENTORY_CAPACITY, &length);
    UmiOutputFile *file = NULL;
    if (status == UMI_STATUS_OK) status = UmiOutputFileCreate(path, &file);
    if (status == UMI_STATUS_OK) {
        status = UmiOutputFileWrite(file, json, length);
        UmiStatus closed = UmiOutputFileClose(file);
        if (status == UMI_STATUS_OK) status = closed;
    }
    UmiOutputFileDestroy(file);
    free(json);
    return status;
}
