# Investigate an accepted banking command

An audit event records a command accepted by the local practice banking service.
Its revision identifies its position in the accepted history. A journal records
the debit and credit lines created by a posting. An approval or reservation can
have an audit event without creating a journal.

## Follow a charge through its history

1. Open Bank's local-practice operations window and choose **Audit investigation**.
   The existing **Audit** table remains available.
2. Set **Workflow** to **charge** and enter the charge request's exact entity ID.
   Leave the other filters blank or set to **All actions**.
3. Choose **Capture audit**. Read the captured revision, filters and match count.
4. Select the submission from the captured-command dropdown. Its stored payload
   shows the account, reference, reason and exact command amount.
5. Select its approval. An approval has no booked journal: it records a workflow
   decision. Posting is a separate accepted command.
6. Select its posting. The details show the journal at that exact revision and
   both account lines, including the Framework clearing account.
7. If the charge was reversed, select that later command. Its compensating
   journal remains separate from the original posting, even if its business
   date was entered earlier than the posting date.
8. Choose **Copy captured commands CSV** for command evidence, or **Copy captured
   journal lines CSV** for the linked debit and credit lines.

The captured view is read-only. It does not submit a command, approve a payment,
reload a database or change a balance. Any financial action continues through
the existing separate review and execution workflow.

## Choose the right filters

All filters must match together. IDs match exactly and are case-sensitive: `pay`
does not match `payment`, and `Maker` does not match `maker`. Entity means the
command's entity field, not every command related to an account or customer.
For an account's monetary postings, use [account activity](BANK_ACCOUNT_ACTIVITY.md).

Different workflows can use the same entity ID. Set a workflow to distinguish,
for example, a manual hold from a transfer with the same ID. Card authorisations,
captures, voids and refunds belong to the card workflow. Selecting one action
narrows a workflow further; an incompatible action/workflow pair yields no rows.

Revision and business-date bounds are inclusive. Blank revisions, or `0`, mean
unbounded. Dates use exactly `YYYY-MM-DD`; a blank date is unbounded. A starting
bound after its ending bound is invalid. Valid filters with no matching events
produce an empty captured report, not a missing-account error.

Rows stay in accepted revision order. Business dates and timestamps are values
supplied with the command. They do not prove the time when a person performed an
action. A backdated event therefore remains after earlier accepted revisions.

## Keep captured evidence understandable

Changing filters does not change the displayed report or its copy buttons. Choose
**Capture audit** to replace it. A failed capture retains the earlier report and
shows an explanation. A successful capture clears the old row selection so it
cannot silently select a different command.

The report owns its copies. Later accepted commands, reloads and service closure
do not change it. For another window's committed changes, use the operations
window's existing reload action, then capture again. Capture alone reads the
currently loaded service and does not fetch a newer database revision.

CSV exports include the captured revision and filter context. Every export has
a summary, even with no matching rows. The commands export includes original
payload fields; the journal export contains one row per debit or credit line.
Join them by accepted revision. A posting's two sides are separate lines, so do
not add both as if they were two customer payments. Amounts are exact integer
minor units with currency and scale; the report does not total different
currencies or perform currency conversion. Shared CSV escaping protects literal
formula-like text, but spreadsheet import settings still matter.

## Use the Framework API

1. Include `umicom/bank_operations/audit_report.h`.
2. Zero-initialize `UmiBankAuditQuery`, then assign any exact IDs, workflow,
   action, revision bounds or dates you need.
3. On the service's owning thread, call `UmiBankAuditCapture`. A successful
   result is an owned `UmiBankAuditReport`; a failure returns a null report.
4. Read `UmiBankAuditReadSummary`, then `UmiBankAuditRowAt` for each captured
   index. Use `UmiBankAuditJournalAt` with that event index and a journal index.
5. Use `UmiBankAuditDescribe` for the overview and `UmiBankAuditDescribeEvent`
   for one command. Pass a null buffer and zero capacity to measure the required
   bytes, allocate that size, then format again. The size includes the final NUL.
6. Export the same report with `UmiBankAuditExportCsv` or
   `UmiBankAuditExportJournalsCsv`. Destroy each returned CSV document.
7. Destroy the report with `UmiBankAuditDestroy` when finished. The banking
   service need not remain open while you read the report.

## Scope and limits

This is a local simulation audit, not a regulated bank audit service or proof of
authentication. Actor identity and capability flags come from the trusted host.
Failed attempts are not retained as accepted events. Repeating an identical
idempotent request returns the original receipt and creates no new audit event.
This report cannot determine how many times that request was retried.

The service retains at most 256 accepted events and 256 journals. Captures copy
bounded state to heap memory. Exports use the shared 4 MiB CSV limit and fail
without publishing a partial document if invalid UTF-8 or capacity prevents
export. The original captured event remains available through copied reads.
Historical payload fields do not become current account or transfer state merely
because they appear in this report.
