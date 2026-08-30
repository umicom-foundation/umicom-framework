/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/kit/kit_json.c
 * PURPOSE: Implement bounded machine-readable developer-kit output.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/kit_json.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static UmiStatus append_raw(char *output,
                            size_t capacity,
                            size_t *position,
                            const char *format,
                            ...)
{
    int written;
    va_list arguments;
    if (*position >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    va_start(arguments, format);
    written = vsnprintf(output + *position, capacity - *position, format, arguments);
    va_end(arguments);
    if (written < 0 || (size_t)written >= capacity - *position)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    *position += (size_t)written;
    return UMI_STATUS_OK;
}

static UmiStatus append_json_string(char *output,
                                    size_t capacity,
                                    size_t *position,
                                    const char *text)
{
    const unsigned char *cursor = (const unsigned char *)(text != NULL ? text : "");
    UmiStatus status = append_raw(output, capacity, position, "\"");
    if (status != UMI_STATUS_OK) return status;
    while (*cursor != 0U) {
        unsigned char value = *cursor++;
        if (value == '"' || value == '\\') {
            status = append_raw(output, capacity, position, "\\%c", (int)value);
        } else if (value == '\n') {
            status = append_raw(output, capacity, position, "\\n");
        } else if (value == '\r') {
            status = append_raw(output, capacity, position, "\\r");
        } else if (value == '\t') {
            status = append_raw(output, capacity, position, "\\t");
        } else if (value < 0x20U) {
            status = append_raw(output, capacity, position, "\\u%04x", (unsigned)value);
        } else {
            status = append_raw(output, capacity, position, "%c", (int)value);
        }
        if (status != UMI_STATUS_OK) return status;
    }
    return append_raw(output, capacity, position, "\"");
}

static UmiStatus append_property(char *output,
                                 size_t capacity,
                                 size_t *position,
                                 const char *name,
                                 const char *value,
                                 int comma)
{
    UmiStatus status;
    status = append_json_string(output, capacity, position, name);
    if (status != UMI_STATUS_OK) return status;
    status = append_raw(output, capacity, position, ":");
    if (status != UMI_STATUS_OK) return status;
    status = append_json_string(output, capacity, position, value);
    if (status != UMI_STATUS_OK) return status;
    return comma != 0 ? append_raw(output, capacity, position, ",") : UMI_STATUS_OK;
}

UmiStatus umi_toolchain_kit_json_write(
    const UmiToolchainKitSnapshot *kit,
    const UmiToolchainKitHealthSnapshot *health,
    const UmiToolchainKitProvenanceSnapshot *provenance,
    char *out_json,
    size_t capacity)
{
    size_t position = 0U;
    UmiStatus status;
    if (kit == NULL || out_json == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    out_json[0] = '\0';
    if ((status = append_raw(out_json, capacity, &position, "{")) != UMI_STATUS_OK ||
        (status = append_property(out_json, capacity, &position, "id", kit->id, 1)) != UMI_STATUS_OK ||
        (status = append_property(out_json, capacity, &position, "name", kit->display_name, 1)) != UMI_STATUS_OK ||
        (status = append_property(out_json, capacity, &position, "profile", kit->profile_id, 1)) != UMI_STATUS_OK ||
        (status = append_property(out_json, capacity, &position, "sdk", kit->sdk_id, 1)) != UMI_STATUS_OK ||
        (status = append_property(out_json, capacity, &position, "hostTriple", kit->host_triple, 1)) != UMI_STATUS_OK ||
        (status = append_property(out_json, capacity, &position, "targetTriple", kit->target_triple, 1)) != UMI_STATUS_OK ||
        (status = append_property(out_json, capacity, &position, "sysroot", kit->sysroot, 1)) != UMI_STATUS_OK ||
        (status = append_property(out_json, capacity, &position, "cCompiler", kit->c_compiler, 1)) != UMI_STATUS_OK ||
        (status = append_property(out_json, capacity, &position, "assembler", kit->assembler, 1)) != UMI_STATUS_OK ||
        (status = append_property(out_json, capacity, &position, "linker", kit->linker, 1)) != UMI_STATUS_OK ||
        (status = append_property(out_json, capacity, &position, "debugger", kit->debugger, 1)) != UMI_STATUS_OK ||
        (status = append_property(out_json, capacity, &position, "python", kit->python, 1)) != UMI_STATUS_OK ||
        (status = append_property(out_json, capacity, &position, "state", umi_toolchain_kit_state_text(kit->state), 1)) != UMI_STATUS_OK) {
        return status;
    }
    status = append_raw(out_json, capacity, &position,
                        "\"enabled\":%s,\"capabilities\":%llu,\"revision\":%llu",
                        kit->enabled != 0 ? "true" : "false",
                        (unsigned long long)kit->capabilities,
                        (unsigned long long)kit->revision);
    if (status != UMI_STATUS_OK) return status;
    if (health != NULL) {
        status = append_raw(out_json, capacity, &position,
                            ",\"health\":{\"state\":");
        if (status != UMI_STATUS_OK) return status;
        status = append_json_string(out_json, capacity, &position,
                                    umi_toolchain_kit_health_state_text(health->state));
        if (status != UMI_STATUS_OK) return status;
        status = append_raw(out_json, capacity, &position,
                            ",\"issues\":%llu,\"issueCount\":%llu,\"ready\":%s}",
                            (unsigned long long)health->issue_flags,
                            (unsigned long long)health->issue_count,
                            health->ready != 0 ? "true" : "false");
        if (status != UMI_STATUS_OK) return status;
    }
    if (provenance != NULL) {
        status = append_raw(out_json, capacity, &position, ",\"provenance\":{");
        if (status != UMI_STATUS_OK) return status;
        status = append_property(out_json, capacity, &position,
                                 "fingerprint", provenance->fingerprint, 1);
        if (status != UMI_STATUS_OK) return status;
        status = append_property(out_json, capacity, &position,
                                 "compilerVersion", provenance->c_compiler_version, 1);
        if (status != UMI_STATUS_OK) return status;
        status = append_property(out_json, capacity, &position,
                                 "sdkVersion", provenance->sdk_version, 0);
        if (status != UMI_STATUS_OK) return status;
        status = append_raw(out_json, capacity, &position, "}");
        if (status != UMI_STATUS_OK) return status;
    }
    return append_raw(out_json, capacity, &position, "}\n");
}
