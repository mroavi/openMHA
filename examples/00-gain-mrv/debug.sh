export LD_LIBRARY_PATH=/home/mroavi/julia/usr/lib:/home/mroavi/repos/TinyB/submodules/openMHA/external_libs/x86_64-linux-gcc-7/lib
export MHA_LIBRARY_PATH=/home/mroavi/repos/TinyB/submodules/openMHA/lib;/home/mroavi/repos/TinyB/submodules/openMHA/bin
gdb --args ../../bin/mha ?read:gain_live.cfg cmd=start

