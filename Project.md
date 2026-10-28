
**Purpose**
- Want to get familiar with software-assisted hardware design (but just using Verilog is boring)
- Want to learn auto-differentiation internals
- Bear consequences of decisions along a stack; most projects before have been standalone

**Scope**
1. Small language `sluggish` with metaprogramming ability + library support
2. Library `Slughw` implemented in `sluggish` for hardware design (HDL library)
3. Miniature CPU implementation in `Slughw` -> FPGA realization
4. Miniature GPU/ASIC implementation in `Slughw` -> FPGA realization
5. Library `Slugdx` implemented in `sluggish` for auto-differentiation
6. Library `Slugml` implemented on top of `Slugdx` for general-purpose ML workflows
7. Small LLM implementation on top of `Slugml`; hopefully ~1 million parameters
