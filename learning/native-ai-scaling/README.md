# Native model and adapter capacity planning

The Framework owns this experimental C23 capacity estimator. It projects the
parameter count and selected persistent allocations for a proposed future
multi-layer model. It **does not** increase the existing native decoder's
fixed context, parameter or layer limits. The output is a **partial** memory
estimate and is insufficient for GPU purchasing or scheduling a training job.

Run the `framework.ai.scaling.estimate` CTest after configuring this directory
with CMake and Ninja. The optional demonstration displays example values. Model
mathematics and all application logic remain in Umicom Framework.
