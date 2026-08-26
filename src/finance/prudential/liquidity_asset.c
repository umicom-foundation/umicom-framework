/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/liquidity_asset.c
 *
 * PURPOSE:
 *   Represent an eligible liquidity asset and calculate post-haircut value.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/liquidity_asset.h"

#include <string.h>
UmiStatus umi_pru_liquidity_asset_init(UmiPrudentialLiquidityAsset *asset,const char *asset_id,UmiHqlaLevel level,double market_value,double haircut,UmiCurrency currency) { UmiStatus s; if(asset==NULL||asset_id==NULL||market_value<0.0||haircut<0.0||haircut>1.0||level<UMI_PRU_HQLA_LEVEL1||level>UMI_PRU_HQLA_LEVEL2B)return UMI_STATUS_INVALID_ARGUMENT; memset(asset,0,sizeof *asset); s=umi_pru_copy_text(asset->asset_id,sizeof asset->asset_id,asset_id); if(s!=UMI_STATUS_OK)return s; asset->level=level; asset->market_value=market_value; asset->haircut=haircut; asset->eligible_value=market_value*(1.0-haircut); asset->currency=currency; return UMI_STATUS_OK; }
