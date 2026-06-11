/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the
"Apache License"); you may not use this file except in compliance with the
Apache License. You may obtain a copy of the Apache License at
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2025 Audiokinetic Inc.
*******************************************************************************/
#pragma once
#ifndef WP_CeSoundFIRTraceConv_24FX_H
#define WP_CeSoundFIRTraceConv_24FX_H

#include "pocketfft-cpp/pocketfft_hdronly.h"
#include "Dr_wav/dr_wav.h"

#include "WP_CeSoundFIRTraceConv_24FXParams.h"
#include "UEDataStruct.h"
#include <cmath>
#include <complex>
#define _USE_MATH_DEFINES
#include <vector>

typedef std::vector<float>  CsVector;
typedef std::vector <std::vector<float>> CsVector2;
typedef std::vector<std::complex<float>> CsVectorC;
typedef std::vector<std::vector<std::complex<float>>> CsVector2C;
typedef std::vector<std::vector<std::vector<std::complex<float>>>> CsVector3C;

typedef pocketfft::shape_t CsShape;
typedef pocketfft::stride_t CsStride;

typedef std::complex<float> CsC;

/// See https://www.audiokinetic.com/library/edge/?source=SDK&id=soundengine__plugins__effects.html
/// for the documentation about effect plug-ins
class WP_CeSoundFIRTraceConv_24FX
    : public AK::IAkOutOfPlaceEffectPlugin
{
public:
    WP_CeSoundFIRTraceConv_24FX();
    ~WP_CeSoundFIRTraceConv_24FX();

    //=================================================================================

    enum class CeFreq
    {
        Hz63, Hz128, Hz250, Hz500, Hz1000, Hz2000, Hz4000, Hz8000
    };

    // My variables
    AkUInt16 m_bufferSize{ 512 };
    const double m_pi{ std::acos(-1.0) };
    const CsC m_minus_i{ (0, -1) };
    bool m_filterExRunning{ false };
    bool m_FDL_XInitialised{ false };
    bool m_FDL_HInitialised{ false };
    bool m_processTail{ false };
    INT32 m_dataVersion{ -1 };
    void* m_vpGameData = nullptr;
    unsigned int m_currentChannel{ 0 };
	int m_currentSampleRate{ 0 };

    CsVector2 m_UPOLSInput;

    CsVector m_testFIR;
    CsVector2 m_meldaFIR;

    std::vector<CsVector2> m_linConvOverflow;

    // FDL variables
    //CsVector m_FDLBuffer;
    CsVector2C m_FDL_H;
    CsVector2C m_FDL_Htemp;
    CsVector3C m_FDL_X;
    CsVector2C m_FDL_Result;

    //=================================================================================

public:

    /// Plug-in initialization.
    /// Prepares the plug-in for data processing, allocates memory and sets up the initial conditions.
    AKRESULT Init(AK::IAkPluginMemAlloc* in_pAllocator, AK::IAkEffectPluginContext* in_pContext, AK::IAkPluginParam* in_pParams, AkAudioFormat& in_rFormat) override;

    /// Release the resources upon termination of the plug-in.
    AKRESULT Term(AK::IAkPluginMemAlloc* in_pAllocator) override;

    /// The reset action should perform any actions required to reinitialize the
    /// state of the plug-in to its original state (e.g. after Init() or on effect bypass).
    AKRESULT Reset() override;

    /// Plug-in information query mechanism used when the sound engine requires
    /// information about the plug-in to determine its behavior.
    AKRESULT GetPluginInfo(AkPluginInfo& out_rPluginInfo) override;

    /// Effect plug-in DSP execution.
    void Execute(AkAudioBuffer* in_pBuffer, AkUInt32 in_ulnOffset, AkAudioBuffer* out_pBuffer) override;

    /// Skips execution of some frames, when the voice is virtual playing from elapsed time.
    /// This can be used to simulate processing that would have taken place (e.g. update internal state).
    /// Return AK_DataReady or AK_NoMoreData, depending if there would be audio output or not at that point.
    AKRESULT TimeSkip(AkUInt32 &io_uFrames) override;

private:
	// My functions
	void defaultExecute(AkAudioBuffer* in_pBuffer, AkUInt32 in_ulnOffset, AkAudioBuffer* out_pBuffer);
    /// <summary>
    /// Partitions an impulse response buffer and returns information about the partitioning.
    /// </summary>
    /// <param name="FIR">Pointer to the first element of the impulse response (FIR) buffer to partition.</param>
    /// <returns>A CsVector2 containing the partitioning result (for example, partition sizes or offsets). The exact interpretation depends on the implementation.</returns>
    void partitioningAndWriteFilterToTemp(const CsVector& FIR);

    /// <summary>
    /// Combines multiple FIR passes into a single CsVector.
    /// </summary>
    /// <param name="FIR">Pointer to the first CsVector2 in an array or sequence of FIR passes to be combined.</param>
    /// <returns>A CsVector containing the combined FIR result.</returns>
    CsVector combineFIRPasses(CsVector2& FIR);
    /// <summary>
    /// Performs the GUPOLS operation.
    /// </summary>
    void GUPOLS();
    /// <summary>
    /// Applies the UPOLS operation to the provided CsVector buffer and returns the computed CsVector result.
    /// </summary>
    /// <param name="xBuffer">Pointer to the input CsVector buffer to be processed by UPOLS. Must point to a valid CsVector instance containing the data to use.</param>
    /// <returns>A CsVector containing the result of the UPOLS computation.</returns>
    CsVector UPOLS(const CsVector& xBuffer);
    /// <summary>
    /// Performs a filter exchange operation.
    /// </summary>
    CsVector filterExchange( CsVector h);
    /// <summary>
    /// Creates a CsVector from a buffer of AkReal32 samples, formatted for FDL usage.
    /// </summary>
    /// <param name="xBuffer">A vector of AkReal32 samples to convert. Passed by value and therefore copied into the function.</param>
    /// <returns>A CsVector containing the input samples arranged/converted for FDL processing.</returns>
    CsVector makeFDLBuffer(std::vector<AkReal32> xBuffer);
    /// <summary>
    /// Initializes the FDL subsystem or state using the provided 2D vector.
    /// </summary>
    /// <param name="h">A 2D vector (CsVector2) used to initialize FDL. Contains the values required by the initialization routine.</param>
    void initialiseFDLWithFilter(CsVector h);
    /// <summary>
    /// Updates the FDL using the provided CsVector buffer.
    /// </summary>
    /// <param name="xBuffer">A CsVector containing data used to update the FDL. The argument is passed by value.</param>
    void pushStreamToFDL(CsVector xBuffer);
    /// <summary>
    /// Performs convolution on signals.
    /// </summary>
    void convoluteSignals();
    /// <summary>
    /// Computes and returns the sum of the FDL as a CsVector.
    /// </summary>
    /// <returns>A CsVector containing the computed sum.</returns>
    CsVector sumFDL();
    /// <summary>
    /// Signals that a filter has been updated and provides the new 2D filter value.
    /// </summary>
    /// <param name="filter">The updated filter value as a CsVector2 (2D vector).</param>
    //void initialiseAndUpdateFilter(const CsVector2& filter);


    std::vector<float> linearConvolution(const std::vector<float>& input, const CsVector& filter)
    {
        size_t N = input.size();
        size_t M = filter.size();
        std::vector<float> output(N + M - 1, 0.0f);
        for (size_t n{ 0 }; n < output.size(); n++)
        {
            for (size_t m{ 0 }; m < M; m++)
            {
                if (n >= m && n - m < N)
                {
                    output[n] += input[n - m] * filter[m];
                }
            }
        }
        return output;
    };

    std::vector<float> sinc(CeFreq f, int N, int Fs)
    {
        float frequency = 0.0f;
        switch (f)
        {
        case CeFreq::Hz63:
            frequency = 63.0f;
            break;
        case CeFreq::Hz128:
            frequency = 128.0f;
            break;
        case CeFreq::Hz250:
            frequency = 250.0f;
            break;
        case CeFreq::Hz500:
            frequency = 500.0f;
            break;
        case CeFreq::Hz1000:
            frequency = 1000.0f;
            break;
        case CeFreq::Hz2000:
            frequency = 2000.0f;
            break;
        case CeFreq::Hz4000:
            frequency = 4000.0f;
            break;
        case CeFreq::Hz8000:
            frequency = 8000.0f;
            break;
        }
        auto M = (N - 1) / 2;
        auto fc = frequency / static_cast<float>(Fs);
        std::vector<float> h(N, 0.0f);
        std::vector<float> window(N, 0.0f);
        float a0 = 0.42f;
        float a1 = 0.5f;
        float a2 = 0.08f;
        for (size_t n{ 0 }; n < N; n++)
        {
            window[n] = a0 - a1 * std::cos((2 * m_pi * n) / N) + a2 * std::cos((4 * m_pi * n) / N);
        };
        for (size_t n{ 1 }; n <= N; n++)
        {
            h[n-1] = (std::sin(2 * m_pi * fc * (n - M / 2)) / (m_pi * (n - M / 2))) * window[n-1];
        }
        return h;
    }

    std::vector<float> sinc(CeFreq f, int Fs)
    {
        auto N = 4000;
        float frequency = 0.0f;
        switch (f)
        {
        case CeFreq::Hz63:
            frequency = 63.0f;
            break;
        case CeFreq::Hz128:
            frequency = 128.0f;
            break;
        case CeFreq::Hz250:
            frequency = 250.0f;
            break;
        case CeFreq::Hz500:
            frequency = 500.0f;
            break;
        case CeFreq::Hz1000:
            frequency = 1000.0f;
            break;
        case CeFreq::Hz2000:
            frequency = 2000.0f;
            break;
        case CeFreq::Hz4000:
            frequency = 4000.0f;
            break;
        case CeFreq::Hz8000:
            frequency = 8000.0f;
            break;
        }
        auto M = (N - 1) / 2;
        auto fc = frequency / static_cast<float>(Fs);
        std::vector<float> h(N, 0.0f);
        std::vector<float> window(N, 0.0f);
        float a0 = 0.42f;
        float a1 = 0.5f;
        float a2 = 0.08f;
        for (size_t n{ 0 }; n < N; n++)
        {
            window[n] = a0 - a1 * std::cos((2 * m_pi * n) / N) + a2 * std::cos((4 * m_pi * n) / N);
        };
        for (size_t n{ 1 }; n <= N; n++)
        {
            h[n - 1] = (std::sin(2 * m_pi * fc * (n - M / 2)) / (m_pi * (n - M / 2))) * window[n - 1];
        }
        return h;
    }

    std::vector<float> normaliseEnergy (std::vector<float> & FIR)
    {
        float energy = 0.0f;
        for (const auto& sample : FIR)
        {
			if (sample != 0.0f)
            energy += sample * sample;
        }
        float normFactor = std::sqrt(energy);
        if (normFactor > 0.0f)
        {
            for (auto& sample : FIR)
            {
                sample /= normFactor;
            }
        }
        return FIR;
	}
    //==================================================================================
    // Wwise variables
    WP_CeSoundFIRTraceConv_24FXParams* m_pParams;
    AK::IAkPluginMemAlloc* m_pAllocator;
    AK::IAkEffectPluginContext* m_pContext;
    //From execute function

    //==================================================================================
	

    
    
};



#endif // WP_CeSoundFIRTraceConv_24FX_H
