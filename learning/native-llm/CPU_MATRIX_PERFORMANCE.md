# Native CPU matrix kernels and performance evidence

**Umicom Framework · Sammy Hegab · Umicom Foundation · MIT**

The Umicom training tape and key/value inference cache now share the same
Framework-owned matrix multiplication operator, provided by
`include/umicom/ai/training/matrix_kernel.h` and
`src/ai/training/matrix_kernel.c`. It is an inspectable C23 CPU baseline, not an
external BLAS dependency or a claim that the current miniature decoder is
ready for general-purpose code generation.

## Why consolidate the matrix implementation?

The training tape previously performed each matrix dot product using a separate
column loop. The key/value inference cache had a similar local row-projection
function. Both remain preserved inside reviewable disabled source blocks, while
the running implementations delegate to the one reusable numerical contract.
A later SIMD or Assembly backend can replace an authorised kernel without
creating competing maths implementations in Studio, LLM or Trader.

For an input A shaped [rows, inner] and a weight matrix B shaped
[inner, columns], the output C has shape [rows, columns]. Each element is:

    C[row,column] = sum over k from 0 to inner-1 of A[row,k] * B[k,column]

Every output accumulates in double in increasing k order and is then stored
once as float32. The reference and tiled paths produce exactly the same
results for the bounded regression inputs; under different FP optimisation
flags, such as unsafe reassociation, bit-identical results must be requalified.

The tiled implementation operates on 16 neighbouring output columns at a time
and reuses contiguous weight-row reads. This may improve cache behaviour,
but may be slower than reference for some shapes or compiler optimisation
levels. The automatic policy conservatively uses reference for one-row, short-
inner, wide-output projections and tiled for other supported shapes. It makes
no assumption about the host CPU architecture and cannot guarantee speedups.

## Public contract and memory ownership

`UmiAiTrainingMatrixMultiply` accepts only finite float32 values and checked
nonzero dimensions. Each of the two inputs and the output is bounded by
`UMI_AI_MATRIX_MAX_ELEMENTS` (4096 float values). Output is staged in a
fixed local scratch buffer before publishing, including when output storage
aliases an input. Rejections preserve caller output. There is no heap
allocation or global mutable policy. The caller serialises any concurrent
access to its own input/output buffers.

The scratch buffer reserves 4096 floats on the calling thread's stack; this
is appropriate for the current laboratory and must be reviewed before
adapting the kernel to tiny embedded stacks. Enlarging the resource limit
requires tests for overflow, bounds, memory budgets and safe stack usage.

`UmiAiTrainingMatrixEstimateWork` returns multiply-accumulate counts, nominal
floating-point operation counts and the logical matrix byte counts. These
numbers are workload descriptions, not measurements of physical memory
traffic, throughput, allocated process memory or GPU performance.

## Build and run on Windows

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake -S ".\framework\learning\native-llm" `
    -B ".\build\windows-ucrt64-native-llm-debug" `
    -G Ninja -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build ".\build\windows-ucrt64-native-llm-debug" --parallel 2
ctest --test-dir ".\build\windows-ucrt64-native-llm-debug" `
    --output-on-failure --no-tests=error
& ".\build\windows-ucrt64-native-llm-debug\umicom-native-llm-matrix-benchmark.exe" 1000
```

The native benchmark reports `clock()` **process CPU time**, not wall time.
Run Release and Debug separately and repeat measurements on an otherwise idle
computer. Compare inputs of the same shape, with the same compiler flags and
warm-up conditions. A higher reference/tiled ratio means the tiled path was
faster **in that one observation**. No CI test passes or fails based on timing.

## Validation and limitations

- `framework.ai.training.matrix_contract` checks reference/tiled/automatic
  output parity, aliasing, bounds, estimates, invalid arguments and overflow.
- `framework.ai.training.matrix_tape` checks canonical multiplication and
  backward gradients against independently calculated derivatives.
- `framework.ai.training.matrix_inference` compares KV cache results to the
  canonical decoder before and after a full-window rebuild.
- All established training, checkpoint, generation and audit tests remain
  registered unchanged.

This update **does not raise** the current 4096-element tensor limit, the
32-token decoder context limit, or the experimental decoder's parameter budget.
The next scalability work should address configurable memory ownership,
quantitative profiling of entire model operations, and a separately verified
accelerated backend. Increasing limits without addressing these contracts is
not equivalent to supporting larger language models.
