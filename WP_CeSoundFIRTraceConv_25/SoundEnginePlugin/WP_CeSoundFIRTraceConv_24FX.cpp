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
#define LIVE 0

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
    
    OutputDebugStringW(L"Init\n");

    std::fstream file("C:\\Users\\cedri\\Desktop\\output3.txt");
    
    double x;
    while (file >> x)
    {
        m_testFIR.emplace_back(x);
    }
	
#if TESTING
    std::fstream fileResult("C:\\Users\\cedri\\Desktop\\ComplexResult.csv");
    CsVector2C expectedResult;
    std::string line;
    while (std::getline(fileResult, line)) {

        std::stringstream ss(line);

        std::string cell;

        std::vector<CsC> row;

        while (std::getline(ss, cell, ',')) {
            row.push_back(parseComplex(cell));
        }

        expectedResult.push_back(row);
    }

    file.close();
    fileResult.close();
#endif
    initialiseFDLWithFilter(m_testFIR);

#if TESTING
	_ASSERT_EXPR(expectedResult.size() == m_FDL_H.size(), L"Expected result size does not match FDL_H size");

    float tolerance = 1e-5f;

    for (auto i{ 0 }; i < expectedResult.size(); i++)
    {
        _ASSERT_EXPR(m_FDL_H[i].size() == expectedResult[i].size(), L"Col mismatch");
        for (auto j{ 0 }; j < expectedResult[i].size(); j++)
        {
            _ASSERT_EXPR(std::abs(expectedResult[i][j].real() - m_FDL_H[i][j].real()) < tolerance, L"Real part mismatch");
            _ASSERT_EXPR(std::abs(expectedResult[i][j].imag() - m_FDL_H[i][j].imag()) < tolerance, L"Imaginary part mismatch");
        }
    }
#endif
    
	//m_testFIR.resize(24000, 1.0f);
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
#if LIVE
    //Unit Test and assertions
    AkUInt32 dataSize{ 0 };
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
    }

	/*std::wstring msg = L"Block Size: " + std::to_wstring(in_pBuffer->MaxFrames()) + L"\n";
	OutputDebugStringW(msg.c_str());*/
    defaultExecute(in_pBuffer, in_ulnOffset, out_pBuffer);
    //TODO bypass when filter is empty
#endif
    const AkUInt32 uNumChannels = in_pBuffer->NumChannels();

    
 
    AkUInt16 uFramesConsumed;
    AkUInt16 uFramesProduced;
    for (AkUInt32 i = 0; i < uNumChannels; ++i)
    {
		m_currentChannel = i;

		std::wstring msg = L"Processing Channel: " + std::to_wstring(i) + L"\n";
		OutputDebugStringW(msg.c_str());
        if(!m_FDL_XInitialised)
        {
            m_FDL_XInitialised = true;
            m_FDL_X.resize(uNumChannels, CsVector2C(m_FDL_H.size(), CsVectorC(m_bufferSize + 1, CsC(0.0, 0.0))));
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
				/*auto maxElem = std::max_element(m_UPOLSInput[m_currentChannel].begin(), m_UPOLSInput[m_currentChannel].end());
				std::wstring msg = L"Max element m_UPOLSInput: " + std::to_wstring(*maxElem) + L"\n";
				OutputDebugStringW(msg.c_str());*/
                //OutputDebugStringW(L"Processing Block\n");
                CsVector output;
                output = UPOLS(m_UPOLSInput[m_currentChannel]);
                appendVectorText(m_filename, output);
                for(auto &sample : output)
                {
					*pOutBuf++ = sample;
					++uFramesProduced;
				}
                /*while (uFramesProduced < out_pBuffer->MaxFrames())
                {
                    *pOutBuf++ = output[0];
                    ++uFramesProduced;
                }*/
                

                //m_UPOLSInput.clear();                 
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

	OutputDebugStringW(L"UPOLS exit\n");
    return output;
}

//void WP_CeSoundFIRTraceConv_24FX::initialiseAndUpdateFilter(const CsVector2& filter)
//{
//    CsVector2 partitionedFilter{ partitioningIR(filter) };
//    initialiseAndUpdateFDLWithFilter(partitionedFilter, m_FDL_H);
//}

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

    if (m_FDL_X[m_currentChannel].size() > m_FDL_H.size())
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

CsC WP_CeSoundFIRTraceConv_24FX::parseComplex(const std::string& s)
{
    // Find separator between real and imaginary part
    //size_t plusPos = s.find('+');
    //size_t minusPos = s.find('-', 1);

    //size_t splitPos;

    //if (plusPos != std::string::npos)
    //    splitPos = plusPos;
    //else
    //    splitPos = minusPos;

    //float real = std::stof(s.substr(0, splitPos));

    //std::string imagStr = s.substr(splitPos);

    //// Remove trailing 'i'
    //imagStr.pop_back();

    //float imag = std::stof(imagStr);

    //return CsC(real, imag);
    std::stringstream ss(s);
    float real = 0.0f;
    float imag = 0.0f;
    char sign = '+';
    char i_char = '\0';

    // 1. Read the real part
    if (!(ss >> real)) {
        // Handle error: couldn't read real number
    }

    // 2. Read the operator ('+' or '-')
    // If the next thing is 'i', it means there was no real part (e.g., "4i" or "-4i")
    // This stream approach assumes standard "a + bi" or "a - bi" format.
    ss >> sign;

    if (sign == '+' || sign == '-') {
        // 3. Read the imaginary magnitude
        if (ss >> imag) {
            // Read the trailing 'i'
            ss >> i_char;
        }
        else {
            // Edge case: string was "3 + i" or "3 - i", meaning imag is 1 or -1
            ss.clear();
            ss >> i_char; // try to read the 'i'
            imag = 1.0f;
        }

        if (sign == '-') {
            imag = -imag;
        }
    }
    else if (sign == 'i') {
        // Pure imaginary number format like "3.0i" (real was parsed as 3.0, but it was actually imag)
        imag = real;
        real = 0.0f;
    }

    return CsC(real, imag);
}

void WP_CeSoundFIRTraceConv_24FX::appendVectorText(const std::string& filename, const std::vector<float>& vec) {
    // Open in standard text append mode
    std::ofstream outFile(filename, std::ios::app);

    if (!outFile) {
        std::cerr << "Error opening file for writing!" << std::endl;
        return;
    }

    // Write elements separated by spaces, and a newline at the end of the vector
    for (float val : vec) {
        outFile << val << " ";
    }
    outFile << "\n";
}