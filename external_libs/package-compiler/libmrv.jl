# libmrv.jl
# https://docs.julialang.org/en/v1/manual/embedding/index.html

module MyModule

using FFTW, SampledSignals, ForneyLab

Base.@ccallable function julia_scale(buf::Vector{Cfloat}, factor::Cfloat)::Cint

    channel = 1
    num_channels = 2

    buf[channel:num_channels:end] = buf[channel:num_channels:end] .* factor

    return 0
end


Base.@ccallable function julia_spectrum_size(buf::Vector{Cfloat}, samplerate::Cfloat)::Cint

    num_channels = 2
    channel_buf = buf[1:num_channels:end]
    sample_buffer = SampleBuf(channel_buf, samplerate)

    fmin = 0Hz
    fmax = 10000Hz

    return length(abs.(fft(sample_buffer)[fmin..fmax]).data)
end

Base.@ccallable function julia_fftw(fttres::Vector{Cfloat}, buf::Vector{Cfloat}, samplerate::Cfloat, channel::Cint)::Cint

    num_channels = 2
    channel_buf = buf[channel:num_channels:end]
    sample_buffer = SampleBuf(channel_buf, samplerate)

    fmin = 0Hz
    fmax = 10000Hz

    fttres[:] = abs.(fft(sample_buffer)[fmin..fmax]).data

    return length(fttres)
end


Base.@ccallable function julia_main(buf::Vector{Cfloat})::Vector{Cfloat}

    buf_size = length(buf)

    mx_t = 0.0
    vx_t = 1.0

    messages = Array{Message}(undef, 3)
    marginals = Dict()

    for t in 1:buf_size

        # Prepare data and prior statistics
        data = Dict(:z  => convert(Float64, buf[t]),
                    :mx => mx_t,
                    :vx => vx_t)

        # Execute algorithm
        messages[1] = ruleSPGaussianMeanVarianceOutNPP(nothing, Message(Univariate, PointMass, m=data[:mx]), Message(Univariate, PointMass, m=data[:vx]))
        messages[2] = ruleSPGaussianMeanVarianceMPNP(Message(Univariate, PointMass, m=data[:z]), nothing, Message(Univariate, PointMass, m=1.0))
        messages[3] = ruleSPMultiplicationIn1GNP(messages[2], nothing, Message(Univariate, PointMass, m=2))

        marginals[:x] = messages[1].dist * messages[3].dist

        # Extract posterior statistics
        mx_t = mean(marginals[:x])
        vx_t = var(marginals[:x])

        # Store to buffer
        buf[t] = convert(Cfloat, mx_t)
    end

    # Perform garbage collection
    GC.gc()

    return buf
end

Base.@ccallable function julia_main2(buf::Vector{Cfloat},
    z_prev_m::Vector{Cfloat},
    z_prev_v::Vector{Cfloat})::Vector{Cfloat}

    buf_size = length(buf)

    obs_var = 0.05

    for t in 2:2:buf_size

        # Store noisy sine wave in channel 2
        buf[t] = 0.8 * sin(2π * (t/buf_size)) + sqrt(obs_var) * randn() # from PortAudioSine example

        # Copy channel 2 to channel 1 (Temp)
        buf[t-1] = buf[t]

        xi = 1.0/(z_prev_v[1] + 0.001) * z_prev_m[1] + 1.0/obs_var * buf[t]
        w = 1.0/(z_prev_v[1] + 0.001) + 1.0/obs_var

        # Extract posterior statistics
        z_prev_m[1] = 1.0/w * xi
        z_prev_v[1] = 1.0/w

        # Store in filtered wave in buffer
        buf[t] = z_prev_m[1]
        # buf[t] = 0.8 * sin(2π * (t/buf_size)) # from PortAudioSine example
        # buf[t] = 0.1 * sin(2π * 440 * ((t/2)/44100)) # from https://www.coursera.org/learn/audio-signal-processing/lecture/UMfgf/sinusoids

    end

    return buf
end

end
