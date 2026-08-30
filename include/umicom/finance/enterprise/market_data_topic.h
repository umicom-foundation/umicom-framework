/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/market_data_topic.h
 *
 * PURPOSE:
 *   Define canonical market-data topics independently from vendor feed identifiers.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_MARKET_DATA_TOPIC_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_MARKET_DATA_TOPIC_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterpriseMarketDataTopic {
    char topic_id[UMI_ENTERPRISE_ID_CAPACITY];
    UmiFinancialId instrument_id;
    char field[UMI_ENTERPRISE_NAME_CAPACITY];
} UmiEnterpriseMarketDataTopic;

/* Initialise a topic from canonical instrument and field identifiers. */
UmiStatus umi_enterprise_market_data_topic_init(UmiEnterpriseMarketDataTopic *topic, const char *topic_id, const char *instrument_id, const char *field);

#ifdef __cplusplus
}
#endif

#endif
