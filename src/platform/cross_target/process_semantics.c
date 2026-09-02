/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/process_semantics.c
 *
 * PURPOSE:
 *   Describe process creation, signals and job/process-group capabilities across host and Umicom OS targets.
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

#include "umicom/platform/cross_target/process_semantics.h"

/*
 * Provide the ct process semantics default operation used by this module and its client
 * applications.
 */
UmiCtProcessSemantics umi_ct_process_semantics_default(UmiCtOperatingSystem os){UmiCtProcessSemantics s={true,true,true,true,false,true};/* Apply this branch only when its contract condition is satisfied. */ if(os==UMI_CT_OS_WINDOWS){s.fork=false;s.process_groups=false;s.job_objects=true;}else /* Apply this branch only when its contract condition is satisfied. */ if(os==UMI_CT_OS_BARE_METAL){s.spawn=false;s.fork=false;s.signals=false;s.process_groups=false;s.inherited_handles=false;}return s;}
/*
 * Provide the ct process semantics support operation used by this module and its client
 * applications.
 */
UmiCtSupportLevel umi_ct_process_semantics_support(const UmiCtProcessSemantics*s,bool f,bool g){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||!s->spawn)return UMI_CT_SUPPORT_NONE;/* Protect caller-owned memory by checking that required state is available before it is used. */ if((f&&!s->fork)||(g&&!s->process_groups))return UMI_CT_SUPPORT_DEGRADED;return UMI_CT_SUPPORT_NATIVE;}
