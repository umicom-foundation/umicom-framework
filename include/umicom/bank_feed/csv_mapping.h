/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/bank_feed/csv_mapping.h
 * PURPOSE: Describe user-selected statement columns without guessing bank formats.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_FEED_CSV_MAPPING_H
#define UMICOM_BANK_FEED_CSV_MAPPING_H
#include "umicom/bank_feed/csv_import.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BANK_CSV_HEADER_BYTES 256U
typedef enum UmiBankCsvField {
    UMI_BANK_CSV_ACCOUNT, UMI_BANK_CSV_TRANSACTION, UMI_BANK_CSV_REFERENCE,
    UMI_BANK_CSV_INFORMATION, UMI_BANK_CSV_DATE, UMI_BANK_CSV_CURRENCY,
    UMI_BANK_CSV_AMOUNT, UMI_BANK_CSV_STATUS, UMI_BANK_CSV_MUTABILITY,
    UMI_BANK_CSV_DEBIT, UMI_BANK_CSV_CREDIT, UMI_BANK_CSV_FIELD_COUNT
} UmiBankCsvField;
typedef enum UmiBankCsvDateOrder {
    UMI_BANK_CSV_DATE_ISO, UMI_BANK_CSV_DATE_DAY_FIRST, UMI_BANK_CSV_DATE_MONTH_FIRST
} UmiBankCsvDateOrder;
typedef enum UmiBankCsvAmountMode {
    UMI_BANK_CSV_AMOUNT_SIGNED, UMI_BANK_CSV_AMOUNT_SEPARATE
} UmiBankCsvAmountMode;

/* Names are exact, case-sensitive UTF-8 header text. An empty name means that
 * field is absent, not that an empty source cell should acquire a fallback.
 * Fixed arrays let a worker own its settings without borrowing UI strings. */
typedef struct UmiBankFeedCsvMapping {
    char headers[UMI_BANK_CSV_FIELD_COUNT][UMI_BANK_CSV_HEADER_BYTES];
    char account[161], currency[4];
    UmiBankCsvDateOrder dateOrder;
    UmiBankCsvAmountMode amountMode;
    bool booked, mutableRecord;
} UmiBankFeedCsvMapping;

/** Initialise the nine normalised column names, ISO dates and signed amounts.
 * Missing status defaults to pending and missing mutability to mutable. The
 * caller must explicitly clear a header before that fallback can apply. NULL
 * is accepted. No allocation, file access or persistence occurs. */
void UmiBankFeedCsvMappingInit(UmiBankFeedCsvMapping *mapping);

/** Import a comma-separated UTF-8 statement using caller-selected headers.
 * All source headers must be nonempty, unique and shorter than 256 bytes;
 * unused columns are ignored, but every row must retain the header width.
 * A selected header must exist exactly once. A column cannot fill two roles.
 * Date is required: YYYY-MM-DD, DD/MM/YYYY or MM/DD/YYYY, selected explicitly.
 * Account and currency each require either a column or a supplied constant,
 * never both. Optional ID, reference and information may be unmapped.
 * Signed mode requires amount and forbids debit/credit mappings. Separate mode
 * requires debit and credit, forbids amount, and accepts nonnegative magnitudes.
 * One side may be empty or zero; both nonzero or both empty are rejected. Both
 * zero produces a zero credit. Group separators, rounding and trimming are not
 * supported. Missing status/mutability use the declared booleans; mapped cells
 * use BOOK/PDNG and Immutable/Mutable. Pending always remains mutable.
 * Normalised field, byte and row limits apply. Failure clears *out; issue uses
 * original one-based CSV record/column, with zero for settings or constants.
 * Settings/input are borrowed only during this call; the result owns its data.
 * This does not authenticate a bank, prove completeness, post or save data. */
UmiStatus UmiBankFeedCsvMappedCreate(const void *bytes, size_t length,
                                    const UmiBankFeedCsvMapping *mapping,
                                    UmiBankFeed **out, UmiBankFeedCsvIssue *issue);
#ifdef __cplusplus
}
#endif
#endif
