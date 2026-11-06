/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/discounting_engine.c
 *
 * PURPOSE:
 *   Price deterministic cashflows by exponential discounting.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/discounting_engine.h"
#include "umicom/finance/quant/discount_curve.h"

#include <math.h>
#include <string.h>

/*
 * Provide the quant discounting engine cashflow operation used by this module and its
 * client applications.
 */
/* The former local formula is retained for review. The active implementation
 * shares Framework's validated discount contract instead of duplicating it. */
#if 0
UmiStatus umi_quant_discounting_engine_cashflow(double amount,double zero_rate,int32_t days,double *out_pv){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_pv==NULL||days<0)return UMI_STATUS_INVALID_ARGUMENT;*out_pv=amount*exp(-zero_rate*((double)days/365.0));return UMI_STATUS_OK;}
#endif
UmiStatus umi_quant_discounting_engine_cashflow(double amount, double zero_rate,
                                               int32_t days, double *out_pv)
{
    return umi_quant_discount_curve_present_value(amount, zero_rate, days, out_pv);
}
/*
 * Provide the quant discounting engine portfolio operation used by this module and its
 * client applications.
 */
/* The earlier unguarded accumulator is retained for review. A private candidate
 * is now checked after each addition, before publishing the portfolio result. */
#if 0
UmiStatus umi_quant_discounting_engine_portfolio(const double *amounts,const double *zero_rates,const int32_t *days,size_t count,double *out_pv){size_t i;double total=0.0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(amounts==NULL||zero_rates==NULL||days==NULL||out_pv==NULL||count>4096U)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<count;++i){double pv;UmiStatus status=umi_quant_discounting_engine_cashflow(amounts[i],zero_rates[i],days[i],&pv);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(status!=UMI_STATUS_OK)return status;total+=pv;}*out_pv=total;return UMI_STATUS_OK;}
#endif
UmiStatus umi_quant_discounting_engine_portfolio(const double *amounts, const double *zero_rates,
                                                const int32_t *days, size_t count, double *out_pv)
{
    if (amounts == NULL || zero_rates == NULL || days == NULL || out_pv == NULL || count > 4096U)
        return UMI_STATUS_INVALID_ARGUMENT;
    double total = 0.0;
    for (size_t i = 0U; i < count; ++i) {
        double pv;
        UmiStatus status = umi_quant_discounting_engine_cashflow(amounts[i], zero_rates[i], days[i], &pv);
        if (status != UMI_STATUS_OK) return status;
        double candidate = total + pv;
        if (!isfinite(candidate)) return UMI_STATUS_CAPACITY_EXCEEDED;
        total = candidate;
    }
    *out_pv = total;
    return UMI_STATUS_OK;
}
