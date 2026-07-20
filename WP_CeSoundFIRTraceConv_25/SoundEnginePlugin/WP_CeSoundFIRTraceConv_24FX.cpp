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

#include "WP_CeSoundFIRTraceConv_24FX.h"
#include "../WP_CeSoundFIRTraceConv_24Config.h"
#include <fstream>
#include <iostream>
#include <Windows.h>
#include <AK/AkWwiseSDKVersion.h>
#include <crtdbg.h>
//#include <afx.h>
#define TESTING 0
#define LIVE 1

AK::IAkPlugin* CreateWP_CeSoundFIRTraceConv_24FX(AK::IAkPluginMemAlloc* in_pAllocator)
{
    OutputDebugStringW(L"Create\n");
    return AK_PLUGIN_NEW(in_pAllocator, WP_CeSoundFIRTraceConv_24FX());
}

AK::IAkPluginParam* CreateWP_CeSoundFIRTraceConv_24FXParams(AK::IAkPluginMemAlloc* in_pAllocator)
{
    OutputDebugStringW(L"CreateParams\n");
    return AK_PLUGIN_NEW(in_pAllocator, WP_CeSoundFIRTraceConv_24FXParams());
}

AK_IMPLEMENT_PLUGIN_FACTORY(WP_CeSoundFIRTraceConv_24FX, AkPluginTypeEffect, WP_CeSoundFIRTraceConv_24Config::CompanyID, WP_CeSoundFIRTraceConv_24Config::PluginID)

WP_CeSoundFIRTraceConv_24FX::WP_CeSoundFIRTraceConv_24FX()
    : m_pParams(nullptr)
    , m_pAllocator(nullptr)
    , m_pContext(nullptr)
{
    OutputDebugStringW(L"Constructor\n");
}

WP_CeSoundFIRTraceConv_24FX::~WP_CeSoundFIRTraceConv_24FX()
{
	OutputDebugStringW(L"Destructor\n");
    m_vpGameData = nullptr;
    delete m_vpGameData;
}

AKRESULT WP_CeSoundFIRTraceConv_24FX::Init(AK::IAkPluginMemAlloc* in_pAllocator, AK::IAkEffectPluginContext* in_pContext, AK::IAkPluginParam* in_pParams, AkAudioFormat& in_rFormat)
{
    m_pParams = (WP_CeSoundFIRTraceConv_24FXParams*)in_pParams;
    m_pAllocator = in_pAllocator;
    m_pContext = in_pContext;
    m_currentSampleRate = in_rFormat.uSampleRate;
    

    int select = m_pParams->RTPC.ifirSelect;
    std::wstring msg = L"FIR Select: " + std::to_wstring(select) + L"\n";
    OutputDebugStringW(msg.c_str());
    OutputDebugStringW(L"Init\n");
#if LIVE == 0
    m_meldaFIR.resize(filePaths.size());
	for (size_t i = 0; i < filePaths.size(); i++)
    {
		const char* filePath = filePaths[i].c_str();
        AkUInt32 channels;
        AkUInt32 sampleRate;
        drwav_uint64 totalPCMFrameCount;
        float* pSamples = drwav_open_file_and_read_pcm_frames_f32(filePath, &channels, &sampleRate, &totalPCMFrameCount, nullptr);

        m_meldaFIR[i].reserve(static_cast<size_t>(totalPCMFrameCount));

        for (drwav_uint64 frame = 0; frame < totalPCMFrameCount; ++frame) {
            m_meldaFIR[i].push_back(
                pSamples[frame * channels]
            );
        }

        drwav_free(pSamples, nullptr);
    }
  
	initialiseFDLWithFilter(m_meldaFIR[0]);

    /*std::fstream file("C:\\Users\\cedri\\Desktop\\output3.txt");
    
    double x;
    while (file >> x)
    {
        m_testFIR.emplace_back(x);
    }
    m_testFIR = normaliseEnergy(m_testFIR);
    
    initialiseFDLWithFilter(m_testFIR);
    
	//m_testFIR.resize(24000, 1.0f);*/
#endif
	AkChannelConfig channelConfig;
    m_pContext->GetParentChannelConfig(channelConfig);
    if(channelConfig.uNumChannels == 0 && channelConfig.uNumChannels != 1)
    {
        OutputDebugStringW(L"Invalid number of channels\n");
        return AK_UnsupportedChannelConfig;
	}
    return AK_Success;
}

AKRESULT WP_CeSoundFIRTraceConv_24FX::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
	OutputDebugStringW(L"Term\n");
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}

AKRESULT WP_CeSoundFIRTraceConv_24FX::Reset()
{
	OutputDebugStringW(L"Reset\n");
    return AK_Success;
}

AKRESULT WP_CeSoundFIRTraceConv_24FX::GetPluginInfo(AkPluginInfo& out_rPluginInfo)
{
    OutputDebugStringW(L"GetPluginInfo\n");
    out_rPluginInfo.eType = AkPluginTypeEffect;
    out_rPluginInfo.bIsInPlace = false;
	out_rPluginInfo.bCanProcessObjects = false;
    out_rPluginInfo.uBuildVersion = AK_WWISESDK_VERSION_COMBINED;
    return AK_Success;
}

void WP_CeSoundFIRTraceConv_24FX::Execute(AkAudioBuffer* in_pBuffer, AkUInt32 in_ulnOffset, AkAudioBuffer* out_pBuffer)
{
#if LIVE == 0
    OutputDebugStringW(L"Execute\n");
    if (m_pParams->m_paramChangeHandler.HasChanged(PARAM_FIRSELECT_ID))
    {
        //select = m_pParams->RTPC.ifirSelect;
        std::wstring msg = L"FIR Select: " + std::to_wstring(m_pParams->RTPC.ifirSelect);
		OutputDebugStringW(msg.c_str());
        partitioningAndWriteFilterToTemp(m_meldaFIR[m_pParams->RTPC.ifirSelect - 1]);
        m_filterExRunning = true;
    }
#endif
#if LIVE

    //Unit Test and assertions
    unsigned int dataSize{ 0 };
    m_pContext->GetPluginCustomGameData(m_vpGameData, dataSize);
    if(m_vpGameData == nullptr)
    {
        
		defaultExecute(in_pBuffer, in_ulnOffset, out_pBuffer);
		OutputDebugStringW(L"No Data\n");
		AKPLATFORM::OutputDebugMsg("No game data attached to plugin.\n");
        return;
	}
    UEDataStruct* gameData = static_cast<UEDataStruct*> (m_vpGameData);
    if (m_dataVersion != gameData->version)
    {
		OutputDebugStringW(L"Updating filter\n");
        m_dataVersion = gameData->version;
        if(!m_FDL_HInitialised)
        {
            CsVector h = combineFIRPasses(gameData->impulses);
            initialiseFDLWithFilter(h);
			m_FDL_HInitialised = true;
        }
        else
        {
            CsVector h = combineFIRPasses(gameData->impulses);
		    
			partitioningAndWriteFilterToTemp(h);
			m_filterExRunning = true;
        }
    }

	
#endif
    const AkUInt32 uNumChannels = in_pBuffer->NumChannels();

    AkUInt16 uFramesConsumed;
    AkUInt16 uFramesProduced;

    if (in_pBuffer->eState == AK_NoMoreData)
    {
        //OutputDebugStringW(L"Processing Tail\n");
        static bool onlyOnce{ false };
        if (!onlyOnce)
        {
            AkReal32* AK_RESTRICT pInBuf = (AkReal32 * AK_RESTRICT)in_pBuffer->GetChannel(0) + in_ulnOffset;
            AkReal32* AK_RESTRICT pOutBuf = (AkReal32 * AK_RESTRICT)out_pBuffer->GetChannel(0) + out_pBuffer->uValidFrames;

            uFramesConsumed = 0;
            uFramesProduced = 0;
            while (uFramesConsumed < in_pBuffer->MaxFrames() && uFramesConsumed < 2 * m_bufferSize)
            {
                m_UPOLSInput[0].emplace_back(*pInBuf++);
                ++uFramesConsumed;
            }
            CsVector output;
            output = UPOLS(m_UPOLSInput[0]);
            for (auto& sample : output)
            {
                *pOutBuf++ = sample;
                ++uFramesProduced;
            }
            
            onlyOnce = true;
            out_pBuffer->eState = AK_DataReady;
            out_pBuffer->uValidFrames = out_pBuffer->MaxFrames();
            return;
        }
        if (m_FDL_X[m_currentChannel].size() == 1)
        {

            convoluteSignals();
            CsVector output;
            AkReal32* AK_RESTRICT pOutBuf = (AkReal32 * AK_RESTRICT)out_pBuffer->GetChannel(0) + out_pBuffer->uValidFrames;
            output = sumFDL();
            for  (auto & sample : output)
                *pOutBuf++ = sample;
            out_pBuffer->eState = AK_NoMoreData;
            out_pBuffer->uValidFrames = out_pBuffer->MaxFrames();
            return;
        }
        //move FDL
		CsVectorC zeros(m_bufferSize + 1, CsC(0.0, 0.0));
        m_FDL_X[m_currentChannel].insert(m_FDL_X[m_currentChannel].begin(), zeros);

        if (m_FDL_X[m_currentChannel].size() > m_FDL_H.size())
        {
            m_FDL_X[m_currentChannel].pop_back();
        }
        //Pop Front of H and X
        m_FDL_X[m_currentChannel].erase(m_FDL_X[m_currentChannel].begin(), m_FDL_X[m_currentChannel].begin() + 1);
		m_FDL_H.erase(m_FDL_H.begin(), m_FDL_H.begin() + 1);
        //Conv
		convoluteSignals();
        //Output
		CsVector output;
        AkReal32* AK_RESTRICT pOutBuf = (AkReal32 * AK_RESTRICT)out_pBuffer->GetChannel(0) + out_pBuffer->uValidFrames;
        output = sumFDL();
        for (auto& sample : output)
            *pOutBuf++ = sample;
        out_pBuffer->eState = AK_DataReady;
        out_pBuffer->uValidFrames += output.size();
		return;
    }

    if(in_pBuffer->eState != AK_NoMoreData)
    {
        for (AkUInt32 i = 0; i < uNumChannels; ++i)
        {
            m_currentChannel = i;

            std::wstring msg = L"Processing Channel: " + std::to_wstring(i) + L"\n";
            OutputDebugStringW(msg.c_str());
            if (!m_FDL_XInitialised)
            {
                m_FDL_XInitialised = true;
                m_FDL_X.resize(uNumChannels);// , CsVector2C(m_FDL_H.size(), CsVectorC(m_bufferSize + 1, CsC(0.0, 0.0))));
                m_UPOLSInput.resize(uNumChannels, CsVector(0));
            }

            AkReal32* AK_RESTRICT pInBuf = (AkReal32 * AK_RESTRICT)in_pBuffer->GetChannel(i) + in_ulnOffset;
            AkReal32* AK_RESTRICT pOutBuf = (AkReal32 * AK_RESTRICT)out_pBuffer->GetChannel(i) + out_pBuffer->uValidFrames;

            //m_bufferSize = in_pBuffer->MaxFrames();
            uFramesConsumed = 0;
            uFramesProduced = 0;

            while (uFramesConsumed < in_pBuffer->uValidFrames
                && uFramesProduced < out_pBuffer->MaxFrames())
            {
                while (uFramesConsumed < in_pBuffer->MaxFrames() && uFramesConsumed < 2 * m_bufferSize)
                {
                    //m_UPOLSInput.push_back(*pInBuf++);
                    m_UPOLSInput[m_currentChannel].emplace_back(*pInBuf++);
                    ++uFramesConsumed;
                }
                //Check length of InputBuffer -> needs 2B lenght
                if (m_UPOLSInput[m_currentChannel].size() == 2 * m_bufferSize)
                {
                    //OutputDebugStringW(L"Processing Block\n");
                    CsVector output;
                    output = UPOLS(m_UPOLSInput[m_currentChannel]);
                    for (auto& sample : output)
                    {
                        *pOutBuf++ = sample;
                        ++uFramesProduced;
                    }

                    m_UPOLSInput[m_currentChannel].erase(m_UPOLSInput[m_currentChannel].begin(), m_UPOLSInput[m_currentChannel].begin() + m_bufferSize);
                    m_UPOLSInput[m_currentChannel].shrink_to_fit();

                    //OutputDebugStringW(L"Block Processed\n");
                }
                else
                {
                    OutputDebugStringW(L"Not enough data to process\n");
                    continue;
                }
            }
        }
    }

    in_pBuffer->uValidFrames -= uFramesConsumed;
    out_pBuffer->uValidFrames += uFramesProduced;

    if (out_pBuffer->uValidFrames == out_pBuffer->MaxFrames())
        out_pBuffer->eState = AK_DataReady;
    else
        out_pBuffer->eState = AK_DataNeeded;
}

AKRESULT WP_CeSoundFIRTraceConv_24FX::TimeSkip(AkUInt32 &io_uFrames)
{
    OutputDebugStringW(L"TimeSkip\n");
    return AK_DataReady;
}


//===========================================================================================

CsVector WP_CeSoundFIRTraceConv_24FX::UPOLS(const CsVector& xBuffer)
{
    CsVector output(m_bufferSize, 0);

    //If not filter is present, return output with only 0
    if (m_FDL_H.empty())
        return output;

	//Push new new input to FDL and update FDL with new input
    pushStreamToFDL(xBuffer);

    convoluteSignals();
    output = sumFDL();
    if (m_filterExRunning)
    {
        CsVector fadedOutput = filterExchange(output);
		m_filterExRunning = false;
		OutputDebugStringW(L"UPOLS exit with filter exchange\n");
		return fadedOutput;
    }

	OutputDebugStringW(L"UPOLS exit\n");
    return output;
}


void WP_CeSoundFIRTraceConv_24FX::convoluteSignals()
{
	//OutputDebugStringW(L"Convolution entry\n");
	/*std::wstring msg = L"Convolution with " + std::to_wstring(m_FDL_H.size()) + L" partitions and x partitions " + std::to_wstring(m_FDL_X[m_currentChannel].size()) + L"\n";
	OutputDebugStringW(msg.c_str());*/
    m_FDL_Result.clear();
    m_FDL_Result.resize(m_FDL_X[m_currentChannel].size(), CsVectorC(m_bufferSize + 1, CsC(0.0, 0.0)));
    for (size_t i{0}; i<m_FDL_X[m_currentChannel].size(); i++)
    {
        for (size_t j {0}; j<m_bufferSize + 1; j++)
        {
            m_FDL_Result[i][j] = m_FDL_H[i][j] * m_FDL_X[m_currentChannel][i][j];
        }
    }
	//OutputDebugStringW(L"Convolution exit\n");
}

void WP_CeSoundFIRTraceConv_24FX::partitioningAndWriteFilterToTemp(const CsVector& FIR)
{
    AkUInt16 P = std::ceil(FIR.size() / static_cast<float>(m_bufferSize));

    m_FDL_Htemp.clear();
    m_FDL_Htemp.resize(P, CsVectorC(m_bufferSize + 1, CsC(0.0, 0.0)));

    for (auto i{ 0 }; i < P; i++)
    {
        CsVector tempBuffer(2 * m_bufferSize, 0.0f);
        std::vector<std::complex<float>> fftOutput(m_bufferSize + 1, CsC(0.0, 0.0));
        for (auto j{ 0 }; j < m_bufferSize; j++)
        {
            if (i * m_bufferSize + j < FIR.size())
                tempBuffer[j] = FIR[i * m_bufferSize + j];
        }
        CsShape shape = { tempBuffer.size() };
        CsStride stride_in = { sizeof(float) };
        CsStride stride_out = { sizeof(std::complex<float>) };
        CsShape axes = { 0 };
        pocketfft::detail::r2c(
            shape,
            stride_in,
            stride_out,
            axes,
            pocketfft::FORWARD,
            tempBuffer.data(),
            fftOutput.data(),
            1.0f);
        m_FDL_Htemp.push_back(fftOutput);
    }
}

CsVector WP_CeSoundFIRTraceConv_24FX::combineFIRPasses(CsVector2& h2)
{
    int longestImpulse = [&h2]() {
        int maxLength = 0;
        for (const auto& impulse : h2)
        {
            if (impulse.size() > maxLength)
                maxLength = impulse.size();
        }
        return maxLength;
		}();

	CsVector combinedFIR (longestImpulse, 0.0f);
    std::vector<int> nonZeroIndex;
	//1. sqrt of energy of each impulse response h2
	//2. convolve sinc with sqrt of energy of each impulse response
	//3. sum the convolved signals to get the final FIR filter at (t) h(t) = sum(sqrt(h2[i][t]) * sinc(t))

    for (auto i{ 0 }; i < h2.size(); i++)
    {
        for (auto j{ 0 }; j < h2[i].size(); j++)
        {
            if (h2[i][j] != 0.0f)
            {
                h2[i][j] = std::sqrt(h2[i][j]);
                nonZeroIndex.push_back(j);
            }
        }
    }
    std::sort(nonZeroIndex.begin(), nonZeroIndex.end());
    auto new_End = std::unique(nonZeroIndex.begin(), nonZeroIndex.end());
    nonZeroIndex.erase(new_End, nonZeroIndex.end());

    for (auto k{ 0 }; k < nonZeroIndex.size(); k++)
    {
        for (auto l{ 0 }; l < h2.size(); l++)
        {
            if (nonZeroIndex[k] > h2[l].size() - 1)
                continue;
            else
                combinedFIR[nonZeroIndex[k]] += h2[l][nonZeroIndex[k]];
        }
    }
    /*for (auto k{ 0 }; k < nonZeroIndex.size(); k++)
    {
        float sum = 0.0f;
        for (auto l{0}; l<h2.size(); l++)
        {
            if(h2[l].size() <= nonZeroIndex[k])
				continue;
            if(h2[l][nonZeroIndex[k]] != 0.0f)
            {
                auto tempVec = linearConvolution(h2[l], sinc(static_cast<CeFreq>(l), m_currentSampleRate));
                sum += tempVec[nonZeroIndex[k]];
            }
        }
        combinedFIR[nonZeroIndex[k]] = sum;
    }*/
	combinedFIR = normaliseEnergy(combinedFIR);
	return combinedFIR;
}

CsVector WP_CeSoundFIRTraceConv_24FX::makeFDLBuffer(std::vector<AkReal32> xBuffer)
{
	//OutputDebugStringW(L"Making FDL buffer\n");
    CsVector tempBuffer (xBuffer.size()*2, 0.0f);
    for (size_t i{ 0 }; i < xBuffer.size(); i++)
    {
        tempBuffer[i] = xBuffer[i];
    }
    return tempBuffer;
}

void WP_CeSoundFIRTraceConv_24FX::initialiseFDLWithFilter(CsVector h)
{
    AkUInt16 P = std::ceil(h.size()/ static_cast<float>(m_bufferSize));
	
    for (auto i{ 0 }; i < P; i++)
    {
        CsVector tempBuffer(2 * m_bufferSize, 0.0f);
        std::vector<std::complex<float>> fftOutput(m_bufferSize + 1, CsC(0.0, 0.0));
        for (auto j{ 0 }; j < m_bufferSize; j++)
        {
            if (i * m_bufferSize + j < h.size())
                tempBuffer[j] = h[i * m_bufferSize + j];
        }
		CsShape shape = { tempBuffer.size() };
		CsStride stride_in = { sizeof(float) };
		CsStride stride_out = { sizeof(std::complex<float>) };
		CsShape axes = { 0 };
        pocketfft::detail::r2c(
            shape,
            stride_in,
            stride_out,
            axes,
            pocketfft::FORWARD,
            tempBuffer.data(),
            fftOutput.data(),
            1.0f);
        m_FDL_H.push_back(fftOutput);
    }
}

void WP_CeSoundFIRTraceConv_24FX::pushStreamToFDL(CsVector xBuffer)
{
	//OutputDebugStringW(L"Updating FDL for FDL_X\n");
    
    CsVectorC X (m_bufferSize + 1, CsC(0.0, 0.0));

    CsShape shape = { xBuffer.size() };
    CsStride stride_in = { sizeof(float) };
    CsStride stride_out = { sizeof(std::complex<float>) };
    CsShape axes = { 0 };
    pocketfft::detail::r2c(
        shape,
        stride_in,
        stride_out,
        axes,
        pocketfft::FORWARD,
        xBuffer.data(),
        X.data(),
		1.0f);


	m_FDL_X[m_currentChannel].insert(m_FDL_X[m_currentChannel].begin(), X);

    while (m_FDL_X[m_currentChannel].size() > m_FDL_H.size())
    {
        m_FDL_X[m_currentChannel].pop_back();
    }
}

CsVector WP_CeSoundFIRTraceConv_24FX::sumFDL()
{
	//OutputDebugStringW(L"Summing FDL\n");
    CsVectorC sum_complex (m_bufferSize + 1, CsC(0.0, 0.0));
    CsVector output(2 * m_bufferSize, 0.0f);


    for (CsVectorC& currentFDL : m_FDL_Result)
    {
		/*CsC maxVal = *std::max_element(currentFDL.begin(), currentFDL.end(), [](const CsC& a, const CsC& b) { return std::abs(a) < std::abs(b); });
        if (maxVal == CsC(0.0, 0.0))
            continue;*/
        for (size_t i{ 0 }; i < currentFDL.size(); i++)
        {
            sum_complex[i] += currentFDL[i];
        }
    }

    CsShape shape = { output.size() };
    CsStride stride_in = { sizeof(std::complex<float>) };
    CsStride stride_out = { sizeof(float) };
    CsShape axes = { 0 };
    pocketfft::detail::c2r(
        shape,
        stride_in,
        stride_out,
        axes,
        pocketfft::BACKWARD,
        sum_complex.data(),
		output.data(),
		1.0f/1024.0f);

    output.erase(output.begin(), output.begin() + m_bufferSize);
	output.shrink_to_fit();

	//OutputDebugStringW(L"Summing FDL exit\n");
    return output;
}

CsVector WP_CeSoundFIRTraceConv_24FX::filterExchange(CsVector oldX)
{
    CsVector newX(oldX.size(), 0.0f);
	CsVector Fout(oldX.size(), 0.0f);
	CsVector Fin(oldX.size(), 0.0f);

    //calculate new output
    m_FDL_Result.clear();
    m_FDL_Result.resize(m_FDL_Htemp.size(), CsVectorC(m_bufferSize + 1, CsC(0.0, 0.0)));
    for (size_t i{ 0 }; i < m_FDL_Result.size(); i++)
    {
        for (size_t j{ 0 }; j < m_bufferSize + 1; j++)
        {
            if(i < m_FDL_Htemp.size() && i < m_FDL_X[m_currentChannel].size())
            m_FDL_Result[i][j] = m_FDL_Htemp[i][j] * m_FDL_X[m_currentChannel][i][j];
        }
    }

    CsVector output(2 * m_bufferSize, 0.0f);
    CsVectorC sum_complex(m_bufferSize + 1, CsC(0.0, 0.0));

    for (CsVectorC& currentFDL : m_FDL_Result)
    {
        for (size_t i{ 0 }; i < currentFDL.size(); i++)
        {
            sum_complex[i] += currentFDL[i];
        }
    }

    CsShape shape = { output.size() };
    CsStride stride_in = { sizeof(std::complex<float>) };
    CsStride stride_out = { sizeof(float) };
    CsShape axes = { 0 };
    pocketfft::detail::c2r(
        shape,
        stride_in,
        stride_out,
        axes,
        pocketfft::BACKWARD,
        sum_complex.data(),
        output.data(),
        1.0f / 1024.0f);

    output.erase(output.begin(), output.begin() + m_bufferSize);
    output.shrink_to_fit();


    for (auto i{ 0 }; i < oldX.size(); i++)
    {
        Fout[i] = std::pow(std::cos((m_pi * i) / (2 * oldX.size())), 2);
        Fin[i] = std::pow(std::sin((m_pi * i) / (2 * oldX.size())), 2);
        newX[i] = Fout[i] * oldX[i] + Fin[i] * output[i];
	}
    
    m_FDL_H.clear();
	m_FDL_H = m_FDL_Htemp;
	m_FDL_H.shrink_to_fit();

    return newX;
}

void WP_CeSoundFIRTraceConv_24FX::defaultExecute(AkAudioBuffer* in_pBuffer, AkUInt32 in_ulnOffset, AkAudioBuffer* out_pBuffer)
{
    /*std::wstring msg = L"Default Execute, Offset: " + std::to_wstring(in_ulnOffset) + L"\n";
	OutputDebugStringW(msg.c_str());*/
    const AkUInt32 uNumChannels = in_pBuffer->NumChannels();

    AkUInt16 uFramesConsumed;
    AkUInt16 uFramesProduced;
    for (AkUInt32 i = 0; i < uNumChannels; ++i)
    {
        AkReal32* AK_RESTRICT pInBuf = (AkReal32 * AK_RESTRICT)in_pBuffer->GetChannel(i) + in_ulnOffset;
        AkReal32* AK_RESTRICT pOutBuf = (AkReal32 * AK_RESTRICT)out_pBuffer->GetChannel(i) + out_pBuffer->uValidFrames;

#if LIVE
        uFramesConsumed = 0;
        uFramesProduced = 0;
        while (uFramesConsumed < in_pBuffer->uValidFrames
            && uFramesProduced < out_pBuffer->MaxFrames())
        {
            // Execute DSP that consumes input and produces output at different rate here
            *pOutBuf++ = *pInBuf++;
            ++uFramesConsumed;
            ++uFramesProduced;
        }
#endif
#if LIVE == 0
        std::vector<float> convInput(pInBuf, pInBuf + in_pBuffer->uValidFrames);
        
        for(auto j{0}; j<in_pBuffer->uValidFrames; j++)
        {
            /*std::wstring msg = L"Input sample: " + std::to_wstring(*pInBuf) + L"\n";
			OutputDebugStringW(msg.c_str());
            convInput.push_back(*pInBuf++);*/
			
            if(!m_linConvOverflow.empty() && m_linConvOverflow[i].size()!=0)
            {
				//OutputDebugStringW(L"Adding overflow\n");
                for (auto k{ 0 }; k < m_linConvOverflow[i].size(); k++)
                {
					if (j < m_linConvOverflow[i][k].size())
                    convInput[j] *= m_linConvOverflow[i][k][j];
                }
            }
        }
		if (!m_linConvOverflow.empty())
        {
            for (auto l{ 0 }; l < m_linConvOverflow[i].size(); l++)
            {
                if (m_linConvOverflow[i][l].size() <= in_pBuffer->uValidFrames)
                    m_linConvOverflow[i][l].erase(m_linConvOverflow[i][l].begin(), m_linConvOverflow[i][l].end());
                else
                    m_linConvOverflow[i].erase(m_linConvOverflow[i].begin() + l);
            }
        }
		std::vector<float> convOutput = linearConvolution(convInput, m_testFIR);
        for(auto n{0}; n<out_pBuffer->MaxFrames(); n++)
            *pOutBuf++ = convOutput[n];

        convOutput.erase(convOutput.begin(), convOutput.begin() + out_pBuffer->MaxFrames());
        if(m_linConvOverflow.empty())
        {
            m_linConvOverflow.resize(uNumChannels);
        }
		m_linConvOverflow[i].push_back(convOutput);

		uFramesConsumed = convInput.size();
		uFramesProduced = convInput.size(); 
#endif
    }

    in_pBuffer->uValidFrames -= uFramesConsumed;
    out_pBuffer->uValidFrames += uFramesProduced;

    if (in_pBuffer->eState == AK_NoMoreData && in_pBuffer->uValidFrames == 0)
        out_pBuffer->eState = AK_NoMoreData;
    else if (out_pBuffer->uValidFrames == out_pBuffer->MaxFrames())
        out_pBuffer->eState = AK_DataReady;
    else
        out_pBuffer->eState = AK_DataNeeded;
}

//void WP_CeSoundFIRTraceConv_24FX::SetCustomData(const UEDataStruct& filterData)
//{
//    initialiseAndUpdateFilter(filterData.h2);
//}

