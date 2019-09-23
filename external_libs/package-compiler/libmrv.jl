# libmrv.jl
Base.@ccallable foo(x::Cint)::Cint = 42
Base.@ccallable bar(x::Cint)::Cint = x
Base.@ccallable function baz(x::Cint)::Cint
    println("\tI am baz.")
    return 0
end

module Hello

using UnicodePlots

Base.@ccallable function julia_main()::Cint
    @show sin(0.0)
    println(lineplot(1:100, sin.(range(0, stop=2π, length=100))))
    return 0
end

end

Base.@ccallable function print_arr(arr::Vector{Cfloat})::Cint
    println(arr)
    return 0
end
