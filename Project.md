
**Purpose**
- Want to get familiar with software-assisted hardware design (but just using Verilog is boring)
- Want to learn auto-differentiation internals
- Bear consequences of decisions along a stack; most projects before have been standalone

**Scope**
1. Small language `sluggish` with metaprogramming ability + library support
    - Core should be routine given PMax implementation
    - Extensions (generics, meta-programming, real library support) are new challenges
2. Library `Slughw` implemented in `sluggish` for hardware design (HDL library)
3. Hardware simulator for `Slughw` output testing
4. Miniature CPU implementation in `Slughw` -> FPGA realization
5. Miniature GPU/ASIC implementation in `Slughw` -> FPGA realization
6. Library `Slugdx` implemented in `sluggish` for auto-differentiation
7. Library `Slugml` implemented on top of `Slugdx` for general-purpose ML workflows
8. Small LLM implementation on top of `Slugml`; hopefully ~1 million parameters

**Reflections**
- Project is extremely serial; later steps are dependent on earlier ones
- Unfamiliar with internals of both JAX and HDL compilers
- Should keep development in each step narrow -> get deeper through the pipeline quickly
    - Then thicken as needed; e.g. add generics to `sluggish` while developing `Slugdx` or similar.
