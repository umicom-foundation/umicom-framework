/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/access_plan.h
 *
 * PURPOSE:
 *   Describe a local Guest, Paper or Live entry without holding broker credentials.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_ACCESS_PLAN_H
#define UMICOM_BROKER_ACCESS_PLAN_H
#include "umicom/broker_connectivity/connection.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiBrokerAccessMode {
    UMI_BROKER_ACCESS_GUEST = 0,
    UMI_BROKER_ACCESS_PAPER = 1,
    UMI_BROKER_ACCESS_LIVE = 2
} UmiBrokerAccessMode;

/* This value describes connection intent, never broker authority. It contains
 * no username, password, authenticator code, token or account balance. */
typedef struct UmiBrokerAccessPlan {
    UmiBrokerAccessMode mode;
    UmiIbkrProgram program;
    uint16_t port;
    int clientId;
} UmiBrokerAccessPlan;

/* Each operation leaves its destination unchanged when it refuses input.
 * No function in this interface opens a socket or writes a settings file. */
UmiStatus UmiBrokerAccessPlanCreate(UmiBrokerAccessMode mode,
    UmiBrokerAccessPlan *outPlan);
UmiStatus UmiBrokerAccessPlanValidate(const UmiBrokerAccessPlan *plan);
UmiStatus UmiBrokerAccessPlanSetProgram(UmiBrokerAccessPlan *plan,
    UmiIbkrProgram program);
/* The acknowledgement is needed for reading a Live session. It does not
 * permit an order. Guest has no connection options and returns UNAVAILABLE. */
UmiStatus UmiBrokerAccessPlanConnectionOptions(const UmiBrokerAccessPlan *plan,
    bool acknowledgeLive, UmiIbkrConnectionOptions *outOptions);
const char *UmiBrokerAccessModeName(UmiBrokerAccessMode mode);
const char *UmiBrokerAccessAuthenticationGuide(UmiBrokerAccessMode mode);
#ifdef __cplusplus
}
#endif
#endif
