/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/filesystem_semantics.c
 *
 * PURPOSE:
 *   Describe symlink, permission, atomic-replace and file-watch capabilities required by portable Framework storage.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/filesystem_semantics.h"

UmiCtFilesystemSemantics umi_ct_filesystem_semantics_default(UmiCtOperatingSystem os){UmiCtFilesystemSemantics s={true,true,true,true,true,true};if(os==UMI_CT_OS_WINDOWS){s.posix_permissions=false;}if(os==UMI_CT_OS_BARE_METAL){s.symlink=false;s.posix_permissions=false;s.recursive_watch=false;s.sparse_files=false;}return s;}
uint32_t umi_ct_filesystem_semantics_score(const UmiCtFilesystemSemantics*s){if(s==NULL)return 0U;return (s->symlink?1U:0U)+(s->posix_permissions?1U:0U)+(s->atomic_replace?1U:0U)+(s->recursive_watch?1U:0U)+(s->sparse_files?1U:0U)+(s->case_preserving?1U:0U);}
