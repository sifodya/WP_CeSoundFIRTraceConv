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
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

AK::IAkPlugin* CreateWP_CeSoundFIRTraceConv_24FX(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, WP_CeSoundFIRTraceConv_24FX());
}

AK::IAkPluginParam* CreateWP_CeSoundFIRTraceConv_24FXParams(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, WP_CeSoundFIRTraceConv_24FXParams());
}

AK_IMPLEMENT_PLUGIN_FACTORY(WP_CeSoundFIRTraceConv_24FX, AkPluginTypeEffect, WP_CeSoundFIRTraceConv_24Config::CompanyID, WP_CeSoundFIRTraceConv_24Config::PluginID)

WP_CeSoundFIRTraceConv_24FX::WP_CeSoundFIRTraceConv_24FX()
    : m_pParams(nullptr)
    , m_pAllocator(nullptr)
    , m_pContext(nullptr)
{}

WP_CeSoundFIRTraceConv_24FX::~WP_CeSoundFIRTraceConv_24FX()
{
    m_vpGameData = nullptr;
    delete m_vpGameData;
}

AKRESULT WP_CeSoundFIRTraceConv_24FX::Init(AK::IAkPluginMemAlloc* in_pAllocator, AK::IAkEffectPluginContext* in_pContext, AK::IAkPluginParam* in_pParams, AkAudioFormat& in_rFormat)
{
    m_pParams = (WP_CeSoundFIRTraceConv_24FXParams*)in_pParams;
    m_pAllocator = in_pAllocator;
    m_pContext = in_pContext;
    
    OutputDebugStringW(L"Init 7\n");

    /*drwav wav;
    if (!drwav_init_file(&wav, "C:\\Users\\cedri\\Desktop\\ImpulsRecTest1 - Copy.wav", NULL))
    {
        throw std::runtime_error("Failed to open Wav file");
    }

    m_testFIR.resize(wav.totalPCMFrameCount * wav.channels);
    drwav_read_pcm_frames_f32(&wav, wav.totalPCMFrameCount, m_testFIR.data());

    drwav_uninit(&wav);*/

    /*std::fstream file("C:\\Users\\cedri\\Desktop\\output.txt");
    double x;
    while (file >> x)
    {
        m_testFIR.push_back(x);
    }*/
    
	
    /*std::wstring msg = L"Vector size: " + std::to_wstring(m_testFIR.size()) + L"\n";
    OutputDebugStringW(msg.c_str());
    for (double v : m_testFIR) {
        std::cout << v << "\n";
    }*/


    /*OutputDebugStringW(L"Before partition\n");
    CsVector2 partitionedFilter{ partitioningIR_single(m_testFIR) };
	OutputDebugStringW(L"Before initialise\n");
    initialiseAndUpdateFDLWithFilter(partitionedFilter);*/

	m_testFIR.resize(1024, 1.0f);
    return AK_Success;
}

AKRESULT WP_CeSoundFIRTraceConv_24FX::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}

AKRESULT WP_CeSoundFIRTraceConv_24FX::Reset()
{
    return AK_Success;
}

AKRESULT WP_CeSoundFIRTraceConv_24FX::GetPluginInfo(AkPluginInfo& out_rPluginInfo)
{
    out_rPluginInfo.eType = AkPluginTypeEffect;
    out_rPluginInfo.bIsInPlace = false;
	out_rPluginInfo.bCanProcessObjects = false;
    out_rPluginInfo.uBuildVersion = AK_WWISESDK_VERSION_COMBINED;
    return AK_Success;
}

void WP_CeSoundFIRTraceConv_24FX::Execute(AkAudioBuffer* in_pBuffer, AkUInt32 in_ulnOffset, AkAudioBuffer* out_pBuffer)
{
    //Unit Test and assertions
    /*AkUInt32 dataSize{ 0 };
    m_pContext->GetPluginCustomGameData(m_vpGameData, dataSize);
    if(m_vpGameData == nullptr)
    {
        
		defaultExecute(in_pBuffer, in_ulnOffset, out_pBuffer);
		OutputDebugStringW(L"No Data\n");
        return;
	}
    UEDataStruct* gameData = static_cast<UEDataStruct*> (m_vpGameData);
    if (m_dataVersion != gameData->version)
    {
		OutputDebugStringW(L"Updating filter\n");
        m_dataVersion = gameData->version;
        initialiseAndUpdateFilter(gameData->impulses);
    }*/


    /*if(m_pContext->CanPostMonitorData())
    {
		CsVector nonZeroSamples;
        for (const CsVector& impulse : gameData->impulses)
        {
            for (const float& sample : impulse)
            {
                if(sample != 0.0f)
                {
                    nonZeroSamples.push_back(sample);
				}
            }
		}
		m_pContext->PostMonitorData(&nonZeroSamples, sizeof(nonZeroSamples));
    }*/
	std::wstring msg = L"Block Size: " + std::to_wstring(in_pBuffer->MaxFrames()) + L"\n";
	OutputDebugStringW(msg.c_str());
    defaultExecute(in_pBuffer, in_ulnOffset, out_pBuffer);
    //TODO bypass when filter is empty

   // const AkUInt32 uNumChannels = in_pBuffer->NumChannels();

   // AkUInt16 uFramesConsumed;
   // AkUInt16 uFramesProduced;
   // for (AkUInt32 i = 0; i < uNumChannels; ++i)
   // {
   //     AkReal32* AK_RESTRICT pInBuf = (AkReal32 * AK_RESTRICT)in_pBuffer->GetChannel(i) + in_ulnOffset;
   //     AkReal32* AK_RESTRICT pOutBuf = (AkReal32 * AK_RESTRICT)out_pBuffer->GetChannel(i) + out_pBuffer->uValidFrames;

   //     uFramesConsumed = 0;
   //     uFramesProduced = 0;
   //     while (uFramesConsumed < in_pBuffer->uValidFrames
   //         && uFramesProduced < out_pBuffer->MaxFrames())
   //     {
   //         
   //         while (uFramesConsumed < m_bufferSize)
   //         {
			//	//TODO safety check pInBuf is not out of bounds
   //             m_UPOLSInput.push_back(*pInBuf++);
			//    ++uFramesConsumed;
   //         }
			//CsVector output = UPOLS(m_UPOLSInput);
   //         while (uFramesProduced < out_pBuffer->MaxFrames())
   //         {
   //             *pOutBuf++ = output[0];
   //             output.erase(output.begin());
   //             ++uFramesProduced;
			//}
   //         output.clear();
			//m_UPOLSInput.clear();
   //         // Execute DSP that consumes input and produces output at different rate here
   //         //*pOutBuf++ = *pInBuf++;
   //         
   //     }
   // }

   // in_pBuffer->uValidFrames -= uFramesConsumed;
   // out_pBuffer->uValidFrames += uFramesProduced;

   // if (in_pBuffer->eState == AK_NoMoreData && in_pBuffer->uValidFrames == 0)
   //     out_pBuffer->eState = AK_NoMoreData;
   // else if (out_pBuffer->uValidFrames == out_pBuffer->MaxFrames())
   //     out_pBuffer->eState = AK_DataReady;
   // else
   //     out_pBuffer->eState = AK_DataNeeded;
}

AKRESULT WP_CeSoundFIRTraceConv_24FX::TimeSkip(AkUInt32 &io_uFrames)
{
    return AK_DataReady;
}

//===========================================================================================

CsVector WP_CeSoundFIRTraceConv_24FX::UPOLS(const CsVector& xBuffer)
{
    CsVector output(xBuffer.size(), 0);

    //If not filter is present, return output with only 0
    if (m_FDL_H.empty())
        return output;

	//Push new new input to FDL and update FDL with new input
    updateFDL(makeFDLBuffer(xBuffer));

    convoluteSignals();
    output = sumFDL();

	OutputDebugStringW(L"UPOLS exit\n");
    return output;
}

void WP_CeSoundFIRTraceConv_24FX::initialiseAndUpdateFilter(const CsVector2& filter)
{
    CsVector2 partitionedFilter{ partitioningIR(filter) };
    initialiseAndUpdateFDLWithFilter(partitionedFilter);
}

void WP_CeSoundFIRTraceConv_24FX::convoluteSignals()
{
	OutputDebugStringW(L"Convolution entry\n");
    m_FDL_Result.clear();
    m_FDL_Result.resize(m_FDL_H.size(), CsVectorC(m_FDL_H[0].size(), CsC(0.0, 0.0)));
    for (size_t i{0}; i<m_FDL_X.size(); i++)
    {
        for (size_t j {0}; j<m_FDL_H[i].size(); j++)
        {
            m_FDL_Result[i][j] = m_FDL_H[i][j] * m_FDL_X[i][j];
        }
    }
	OutputDebugStringW(L"Convolution exit\n");
}

CsVector2 WP_CeSoundFIRTraceConv_24FX::partitioningIR(const CsVector2& FIR)
{
    CsVector combinedFIR = combineFIRPasses(FIR);
    AkUInt16 P = std::ceil(combinedFIR.size() / static_cast<double>(m_bufferSize));
    CsVector2 partitionedFIR(P, CsVector(2 * m_bufferSize, 0.0f));

    for (size_t i{ 0 }; i<=partitionedFIR.size(); i++)
    {
        for(int j {0}; j <= m_bufferSize; j++)
        {
            if(i * m_bufferSize + j < combinedFIR.size())
            partitionedFIR[i][j] = combinedFIR[i * m_bufferSize + j];
        }
    }

    return partitionedFIR;
}

CsVector2 WP_CeSoundFIRTraceConv_24FX::partitioningIR_single(const CsVector& FIR)
{
	OutputDebugStringW(L"Partition entry\n");
    AkUInt16 P = std::ceil(FIR.size() / static_cast<double>(m_bufferSize));
    CsVector2 partitionedFIR(P, CsVector(2 * m_bufferSize, 0.0f));

    for (size_t i{ 0 }; i <= partitionedFIR.size(); i++)
    {
        for (int j{ 0 }; j < m_bufferSize; j++)
        {
            if (i * m_bufferSize + j < FIR.size())
            {
                //OutputDebugStringW(L"Partitioning\n");
                partitionedFIR[i][j] = FIR[i * m_bufferSize + j];
            }
        }
    }
	OutputDebugStringW(L"Partition exit\n");
    return partitionedFIR;
}

//TODO: implement method for combining arrays with dissertation of kurt
CsVector WP_CeSoundFIRTraceConv_24FX::combineFIRPasses(const CsVector2& FIR)
{
    auto largestVector = std::max_element(FIR.begin(), FIR.end(), [](const CsVector& a, const CsVector& b) {return a.size() < b.size(); });

    CsVector combinedFIR(largestVector->size(), 0.0f);
    for (size_t i{ 0 }; i < largestVector->size(); i++)
    {
        /*for (size_t j{ 0 }; j < FIR->size(); j++)
        {
            if (i < (*FIR)[j].size())
            {
                combinedFIR[i] += (*FIR)[j][i];
            }
        }*/

        for (CsVector currentFIR : FIR)
        {
            if (i < currentFIR.size())
            {
                combinedFIR[i] += currentFIR[i];
            }
        }
    }
    return combinedFIR;
}

CsVectorC WP_CeSoundFIRTraceConv_24FX::FFT(CsVector& xStream)
{
    size_t N = xStream.size();
    CsVectorC W(N), X(N), X_U, X_G;
    

    if (N == 1)
    {
        CsC c{ xStream[0], 0};
        X[0] = c;
        return X;
    }

    CsVector xStream_G;
    CsVector xStream_U;

    for (size_t j{ 0 }; j < xStream.size(); j += 2)
        xStream_G.push_back(xStream[j]);

    for(size_t g{1}; g<xStream.size(); g += 2)
        xStream_U.push_back(xStream[g]);

    X_G = FFT(xStream_G);
    X_U = FFT(xStream_U);

    for (int k {0}; k <= N - 1; k++)
    {
        double kN{ k / static_cast<double>(N) };
        W[k] = std::exp(m_minus_i * 2.0 * m_pi * kN);
    }

    for (int i{ 0 }; i < N / 2; i++)
    {
        X[i] = X_G[i] + W[i] * X_U[i];
        X[i+N/2] = X_G[i] - W[i] * X_U[i];
    }
    return X;
}

CsVectorC WP_CeSoundFIRTraceConv_24FX::FFT_C(CsVectorC& xStream)
{
    size_t N = xStream.size();
    CsVectorC W(N), X(N), X_U, X_G;

    if (N == 1)
    {
        X[0] = xStream[0];
        return X;
    }

    CsVectorC xStream_G;
    CsVectorC xStream_U;

    for (size_t j{ 0 }; j < N; j += 2)
        xStream_G.push_back(xStream[j]);
    for (size_t j{ 1 }; j < N; j += 2)
        xStream_U.push_back(xStream[j]);

    X_G = FFT_C(xStream_G);
    X_U = FFT_C(xStream_U);

    for (size_t k{ 0 }; k <= N-1; k++)
    {
        double kN = k / N;
        W[k] = std::exp(CsC(0, -2.0 * m_pi * kN));
    }

    for (size_t i{ 0 }; i < N / 2; i++)
    {
        X[i] = X_G[i] + W[i] * X_U[i];
        X[i + N / 2] = X_G[i] - W[i] * X_U[i];
    }

    return X;
}

CsVector WP_CeSoundFIRTraceConv_24FX::makeFDLBuffer(std::vector<AkReal32> xBuffer)
{
	OutputDebugStringW(L"Making FDL buffer\n");
    CsVector tempBuffer (xBuffer.size()*2, 0.0f);
    for (size_t i{ 0 }; i < xBuffer.size(); i++)
    {
        tempBuffer[i] = xBuffer[i];
        //tempBuffer[i + m_FDLBuffer.size() / 2] = xBuffer[i];
    }
    return tempBuffer;
}

void WP_CeSoundFIRTraceConv_24FX::initialiseAndUpdateFDLWithFilter(CsVector2 h)
{
    CsVector2C H(h.size(), CsVectorC (h[0].size()));

	OutputDebugStringW(L"Before FFT \n");
    for (size_t i{0}; i<h.size(); i++)
         H[i] = FFT(h[i]);
   

	OutputDebugStringW(L"After FFT \n");
    if (m_FDL_H.empty())
    {
		OutputDebugStringW(L"Initialising FDL\n");
		m_FDL_H.resize(H.size(), CsVectorC(H[0].size()));
        for (size_t j{ 0 }; j < H.size(); j++)
            m_FDL_H[j] = H[j];
    }
    else
    {
		OutputDebugStringW(L"Updating FDL\n");
        if (m_filterExRunning)
            return;
        for (size_t j{ 0 }; j < H.size(); j++)
            m_FDL_Htemp[j] = H[j];

        filterExchange();
    }
}

void WP_CeSoundFIRTraceConv_24FX::updateFDL(CsVector xBuffer)
{
	OutputDebugStringW(L"Updating FDL for FDL_X\n");
    CsVectorC X;
    //for (size_t i{ 0 }; i < xBuffer.size(); i++)
        X = FFT(xBuffer);
    
    m_FDL_X.push_back(X);

    if (m_FDL_X.size() > m_FDL_H.size())
        m_FDL_X.pop_back();
}

CsVector WP_CeSoundFIRTraceConv_24FX::sumFDL()
{
	OutputDebugStringW(L"Summing FDL\n");
    CsVectorC sum (m_FDL_Result[0].size(), CsC(0.0, 0.0));
    CsVector output(sum.size(), 0.0f);

    for (CsVectorC& currentFDL : m_FDL_Result)
    {
        for (size_t i{ 0 }; i < currentFDL.size(); i++)
            sum[i] += currentFDL[i];
    }

    output = IFFT(sum);

    for (size_t j{ 0 }; j < output.size() / 2; j++)
        output.pop_back();

	OutputDebugStringW(L"Summing FDL exit\n");
    return output;
}

CsVector WP_CeSoundFIRTraceConv_24FX::IFFT(CsVectorC& sum)
{
    size_t N = sum.size();
    CsVectorC X_conj(N);

    for (size_t i{ 0 }; i < N; i++)
        X_conj[i] = std::conj(sum[i]);

    CsVectorC X = FFT_C(X_conj);

    CsVector result(N);
    for (size_t i{ 0 }; i < N; i++)
    {
        CsC val = std::conj(X[i] / static_cast<double>(N));
        result[i] = static_cast<float>(val.real());
    }

    return result;
}


void WP_CeSoundFIRTraceConv_24FX::filterExchange()
{
    m_filterExRunning = true;
    m_FDL_H.clear();
    m_FDL_H = m_FDL_Htemp;
    m_FDL_Htemp.clear();
    m_filterExRunning = false;
}

void WP_CeSoundFIRTraceConv_24FX::defaultExecute(AkAudioBuffer* in_pBuffer, AkUInt32 in_ulnOffset, AkAudioBuffer* out_pBuffer)
{
	OutputDebugStringW(L"Default Execute\n");
    const AkUInt32 uNumChannels = in_pBuffer->NumChannels();

    AkUInt16 uFramesConsumed;
    AkUInt16 uFramesProduced;
    for (AkUInt32 i = 0; i < uNumChannels; ++i)
    {
        AkReal32* AK_RESTRICT pInBuf = (AkReal32 * AK_RESTRICT)in_pBuffer->GetChannel(i) + in_ulnOffset;
        AkReal32* AK_RESTRICT pOutBuf = (AkReal32 * AK_RESTRICT)out_pBuffer->GetChannel(i) + out_pBuffer->uValidFrames;

        uFramesConsumed = 0;
        uFramesProduced = 0;
        //while (uFramesConsumed < in_pBuffer->uValidFrames
        //    && uFramesProduced < out_pBuffer->MaxFrames())
        //{
        //    // Execute DSP that consumes input and produces output at different rate here
        //    *pOutBuf++ = *pInBuf++;
        //    ++uFramesConsumed;
        //    ++uFramesProduced;
        //}
		CsVector convInput;
        for(auto j{0}; j<in_pBuffer->uValidFrames; j++)
        {
            convInput.push_back(*pInBuf++);
            if(!m_linConvOverflow.empty())
            {
                for (auto k{ 0 }; k < m_linConvOverflow.size(); k++)
                    convInput[j] += m_linConvOverflow[k][j];
            }
        }
		CsVector convOutput = linearConvolution(convInput, m_testFIR);
        for(auto n{0}; n<out_pBuffer->MaxFrames(); n++)
            *pOutBuf++ = convOutput[n];

        convOutput.erase(convOutput.begin(), convOutput.begin() + out_pBuffer->MaxFrames());
		m_linConvOverflow.push_back(convOutput);

		uFramesConsumed = convInput.size();
		uFramesProduced = convOutput.size();
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
//    initialiseAndUpdateFilter(filterData.impulses);
//}

CsVector WP_CeSoundFIRTraceConv_24FX::linearConvolution(const CsVector& input, const CsVector& filter)
{
	OutputDebugStringW(L"Linear convolution entry\n");
    size_t N = input.size();
    size_t M = filter.size();
    CsVector output(N + M - 1, 0.0f);
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
	OutputDebugStringW(L"Linear convolution exit\n");
	return output;
}