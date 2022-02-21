/*  OpenVLBI - Open Source Very Long Baseline Interferometry
*   Copyright © 2017-2021  Ilia Platone
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
    plot_flags_synced = 4
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
        * \brief Add a new OpenVLBI context by giving it a name. VLBI::Server has an internal context collection
        * \param name The name of the new context
        */
        void AddContext(const char *name);

        /**
        * \brief Delete an existing OpenVLBI context by name.
        * \param name The name of the context to be deleted
        */
        void DelContext(const char *name);

        /**
        * \brief Set the current OpenVLBI context by passing its name.
        * \param name The name of the context
        */
        void SetContext(const char *name);

        /**
        * \brief Obtain the current OpenVLBI context object.
        * \return The vlbi_context object representing the current context
        */
        vlbi_context GetContext(void);

        /**
        * \brief Obtain the name current OpenVLBI context.
        * \return The name of the current context
        */
        inline char* CurrentContext(void)
        {
            return context;
        }

        /**
        * \brief Create a new model from a picture, give it a name and add it to the current context.
        * \param name The name of the new model
        * \param format The format of the new model, can be one of png, jpeg or fits
        * \param b64 The file buffer base64 encoded
        */
        void AddModel(const char *name, char *format, char *b64);

        /**
        * \brief Obtain the dsp_stream_p object of a model by passing its name.
        * \param name The name of the model
        * \return The dsp_stream_p object representing the model
        */
        dsp_stream_p GetModel(const char *name);

        /**
        * \brief Obtain the base64 encoded file buffer of a model by passing its name.
        * \param name The name of the model
        * \param format The format of the picture exported, can be one of png, jpeg or fits
        * \return The file buffer base64 encoded
        */
        char* GetModel(const char *name, char *format);

        /**
        * \brief Delete from the current context an existing model by name.
        * \param name The name of the model to be deleted
        */
        void DelModel(const char *name);

        /**
        * \brief Get the names of all the models of the current context.
        * \param names The pointer of a char* array to be filled with the names of the models
        * \return The number of models in the current context
        */
        int GetModels(char** names);

        /**
        * \brief Create a new node from a monodimensional FITS file, give it a name and add it to the current context.
        * \param name The name of the new node
        * \param b64 The file buffer base64 encoded
        */
        void AddNode(const char *name, char *b64);

        /**
        * \brief Create a new node from a its raw data, give it a name and add it to the current context.
        * \param name The name of the new node
        * \param locations A pointer to its location(s), will be pointed from the new node, so don't free() it until the node is deleted
        * \param buf The data buffer of the new node. Will be casted, according the current value of Bps, to the element type with the current word size
        * \param len The number of elements
        * \param starttime The UTC time of the first element
        * \param geo If 1, consider all elements of location as geographic coordinates, if 0 as relative to the current context' station location
        */
        void AddNode(const char *name, dsp_location *locations, void *buf, int len, timespec starttime, bool geo);

        /**
        * \brief Delete from the current context an existing node by name.
        * \param name The name of the node to be deleted
        */
        void DelNode(const char *name);

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
        * \brief Shift a model by its dimension in-place
        * \param name The name of the model
        */
        void Shift(const char *name);

        /**
        * \brief Set the target right ascension coordinate, do this before calling Plot()
        * \param value The RA coordinate in hours
        */
        inline virtual void SetRa(double value)
        {
            Ra = value;
        }

        /**
        * \brief Set the target declination coordinate, do this before calling Plot()
        * \param value The Declination coordinate in degrees
        */
        inline virtual void SetDec(double value)
        {
            Dec = value;
        }

        /**
        * \brief Set the frequency observed, do this before calling Plot()
        * \param value The frequency in Hz
        */
        inline virtual void SetFreq(double value)
        {
            Freq = value;
        }

        /**
        * \brief Set the sampling frequency, do this before calling Plot()
        * \param value The sample rate, in samples per second
        */
        inline virtual void SetSampleRate(double value)
        {
            SampleRate = value;
        }

        /**
        * \brief Set the bytes per sample, do this before calling AddNode()
        * \param value The word width in bytes
        */
        inline virtual void SetBps(int value)
        {
            Bps = value;
        }

        /**
        * \brief Set the plot width, do this before calling Plot()
        * \param value plot width in pixels
        */
        inline virtual void SetWidth(int value)
        {
            w = value;
        }

        /**
        * \brief Set the plot height, do this before calling Plot()
        * \param value plot height in pixels
        */
        inline virtual void SetHeight(int value)
        {
            h = value;
        }

        /**
        * \brief Get the current right ascension coordinate
        * \return The current right ascension in hours
        */
        inline double GetRa(void)
        {
            return Ra;
        }

        /**
        * \brief Get the current declination coordinate
        * \return The current declination in hours
        */
        inline double GetDec(void)
        {
            return Dec;
        }

        /**
        * \brief Get the current frequency
        * \return The frequency in Hz
        */
        inline double GetFreq(void)
        {
            return Freq;
        }

        /**
        * \brief Get the current sample rate
        * \return The sampling rate in sps
        */
        inline double GetSampleRate(void)
        {
            return SampleRate;
        }

        /**
        * \brief Get the bytes per sample
        * \return The word width in bytes
        */
        inline double GetBps(void)
        {
            return Bps;
        }

        /**
        * \brief Get the plot width
        * \return The plot width in pixels
        */
        inline double GetWidth(void)
        {
            return w;
        }

        /**
        * \brief Get the plot height
        * \return The plot height in pixels
        */
        inline double GetHeight(void)
        {
            return h;
        }

        /**
        * \brief Set the input stream
        * \param in The input stream file pointer
        */
        void SetInput(FILE* in)
        {
            input = in;
        }

        /**
        * \brief Get the input stream
        * \return The input stream file pointer
        */
        inline FILE* GetInput()
        {
            return input;
        }

        /**
        * \brief Set the output stream
        * \param out The output stream file pointer
        */
        void SetOutput(FILE* out)
        {
            output = out;
        }

        /**
        * \brief Get the output stream
        * \return The output stream file pointer
        */
        inline FILE* GetOutput()
        {
            return output;
        }

    private:
        double Ra;
        double Dec;
        double Freq;
        double SampleRate;
        int Bps;
        int w;
        int h;
        FILE *input, *output;
        char *context;
};
extern VLBI::Server *server;
/**\}*/
};

#endif //VLBI_SERVER_H
