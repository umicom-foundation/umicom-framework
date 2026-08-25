/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/bundle_manifest.c
 *
 * PURPOSE:
 *   bundle manifest counts, total bytes and aggregate content fingerprint.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/bundle_manifest.h"

void umi_dr_bundle_manifest_init(UmiDrBundleManifest*m){if(m!=NULL)*m=(UmiDrBundleManifest){0};}UmiStatus umi_dr_bundle_manifest_add(UmiDrBundleManifest*m,const UmiDrBundleFile*f){if(m==NULL||f==NULL||!umi_dr_bundle_file_valid(f))return UMI_STATUS_INVALID_ARGUMENT;if(UINT64_MAX-m->total_bytes<f->size_bytes)return UMI_STATUS_CAPACITY_EXCEEDED;m->file_count++;m->total_bytes+=f->size_bytes;m->fingerprint=umi_dr_hash_combine(m->fingerprint,umi_dr_hash_text(f->digest));return UMI_STATUS_OK;}
