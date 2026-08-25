#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/portfolio_position.h"

int main(void){ UmiEnterprisePortfolioPosition p; CHECK(umi_enterprise_portfolio_position_init(&p,"bond",2.0,101.0)==UMI_STATUS_OK); CHECK(p.market_value==202.0); return 0; }
