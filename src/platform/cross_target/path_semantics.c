/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/path_semantics.c
 *
 * PURPOSE:
 *   Describe path separators, case sensitivity and drive/root rules for portable Framework filesystem code.
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

#include "umicom/platform/cross_target/path_semantics.h"

#include <ctype.h>
UmiCtPathSemantics umi_ct_path_semantics_default(UmiCtOperatingSystem os){UmiCtPathSemantics s={'/','\\',true,false,false,4096U};if(os==UMI_CT_OS_WINDOWS){s.separator='\\';s.alternate_separator='/';s.case_sensitive=false;s.drive_letters=true;s.unc_paths=true;s.max_path_hint=32767U;}return s;}
bool umi_ct_path_is_absolute(const UmiCtPathSemantics*s,const char*p){if(s==NULL||p==NULL||p[0]=='\0')return false;if(p[0]==s->separator||p[0]==s->alternate_separator)return true;return s->drive_letters&&isalpha((unsigned char)p[0])&&p[1]==':';}
