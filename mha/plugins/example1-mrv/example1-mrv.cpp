// This file is part of the HörTech Open Master Hearing Aid (openMHA)
// Copyright © 2004 2007 2009 2010 2012 2013 2014 2015 2017 2018 HörTech gGmbH
//
// openMHA is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, version 3 of the License.
//
// openMHA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License, version 3 for more details.
//
// You should have received a copy of the GNU Affero General Public License, 
// version 3 along with openMHA.  If not, see <http://www.gnu.org/licenses/>.


#include <QApplication>
#include "mainwindow.h"

#define JULIA_ENABLE_THREADING

// Julia headers (for initialization and gc commands)
#include "uv.h"
#include "julia.h"

/*
 * The simplest example of an \mha plugin.
 *
 * This plugin scales one channel of the input signal, working in the
 * time domain.
 */

#include "mha_plugin.hh"
#include <iostream>
#include <iomanip> // for std::setprecision()
#include <dlfcn.h>
#include <string.h>
#include <thread>

using namespace std;

extern "C" {

//extern void jl_init_with_image__threading(void *, char *);
extern void jl_atexit_hook(int);

// Declare C prototype of a function defined in Julia
extern int julia_scale(jl_array_t *, float);
extern int julia_fftw(jl_array_t *, jl_array_t *, float);
extern int julia_main(jl_array_t *);
extern int julia_main2(jl_array_t *, jl_array_t *, jl_array_t *);
}

/** This C++ class implements the simplest example plugin for the
 * step-by-step tutorial.  It inherits from MHAPlugin::plugin_t for
 * correct integration in the configuration language interface.  */
class example1_t : public MHAPlugin::plugin_t<int> {
public:
    /** The constructor has to take these three
     * arguments, but it does not have to use them. However, the base
     * class has to be initialized. */
    example1_t(algo_comm_t &ac,
               const std::string &chain_name,
               const std::string &algo_name)
            : MHAPlugin::plugin_t<int>("", ac) {
        plot_thread = std::thread(&example1_t::plot_function, std::ref(*this));
        last_wave = new mha_wave_t;

        // https://stackoverflow.com/questions/5907031/printing-the-correct-number-of-decimal-points-with-cout
        std::cout << std::fixed;
        std::cout << std::setprecision(2);
    }

    /** Release may be empty */
    void release(void) {
        dlclose(dl_handle); // mrv: is this where I should run this?
        delete(last_wave);
        delete(spectrum);
        qDebug() << "example1_t resources released";
    }

    /** Plugin preparation. This plugin checks that the input signal has the
     * waveform domain and contains at least one channel
     * @param signal_info
     *   Structure containing a description of the form of the signal (domain,
     *   number of channels, frames per block, sampling rate.
     */
    void prepare(mhaconfig_t &signal_info) {

        dl_handle = dlopen(libmrv_path.c_str(), RTLD_GLOBAL | RTLD_NOW);
        if (!dl_handle) {
            /* Failed to load the library */
            throw MHA_Error(__FILE__, __LINE__, dlerror());
        }
        dlerror();

        if (signal_info.domain != MHA_WAVEFORM)
            throw MHA_Error(__FILE__, __LINE__,
                            "This plugin can only process waveform signals.");
        if (signal_info.channels < 1)
            throw MHA_Error(__FILE__, __LINE__,
                            "This plugin requires at least one input channel.");

        spectrum = new float[signal_info.fragsize * signal_info.channels]; // TODO: this is not the real size of the spectrum array
        sample_rate = signal_info.srate;

        return;
    }

    /** Signal processing performed by the plugin.
     * This plugin multiplies the signal in the first audio channel by
     * a factor 0.1.
     * @param signal
     *   Pointer to the input signal structure.
     * @return
     *   Returns a pointer to the input signal structure,
     *   with a the signal modified by this plugin.
     *   (In-place processing)
     */
    mha_wave_t *process(mha_wave_t *wave) {

        static unsigned  int counter = 0;
        static float z_prev_m = 0.0;
        static float z_prev_v = 1000.0;

#if 0
        unsigned int channel = 0; // channels and frames counting starts with 0
        float factor = 0.1f;
        unsigned int frame;

        // Scale channel number "channel" by "factor":
        for (frame = 0; frame < wave->num_frames; frame++) {
            // Waveform channels are stored interleaved.
            wave->buf[wave->num_channels * frame + channel] *= factor;
        }
#else
        // Do this once
        if(counter == 0) {
            // Initialize Julia runtime
            jl_init_with_image__threading(NULL, (char *) libmrv_path.c_str());
        }

        // 1-dimensional 32 bit floating point array
        jl_value_t *array_type = jl_apply_array_type((jl_value_t *) jl_float32_type, 1);

        // Generate a thin wrappers around the existing arrays
        // (the 0 in the last argument is a boolean indicating whether Julia should take ownership of the data)
        jl_array_t *spectrum_wrapper = jl_ptr_to_array_1d(array_type, spectrum, 30, 0); // TODO: remove hard-coded 30
        jl_array_t *buff_wrapper = jl_ptr_to_array_1d(array_type, wave->buf, wave->num_frames * wave->num_channels, 0);

        // Call the function inside the shared library that was written in Julia
        unsigned int spectrum_size = 0;
        spectrum_size = julia_fftw(spectrum_wrapper, buff_wrapper, sample_rate);

        // Call the function inside the shared library that was written in Julia
//        julia_scale(buff_wrapper, 0.2);
//        julia_main(buff_wrapper);

        jl_array_t *z_prev_m_wrapper = jl_ptr_to_array_1d(array_type, &z_prev_m, 1, 0);
        jl_array_t *z_prev_v_wrapper = jl_ptr_to_array_1d(array_type, &z_prev_v, 1, 0);
        julia_main2(buff_wrapper, z_prev_m_wrapper, z_prev_v_wrapper);
//        qDebug() << z_prev_m;
#endif

        // Visualize the signal using Qt
        std::memcpy(last_wave, wave, sizeof(mha_wave_t));
        if (mainWindow && ( (counter++)%16 == 0)) {
            emit mainWindow->samplesReady(last_wave, spectrum, spectrum_size); // emit signal
        }

        // Algorithms may process data in-place and return the input signal
        // structure as their output signal:
        return wave;
    }

private:
    std::thread plot_thread; // https://stackoverflow.com/questions/18163284/storing-an-stdthread-object-as-a-class-member
    std::unique_ptr<MainWindow> mainWindow = nullptr;
    mha_wave_t *last_wave = nullptr;
    string libmrv_path = "/home/mroavi/repos/TinyB/submodules/openMHA/external_libs/x86_64-linux-gcc-7/lib/libmrv.so";
    void *dl_handle;
    float * spectrum;
    float sample_rate;


    int plot_function() {
        int argc = 0;
        char **argv = NULL;

        QApplication application(argc, argv);
        mainWindow = std::make_unique<MainWindow>(); // lambda capture by reference
        mainWindow->show();
        return application.exec();
    }
};

/*
 * This macro connects the example1_t class with the \mha plugin C interface
 * The first argument is the class name, the other arguments define the 
 * input and output domain of the algorithm.
 */
MHAPLUGIN_CALLBACKS(example1, example1_t, wave, wave
)

/*
 * This macro creates code classification of the plugin and for
 * automatic documentation.
 *
 * The first argument to the macro is a space separated list of
 * categories, starting with the most relevant category. The second
 * argument is a LaTeX-compatible character array with some detailed
 * documentation of the plugin.
 */
MHAPLUGIN_DOCUMENTATION\
(example1,
 "example level-modification audio-channels",
 "The {\\bf simplest} example of an \\mha{} plugin.\n\n"
 "This plugin scales one channel of the input signal, working in the "
 "time domain."
)

// Local Variables:
// compile-command: "make"
// c-basic-offset: 4
// indent-tabs-mode: nil
// coding: utf-8-unix
// End:
