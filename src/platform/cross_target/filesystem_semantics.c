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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/filesystem_semantics.h"

/*
 * Provide the ct filesystem semantics default operation used by this module and its client
 * applications.
 */
UmiCtFilesystemSemantics umi_ct_filesystem_semantics_default(UmiCtOperatingSystem os){UmiCtFilesystemSemantics s={true,true,true,true,true,true};/* Apply this branch only when its contract condition is satisfied. */ if(os==UMI_CT_OS_WINDOWS){s.posix_permissions=false;}/* Apply this branch only when its contract condition is satisfied. */ if(os==UMI_CT_OS_BARE_METAL){s.symlink=false;s.posix_permissions=false;s.recursive_watch=false;s.sparse_files=false;}return s;}
/*
 * Provide the ct filesystem semantics score operation used by this module and its client
 * applications.
 */
uint32_t umi_ct_filesystem_semantics_score(const UmiCtFilesystemSemantics*s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return 0U;return (s->symlink?1U:0U)+(s->posix_permissions?1U:0U)+(s->atomic_replace?1U:0U)+(s->recursive_watch?1U:0U)+(s->sparse_files?1U:0U)+(s->case_preserving?1U:0U);}
