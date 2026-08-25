/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/connector_health.c
 *
 * PURPOSE:
 *   Aggregate connector connect/error evidence into readiness state and error rate.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/connector_health.h"
#include <string.h>
#include <limits.h>


void umi_fabric_connector_health_init(UmiFabricConnectorHealth *health){if(health!=NULL){(void)memset(health,0,sizeof(*health));health->state=UMI_FABRIC_STATE_UNKNOWN;}}
void umi_fabric_connector_health_record(UmiFabricConnectorHealth *health,bool connected,bool error,bool message){if(health==NULL)return;if(connected)health->connects++;else health->disconnects++;if(error)health->errors++;if(message)health->messages++;health->state=error?UMI_FABRIC_STATE_DEGRADED:(connected?UMI_FABRIC_STATE_READY:UMI_FABRIC_STATE_UNKNOWN);}
double umi_fabric_connector_health_error_rate(const UmiFabricConnectorHealth *health){uint64_t total;if(health==NULL)return 0.0;total=health->messages+health->errors;if(total==0U)return 0.0;return ((double)health->errors*100.0)/(double)total;}
