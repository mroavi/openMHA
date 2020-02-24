# This is an example of how to use PackageCompiler.jl to generate a shared lib.
# The resulting library files can be called from a C++ program.

using Pkg

# ------------------------------------------------------------------------------
# Build the shared library
# ------------------------------------------------------------------------------

output_dir =        joinpath(@__DIR__, "../x86_64-Darwin-clang/lib")
pkg_compiler_dir =  joinpath(@__DIR__, "../../../PackageCompiler.jl")
lib_src_dir =       joinpath(@__DIR__, ".")

# Activate PackageCompiler project
Pkg.activate(pkg_compiler_dir)

# Build the shared library
run(`julia --project=$(pkg_compiler_dir)/Project.toml $(pkg_compiler_dir)/juliac.jl -vas -O0 -g2 -d $(output_dir) $(lib_src_dir)/libmrv.jl`)
