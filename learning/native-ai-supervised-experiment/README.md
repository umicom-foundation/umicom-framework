# Native supervised learning — a complete bounded laboratory

**Umicom Foundation · Sammy Hegab · MIT**

This educational laboratory demonstrates the actual Framework-owned native C23
pipeline from reviewed prompt/completion JSONL records to response-only token
windows, reproducible training order, projection-level adapter SGD, independent
held-out evaluation, and a portable integrity-checked experimental receipt.

It uses **only fictional and explicitly licensed test fixtures**. The program
prints aggregate counts, cross-entropy and accuracy; it does not log prompts,
completion text, personal data or source filenames. It does not train a
production model or load third-party documents. Training bytes still require
external human review and the existing Framework source-admission gates.

## Build with CMake and Ninja

From the parent Umicom Applications directory:

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"

cmake -S ".\framework\learning\native-ai-supervised-experiment" `
  -B ".\build\windows-ucrt64-supervised-debug" -G Ninja `
  -DCMAKE_C_COMPILER="C:\msys64\ucrt64\bin\gcc.exe" `
  -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON

cmake --build ".\build\windows-ucrt64-supervised-debug" --parallel 2
ctest --test-dir ".\build\windows-ucrt64-supervised-debug" `
  -R "^framework\.ai\.supervised\." --output-on-failure --no-tests=error

& ".\build\windows-ucrt64-supervised-debug\umicom-native-supervised-training.exe"
```

CTest includes the existing Framework model, a deterministic held-out trial,
invalid-input coverage, byte-integrity checks and strict receipt parsing.
A missing test executable is **Not Run**, not a mathematical result; build
successfully before assessing the tests. Compare Debug and Release only when
both have completed independent numerical checks.

## Why the experiment is bounded

- The current educational decoder context is at most 32 tokens. Long approved
  examples are refused instead of being silently shortened.
- Frozen decoder parameters remain unchanged. Only selected low-rank attention
  and feed-forward adapter matrices are updated.
- The evaluation split is grouped by original reviewed sources before any
  byte windows are created; the run does not train on held-out windows.
- The published loss is averaged over selected response targets, not over
  ignored prompt tokens or padding. Accuracy is selected-target exact top-1
  agreement, not model comprehension or a human safety assessment.
- The native report and receipt contain **no training text**, but the in-memory
  dataset necessarily holds tokenised training content while the run is live.
- A multi-epoch training failure can leave prior successful SGD steps applied.
  The report is not published on failure, and this is **not exact resume**.
- The 208-byte SHA-256 protected receipt is **unsigned metadata**. It cannot
  restore adapter parameters or optimiser state, certify data-use rights or
  authenticate an external model publisher. Persistent adapter/optimiser
  checkpoints and signed promotion remain separate engineering tasks.

The Framework, not an application or a teaching document, remains the sole
implementation authority for tensor operations, tokenisation, source review,
training control and evidence validation.
