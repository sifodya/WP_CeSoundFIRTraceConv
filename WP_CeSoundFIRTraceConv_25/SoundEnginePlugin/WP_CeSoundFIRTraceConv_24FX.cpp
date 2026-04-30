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

#define FFTW_STATIC
extern "C"
{
#include "fftw/fftw3.h"
}

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
    OutputDebugStringW(L"Constructor\n");
}

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
    
    OutputDebugStringW(L"Init\n");

    std::fstream file("C:\\Users\\cedri\\Desktop\\output.txt");
    double x;
    while (file >> x)
    {
        m_testFIR.push_back(x);
    }
    
    CsVector2 partitionedFilter{ partitioningIR_single(m_testFIR) };
    initialiseAndUpdateFDLWithFilter(partitionedFilter, m_FDL_H);

	//m_testFIR.resize(24000, 1.0f);
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

	/*std::wstring msg = L"Block Size: " + std::to_wstring(in_pBuffer->MaxFrames()) + L"\n";
	OutputDebugStringW(msg.c_str());*/
    //defaultExecute(in_pBuffer, in_ulnOffset, out_pBuffer);
    //TODO bypass when filter is empty

    const AkUInt32 uNumChannels = in_pBuffer->NumChannels();

    AkUInt16 uFramesConsumed;
    AkUInt16 uFramesProduced;
    for (AkUInt32 i = 0; i < uNumChannels; ++i)
    {
        AkReal32* AK_RESTRICT pInBuf = (AkReal32 * AK_RESTRICT)in_pBuffer->GetChannel(i) + in_ulnOffset;
        AkReal32* AK_RESTRICT pOutBuf = (AkReal32 * AK_RESTRICT)out_pBuffer->GetChannel(i) + out_pBuffer->uValidFrames;

		m_bufferSize = in_pBuffer->MaxFrames();
        uFramesConsumed = 0;
        uFramesProduced = 0;
        while (uFramesConsumed < in_pBuffer->uValidFrames
            && uFramesProduced < out_pBuffer->MaxFrames())
        {
            while (uFramesConsumed < in_pBuffer->MaxFrames())
            {
                m_UPOLSInput.push_back(*pInBuf++);
			    ++uFramesConsumed;
            }
			CsVector output = UPOLS(m_UPOLSInput);
            while (uFramesProduced < out_pBuffer->MaxFrames())
            {
                *pOutBuf++ = output[0];
                ++uFramesProduced;
			}
            
            output.clear();
            output.erase(output.begin(), output.end());
			m_UPOLSInput.clear();
			OutputDebugStringW(L"Block Processed\n");
        }
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
    initialiseAndUpdateFDLWithFilter(partitionedFilter, m_FDL_H);
}

void WP_CeSoundFIRTraceConv_24FX::convoluteSignals()
{
	//OutputDebugStringW(L"Convolution entry\n");
    m_FDL_Result.clear();
    m_FDL_Result.resize(m_FDL_H.size(), CsVectorC(m_FDL_H[0].size(), CsC(0.0, 0.0)));
    for (size_t i{0}; i<m_FDL_X.size(); i++)
    {
        for (size_t j {0}; j<m_FDL_H[i].size(); j++)
        {
            m_FDL_Result[i][j] = m_FDL_H[i][j] * m_FDL_X[i][j];
        }
    }
	//OutputDebugStringW(L"Convolution exit\n");
}

CsVector2 WP_CeSoundFIRTraceConv_24FX::partitioningIR(const CsVector2& FIR)
{
    CsVector combinedFIR = combineFIRPasses(FIR);
    AkUInt16 P = std::ceil(combinedFIR.size() / static_cast<double>(m_bufferSize));
    CsVector2 partitionedFIR(P, CsVector(2 * m_bufferSize, 0.0f));

    for (size_t i{ 0 }; i<=partitionedFIR.size(); i++)
    {
        for(int j {0}; j < m_bufferSize; j++)
        {
            if(i * m_bufferSize + j < combinedFIR.size())
            partitionedFIR[i][j] = combinedFIR[i * m_bufferSize + j];
        }
    }

    return partitionedFIR;
}

CsVector2 WP_CeSoundFIRTraceConv_24FX::partitioningIR_single(const CsVector& FIR)
{
    AkUInt16 P = std::ceil(FIR.size() / static_cast<double>(m_bufferSize));
    CsVector2 partitionedFIR(P, CsVector(2 * m_bufferSize, 0.0f));

    for (size_t i{ 0 }; i <= partitionedFIR.size(); i++)
    {
        for (int j{ 0 }; j < m_bufferSize; j++)
        {
            if (i * m_bufferSize + j < FIR.size())
            {
                partitionedFIR[i][j] = FIR[i * m_bufferSize + j];
            }
        }
    }
    return partitionedFIR;
}

//TODO: implement correct combination of FIR passes, for now just summing them up
CsVector WP_CeSoundFIRTraceConv_24FX::combineFIRPasses(const CsVector2& FIR)
{
    auto largestVector = std::max_element(FIR.begin(), FIR.end(), [](const CsVector& a, const CsVector& b) {return a.size() < b.size(); });

    CsVector combinedFIR(largestVector->size(), 0.0f);
    for (size_t i{ 0 }; i < largestVector->size(); i++)
    {
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
        double kN = k / static_cast<double>(N);
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
	//OutputDebugStringW(L"Making FDL buffer\n");
    CsVector tempBuffer (xBuffer.size()*2, 0.0f);
    for (size_t i{ 0 }; i < xBuffer.size(); i++)
    {
        tempBuffer[i] = xBuffer[i];
    }
    return tempBuffer;
}

void WP_CeSoundFIRTraceConv_24FX::initialiseAndUpdateFDLWithFilter(CsVector2 h, CsVector2C fdl)
{
    CsVector2C H(h.size(), CsVectorC (h[0].size()));
	int N = h[0].size();
	std::vector<fftwf_complex> fftwOutput(N / 2 + 1);

	

	//OutputDebugStringW(L"Before FFT \n");
    for (size_t i{0}; i<h.size(); i++)
    {
        fftwf_plan fftwPlan = fftwf_plan_dft_r2c_1d(N, h[i].data(), fftwOutput.data(), FFTW_ESTIMATE);
		fftwf_execute(fftwPlan);
        CsVectorC H_complex(fftwOutput.size(), CsC(0.0, 0.0));
        for (const auto& c : fftwOutput)
        {
            H_complex.emplace_back(c[0], c[1]);
        }
        H[i] = H_complex;
		fftwf_destroy_plan(fftwPlan);
    }
   

	// OutputDebugStringW(L"After FFT \n");
    if (m_FDL_H.empty())
    {
		//OutputDebugStringW(L"Initialising FDL\n");
		m_FDL_H.resize(H.size(), CsVectorC(H[0].size()));
        for (size_t j{ 0 }; j < H.size(); j++)
            m_FDL_H[j] = H[j];
    }
    else
    {
		//OutputDebugStringW(L"Updating FDL\n");
        if (m_filterExRunning)
            return;
        for (size_t j{ 0 }; j < H.size(); j++)
            m_FDL_Htemp[j] = H[j];

        filterExchange();
    }
}

void WP_CeSoundFIRTraceConv_24FX::updateFDL(CsVector xBuffer)
{
	//OutputDebugStringW(L"Updating FDL for FDL_X\n");
	int N = xBuffer.size();
    //float* in = (float*)fftwf_malloc(sizeof(float) * N);
    //fftwf_complex* out = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * (N / 2 + 1));
    std::vector<fftwf_complex> X (N/2+1);

    //X = FFT(xBuffer);
	fftwf_plan fftwPlan = fftwf_plan_dft_r2c_1d(N, xBuffer.data(), X.data(), FFTW_ESTIMATE);
	fftwf_execute(fftwPlan);
    
	CsVectorC X_complex(X.size(), CsC(0.0, 0.0));
    for(const auto& c : X)
    {
        X_complex.emplace_back(c[0], c[1]);
	}


    m_FDL_X.push_back(static_cast<CsVectorC>(X_complex));

    if (m_FDL_X.size() > m_FDL_H.size())
        m_FDL_X.pop_back();

	fftwf_destroy_plan(fftwPlan);
}

CsVector WP_CeSoundFIRTraceConv_24FX::sumFDL()
{
	//OutputDebugStringW(L"Summing FDL\n");
    CsVectorC sum_complex (m_FDL_Result[0].size(), CsC(1.0, 1.0));
    CsVector output(sum_complex.size(), 0.0f);

    for (CsVectorC& currentFDL : m_FDL_Result)
    {
		CsC maxVal = *std::max_element(currentFDL.begin(), currentFDL.end(), [](const CsC& a, const CsC& b) { return std::abs(a) < std::abs(b); });
        if (maxVal == CsC(0.0, 0.0))
            continue;
        for (size_t i{ 0 }; i < currentFDL.size(); i++)
        {
            sum_complex[i] += currentFDL[i];
        }
    }


	int N = sum_complex.size();
	std::vector<fftwf_complex> sum_fftw(sum_complex.size());
    for(size_t i { 0 }; i<sum_complex.size(); i++)
    {
        sum_fftw[i][0] = sum_complex[i].real();
        sum_fftw[i][1] = sum_complex[i].imag();
	}

	fftwf_plan fftwPlan = fftwf_plan_dft_c2r_1d(N, sum_fftw.data(), output.data(), FFTW_ESTIMATE);

    //output = IFFT(sum_complex);
	fftwf_execute(fftwPlan);
    int upperBound = output.size() / 2;

    for (auto j{ 0 }; j < upperBound; j++)
        output.pop_back();

	fftwf_destroy_plan(fftwPlan);
	//OutputDebugStringW(L"Summing FDL exit\n");
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

//TODO filter exchange with envelopes
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
    /*std::wstring msg = L"Default Execute, Offset: " + std::to_wstring(in_ulnOffset) + L"\n";
	OutputDebugStringW(msg.c_str());*/
    const AkUInt32 uNumChannels = in_pBuffer->NumChannels();

    AkUInt16 uFramesConsumed;
    AkUInt16 uFramesProduced;
    for (AkUInt32 i = 0; i < uNumChannels; ++i)
    {
        AkReal32* AK_RESTRICT pInBuf = (AkReal32 * AK_RESTRICT)in_pBuffer->GetChannel(i) + in_ulnOffset;
        AkReal32* AK_RESTRICT pOutBuf = (AkReal32 * AK_RESTRICT)out_pBuffer->GetChannel(i) + out_pBuffer->uValidFrames;

        //uFramesConsumed = 0;
        //uFramesProduced = 0;
        //while (uFramesConsumed < in_pBuffer->uValidFrames
        //    && uFramesProduced < out_pBuffer->MaxFrames())
        //{
        //    // Execute DSP that consumes input and produces output at different rate here
        //    *pOutBuf++ = *pInBuf++;
        //    ++uFramesConsumed;
        //    ++uFramesProduced;
        //}
		
		
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

std::vector<float> WP_CeSoundFIRTraceConv_24FX::linearConvolution(const CsVector& input, const CsVector& filter)
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
}