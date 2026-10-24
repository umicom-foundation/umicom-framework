# Umicom Native LLM — Rotary Position and Rolling KV Research

**Development Batch 81–82**  
**Author:** Sammy Hegab — Umicom Foundation  
**Language:** Native C23  
**Status:** Opt-in educational numerical kernel. **Not a trained decoder or compatible checkpoint implementation.**

## 1. Why this module exists

The currently qualified Umicom decoder stores learned absolute position
embeddings. When a full context window rolls, a retained token is assigned a
*different* position in the newly framed input. Both its input representation
and its K/V projections can change. Reusing its previous K/V rows in the
historical decoder would therefore change model behaviour.

The position-contract and position-admission services introduced in Batches
77–80 explicitly bind the **existing** decoder to learned absolute positions.
Their allowed enum values, 128-byte sidecar, checkpoint codec and generation
policies are not changed by this batch. **An old checkpoint remains incompatible
with rotary mathematical semantics.** No automatic checkpoint conversion or
silent fallback is allowed.

This batch instead implements separately named, bounded numerical primitives
for researching a **future separately trained or explicitly qualified** model
family. These primitives do not modify the current decoder, pretrained weights,
checkpoint format, token sampler, RNG or existing KV-cache owner.

## 2. Components and responsibilities

- `rotary_position.h/.c`: validate a bounded rotary configuration, rotate
  adjacent Q/K channel pairs, and compute an unscaled relative-position dot
  product. Output buffers are caller-owned and unchanged on failure.
- `rotary_window.h/.c`: opaque bounded single-head sliding K/V buffer;
  preserves original absolute token positions across evictions. A successful
  append computes an attended context, stores one K/V row, and retains only
  the configured context window. Entire state is staged before publication.
- `inspect_rotary_window.c`: thin native educational consumer, with no model
  or attention mathematics copied into the application.
- `test_training_rotary_*.c`: exercise numerical invariance, independent
  full-window reference parity, eviction, reset and failure atomicity.

**The input to the rolling cache is not a token ID or model checkpoint.** Each
call takes projected Q, K and V vectors. The caller must prove that these
projections are stable for retained tokens: typically a first attention block
where Q/K/V are derived from fixed token embeddings *without learned absolute
positional additions*. That guarantee is **not automatic for later blocks**.

## 3. Mathematics

For pair index `j` within the configured rotary width `d`, let

`theta(position, j) = position * base^(-2*j/d)`.

The input pair `(a,b)` is transformed into:

```text
rotated_a = a*cos(theta) - b*sin(theta)
rotated_b = a*sin(theta) + b*cos(theta)
```

Rotary widths must be positive, even, and no larger than the attention head
width. An unrotated tail is retained exactly. Because a common shift of the
query and key positions cancels in their relative rotation, the Q/K dot
product is mathematically invariant under equal position shifts, within
floating-point rounding.

The ring-cache study stores keys **after** rotary transformation at the
original absolute token positions, alongside unmodified supplied values.
On an append, the new query is rotated at its own absolute position. The
causal attention response is calculated over only retained keys/values:

`softmax(QK^T / sqrt(headChannels)) * V`.

A stable softmax subtracts the maximum score before exponentiation.

## 4. Example workload — not a speed benchmark

With a context capacity of 4 and 12 sequential token rows, full-window
recalculation of all retained Q/K/V projections would process:

`1 + 2 + 3 + 4 + 8*4 = 42` rows.

A growing ring stores exactly one new projection row for each append:

`12` successful rows, `8` evictions, `0` full-window K/V rebuilds.

The avoided *projection-row* work is `30/42 = 71.43%` for this specific
first-block scenario. Attention scoring still inspects retained K rows on each
append. There is no measured 71.43% wall-clock or end-to-end model speedup.

The current production decoder is not this model family; do **not** present
these figures as an acceleration of its historical generation API.

## 5. Compatibility and safety controls

1. **Legacy decoder:** retain learned absolute embeddings unchanged; all
   existing checkpoint and position-admission paths remain authoritative.
2. **Separate model family:** any future full RoPE decoder needs an explicit
   architecture identifier and checkpoint family, not a changed enum alone.
3. **Training:** introduce rotary attention in an independently trained
   decoder (or provide a proved migration) before advertising model parity.
4. **Multi-layer correction:** upper-block cached projections may carry
   dependencies on evicted tokens. A first-block proof is insufficient.
5. **Numerical qualification:** compare all retained attention contexts with
   an independent reference, including boundary positions and rollover.
6. **Bounds:** channel width <=32, capacity <=32, absolute position <=1048576,
   frequency base in [2,1000000]; fail without partial output publication.
7. **Security:** this module does not authenticate checkpoints, files, or
   authors. Use existing trusted-publication controls for external models.

## 6. Build and tests — Windows UCRT64

In PowerShell after selective Beyond Compare merge:

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path
cmake -S '.\framework\learning\native-llm\prefix-work' `
    -B '.\build\windows-ucrt64-native-llm-prefix-debug' -G Ninja `
    -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
if ($LASTEXITCODE -ne 0) { throw 'Configure failed.' }
cmake --build '.\build\windows-ucrt64-native-llm-prefix-debug' --parallel 2
if ($LASTEXITCODE -ne 0) { throw 'Build failed.' }
ctest --test-dir '.\build\windows-ucrt64-native-llm-prefix-debug' `
    -R '^framework\.ai\.training\.rotary_' --output-on-failure --no-tests=error
if ($LASTEXITCODE -ne 0) { throw 'Rotary regression tests failed.' }
```

## 7. Validation limitations

The new C23 primitives, three new tests and educational executable passed
standalone GCC 14.2 and Clang 17 tests in Release and ASan/UBSan builds. An
isolated **surrogate** CMake target containing only the new modules passed four
focused CTest cases for each compiler; GCC/Clang C++20 public-header syntax
checks passed. This validation used a small test-only matching `UmiStatus`
header, not a full build of the enormous production Framework tree.

**Unverified:** native Windows UCRT64 compiler/linker; entire legacy decoder;
full Umicom Applications/Studio build; real multi-block RoPE training;
checkpoint compatibility with a new architecture; end-to-end speedup.

The next milestone must qualify a separately identified training-ready
rotary decoder while keeping every current checkpoint and inference API
fully functional and numerically unchanged.
