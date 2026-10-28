
**Purpose**
- Want to get familiar with software-assisted hardware design (but just using Verilog is boring)
- Want to learn auto-differentiation internals
- Bear consequences of decisions along a stack; most projects before have been standalone

**Scope**
1. Small language `sluggish` with metaprogramming ability + library support
    - Core should be routine given PMax implementation
    - Extensions (generics, meta-programming, real library support) are new challenges
    - Keep extremely light to implement quickly
2. Library `Slughw` implemented in `sluggish` for hardware design (HDL library)
    - Build without introducing one-off language features; implement purely as importable library
3. Hardware simulator for `Slughw` output testing
    - Light Logisim-like simulation for correctness checks of `Slughw` output
4. Miniature CPU implementation in `Slughw` -> FPGA realization
    - Should be routine given breadboard CPU implementation
    - Not really meant as a CPU project, but just to honour the bootstrap philosophy
5. Miniature GPU/ASIC implementation in `Slughw` -> FPGA realization
    - Key part of the project; fairly unfamiliar with GPU internals
    - Look into old Nvidia chips + Google TPUs
6. Library `Slugdx` implemented in `sluggish` for auto-differentiation
    - Have used JAX but don't know its exact internals
    - Forward tracing should be pretty easy to implement, though (with meta-programming features)
    - Can maybe build e.g. `struct` instances so `Slugdx.Int` has an `__add__` function which then traces `f` and `df/dx` simultaneously for each `f` and `x` -> build Jacobi matrix stepwise
7. Library `Slugml` implemented on top of `Slugdx` for general-purpose ML workflows
8. Small LLM implementation on top of `Slugml`; hopefully ~1 million parameters

**Reflections**
- Project is extremely serial; later steps are dependent on earlier ones
- Unfamiliar with internals of both JAX and HDL compilers
- Should keep development in each step narrow -> get deeper through the pipeline quickly
    - Then thicken as needed; e.g. add generics to `sluggish` while developing `Slugdx` or similar.
- Implement compiler frontend very quickly -> get to IR and potentially machine code emission fast
- Then, get into `Slughw` design, even if the language itself will be extended. Also keep the `Slughw` implementation very minimal so we can quckly get to the hardware simulator + a miniature CPU implementation
- Deepen pipeline quickly plus build large test array -> iterative work (widening of each step's functionality) should be easier
