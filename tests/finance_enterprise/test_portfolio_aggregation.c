#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/portfolio_aggregation.h"

int main(void){ UmiEnterprisePortfolioSnapshot s; UmiEnterprisePortfolioPosition a,b; UmiEnterprisePortfolioAggregation x; CHECK(umi_enterprise_portfolio_snapshot_init(&s,"p",1)==UMI_STATUS_OK); CHECK(umi_enterprise_portfolio_position_init(&a,"a",1.0,10.0)==UMI_STATUS_OK); CHECK(umi_enterprise_portfolio_position_init(&b,"b",-1.0,4.0)==UMI_STATUS_OK); CHECK(umi_enterprise_portfolio_snapshot_add(&s,&a)==UMI_STATUS_OK); CHECK(umi_enterprise_portfolio_snapshot_add(&s,&b)==UMI_STATUS_OK); CHECK(umi_enterprise_portfolio_aggregate(&s,&x)==UMI_STATUS_OK); CHECK(x.net_market_value==6.0&&x.gross_market_value==14.0); return 0; }
