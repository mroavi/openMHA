# libmrv.jl
# https://docs.julialang.org/en/v1/manual/embedding/index.html

module MyModule

using ForneyLab

counter = 0

Base.@ccallable function julia_main(buf::Vector{Cfloat})::Vector{Cfloat}
    global counter

    # if counter == 0
    #     println("Hello from Julia")
    # end

    buf_size = length(buf)
    mx_t = 0.0
    vx_t = 1.0

    # messages::Vector{Message} = Array{Message}(undef, 3)
    # marginals::Dict = Dict()

    z_prev_m = 0.0
    z_prev_v = 1000.0

    # if counter < Inf
    #     println("[$counter][before loop] buf: ", pointer(buf))
    # end

    for t in 1:buf_size
        # Prepare data and prior statistics
        z = convert(Float64, buf[t])
        data = Dict(:z  => convert(Float64,buf[t]),
                    :mx => mx_t,
                    :vx => vx_t)
        # Execute algorithm
        m1 = ruleSPGaussianMeanVarianceOutNPP(nothing, Message(Univariate, PointMass, m=mx_t), Message(Univariate, PointMass, m=vx_t))
        m2 = ruleSPGaussianMeanVarianceMPNP(Message(Univariate, PointMass, m=z), nothing, Message(Univariate, PointMass, m=1.0))
        m3 = ruleSPMultiplicationIn1GNP(m2, nothing, Message(Univariate, PointMass, m=2))
        marginal = m1.dist * m3.dist
        # Extract posterior statistics
        mx_t = ForneyLab.unsafeMean(marginal)
        # vx_t = ForneyLab.unsafeCov(marginal)
        # Store to buffer
        # if counter < Inf
        #     println("[$counter][before convert] buf: ", pointer(buf))
        # end
        buf[t] = convert(Cfloat, mx_t)
        # if counter < Inf
        #     println("[$counter][after convert_] buf: ", pointer(buf))
        # end
        # buf[t] = -buf[t]
        # vx[t] = convert(Cfloat, mx_t)
        # xi = 1.0/(z_prev_v + 10.0) * z_prev_m + 1.0/5.0 * buf[t]
        # w  = 1.0/(z_prev_v + 10.0) + 1.0/5.0
        # Extract posterior statistics
        # z_prev_m = 1.0/w * xi
        # z_prev_v = 1.0/w
        # Store in buffer
        # buf[t] = z_prev_m
    end

    if counter % 200 == 0
        GC.gc()
        counter += 1
    end

    return buf
end

Base.@ccallable function julia_scale(buf::Vector{Cfloat}, factor::Cfloat)::Vector{Cfloat}
    # NOTE: The line below does not work! Apparently the range operator (:) is needed
    # buf = buf .* 0.5
    # buf[:] = buf[:] .* factor
    for t in 1:length(buf)
        buf[t] = buf[t] * factor
    end
    return buf
end
Base.@ccallable function julia_sin(buf::Vector{Cfloat}, phase::Cfloat)::Vector{Cfloat}
    for t in 1:length(buf)
        buf[t] = sin(buf[t] + phase)
    end
    return buf
end

end
