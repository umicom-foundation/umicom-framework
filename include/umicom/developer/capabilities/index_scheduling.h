/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/index_scheduling.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Index Scheduling developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_INDEX_SCHEDULING_H
#define UMICOM_DEVELOPER_CAPABILITIES_INDEX_SCHEDULING_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_INDEX_SCHEDULING "umicom.developer.performance.index_scheduling"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_index_scheduling(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_INDEX_SCHEDULING_H */
