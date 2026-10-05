/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/risk_limit.c
 *
 * PURPOSE:
 *   Validate risk-limit configuration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of risk limit. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/risk_limit.h"
#include "../base/value_archive_internal.h"
#include <math.h>
/* Check that risk limit satisfies its contract before another service relies on it. */
/* Infinity is not a supported representation of an unlimited risk limit. */
int umi_risk_limit_valid(const UmiRiskLimit *l){return l!=NULL&&isfinite(l->max_order_quantity)&&isfinite(l->max_order_notional)&&isfinite(l->max_position_quantity)&&isfinite(l->max_daily_loss)&&l->max_order_quantity>0.0&&l->max_order_notional>0.0&&l->max_position_quantity>0.0&&l->max_daily_loss>=0.0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRiskLimitArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x3d78b49ca5a8fcaf);

    return schema;
}
static size_t UmiRiskLimitArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRiskLimitArchiveWrite(UmiArchiveWriter *writer, const UmiRiskLimit *value)
{
    UmiArchiveWriteDouble(writer, value->max_order_quantity);
    UmiArchiveWriteDouble(writer, value->max_order_notional);
    UmiArchiveWriteDouble(writer, value->max_position_quantity);
    UmiArchiveWriteDouble(writer, value->max_daily_loss);
}
static void UmiRiskLimitArchiveRead(UmiArchiveReader *reader, UmiRiskLimit *value)
{
    value->max_order_quantity = UmiArchiveReadDouble(reader);
    value->max_order_notional = UmiArchiveReadDouble(reader);
    value->max_position_quantity = UmiArchiveReadDouble(reader);
    value->max_daily_loss = UmiArchiveReadDouble(reader);
}
static UmiStatus UmiRiskLimitArchiveValidate(const UmiRiskLimit *value)
{
    return umi_risk_limit_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_risk_limit_archive_encode, umi_risk_limit_archive_decode,
    UmiRiskLimit, UmiRiskLimitArchiveSchema, UmiRiskLimitArchiveBound, UmiRiskLimitArchiveWrite, UmiRiskLimitArchiveRead, UmiRiskLimitArchiveValidate)
