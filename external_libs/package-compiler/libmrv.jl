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

Base.@ccallable function julia_main(buf::Vector{Cfloat})::Vector{Cfloat}

    factor::Cfloat = 0.1;
    channel = 1
    num_channels = 2

    buf[channel:num_channels:end] = buf[channel:num_channels:end] .* factor

    return buf
end
