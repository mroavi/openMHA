# This file is part of the HörTech Open Master Hearing Aid (openMHA)
# Copyright © 2014 2017 2018 HörTech gGmbH
#
# openMHA is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, version 3 of the License.
#
# openMHA is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License, version 3 for more details.
#
# You should have received a copy of the GNU Affero General Public License, 
# version 3 along with openMHA.  If not, see <http://www.gnu.org/licenses/>.

include ../plugin.mk

# LDFLAGS += -lmrv -ljulia -L/home/mroavi/repos/TinyB/submodules/openMHA/external_libs/test-mrv

CXXFLAGS += -fPIC -L/home/mroavi/repos/TinyB/submodules/openMHA/external_libs/julia-lib-test -L/home/mroavi/julia/usr/lib -I/home/mroavi/julia/usr/include/julia -I/home/mroavi/julia/usr/include
LDFLAGS += -lmrv -ljulia -lLLVM-6.0 -L/home/mroavi/repos/TinyB/submodules/openMHA/external_libs/julia-lib-test -L/home/mroavi/julia/usr/lib

# CXXFLAGS += -fPIC -lmrv -ljulia -L../../../external_libs/test-mrv -Wl,-rpath -Wl,../../../external_libs/test-mrv
# CFLAGS += -fPIC -lmrv -ljulia -L../../../external_libs/test-mrv -Wl,-rpath -Wl,../../../external_libs/test-mrv
# LDFLAGS += -lmrv -ljulia -L../../../external_libs/test-mrv
# LDLIBS += -lmrv -ljulia

# Local Variables:
# compile-command: "make"
# coding: utf-8-unix
# End: