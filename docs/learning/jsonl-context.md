![Umicom](../../resources/brand/umicom-logo-on-dark.svg)

# Inspect teaching examples before choosing a context

A context is the number of token positions a model processes for one example. Umicom's small native byte decoder supports at most 32 positions. That limit is a teaching implementation limit, not the capacity of every language model.

The native pair format is BOS, prompt bytes, completion bytes, EOS. Adjacent tokens form input/target rows. Each record therefore needs prompt bytes plus completion bytes plus one prediction row. The extra row predicts EOS. Completion-only supervision selects completion bytes plus that EOS row. Padding fills the remaining positions when the example fits.

Use `UmiAiTrainingJsonlInspectContext` from `umicom/ai/training/jsonl_context.h` to inspect strict source_id/prompt/completion JSONL. Link `Umicom::ai`. The parser decodes JSON escapes and checks UTF-8 first. Counts refer to decoded UTF-8 bytes under the native byte tokenizer, not the raw JSON spelling, Unicode character count or a pretrained model's tokenizer.

1. Prepare reviewed records in the existing strict teaching schema.
2. Select a context from 1 through UMI_AI_DECODER_MAX_CONTEXT.
3. Supply the exact input byte span and separate summary/report objects.
4. Check the returned status before reading summary.
5. If parsing succeeds, review fittingRecords, overlongRecords and firstOverlongLine.
6. Decide explicitly how to handle long examples. Do not silently remove an answer or cut source groups across evaluation boundaries.

Overlong records are valid data for inspection, so they are counted instead of stopping the scan. They contribute to requiredRows and shortest/longestRequiredRows. They do not contribute to fittingValidRows, fittingSelectedRows or fittingPaddingRows. Those three counters describe only the fitting subset. The service never truncates text or creates a filtered output file.

A malformed later row, cancellation or arithmetic overflow leaves the entire summary unchanged. The report separately describes progress through accepted rows. A partial report is not a smaller valid dataset. The shared parser uses bounded temporary storage; the audit adds a fixed-size statistics object and retains no corpus or token arrays. This does not claim that file loading or parsing uses no memory.

## Run the SDK example

The existing examples/training_data project includes `umicom-sdk-training-context`. Select a current installed SDK through UMICOM_SDK_ROOT as described in that example's README. Build and run the context target. Its three synthetic records need 3, 4 and 10 rows. With context 4, it reports two fitting records, one overlong record at line 3, 17 required rows across all records, 7 valid fitting rows, 6 selected fitting rows and 1 padding row.

The underlying `UmiAiTrainingBytePairMeasure` service computes the same checked geometry without reading text. It accepts lengths that exceed the current model context so callers can inspect them. A successful measurement alone does not authorize a buffer, training operation or model launch. The existing concrete pair preparer compares this shared shape with its requested context before touching output.

## Limits

Fit does not establish data quality, source rights, sufficient GPU/RAM, useful supervision, appropriate sampling or model accuracy. The one-row context accepts no nonempty completion in this format; inspection can successfully report that every record is overlong. Larger context values may increase model tensor costs, and decoder tensor limits still apply. Use a pretrained model's actual tokenizer and chat template when preparing that model's data; this byte formula is not a substitute.

## Inspect a file through the native CLI

Save the three synthetic records from context.c as UTF-8 JSONL without a byte-order mark, one complete object per physical line. Choose an ordinary absolute file path. Use a matching Framework CLI containing the new context command:

~~~powershell
umicom training-data --help
umicom training-data context "C:\umicom-learning\training\corpus.jsonl" 4
~~~

The command reports the same counters as the in-memory example. It does not print the path, source IDs, prompts or completions. The context argument is unsigned decimal text from 1 through 32; signs, fractional values, surrounding spaces and hexadecimal spelling are refused. An inspection that finishes with overlong records still exits zero: the audit completed, but the dataset has not been approved for training. Exit 1 reports an input refusal, and exit 2 reports command usage. Use `$LASTEXITCODE` immediately after the command in PowerShell.

For C clients, `UmiAiTrainingJsonlInspectContextFile` in `jsonl_file.h` uses the same context inspector behind shared regular-file admission. It reads at most 16 MiB and clears the owned input buffer before release. The original file is unchanged. Clearing the buffer does not erase filesystem caches or all parser copies. Cancellation is checked before reading and during parsing, not inside the synchronous read; use a worker in an interactive application. The summary stays unchanged on failure, while the separate report records parser progress.

A focused check of this command implementation is separate from a rebuilt, deployed full CLI and an installed SDK. A missing subcommand or unresolved symbol means the executable or library must be updated and qualified; do not claim success merely because a new header is present.
