# Understand and measure growing-prefix KV-cache reuse

This laboratory demonstrates a *real* reduction in token-row recomputation, but
only for a well-defined workload: observing every growing prefix of one token
sequence before its context window reaches capacity. It builds on the native
LLM generation measurement work of Lessons 67–68.

## Two experiments with different contracts

The existing `UmiAiTrainingGenerationPerformanceCompare` compares two
implementations of **canonical autoregressive generation**. That generator
left-pads an initial prompt with beginning-of-sequence tokens until the context
window is full. Its learned absolute positional embeddings require a complete
K/V rebuild whenever the full context advances. The previous 8-token-context,
12-token-generation example accordingly computes 96 token rows in either
implementation; these new interfaces do **not** change that behaviour.

This laboratory instead evaluates a growing *un-padded* prefix. Consider six
token IDs, in positions 0 through 5 of an eight-position decoder. We inspect
the logits after prefix length one, then two, and so on. Both approaches see
the **same token positions** at each comparison:

| Prefix length | Recompute from position zero | Retain K/V and append |
| ---: | ---: | ---: |
| 1 | 1 row | 1 row |
| 2 | 2 rows | 1 new row |
| 3 | 3 rows | 1 new row |
| 4 | 4 rows | 1 new row |
| 5 | 5 rows | 1 new row |
| 6 | 6 rows | 1 new row |
| **Total** | **21 rows** | **6 rows** |

Avoided rows: **15 / 21 (about 71%)**. This is a computational-work
observation, not a measured speedup. The K/V cache also incurs allocations,
bookkeeping and memory-access costs.

For `N` prefixes (1 <= N <= decoder context length), full recomputation costs
`N * (N + 1) / 2` rows; append reuse costs `N` rows. The two paths are
numerically compared for **all vocabulary logits** at every prefix using the
existing KV cache and its current 0.00004 absolute-tolerance qualification.
Both paths are against the same decoder instance; no weights are modified.

## Native Framework interfaces

Include `umicom/ai/training/kv_prefix_reuse.h` and call:

- `UmiAiTrainingKvPrefixWorkEstimateBuild` for an allocation-checked, shape-
  validated estimate, with atomic output on rejection.
- `UmiAiTrainingKvPrefixPerformanceCompare` for a qualified comparison. It
  executes the real Framework `KvCachePrefill` and `KvCacheAppend` operations,
  verifies all prefix logits, and independently checks the measured row count.

Both are opt-in. They do not change the decoder ABI, sampling, the existing
streaming session, the original checkpoint format or the production generation
policy.

## Build this focused laboratory

This batch adds a standalone CMake entry point at
`learning/native-llm/prefix-work/CMakeLists.txt`. It *includes* the existing
Framework native-LLM training composition and adds one Framework-owned source
plus two regressions and a read-only CLI; it does not fork the math kernels.

From a PowerShell prompt in your applications workspace:

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path

cmake -S .\framework\learning\native-llm\prefix-work `
    -B .\build\windows-ucrt64-native-llm-prefix-debug -G Ninja `
    -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
if ($LASTEXITCODE -ne 0) { throw 'Prefix laboratory configure failed.' }

cmake --build .\build\windows-ucrt64-native-llm-prefix-debug `
    --target umicom-native-llm-prefix-work `
             umicom-ai-training-kv-prefix-reuse-test `
             umicom-ai-training-kv-prefix-rejections-test `
    --parallel 2
if ($LASTEXITCODE -ne 0) { throw 'Prefix laboratory compile failed.' }

ctest --test-dir .\build\windows-ucrt64-native-llm-prefix-debug `
    -R '^framework\.ai\.training\.kv_prefix_' `
    --output-on-failure --no-tests=error
if ($LASTEXITCODE -ne 0) { throw 'Prefix laboratory tests failed.' }

& '.\build\windows-ucrt64-native-llm-prefix-debug\umicom-native-llm-prefix-work.exe'
if ($LASTEXITCODE -ne 0) { throw 'Prefix laboratory demonstration failed.' }
```

For the complete independent native-LLM laboratory, build the default target
and run all registered CTests from the prefix project build tree. Consult
`learning/native-llm/WINDOWS_NATIVE_LLM_BUILD.md` for the broader environment.

## Measurement interpretation

Process CPU times are optional `clock()` readings. Their measurement windows
include cache operations and per-prefix inspection but exclude initial cache
allocation and final logit-parity comparison. They are not wall-clock latency,
end-to-end generation throughput, or a guaranteed speed ratio. Repeat on your
machine to estimate variability and run under an appropriate profiler.

The report's allocation field concerns **one** decoder-shaped cache. The
comparison temporarily creates two caches to keep weight snapshots equal and
avoid reusing mutable state between experimental paths.

**Limit:** At the first eviction of a full context, the existing learned
absolute-position design still invalidates all retained K/V rows. Switching
to rotary or relative positions would require an explicit model-format,
training, checkpoint-migration and numerical-compatibility programme. Such a
change is outside this batch.
