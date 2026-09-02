/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/connector_health.c
 *
 * PURPOSE:
 *   Aggregate connector connect/error evidence into readiness state and error rate.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/connector_health.h"
#include <string.h>
#include <limits.h>


/*
 * Initialise fabric connector health from caller-provided values so later operations
 * receive a known state.
 */
void umi_fabric_connector_health_init(UmiFabricConnectorHealth *health){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(health!=NULL){(void)memset(health,0,sizeof(*health));health->state=UMI_FABRIC_STATE_UNKNOWN;}}
/*
 * Provide the fabric connector health record operation used by this module and its client
 * applications.
 */
void umi_fabric_connector_health_record(UmiFabricConnectorHealth *health,bool connected,bool error,bool message){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(health==NULL)return;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(connected)health->connects++;/* Use this fallback path when the earlier condition does not apply. */ else health->disconnects++;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(error)health->errors++;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(message)health->messages++;health->state=error?UMI_FABRIC_STATE_DEGRADED:(connected?UMI_FABRIC_STATE_READY:UMI_FABRIC_STATE_UNKNOWN);}
/*
 * Provide the fabric connector health error rate operation used by this module and its
 * client applications.
 */
double umi_fabric_connector_health_error_rate(const UmiFabricConnectorHealth *health){uint64_t total;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(health==NULL)return 0.0;total=health->messages+health->errors;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(total==0U)return 0.0;return ((double)health->errors*100.0)/(double)total;}
