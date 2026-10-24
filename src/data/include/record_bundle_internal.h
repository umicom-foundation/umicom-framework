/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Private immutable bundle representation shared by codec and transaction owner. */
#ifndef UMICOM_DATA_RECORD_BUNDLE_INTERNAL_H
#define UMICOM_DATA_RECORD_BUNDLE_INTERNAL_H
#include "umicom/data/record_bundle.h"
struct UmiRecordBundle {
  size_t count;
  UmiRecordDocument records[UMI_RECORD_BUNDLE_MAX_REVISIONS];
};
UmiStatus umi_record_bundle_validate(const UmiRecordBundle *bundle);
/* Caller owns an active transaction and must roll it back on any refusal.
 * The complete entity occupancy check is shared by plain and receipted restore. */
UmiStatus umi_record_bundle_stage_in_transaction(UmiDataServer *server,
                                                 const UmiRecordBundle *bundle);
#endif
