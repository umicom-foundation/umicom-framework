/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/bundle_manifest.c
 *
 * PURPOSE:
 *   bundle manifest counts, total bytes and aggregate content fingerprint.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/bundle_manifest.h"

/*
 * Initialise dr bundle manifest from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_bundle_manifest_init(UmiDrBundleManifest*m){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m!=NULL)*m=(UmiDrBundleManifest){0};}UmiStatus umi_dr_bundle_manifest_add(UmiDrBundleManifest*m,const UmiDrBundleFile*f){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||f==NULL||!umi_dr_bundle_file_valid(f))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(UINT64_MAX-m->total_bytes<f->size_bytes)return UMI_STATUS_CAPACITY_EXCEEDED;m->file_count++;m->total_bytes+=f->size_bytes;m->fingerprint=umi_dr_hash_combine(m->fingerprint,umi_dr_hash_text(f->digest));return UMI_STATUS_OK;}
