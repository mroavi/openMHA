# libmrv.jl

# using FFTW
#
# Base.@ccallable function fftw(buf::Vector{Cfloat})::Cint
#
#     fftres = abs.(fft(buf))
#
#     return length(fftres)
# end

# libmrv.jl
Base.@ccallable foo(x::Cint)::Cint = 42
Base.@ccallable bar(x::Cint)::Cint = x
Base.@ccallable function baz(x::Cint)::Cint
    println("\tI am baz.")
    return 0
end

Base.@ccallable function julia_main(arr::Vector{Cfloat})::Vector{Cfloat}
    return arr .* 2.0
end
