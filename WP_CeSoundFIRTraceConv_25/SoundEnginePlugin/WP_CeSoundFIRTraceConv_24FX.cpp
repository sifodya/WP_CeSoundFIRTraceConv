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

#include <AK/AkWwiseSDKVersion.h>

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
{
}

WP_CeSoundFIRTraceConv_24FX::~WP_CeSoundFIRTraceConv_24FX()
{
}

AKRESULT WP_CeSoundFIRTraceConv_24FX::Init(AK::IAkPluginMemAlloc* in_pAllocator, AK::IAkEffectPluginContext* in_pContext, AK::IAkPluginParam* in_pParams, AkAudioFormat& in_rFormat)
{
    m_pParams = (WP_CeSoundFIRTraceConv_24FXParams*)in_pParams;
    m_pAllocator = in_pAllocator;
    m_pContext = in_pContext;

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
    void* gameData = nullptr;
    AkUInt32 dataSize{ 0 };
    m_pContext->GetPluginCustomGameData(gameData, dataSize);
    if (gameData != nullptr)
    {
        UEDataStruct* s_gameData = static_cast<UEDataStruct*> (gameData);
        if (dataVersion != s_gameData->version)
        {
            dataVersion = s_gameData->version;
            FilterIsUpdated(s_gameData->impulses);
        }
    }
    const AkUInt32 uNumChannels = in_pBuffer->NumChannels();
    AkUInt32 frames = in_pBuffer->uValidFrames;
    bufferSize = in_pBuffer->uValidFrames;

    AkUInt16 uFramesConsumed;
    AkUInt16 uFramesProduced;

    for (AkUInt32 ch = 0; ch < uNumChannels; ++ch)
    {
        AkReal32* in = (AkReal32*)in_pBuffer->GetChannel(ch) + in_ulnOffset;
        AkReal32* out = (AkReal32*)out_pBuffer->GetChannel(ch) + in_ulnOffset;

        // Copy input chunk into your DSP container
        CsVector currentBuffer(frames);
        for (AkUInt32 n = 0; n < frames; ++n)
        {
            currentBuffer[n] = in[n];
        }

        // Process entire chunk at once
        CsVector convolutedBuffer = UPOLS(&currentBuffer);

        // Write result back
        for (AkUInt32 n = 0; n < frames; ++n)
        {
            out[n] = convolutedBuffer[n];
        }
    }

    /*for (AkUInt32 i = 0; i < uNumChannels; ++i)
    {
        CsVector currentBuffer(in_pBuffer->uValidFrames, 0.0f);
        for (int numSamples{ 0 }; numSamples < in_pBuffer->uValidFrames; numSamples++)
        {
            AkReal32* in = (AkReal32*)in_pBuffer->GetChannel(i) + in_ulnOffset;
            currentBuffer[numSamples] = in[numSamples];
        }
        CsVector convolutedBuffer = UPOLS(&currentBuffer);
        for (int numOfFrames{ 0 }; numOfFrames < out_pBuffer->uValidFrames; numOfFrames++)
        {
            AkReal32* out = (AkReal32*)out_pBuffer->GetChannel(i) + in_ulnOffset;
            out[numOfFrames] = convolutedBuffer[numOfFrames];
        }*/
        //============================================================================================================================
        //AkReal32* AK_RESTRICT pInBuf = (AkReal32* AK_RESTRICT)in_pBuffer->GetChannel(i) + in_ulnOffset;
        //AkReal32* AK_RESTRICT pOutBuf = (AkReal32* AK_RESTRICT)out_pBuffer->GetChannel(i) +  out_pBuffer->uValidFrames;

        //uFramesConsumed = 0;
        //uFramesProduced = 0;
        //while (uFramesConsumed < in_pBuffer->uValidFrames
        //    && uFramesProduced < out_pBuffer->MaxFrames())
        //{
        //    //UPOLS()?????
        //     // Execute DSP that consumes input and produces output at different rate here
        //    *pOutBuf++ = *pInBuf++;
        //    ++uFramesConsumed;
        //    ++uFramesProduced;
        //}
    //}

    //in_pBuffer->uValidFrames -= uFramesConsumed;
    //out_pBuffer->uValidFrames += uFramesProduced;

    out_pBuffer->uValidFrames = in_pBuffer->uValidFrames;

    if (in_pBuffer->eState == AK_NoMoreData && in_pBuffer->uValidFrames == 0)
        out_pBuffer->eState = AK_NoMoreData;
    else if (out_pBuffer->uValidFrames == out_pBuffer->MaxFrames())
        out_pBuffer->eState = AK_DataReady;
    else
        out_pBuffer->eState = AK_DataNeeded;
}

AKRESULT WP_CeSoundFIRTraceConv_24FX::TimeSkip(AkUInt32 &io_uFrames)
{
    return AK_DataReady;
}

//===========================================================================================

CsVector WP_CeSoundFIRTraceConv_24FX::UPOLS(CsVector* xBuffer)
{
    //CsVector samples(xBuffer->uValidFrames);
    CsVector output(xBuffer->size(), 0);
    /*for (int i{ 0 }; i < xBuffer->uValidFrames; i++)
        samples[i] = (*xBuffer)[i];*/

    if (FDL_H.empty())
        return output;
    UpdateFDL(MakeFDLBuffer((*xBuffer)));
    ConvoluteSignals();
    output = SumFDL();

    return output;
}

void WP_CeSoundFIRTraceConv_24FX::FilterIsUpdated(CsVector2 filter)
{
    CsVector2 interpolatedFilter{ InterpolateData(filter) };
    CsVector2 partitionedFilter{ PartitioningIR(&interpolatedFilter) };
    InitialiseFDL(partitionedFilter);
}

void WP_CeSoundFIRTraceConv_24FX::ConvoluteSignals()
{
    for (size_t i{0}; i<FDL_X.size(); i++)
    {
        for (size_t j {0}; j<FDL_X[i].size(); j++)
        {
            FDL_Result[i][j] = FDL_H[i][j] * FDL_X[i][j];
        }
    }
}

CsVector2 WP_CeSoundFIRTraceConv_24FX::PartitioningIR(CsVector2* FIR)
{
    CsVector combinedFIR = CombineFIRPasses(FIR);
    AkUInt16 P = std::ceil(combinedFIR.size() / bufferSize);
    CsVector2 partitionedFIR(P, CsVector(2 * bufferSize, 0.0f));

    for (size_t i{ 0 }; i<=partitionedFIR.size(); i++)
    {
        for(int j {0}; j <= bufferSize; j++)
        {
            if(j<partitionedFIR[i].size())
            partitionedFIR[i][j] = combinedFIR[i * bufferSize + j];
        }
    }

    return partitionedFIR;
}


//TODO: implement method for combining arrays with dissertation of kurt
CsVector WP_CeSoundFIRTraceConv_24FX::CombineFIRPasses(CsVector2* FIR)
{
    auto largestVector = std::max_element(FIR->begin(), FIR->end(), [](const CsVector& a, const CsVector& b) {return a.size() < b.size(); });

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

        for (CsVector currentFIR : *FIR)
        {
            if (i < currentFIR.size())
            {
                combinedFIR[i] += currentFIR[i];
            }
        }
    }
    return combinedFIR;
}

CsVectorC WP_CeSoundFIRTraceConv_24FX::FFT(CsVector* xStream)
{
    size_t N = xStream->size();
    CsVectorC W(N), X(N), X_U, X_G;
    

    if (N == 1)
    {
        CsC c{ (*xStream)[0], 0};
        X[0] = c;
        return X;
    }

    CsVector xStream_G;
    CsVector xStream_U;

    for (size_t j{ 0 }; j < xStream->size(); j += 2)
        xStream_G.push_back((*xStream)[j]);

    for(size_t g{1}; g<xStream->size(); g += 2)
        xStream_U.push_back((*xStream)[g]);

    X_G = FFT(&xStream_G);
    X_U = FFT(&xStream_U);

    for (int k {0}; k <= N - 1; k++)
    {
        double kN{ k / static_cast<double>(N) };
        W[k] = std::exp(minus_i * 2.0 * pi * kN);
    }

    for (int i{ 0 }; i < N / 2; i++)
    {
        X[i] = X_G[i] + W[i] * X_U[i];
        X[i+N/2] = X_G[i] - W[i] * X_U[i];
    }
    return X;
}

CsVectorC WP_CeSoundFIRTraceConv_24FX::FFT_C(CsVectorC* xStream)
{
    size_t N = xStream->size();
    CsVectorC W(N), X(N), X_U, X_G;

    if (N == 1)
    {
        X[0] = (*xStream)[0];
        return X;
    }

    CsVectorC xStream_G;
    CsVectorC xStream_U;

    for (size_t j{ 0 }; j < N; j += 2)
        xStream_G.push_back((*xStream)[j]);
    for (size_t j{ 1 }; j < N; j += 2)
        xStream_U.push_back((*xStream)[j]);

    X_G = FFT_C(&xStream_G);
    X_U = FFT_C(&xStream_U);

    for (size_t k{ 0 }; k <= N-1; k++)
    {
        double kN = k / N;
        W[k] = std::exp(CsC(0, -2.0 * pi * kN));
    }

    for (size_t i{ 0 }; i < N / 2; i++)
    {
        X[i] = X_G[i] + W[i] * X_U[i];
        X[i + N / 2] = X_G[i] - W[i] * X_U[i];
    }

    return X;
}

CsVector WP_CeSoundFIRTraceConv_24FX::MakeFDLBuffer(std::vector<AkReal32> xBuffer)
{
    CsVector tempBuffer (xBuffer.size()*2);
    for (size_t i{ 0 }; i < FDLBuffer.size()/2; i++)
    {
        tempBuffer[i] = FDLBuffer[i];
        tempBuffer[i + FDLBuffer.size() / 2] = xBuffer[i];
    }
    return tempBuffer;
}

void WP_CeSoundFIRTraceConv_24FX::InitialiseFDL(CsVector2 h)
{
    CsVector2C H;
    for (size_t i{0}; i<h.size(); i++)
        H[i] = FFT(&h[i]);
   
    if (FDL_H.empty())
    {
        for (size_t j{ 0 }; j < H.size(); j++)
            FDL_H[j] = H[j];
    }
    else
    {
        if (filterExRunning)
            return;
        for (size_t j{ 0 }; j < H.size(); j++)
            FDL_Htemp[j] = H[j];

        FilterExchange();
    }
}

void WP_CeSoundFIRTraceConv_24FX::UpdateFDL(CsVector xBuffer)
{
    CsVectorC X;
    for (size_t i{ 0 }; i < xBuffer.size(); i++)
        X = FFT(&xBuffer);
    
    FDL_X.push_back(X);

    if (FDL_X.size() > FDL_H.size())
        FDL_X.pop_back();
}

CsVector WP_CeSoundFIRTraceConv_24FX::SumFDL()
{
    CsVectorC sum (FDL_Result[0].size(), 0);
    CsVector output;

    for (CsVectorC currentFDL : FDL_Result)
    {
        for (size_t i{ 0 }; i < currentFDL.size(); i++)
            sum[i] += currentFDL[i];
    }

    output = IFFT(&sum);

    for (size_t j{ 0 }; j < output.size() / 2; j++)
        output.pop_back();

    return output;
}

CsVector WP_CeSoundFIRTraceConv_24FX::IFFT(CsVectorC* sum)
{
    size_t N = sum->size();
    CsVectorC X_conj(N);

    for (size_t i{ 0 }; i < N; i++)
        X_conj[i] = std::conj((*sum)[i]);

    CsVectorC X = FFT_C(&X_conj);

    CsVector result(N);
    for (size_t i{ 0 }; i < N; i++)
    {
        CsC val = std::conj(X[i] / static_cast<double>(N));
        result[i] = static_cast<float>(val.real());
    }

    return result;
}

CsVector2 WP_CeSoundFIRTraceConv_24FX::InterpolateData(CsVector2 UE_Data)
{
    for (CsVector currentPass : UE_Data)
    {
        int firstZero, firstNonZero;
        bool firstZeroFound{ false };
        bool firstNonZeroFound{ false };
        for (int i {0}; i<currentPass.size(); i++)
        {
            if (currentPass[i] == 0.0f && !firstZeroFound)
            {
                firstZero = i - 1;
                firstZeroFound = true;
            }
            if (currentPass[i] != 0.0f && firstZeroFound)
            {
                firstNonZero = i;
                firstNonZeroFound = true;
            }
            if (firstZeroFound && firstNonZeroFound)
            {
                double energy{ 0 };
                if (firstZero == -1)
                {
                    firstZero = 0;
                    energy = 1.0f - currentPass[firstNonZero];
                }
                else
                    energy = currentPass[firstZero] - currentPass[firstNonZero];
                int distance = firstNonZero - firstZero;
                double energyPerStep = energy / distance;
                for (int j{ 0 }; j < distance; j++)
                    currentPass[firstZero + 1 + j] = currentPass[firstZero] - energyPerStep * j;

                firstZeroFound = false;
                firstNonZeroFound = false;
            }

        }
    }
    return UE_Data;
}

void WP_CeSoundFIRTraceConv_24FX::FilterExchange()
{
    filterExRunning = true;
    FDL_H.clear();
    FDL_H = FDL_Htemp;
    FDL_Htemp.clear();
    filterExRunning = false;
}

//void WP_CeSoundFIRTraceConv_24FX::SetCustomData(const UEDataStruct& filterData)
//{
//    FilterIsUpdated(filterData.impulses);
//}