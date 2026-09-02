/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/schema_compatibility.h
 *
 * PURPOSE:
 *   Classify producer/consumer schema compatibility using semantic version and declared compatibility evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_SCHEMA_COMPATIBILITY_H
#define UMICOM_INTEGRATION_FABRIC_SCHEMA_COMPATIBILITY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/schema_descriptor.h"
#ifdef __cplusplus
extern "C" {
#endif


/**
 * List the named fabric schema compatibility values accepted by this public contract.
 */
typedef enum UmiFabricSchemaCompatibility { UMI_FABRIC_SCHEMA_INCOMPATIBLE=0,UMI_FABRIC_SCHEMA_EXACT=1,UMI_FABRIC_SCHEMA_BACKWARD=2,UMI_FABRIC_SCHEMA_FORWARD=3 } UmiFabricSchemaCompatibility;
/**
 * Provide the fabric schema compatibility classify operation used by this module and its
 * client applications.
 */
UmiFabricSchemaCompatibility umi_fabric_schema_compatibility_classify(const UmiFabricSchemaDescriptor *producer,const UmiFabricSchemaDescriptor *consumer);

#ifdef __cplusplus
}
#endif
#endif
