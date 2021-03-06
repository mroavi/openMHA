# This file is part of the HörTech Open Master Hearing Aid (openMHA)
# Copyright © 2005 2006 2007 2008 2012 2013 2016 2017 2018 HörTech gGmbH
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

include ../../../config.mk
include ../../../magic.mk

MHANAME=openMHA

all: $(MHANAME)_application_manual.pdf MHA_writing_fitting_rules.pdf libaudprof_manual.pdf

TEXINPUTS = "./:../:./plugins/:../technical_report/:../flowcharts/fmt_eps/:../flowcharts/fmt_pdf/:../flowcharts/:"

%.pdf: %.tex
	TEXINPUTS=$(TEXINPUTS) pdflatex $<
	TEXINPUTS=$(TEXINPUTS) pdflatex $<
	TEXINPUTS=$(TEXINPUTS) makeindex $(basename $<)
	TEXINPUTS=$(TEXINPUTS) pdflatex $<

%.aux: %.tex
	TEXINPUTS=$(TEXINPUTS) pdflatex $<

clean:
	rm -Rf *.log *.lot *.lof *.aux *.dep *.dvi *.toc *~ *.blg *.bbl *.ilg *.ind *.idx *.out $(MHANAME)_application_manual.pdf $(MHANAME)_gui_manual.pdf plug_*.tex

$(MHANAME)_application_manual.pdf: $(MHANAME)_application_manual.tex ../openMHAdoxygen.sty $(wildcard *.tex)

libaudprof_manual.pdf: libaudprof_manual.tex ../openMHAdoxygen.sty

$(MHANAME)_gui_manual.pdf: $(MHANAME)_gui_manual.tex ../openMHAdoxygen.sty

# Local Variables:
# coding: utf-8-unix
# End:
