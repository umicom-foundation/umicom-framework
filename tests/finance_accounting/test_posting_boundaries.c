/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_posting_boundaries.c
 *
 * PURPOSE:
 *   Check exact journal generation, malformed inputs and complete failure preservation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/finance/accounting/posting_engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
int main(int argc, char **argv) {
    CHECK(argc == 2); const char *mode = argv[1];
    UmiAccountingPostingEngine engine; umi_accounting_posting_engine_init(&engine);
    UmiAccountingAccountingEvent event;
    UmiAccountingPostingRule rule;
    CHECK(umi_accounting_accounting_event_init(&event, "event", "RECEIPT",
        (UmiFinancialDate){2026, 10U, 26U}, 50, "GBP") == UMI_STATUS_OK);
    CHECK(umi_accounting_posting_rule_init(&rule, "rule", "RECEIPT", "cash", "income", true) == UMI_STATUS_OK);
    UmiAccountingJournalEntry *entry = malloc(sizeof *entry), *before = malloc(sizeof *before); CHECK(entry && before);
    memset(entry, 0xA5, sizeof *entry); memcpy(before, entry, sizeof *before);
    int success = 0;
    if (!strcmp(mode, "positive")) success = 1;
    else if (!strcmp(mode, "negative")) { event.amount_minor = -50; success = 1; }
    else if (!strcmp(mode, "maximum")) { event.amount_minor = INT64_MAX; success = 1; }
    else if (!strcmp(mode, "minimum")) event.amount_minor = INT64_MIN;
    else if (!strcmp(mode, "event-id")) event.id.value[0] = '\0';
    else if (!strcmp(mode, "event-type")) event.event_type[0] = '\0';
    else if (!strcmp(mode, "event-date")) event.accounting_date.day = 0U;
    else if (!strcmp(mode, "event-currency")) strcpy(event.currency.code, "gbp");
    else if (!strcmp(mode, "event-zero")) event.amount_minor = 0;
    else if (!strcmp(mode, "event-type-unterminated")) memset(event.event_type, 'a', sizeof event.event_type);
    else if (!strcmp(mode, "event-id-unterminated")) memset(event.id.value, 'a', sizeof event.id.value);
    else if (!strcmp(mode, "event-currency-unterminated")) memset(event.currency.code, 'G', sizeof event.currency.code);
    else if (!strcmp(mode, "rule-id")) rule.id.value[0] = '\0';
    else if (!strcmp(mode, "rule-account")) rule.debit_account_id.value[0] = '\0';
    else if (!strcmp(mode, "rule-type-unterminated")) memset(rule.event_type, 'a', sizeof rule.event_type);
    else if (!strcmp(mode, "rule-account-unterminated")) memset(rule.credit_account_id.value, 'a', sizeof rule.credit_account_id.value);
    else if (!strcmp(mode, "rule-inactive")) rule.active = false;
    else if (!strcmp(mode, "rule-mismatch")) strcpy(rule.event_type, "OTHER");
    else if (!strcmp(mode, "generated-exhausted")) engine.generated_entries = SIZE_MAX;
    else if (!strcmp(mode, "rejected-exhausted")) engine.rejected_events = SIZE_MAX;
    else if (!strcmp(mode, "event-init-atomic") || !strcmp(mode, "event-init-alias")) {
        /* A byte-preservation check needs an exact representation capture. */
#if 0
        UmiAccountingAccountingEvent saved = event;
#endif
        UmiAccountingAccountingEvent saved; memcpy(&saved, &event, sizeof event);
        UmiStatus status = umi_accounting_accounting_event_init(&event, event.id.value, event.event_type,
            event.accounting_date, !strcmp(mode, "event-init-atomic") ? 0 : 75, event.currency.code);
        if (!strcmp(mode, "event-init-atomic")) CHECK(status != UMI_STATUS_OK && memcmp(&saved, &event, sizeof event) == 0);
        else CHECK(status == UMI_STATUS_OK && event.amount_minor == 75 && !strcmp(event.event_type, saved.event_type) &&
            !strcmp(event.id.value, saved.id.value) && !strcmp(event.currency.code, saved.currency.code));
        free(before); free(entry); return 0;
    } else if (!strcmp(mode, "magnitude-minimum") || !strcmp(mode, "magnitude-negative") || !strcmp(mode, "magnitude-null")) {
        int64_t amount = 71;
        event.amount_minor = !strcmp(mode, "magnitude-minimum") ? INT64_MIN : -50;
        UmiStatus status = umi_accounting_accounting_event_absolute_minor_checked(!strcmp(mode, "magnitude-null") ? NULL : &event, &amount);
        if (!strcmp(mode, "magnitude-negative")) CHECK(status == UMI_STATUS_OK && amount == 50);
        else CHECK(status != UMI_STATUS_OK && amount == 71);
        free(before); free(entry); return 0;
    } else if (strcmp(mode, "journal-id") && strcmp(mode, "null-event") && strcmp(mode, "null-rule") &&
        strcmp(mode, "null-output") && strcmp(mode, "null-engine")) return 2;
    /* Preserve the value-copy draft for review. Byte-level refusal assertions
     * capture padding with memcpy rather than relying on struct assignment. */
#if 0
    UmiAccountingPostingEngine originalEngine = engine;
    UmiAccountingAccountingEvent originalEvent = event; UmiAccountingPostingRule originalRule = rule;
#endif
    UmiAccountingPostingEngine originalEngine;
    UmiAccountingAccountingEvent originalEvent; UmiAccountingPostingRule originalRule;
    memcpy(&originalEngine, &engine, sizeof engine);
    memcpy(&originalEvent, &event, sizeof event); memcpy(&originalRule, &rule, sizeof rule);
    UmiStatus status = umi_accounting_posting_engine_generate(!strcmp(mode, "null-engine") ? NULL : &engine,
        !strcmp(mode, "null-event") ? NULL : &event, !strcmp(mode, "null-rule") ? NULL : &rule,
        !strcmp(mode, "journal-id") ? "" : "journal", !strcmp(mode, "null-output") ? NULL : entry);
    CHECK(memcmp(&originalEvent, &event, sizeof event) == 0 && memcmp(&originalRule, &rule, sizeof rule) == 0);
    if (success) {
        CHECK(status == UMI_STATUS_OK && engine.generated_entries == 1U && engine.rejected_events == 0U);
        CHECK(umi_accounting_journal_entry_balanced(entry) && entry->line_count == 2U);
        int64_t expected = !strcmp(mode, "maximum") ? INT64_MAX : 50;
        CHECK(entry->lines[0].debit_minor == expected && entry->lines[1].credit_minor == expected);
        CHECK(!strcmp(entry->lines[0].account_id.value, "cash") && !strcmp(entry->lines[1].account_id.value, "income"));
    } else {
        CHECK(status != UMI_STATUS_OK && memcmp(entry, before, sizeof *entry) == 0);
        if (!strcmp(mode, "generated-exhausted") || !strcmp(mode, "rejected-exhausted") || !strcmp(mode, "null-engine")) {
            CHECK(memcmp(&engine, &originalEngine, sizeof engine) == 0);
            if (strcmp(mode, "null-engine")) CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED);
        } else CHECK(engine.generated_entries == 0U && engine.rejected_events == 1U);
        if (!strcmp(mode, "minimum")) CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED);
        if (!strcmp(mode, "rule-mismatch")) CHECK(status == UMI_STATUS_NOT_FOUND);
    }
    free(before); free(entry); return 0;
}
