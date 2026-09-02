/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/lockfile.c
 *
 * PURPOSE:
 *   Implement the lockfile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Reproducible compiler lockfiles | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/lockfile.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
/* Provide the hash bytes operation used by this module and its client applications. */
static uint64_t hash_bytes(uint64_t hash,const void *data,size_t count)
{ const unsigned char *bytes = (const unsigned char *)data; size_t index; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < count; ++index) { hash ^= bytes[index]; hash *= UINT64_C(1099511628211); } return hash; }
/* Provide the hash string operation used by this module and its client applications. */
static uint64_t hash_string(uint64_t hash,const char *text) { return hash_bytes(hash,text,strlen(text) + 1U); }
/*
 * Provide the compiler manifest hash operation used by this module and its client
 * applications.
 */
uint64_t umi_compiler_manifest_hash(const UmiCompilerProjectManifest *manifest)
{
    uint64_t hash = UINT64_C(1469598103934665603); size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL) return 0U;
    hash = hash_string(hash,manifest->project_id);
    hash = hash_string(hash,manifest->name);
    hash = hash_string(hash,manifest->target);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < manifest->unit_count; ++index) { const UmiCompilerUnitManifest *unit = &manifest->units[index]; hash = hash_string(hash,unit->unit_id); hash = hash_string(hash,unit->source); hash = hash_string(hash,unit->output); hash = hash_string(hash,unit->provider_id); hash = hash_bytes(hash,&unit->language,sizeof(unit->language)); hash = hash_string(hash,unit->exports_abi); hash = hash_bytes(hash,&unit->entry_point,sizeof(unit->entry_point)); }
    return hash;
}
/*
 * Initialise compiler lockfile from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_compiler_lockfile_create(const UmiCompilerProjectManifest *manifest,const UmiCompilerProfile *profile,const char *provider_version,uint64_t now_ns,UmiCompilerLockfile *out_lockfile)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL || profile == NULL || provider_version == NULL || out_lockfile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_lockfile,0,sizeof(*out_lockfile)); out_lockfile->schema_version = 1U; (void)snprintf(out_lockfile->project_id,sizeof(out_lockfile->project_id),"%s",manifest->project_id); (void)snprintf(out_lockfile->profile_id,sizeof(out_lockfile->profile_id),"%s",profile->profile_id); (void)snprintf(out_lockfile->provider_id,sizeof(out_lockfile->provider_id),"%s",profile->provider_id); (void)snprintf(out_lockfile->provider_version,sizeof(out_lockfile->provider_version),"%s",provider_version); (void)snprintf(out_lockfile->target,sizeof(out_lockfile->target),"%s",profile->target.triple); out_lockfile->manifest_hash = umi_compiler_manifest_hash(manifest); out_lockfile->generated_at_ns = now_ns; return UMI_STATUS_OK;
}
/*
 * Write compiler lockfile in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_compiler_lockfile_encode(const UmiCompilerLockfile *lockfile,char *out_text,size_t capacity)
{
    int written; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (lockfile == NULL || out_text == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(out_text,capacity,"schema=%u\nproject=%s\nprofile=%s\nprovider=%s\nprovider_version=%s\ntarget=%s\nmanifest_hash=%" PRIu64 "\ngenerated_at_ns=%" PRIu64 "\n",lockfile->schema_version,lockfile->project_id,lockfile->profile_id,lockfile->provider_id,lockfile->provider_version,lockfile->target,lockfile->manifest_hash,lockfile->generated_at_ns);
    return written < 0 || (size_t)written >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
/*
 * Read compiler lockfile into validated module state and return a status when input cannot
 * be used.
 */
UmiStatus umi_compiler_lockfile_decode(const char *text,UmiCompilerLockfile *out_lockfile)
{
    unsigned int schema = 0U; uint64_t hash = 0U,time = 0U; int matched;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_lockfile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_lockfile,0,sizeof(*out_lockfile));
    matched = sscanf(text,"schema=%u\nproject=%127[^\n]\nprofile=%127[^\n]\nprovider=%127[^\n]\nprovider_version=%63[^\n]\ntarget=%191[^\n]\nmanifest_hash=%" SCNu64 "\ngenerated_at_ns=%" SCNu64,&schema,out_lockfile->project_id,out_lockfile->profile_id,out_lockfile->provider_id,out_lockfile->provider_version,out_lockfile->target,&hash,&time);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (matched != 8 || schema != 1U) { (void)memset(out_lockfile,0,sizeof(*out_lockfile)); return UMI_STATUS_PARSE_ERROR; }
    out_lockfile->schema_version = schema; out_lockfile->manifest_hash = hash; out_lockfile->generated_at_ns = time; return UMI_STATUS_OK;
}
