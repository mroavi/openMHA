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

MOC      := moc
UIC      := uic

CXXFLAGS += -fPIC
LDFLAGS += -fPIC

# TODO: fix absolute paths and support MAC!!!!
# julia-lib
CXXFLAGS += -L../../../external_libs/x86_64-linux-gcc-7/lib -L/home/mroavi/julia/usr/lib
CXXFLAGS += -I/home/mroavi/julia/usr/include/julia -I/home/mroavi/julia/usr/include
LDFLAGS += -lmrv -ljulia -lLLVM-6.0 -L../../../external_libs/x86_64-linux-gcc-7/lib -L/home/mroavi/julia/usr/lib

# qt
INCLUDE  := \
	-I/usr/include/x86_64-linux-gnu/qt5 \
	-I/usr/include/x86_64-linux-gnu/qt5/QtWidgets \
	-I/usr/include/x86_64-linux-gnu/qt5/QtCore \
	-I/usr/include/x86_64-linux-gnu/qt5/QtGui \
	-I../../../external_libs/qcustomplot \
	-I$(BUILD_DIR) \

CXXFLAGS += -L/usr/lib/x86_64-linux-gnu -lpthread
CXXFLAGS += $(INCLUDE)
LDFLAGS += $(INCLUDE) -L/usr/lib/x86_64-linux-gnu -lQt5Quick -lQt5PrintSupport -lQt5Qml -lQt5Network -lQt5Widgets -lQt5Gui -lQt5Core

# mrv: rule to generate ui_mainwindow.h
$(BUILD_DIR)/ui_mainwindow.h: mainwindow.ui
	$(UIC) $< -o $@

# mrv: process moc header with Qt's moc utility
# (Qt's signal/slot functionality requires generation of some extra code)
$(BUILD_DIR)/moc_mainwindow.cpp: mainwindow.h
	$(MOC) $(INCLUDE) $< -o $@

# mrv: process moc header with Qt's moc utility
# (Qt's signal/slot functionality requires generation of some extra code)
$(BUILD_DIR)/moc_qcustomplot.cpp: ../../../external_libs/qcustomplot/qcustomplot.h
	$(MOC) $(INCLUDE) $< -o $@

$(BUILD_DIR)/example1-mrv$(PLUGIN_EXT): $(BUILD_DIR)/ui_mainwindow.h $(BUILD_DIR)/mainwindow.o
$(BUILD_DIR)/example1-mrv$(PLUGIN_EXT): $(BUILD_DIR)/moc_mainwindow.o $(BUILD_DIR)/moc_qcustomplot.o ../../../external_libs/qcustomplot/qcustomplot.o


# Local Variables:
# compile-command: "make"
# coding: utf-8-unix
# End:
