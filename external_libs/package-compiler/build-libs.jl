# This is an example of how to use PackageCompiler.jl to generate a shared lib.
# The resulting library files can be called from a C++ program.

# IMPORTANT: this file must be invoked by openMHA's makefile

using Pkg

# ------------------------------------------------------------------------------
# Build the shared library
# ------------------------------------------------------------------------------

# Get the CFLAGS environment variable
cflags = get(ENV, "CFLAGS", -1)
# Get the include path (last entry) which happens to contain the architecture
include_dir = split(cflags, " ")[end]
# Extract the architecture from the include path
arch = splitdir(splitdir(include_dir)[end-1])[end]

output_dir =        joinpath(@__DIR__, "..", arch, "lib")
pkg_compiler_dir =  joinpath(@__DIR__, "../../../PackageCompiler.jl")
lib_src_dir =       joinpath(@__DIR__, ".")
julia_lib_dir = "/home/mroavi/julia/usr/lib"

# Activate PackageCompiler project
Pkg.activate(pkg_compiler_dir)

# Build the shared library
run(`julia --project=$(pkg_compiler_dir)/Project.toml $(pkg_compiler_dir)/juliac.jl -vas -g2 -d $(output_dir) $(lib_src_dir)/libmrv.jl`)
