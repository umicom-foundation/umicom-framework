
## What this update addresses

An order should not pass a notional check because its price was silently replaced with `1.0`. An execution should not change a position twice because the same report was delivered twice. A Save As dialogue should save the document that opened it, even if the active tab later changes.

This update repairs those behaviours in the existing Framework services and connects the relevant Studio and GTK entry points. There is no replacement order engine, document store or application-local validation library.

[TOC]

## 1. Source baseline

The starting source is the uploaded Applications snapshot with the Project Workflow, Build Review Corrections and Operational State updates applied in order. The current parent and Framework heads were read through the GitHub connection. The parent's Studio gitlink was also read, and selected baseline blob hashes were checked against GitHub.

| Repository | Recorded revision |
|---|---|
| Applications | `b5ff85fefe6f8ecf63fe6678190292293a0ddf87` |
| Framework | `d02989d9c1dd65f3fb2068cfc40723a81d4d2dfc` |
| Studio, as recorded by the parent | `98a24cff506ffc6c63c8cea0e251ef8833a4e7be` |
| Desk, as recorded by the parent | `273aadbcc9dda4e20d63a31e4d43dcb9410b8eeb` |

`src/trading/pretrade_risk.c` and Studio's `file_dialog.inc` matched their remote Git blob hashes after LF normalisation. This was a selected-file verification, not a separate comparison of every remote file.

## 2. Reproduced trading defects

The same seven regression cases were linked first against the original compiled libraries. Every case failed its expected corrected behaviour. The evidence archive contains the actual output and link command.

| Existing operation | Observed before correction | Corrected behaviour |
|---|---|---|
| Unpriced market risk | An order with no price was allowed using the fallback `1.0`. | Market and stop orders require a matching, sufficiently recent quote. |
| Execution store insertion | Inserting the same execution twice increased the count to two. | Duplicate execution IDs are rejected by the store. |
| Realised P&amp;L | Closing one contract 20 points higher with multiplier 20 recorded 20, while unrealised P&amp;L used the multiplier. | The realised amount is 400 before fees, consistent with the contract multiplier. |
| Position fill | A NaN quantity returned success and left a NaN position. | Invalid inputs are rejected without changing the position. |
| Fill on a cancelled order | The fill changed the terminal order to Filled. | The simple order state machine rejects an unsupported terminal-state fill. |
| Duplicate client order | A second order with the same ID was accepted and appended. | The ID collision is rejected and the returned decision is not an approval. |
| Out-of-order quote | An older quote replaced a newer quote and advanced state. | The older update is rejected before quote and alert mutation. |

Terminal-order fill corrections and late broker executions need an explicit reconciliation/correction workflow. They must not be silently discarded by a future live adapter. This batch adds rejection to the reference state machine; it does not implement that broker reconciliation workflow.

## 3. How price evidence is used

The original `umi_pretrade_risk_evaluate` function remains available. Its behaviour changes intentionally for market and stop orders: without a quote it denies the request instead of inventing a price. Limit and stop-limit orders continue to use their required limit price.

`UmiPretradeRiskEvaluateQuoted` adds explicit quote, evaluation time, price policy and copied evidence. It calls the same internal quantity, notional, projected-position and loss-limit calculation; those checks are not duplicated in the applications.

For a market buy, the reference comes from the ask; for a market sell, it comes from the bid. The quote must have valid positive prices, a non-crossed spread, positive size on the selected side, matching instrument metadata and a timestamp no later than the evaluation time. A stop order also considers its trigger price. The optional buffer is applied after selecting the quote/trigger reference.

The development default permits a quote age of 5,000 milliseconds and adds no price buffer. These are configurable development settings, not a reviewed trading policy. Quote time and evaluation time must use the same clock domain. A deterministic replay can supply its own clock; native controls use UTC milliseconds.

The copied evidence identifies the reference source, price, notional, projected position, quote time, evaluation time and quote age. It is not an approval token. Submission evaluates the request again with the current quote, limits and state.

The shared order-ticket view now includes `risk.has-valuation`, `risk.price-source`, `risk.reference-price`, `risk.notional`, `risk.projected-position`, `risk.quote-time-ms`, `risk.evaluated-at-ms` and `risk.quote-age-ms`. An invalid or absent valuation is explicitly marked rather than displayed as a meaningful zero price.

Studio's registered preview command now passes its clock. The native ticket refuses to submit an old reviewed draft after its visible quantity, side, type, time-in-force or price controls have changed. A rejected preview also notifies subscribers so an earlier approval is not left displayed.

The native simulator already seeds UTC timestamps. Its timer now advances to the actual host time, using `UmiTradingSimulationMarketAdvanceTo`, rather than assuming each delayed callback arrived exactly one interval later. The original deterministic `step` API remains. Step validation also rejects count and timestamp/sequence overflow before publishing.

## 4. Execution replay and atomic updates

`UmiExecutionStoreFind` searches the existing bounded store. In the workspace, an exact replay of an already retained report returns success without changing orders, positions, history counts or revision. Reusing that ID with different fields returns a conflict. This exact-match rule includes the report timestamp; adapters must preserve the original report identity when replaying it.

A new execution is applied to candidate order and position values first. Validation and arithmetic must succeed, and the history must have space, before the workspace commits the result. A failed close cannot leave an execution recorded with an unchanged position, or a position changed with an unchanged order.

Weighted average prices use interpolation between positive endpoints instead of multiplying a large price by a large quantity first. Closing P&amp;L uses the instrument multiplier and checks finite results before assigning the candidate. All original public functions remain.

This is owner-thread, in-memory consistency. It is not a durable cross-process transaction. Restart-safe deduplication, provider-scoped execution keys, trade corrections, cancellations, fees and reconciliation still need the Data Server-backed execution workflow.

## 5. Studio Save As and file-dialog lifetime

A Save As request now captures the original `UmiDocumentId`. Its callback uses `UmiDocumentCoordinatorSaveAs` to save that document through the existing coordinator and saver. Switching tabs does not redirect the save; closing the original document returns Not Found without writing a different document.

Save destinations are checked against the other open working copies before synchronising or writing. Saving one open document over another open document's path is rejected. A normalised equivalent of the current path still receives the external-change check, and an error reading the existing file is no longer treated as permission to overwrite it.

Two additional before/after probes used the original public API and the original coordinator implementation. Both old runs overwrote protected content: an occupied destination and a `./` path alias around an external edit. Both corrected runs rejected the save and retained the existing file contents.

Studio's open, Save As and folder chooser callbacks no longer retain a bare runtime pointer. A weak window reference and retained binding token let the callback verify that its original owner still exists. Teardown clears the binding before releasing services. A dismissed chooser is an ordinary cancellation rather than an error notification.

Path checks cover the Framework's normalised path identity. They do not identify hard-link or symbolic-link aliases, or eliminate filesystem races between checking and replacing a file. Native dialogue and shutdown interaction still require Windows/GTK qualification.

## 5a. Merge failure cleanup

The Clang build exposed an uninitialised `document` pointer in the existing three-way merge constructor. Allocation or line-splitting failures could jump to cleanup before that pointer had been assigned, and cleanup then called `free(document)`.

The pointer now starts at NULL. The existing merge test retains its successful automatic-merge and explicit-conflict cases, runs its checks in Release, and adds six invalid-input combinations: an overlong line and too many lines, each in base, left and right input. This is a one-line production initialisation correction, not a replacement merge algorithm.

## 6. Preservation and compatibility

The original public entry points remain. New risk, evidence, lookup, targeted-save and simulation-clock APIs are additive. Existing public structures were not extended; new evidence structures are separate and workspace additions are private.

Three existing test files were extended. The realised-P&amp;L expectation previously omitted the multiplier, while the risk arithmetic case explicitly asserted the `1.0` fallback. The arithmetic overflow checks remain and now use a priced order. The merge test adds invalid-input coverage and keeps checks active in Release. No test was removed to hide a failure.

Studio's previously corrected `build.c`, `build_main.c` and Output surface are not changed. The original 64 lesson records, lesson files and learning examples are not changed. Bank and Desk application sources are not changed. Trader consumes the shared Framework updates without a new application-local risk implementation.

## 7. Remaining priorities

| Priority | Next useful completion test |
|---|---|
| Native Studio | Create a project, edit two files, Save As, switch/close tabs, build, inspect errors, run, install and close with outstanding native dialogues. |
| Trader | Qualify the native preview/submit path, then add governed market-data ingestion, account scope, working-order exposure, daily P&amp;L boundaries and reconciliation. |
| Durable execution | Persist report identities and economic state through one Data Server transaction; prove replay after restart and explicit correction handling. |
| Desk | Continue installed-product readiness, session restore and user-visible failed-launch/restart checks; a launched process alone is not a healthy product. |
| Bank | Connect the existing screens to approved account, balanced-journal, payment and reconciliation services, with authorisation and idempotency. |

The source still uses its existing realised-P&amp;L aggregate as the workspace loss input. It does not implement a business-day reset, account/currency conversion or aggregate open-order reservation in this batch. Prices are assumed positive, and a quote reference is not a guaranteed execution price or a margin model. No live broker connection, live-order arming shortcut or payment execution was added.

## 8. References

The implementation and tests are the primary evidence for the changes above. The validation archive includes their command logs and baseline probes.

- [GTK FileDialog.save](https://docs.gtk.org/gtk4/method.FileDialog.save.html): asynchronous completion and caller data lifetime.
- [GTK DialogError](https://docs.gtk.org/gtk4/error.DialogError.html): dismissal and cancellation values.
- [GObject WeakRef](https://docs.gtk.org/gobject/struct.WeakRef.html): weak/strong reference conversion.
- [GLib get_real_time](https://docs.gtk.org/glib/func.get_real_time.html): UTC microseconds, converted to milliseconds by the native adapter.
- [CTest command reference](https://cmake.org/cmake/help/latest/manual/ctest.1.html): test selection and missing-test handling.
- [Git push](https://git-scm.com/docs/git-push): child-commit publication checks.
