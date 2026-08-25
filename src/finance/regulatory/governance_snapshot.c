/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/governance_snapshot.c
 *
 * PURPOSE:
 *   Aggregate governance, quality, lineage and control readiness metrics.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/governance_snapshot.h"

UmiStatus umi_reg_governance_snapshot_init(UmiGovernanceSnapshot *s,size_t domains,size_t nodes,size_t controls,double score,size_t exceptions){ if(s==NULL||!umi_reg_number_valid(score)||score<0.0||score>1.0)return UMI_STATUS_INVALID_ARGUMENT;s->domains=domains;s->lineage_nodes=nodes;s->controls=controls;s->quality_score=score;s->open_exceptions=exceptions;return UMI_STATUS_OK; }
int umi_reg_governance_snapshot_ready(const UmiGovernanceSnapshot *s,double minq){ return s!=NULL&&umi_reg_number_valid(minq)&&s->domains>0U&&s->controls>0U&&s->quality_score>=minq&&s->open_exceptions==0U?1:0; }
