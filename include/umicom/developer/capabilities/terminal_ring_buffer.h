/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/terminal_ring_buffer.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Terminal Ring Buffer developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_RING_BUFFER_H
#define UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_RING_BUFFER_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_TERMINAL_RING_BUFFER "umicom.developer.performance.terminal_ring_buffer"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_terminal_ring_buffer(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_RING_BUFFER_H */
