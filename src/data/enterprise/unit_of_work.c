/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/unit_of_work.c
 *
 * PURPOSE:
 *   Coordinate identity, change tracking and commit readiness without hiding transaction execution.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/unit_of_work.h"
#include <string.h>

/* Begin makes transaction intent explicit; the backend transaction remains owned by existing Data Server services. */ UmiStatus umi_data_unit_of_work_begin(UmiDataUnitOfWork *unit,const char *unit_id){UmiStatus s;if(unit==NULL||unit_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(unit,0,sizeof(*unit));s=umi_data_enterprise_copy_text(unit->unit_id,sizeof(unit->unit_id),unit_id);if(s!=UMI_STATUS_OK)return s;umi_data_identity_map_init(&unit->identities);umi_data_change_tracker_init(&unit->changes);unit->transaction_open=true;return UMI_STATUS_OK;}
/* Validation is recorded separately so callers can review failures before rollback. */ UmiStatus umi_data_unit_of_work_validate(UmiDataUnitOfWork *unit,bool passed){if(unit==NULL||!unit->transaction_open||unit->committed)return UMI_STATUS_INVALID_STATE;unit->validation_passed=passed;return passed?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE;}
/* Commit is a state gate; execution remains delegated to the canonical transaction layer. */ UmiStatus umi_data_unit_of_work_commit(UmiDataUnitOfWork *unit){if(unit==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(!unit->transaction_open||!unit->validation_passed||unit->committed)return UMI_STATUS_INVALID_STATE;unit->committed=true;unit->transaction_open=false;return UMI_STATUS_OK;}
/* Rollback clears commit eligibility while retaining the unit identifier for audit correlation. */ void umi_data_unit_of_work_rollback(UmiDataUnitOfWork *unit){if(unit==NULL)return;unit->transaction_open=false;unit->validation_passed=false;unit->committed=false;}
