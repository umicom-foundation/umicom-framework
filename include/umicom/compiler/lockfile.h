/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/lockfile.h
 *
 * PURPOSE:
 *   Publish the public lockfile contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_LOCKFILE_H
#define UMICOM_COMPILER_LOCKFILE_H
#include "umicom/compiler/manifest.h"
#include "umicom/compiler/profile.h"
/**
 * Represent the compiler lockfile data shared with callers of this public contract.
 */
typedef struct UmiCompilerLockfile { uint32_t schema_version; char project_id[UMI_COMPILER_ID_CAPACITY]; char profile_id[UMI_COMPILER_ID_CAPACITY]; char provider_id[UMI_COMPILER_ID_CAPACITY]; char provider_version[64U]; char target[UMI_COMPILER_NAME_CAPACITY]; uint64_t manifest_hash; uint64_t generated_at_ns; } UmiCompilerLockfile;
/**
 * Provide the compiler manifest hash operation used by this module and its client
 * applications.
 */
uint64_t umi_compiler_manifest_hash(const UmiCompilerProjectManifest *manifest);
/**
 * Initialise compiler lockfile from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_compiler_lockfile_create(const UmiCompilerProjectManifest *manifest,const UmiCompilerProfile *profile,const char *provider_version,uint64_t now_ns,UmiCompilerLockfile *out_lockfile);
/**
 * Write compiler lockfile in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_compiler_lockfile_encode(const UmiCompilerLockfile *lockfile,char *out_text,size_t capacity);
/**
 * Read compiler lockfile into validated module state and return a status when input cannot
 * be used.
 */
UmiStatus umi_compiler_lockfile_decode(const char *text,UmiCompilerLockfile *out_lockfile);
#endif
