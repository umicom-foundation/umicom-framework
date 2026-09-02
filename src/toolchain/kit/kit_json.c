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

/* Provide the append raw operation used by this module and its client applications. */
static UmiStatus append_raw(char *output,
                            size_t capacity,
                            size_t *position,
                            const char *format,
                            ...)
{
    int written;
    va_list arguments;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (*position >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    va_start(arguments, format);
    written = vsnprintf(output + *position, capacity - *position, format, arguments);
    va_end(arguments);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= capacity - *position)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    *position += (size_t)written;
    return UMI_STATUS_OK;
}

/*
 * Provide the append json string operation used by this module and its client
 * applications.
 */
static UmiStatus append_json_string(char *output,
                                    size_t capacity,
                                    size_t *position,
                                    const char *text)
{
    const unsigned char *cursor = (const unsigned char *)(text != NULL ? text : "");
    UmiStatus status = append_raw(output, capacity, position, "\"");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != 0U) {
        unsigned char value = *cursor++;
        /* Apply this branch only when its contract condition is satisfied. */
        if (value == '"' || value == '\\') {
            status = append_raw(output, capacity, position, "\\%c", (int)value);
        } else /* Apply this branch only when its contract condition is satisfied. */ if (value == '\n') {
            status = append_raw(output, capacity, position, "\\n");
        } else /* Apply this branch only when its contract condition is satisfied. */ if (value == '\r') {
            status = append_raw(output, capacity, position, "\\r");
        } else /* Apply this branch only when its contract condition is satisfied. */ if (value == '\t') {
            status = append_raw(output, capacity, position, "\\t");
        } else /* Apply this branch only when its contract condition is satisfied. */ if (value < 0x20U) {
            status = append_raw(output, capacity, position, "\\u%04x", (unsigned)value);
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            status = append_raw(output, capacity, position, "%c", (int)value);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return append_raw(output, capacity, position, "\"");
}

/* Provide the append property operation used by this module and its client applications. */
static UmiStatus append_property(char *output,
                                 size_t capacity,
                                 size_t *position,
                                 const char *name,
                                 const char *value,
                                 int comma)
{
    UmiStatus status;
    status = append_json_string(output, capacity, position, name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = append_raw(output, capacity, position, ":");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = append_json_string(output, capacity, position, value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return comma != 0 ? append_raw(output, capacity, position, ",") : UMI_STATUS_OK;
}

/*
 * Write toolchain kit json in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_toolchain_kit_json_write(
    const UmiToolchainKitSnapshot *kit,
    const UmiToolchainKitHealthSnapshot *health,
    const UmiToolchainKitProvenanceSnapshot *provenance,
    char *out_json,
    size_t capacity)
{
    size_t position = 0U;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (kit == NULL || out_json == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    out_json[0] = '\0';
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (health != NULL) {
        status = append_raw(out_json, capacity, &position,
                            ",\"health\":{\"state\":");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = append_json_string(out_json, capacity, &position,
                                    umi_toolchain_kit_health_state_text(health->state));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = append_raw(out_json, capacity, &position,
                            ",\"issues\":%llu,\"issueCount\":%llu,\"ready\":%s}",
                            (unsigned long long)health->issue_flags,
                            (unsigned long long)health->issue_count,
                            health->ready != 0 ? "true" : "false");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provenance != NULL) {
        status = append_raw(out_json, capacity, &position, ",\"provenance\":{");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = append_property(out_json, capacity, &position,
                                 "fingerprint", provenance->fingerprint, 1);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = append_property(out_json, capacity, &position,
                                 "compilerVersion", provenance->c_compiler_version, 1);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = append_property(out_json, capacity, &position,
                                 "sdkVersion", provenance->sdk_version, 0);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = append_raw(out_json, capacity, &position, "}");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return append_raw(out_json, capacity, &position, "}\n");
}
