/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/kit/kit_provenance.c
 * PURPOSE: Implement deterministic, non-cryptographic kit provenance evidence.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/kit_provenance.h"

#include <stdio.h>
#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) source = "";
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

/* Provide the fnv1a update operation used by this module and its client applications. */
static uint64_t fnv1a_update(uint64_t hash, const char *text)
{
    const unsigned char *cursor = (const unsigned char *)text;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != 0U) {
        hash ^= (uint64_t)*cursor++;
        hash *= UINT64_C(1099511628211);
    }
    hash ^= UINT64_C(0xff);
    hash *= UINT64_C(1099511628211);
    return hash;
}

/*
 * Provide the toolchain kit provenance fingerprint operation used by this module and its
 * client applications.
 */
UmiStatus umi_toolchain_kit_provenance_fingerprint(
    UmiToolchainKitProvenanceSnapshot *provenance)
{
    uint64_t hash = UINT64_C(14695981039346656037);
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provenance == NULL) return UMI_STATUS_INVALID_ARGUMENT;
#define MIX(field) hash = fnv1a_update(hash, provenance->field)
    MIX(kit_id);
    MIX(profile_id);
    MIX(sdk_id);
    MIX(target_triple);
    MIX(generator);
    MIX(c_compiler);
    MIX(c_compiler_version);
    MIX(cpp_compiler);
    MIX(debugger);
    MIX(sdk_root);
    MIX(sdk_version);
    MIX(sysroot);
#undef MIX
    written = snprintf(provenance->fingerprint,
                       sizeof(provenance->fingerprint),
                       "%016llx", (unsigned long long)hash);
    return written >= 0 && (size_t)written < sizeof(provenance->fingerprint)
        ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}

/*
 * Provide the toolchain kit provenance capture operation used by this module and its
 * client applications.
 */
UmiStatus umi_toolchain_kit_provenance_capture(
    const UmiToolchainKitSnapshot *kit,
    const UmiToolchainProfile *profile,
    const UmiSdkSnapshot *sdk,
    UmiToolchainKitProvenanceSnapshot *out_provenance)
{
    const UmiToolInfo *c_compiler;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (kit == NULL || out_provenance == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_provenance, 0, sizeof(*out_provenance));
    out_provenance->struct_size = (uint32_t)sizeof(*out_provenance);
    out_provenance->api_version = UMI_TOOLCHAIN_KIT_PROVENANCE_API_VERSION;
    copy_text(out_provenance->kit_id, sizeof(out_provenance->kit_id), kit->id);
    copy_text(out_provenance->profile_id, sizeof(out_provenance->profile_id),
              kit->profile_id);
    copy_text(out_provenance->sdk_id, sizeof(out_provenance->sdk_id), kit->sdk_id);
    copy_text(out_provenance->target_triple,
              sizeof(out_provenance->target_triple), kit->target_triple);
    copy_text(out_provenance->generator, sizeof(out_provenance->generator),
              kit->cmake_generator);
    copy_text(out_provenance->c_compiler, sizeof(out_provenance->c_compiler),
              kit->c_compiler);
    copy_text(out_provenance->cpp_compiler, sizeof(out_provenance->cpp_compiler),
              kit->cpp_compiler);
    copy_text(out_provenance->debugger, sizeof(out_provenance->debugger),
              kit->debugger);
    copy_text(out_provenance->sysroot, sizeof(out_provenance->sysroot),
              kit->sysroot);
    out_provenance->revision = kit->revision;

    c_compiler = profile != NULL ? umi_toolchain_profile_c_compiler(profile) : NULL;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (c_compiler != NULL && c_compiler->version[0] != '\0')
        copy_text(out_provenance->c_compiler_version,
                  sizeof(out_provenance->c_compiler_version), c_compiler->version);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (sdk != NULL) {
        copy_text(out_provenance->sdk_root, sizeof(out_provenance->sdk_root), sdk->root);
        copy_text(out_provenance->sdk_version, sizeof(out_provenance->sdk_version),
                  sdk->version);
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (out_provenance->sdk_id[0] == '\0')
            copy_text(out_provenance->sdk_id, sizeof(out_provenance->sdk_id), sdk->id);
    }
    return umi_toolchain_kit_provenance_fingerprint(out_provenance);
}
