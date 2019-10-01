# libmrv.jl
# https://docs.julialang.org/en/v1/manual/embedding/index.html

module MyModule

using FFTW, SampledSignals

Base.@ccallable function julia_main(buf::Vector{Cfloat})::Cint

    factor::Cfloat = 0.1;
    channel = 1
    num_channels = 2

    buf[channel:num_channels:end] = buf[channel:num_channels:end] .* factor

    return 0
end

Base.@ccallable function julia_fftw(fttres::Vector{Cfloat}, buf::Vector{Cfloat}, samplerate::Cfloat)::Cint

    sample_buffer = SampleBuf(buf, samplerate)

    fmin = 0Hz
    fmax = 10000Hz

    fttres[:] = abs.(fft(sample_buffer)[fmin..fmax]).data

    return length(fttres)
end

end