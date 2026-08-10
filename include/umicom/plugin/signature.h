/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/signature.h
 *
 * PURPOSE:
 *   Represent provider-neutral plug-in signature evidence and deterministic
 *   verification outcomes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_SIGNATURE_H
#define UMICOM_PLUGIN_SIGNATURE_H

#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_PLUGIN_SIGNER_CAPACITY 160U
#define UMI_PLUGIN_SIGNATURE_CAPACITY 256U
typedef struct UmiPluginSignature { char signer[UMI_PLUGIN_SIGNER_CAPACITY]; char algorithm[64]; char value[UMI_PLUGIN_SIGNATURE_CAPACITY]; } UmiPluginSignature;
typedef struct UmiPluginSignatureDecision { int verified; char reason[256]; } UmiPluginSignatureDecision;
UmiStatus umi_plugin_signature_verify_checksum(const UmiPluginSignature *signature,
                                               uint64_t checksum,
                                               UmiPluginSignatureDecision *out_decision);
#ifdef __cplusplus
}
#endif

#endif
