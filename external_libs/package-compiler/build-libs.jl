# This is an example of how to use PackageCompiler.jl to generate a shared lib.
# The resulting library files can be called from a C++ program.

# ------------------------------------------------------------------------------
# Build the shared library
# ------------------------------------------------------------------------------

output_dir =        joinpath(@__DIR__, "../x86_64-linux-gcc-7/lib")
pkg_compiler_dir =  joinpath(@__DIR__, "../../../PackageCompiler.jl")
lib_src_dir =       joinpath(@__DIR__, ".")
julia_lib_dir = "/home/mroavi/julia/usr/lib"

# Build the shared library
run(`julia --project=$(pkg_compiler_dir)/Project.toml $(pkg_compiler_dir)/juliac.jl -vas -g2 -d $(output_dir) $(lib_src_dir)/libmrv.jl`)
