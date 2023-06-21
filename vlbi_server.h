/*  OpenVLBI - Open Source Very Long Baseline Interferometry
*   Copyright © 2017-2022  Ilia Platone
*
*   This program is free software; you can redistribute it and/or
*   modify it under the terms of the GNU Lesser General Public
*   License as published by the Free Software Foundation; either
*   version 3 of the License, or (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public License
*   along with this program; if not, write to the Free Software Foundation,
*   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef VLBI_SERVER_H
#define VLBI_SERVER_H

#include <vlbi.h>
#include <dsp.h>

#ifdef _WIN32
#define restrict __restrict
extern ssize_t getdelim(char **restrict lineptr, size_t *restrict n, int delim, FILE *restrict stream);
#endif

namespace VLBI
{

/**
* \defgroup Server OpenVLBI Server C++ API
*
* The OpenVLBI Server, a C++ implementation of an OpenVLBI server or application.<br>
*
* This class offers a simple interface with the commonly used methods, allowing custom overrides and<br>
* extensibility.
*
* \author Ilia Platone
*
* \{*/


/**
* \brief Flags that characterize a plot
* \sa Server::Plot()
*/
typedef enum
{
    ///This indicates that the nodes have a positional stream companion
    plot_flags_moving_baseline = 1,
    ///This will fill all baselines projected pixels with ones
    plot_flags_uv_coverage = 2,
    ///This indicates that the nodes are synced already and no delay calculation will be done
    plot_flags_synced = 4,
    ///This will use a custom visibility delegate
    plot_flags_custom_delegate = 8,
} vlbi_plot_flags;

/**
* \brief Inherit this class to create an OpenVLBI server application
*/
class Server
{
    public:
        /**
        * \brief Constructor, initializes all the internal variables
        */
        Server(void);
        /**
        * \brief Destructor, destroys this object
        */
        virtual ~Server(void);

        /**
        * \brief Called immediately after main(), can be overriden to add your custom arguments
        * \param argc Non-negative value representing the number of arguments passed to the program from the environment in which the program is run.
        * \param argv Pointer to the first element of an array of argc + 1 pointers, of which the last one is null and the previous ones, if any, point to strings that represent the arguments passed to the program from the host environment.
        */
        virtual int Init(int argc, char** argv)
        {
            (void)argc;
            (void)argv;
            return 1;
        }

        /**
        * \brief main() creates a loop that calls Parse(), you can use this one, which uses the standard syntax or override it with your own implementation
        */
        virtual void Parse(void);

        /**
        * \brief add a new OpenVLBI context by giving it a name. VLBI::Server has an internal context collection
        * \param name The name of the new context
        */
        void addContext(const char *name);

        /**
        * \brief delete an existing OpenVLBI context by name.
        * \param name The name of the context to be deleted
        */
        void delContext(const char *name);

        /**
        * \brief set the current OpenVLBI context by passing its name.
        * \param name The name of the context
        */
        void setContext(const char *name);

        /**
        * \brief Obtain the current OpenVLBI context object.
        * \return The vlbi_context object representing the current context
        */
        vlbi_context getContext(void);

        /**
        * \brief Obtain the name current OpenVLBI context.
        * \return The name of the current context
        */
        inline char* currentContext(void)
        {
            return context;
        }

        /**
        * \brief Create a new model from a picture, give it a name and add it to the current context.
        * \param name The name of the new model
        * \param format The format of the new model, can be one of png, jpeg or fits
        * \param b64 The file buffer base64 encoded
        */
        void addModel(const char *name, char *format, char *b64);

        /**
        * \brief Obtain the dsp_stream_p object of a model by passing its name.
        * \param name The name of the model
        * \return The dsp_stream_p object representing the model
        */
        dsp_stream_p getModel(const char *name);

        /**
        * \brief Obtain the base64 encoded file buffer of a model by passing its name.
        * \param name The name of the model
        * \param format The format of the picture exported, can be one of png, jpeg or fits
        * \return The file buffer base64 encoded
        */
        char* getModel(const char *name, char *format);

        /**
        * \brief delete from the current context an existing model by name.
        * \param name The name of the model to be deleted
        */
        void delModel(const char *name);

        /**
        * \brief get the names of all the models of the current context.
        * \param names The pointer of a char* array to be filled with the names of the models
        * \return The number of models in the current context
        */
        int getModels(char** names);

        /**
        * \brief Create a new node from a monodimensional image FITS file, give it a name and add it to the current context.
        * \param name The name of the new node
        * \param b64 The file buffer base64 encoded
        */
        void addNode(const char *name, char *b64);

        /**
        * \brief Create as many nodes as the rows number of an SDFITS file, give it a name and add it to the current context.
        * \param name The name of the new node
        * \param b64 The file buffer base64 encoded
        */
        void addNodes(const char *name, char *b64);

        /**
        * \brief Create a new node from a its raw data, give it a name and add it to the current context.
        * \param name The name of the new node
        * \param locations A pointer to its location(s), will be pointed from the new node, so don't free() it until the node is deleted
        * \param buf The data buffer of the new node. Will be casted, according the current value of Bps, to the element type with the current word size
        * \param len The number of elements
        * \param starttime The UTC time of the first element
        * \param geo If 1, consider all elements of location as geographic coordinates, if 0 as relative to the current context' station location
        */
        void addNode(const char *name, dsp_location *locations, void *buf, int len, timespec starttime, bool geo);

        /**
        * \brief delete from the current context an existing node by name.
        * \param name The name of the node to be deleted
        */
        void delNode(const char *name);

        /**
        * \brief Copy a node into another node
        * \param name The name of the new node
        * \param node The name of the node to be copied
        */
        void CopyNode(const char *name, const char *node);

        /**
        * \brief Plot the current observation into a new model.
        * \param name The name of the new model
        * \param flags The vlbi_plot_flags that characterize this observation
        */
        void Plot(const char *name, int flags);

        /**
        * \brief Obtain an inverse fourier transform from the magnitude and phase models passed.
        * \param name The name of the new model
        * \param magnitude The name of the model used as magnitude
        * \param phase The name of the model used as phase
        */
        void Idft(const char *name, const char *magnitude, const char *phase);

        /**
        * \brief Save the magnitude and phase to new models obtained by the fourier transform of the model passed.
        * \param name The name of the model
        * \param magnitude The name of the new model that will contain the magnitude
        * \param phase The name of the new model that will contain the phase
        */
        void Dft(const char *name, const char *magnitude, const char *phase);

        /**
        * \brief Mask a model with another model by multiplication
        * \param name The name of the new model
        * \param model The name of the model to be masked
        * \param mask The name of the mask model
        */
        void Mask(const char *name, const char *model, const char *mask);

        /**
        * \brief Stack a model with another model
        * \param name The name of the new model
        * \param model1 The name of the first model to be stacked
        * \param model2 The name of the second model to be stacked
        */
        void Stack(const char *name, const char *model1, const char *model2);

        /**
        * \brief Copy a model into another model
        * \param name The name of the new model
        * \param model The name of the model to be copied
        */
        void Copy(const char *name, const char *model);

        /**
        * \brief Diff a model with another model
        * \param name The name of the new model
        * \param model1 The name of the first model
        * \param model2 The name of the second model to be subtracted from model1
        */
        void Diff(const char *name, const char *model1, const char *model2);

        /**
        * \brief Convolute a model with a convolution matrix model
        * \param name The name of the new model
        * \param model1 The name of the model
        * \param model2 The name of the model used as convolution matrix
        */
        void Convolute(const char *name, const char *model1, const char *model2);

        /**
        * \brief Apply a low pass filter on a node buffer
        * \param name The name of the new node
        * \param node The name of the unfiltered node
        * \param freq The cutoff frequency in radians
        */
        void LowPass(const char *name, const char *node, double freq);

        /**
        * \brief Apply a high pass filter on a node buffer
        * \param name The name of the new node
        * \param node The name of the unfiltered node
        * \param freq The cutoff frequency in radians
        */
        void HighPass(const char *name, const char *node, double freq);

        /**
        * \brief Apply a band pass filter on a node buffer
        * \param name The name of the new node
        * \param node The name of the unfiltered node
        * \param lofreq The low cut frequency in radians
        * \param hifreq The high cut frequency in radians
        */
        void BandPass(const char *name, const char *node, double lofreq, double hifreq);

        /**
        * \brief Apply a band reject filter on a node buffer
        * \param name The name of the new node
        * \param node The name of the unfiltered node
        * \param lofreq The low cut frequency in radians
        * \param hifreq The high cut frequency in radians
        */
        void BandReject(const char *name, const char *node, double lofreq, double hifreq);

        /**
        * \brief Shift a model by its dimension in-place
        * \param name The name of the model
        */
        void Shift(const char *name);

        /**
        * \brief set the target right ascension coordinate, do this before calling Plot()
        * \param value The RA coordinate in hours
        */
        inline virtual void setRa(double value)
        {
            Ra = value;
        }

        /**
        * \brief set the target declination coordinate, do this before calling Plot()
        * \param value The Declination coordinate in degrees
        */
        inline virtual void setDec(double value)
        {
            Dec = value;
        }

        /**
        * \brief set the frequency observed, do this before calling Plot()
        * \param value The frequency in Hz
        */
        inline virtual void setFreq(double value)
        {
            Freq = value;
        }

        /**
        * \brief set the sampling frequency, do this before calling Plot()
        * \param value The sample rate, in samples per second
        */
        inline virtual void setSampleRate(double value)
        {
            SampleRate = value;
        }

        /**
        * \brief set the bytes per sample, do this before calling addNode()
        * \param value The word width in bytes
        */
        inline virtual void setBps(int value)
        {
            Bps = value;
        }

        /**
        * \brief set the plot width, do this before calling Plot()
        * \param value plot width in pixels
        */
        inline virtual void setWidth(int value)
        {
            w = value;
        }

        /**
        * \brief set the plot height, do this before calling Plot()
        * \param value plot height in pixels
        */
        inline virtual void setHeight(int value)
        {
            h = value;
        }

        /**
        * \brief get the current right ascension coordinate
        * \return The current right ascension in hours
        */
        inline double getRa(void)
        {
            return Ra;
        }

        /**
        * \brief get the current declination coordinate
        * \return The current declination in hours
        */
        inline double getDec(void)
        {
            return Dec;
        }

        /**
        * \brief get the current frequency
        * \return The frequency in Hz
        */
        inline double getFreq(void)
        {
            return Freq;
        }

        /**
        * \brief get the current sample rate
        * \return The sampling rate in sps
        */
        inline double getSampleRate(void)
        {
            return SampleRate;
        }

        /**
        * \brief set the current correlation order
        * \param order The correlation order
        */
        void setCorrelationOrder(int order);

        /**
        * \brief get the bytes per sample
        * \return The word width in bytes
        */
        inline double getBps(void)
        {
            return Bps;
        }

        /**
        * \brief get the plot width
        * \return The plot width in pixels
        */
        inline double getWidth(void)
        {
            return w;
        }

        /**
        * \brief get the plot height
        * \return The plot height in pixels
        */
        inline double getHeight(void)
        {
            return h;
        }

        /**
        * \brief set the input stream
        * \param in The input stream file pointer
        */
        void setInput(FILE* in)
        {
            input = in;
        }

        /**
        * \brief get the input stream
        * \return The input stream file pointer
        */
        inline FILE* getInput()
        {
            return input;
        }

        /**
        * \brief set the output stream
        * \param out The output stream file pointer
        */
        void setOutput(FILE* out)
        {
            output = out;
        }

        /**
        * \brief get the output stream
        * \return The output stream file pointer
        */
        inline FILE* getOutput()
        {
            return output;
        }

        /**
        * \brief set the delegate function
        * \param func The new delegate
        */
        inline void setDelegate(vlbi_func2_t func)
        {
            delegate = func;
        }

        /**
        * \brief get the current delegate function
        * \return The current delegate
        */
        inline vlbi_func2_t getDelegate() { return delegate; }

    private:
        vlbi_func2_t delegate;
        double Ra;
        double Dec;
        double Freq;
        double SampleRate;
        int Bps;
        int w;
        int h;
        FILE *input, *output;
        char *context;
        char *tmpdir { nullptr };
};
extern VLBI::Server *server;
/**\}*/
};

#endif //VLBI_SERVER_H
