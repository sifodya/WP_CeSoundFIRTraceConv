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

#ifndef WP_CeSoundFIRTraceConv_24FX_H
#define WP_CeSoundFIRTraceConv_24FX_H

#include "WP_CeSoundFIRTraceConv_24FXParams.h"
#include "UEDataStruct.h"
#include <cmath>
#include <complex>
#define _USE_MATH_DEFINES
#include <vector>

typedef std::vector<float>  CsVector;
typedef std::vector <std::vector<float>> CsVector2;
typedef std::vector<std::complex<double>> CsVectorC;
typedef std::vector<std::vector<std::complex<double>>> CsVector2C;

typedef std::complex<double> CsC;

/// See https://www.audiokinetic.com/library/edge/?source=SDK&id=soundengine__plugins__effects.html
/// for the documentation about effect plug-ins
class WP_CeSoundFIRTraceConv_24FX
    : public AK::IAkOutOfPlaceEffectPlugin
{
public:
    WP_CeSoundFIRTraceConv_24FX();
    ~WP_CeSoundFIRTraceConv_24FX();

    //=================================================================================

    

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

    //void SetCustomData(const UEDataStruct& filterData);

private:
    CsVector2 InterpolateData(CsVector2 UE_Data);
    CsVector2 PartitioningIR(CsVector2* FIR);
    CsVector CombineFIRPasses(CsVector2* FIR);
    CsVectorC FFT(CsVector* xStream);
    CsVectorC FFT_C(CsVectorC* xStream);
    CsVector IFFT(CsVectorC* sum);
    void GUPOLS();
    CsVector UPOLS(CsVector* xBuffer);
    void FilterExchange();
    CsVector MakeFDLBuffer(std::vector<AkReal32> xBuffer);
    void InitialiseFDL(CsVector2 h);
    void UpdateFDL(CsVector xBuffer);
    void ConvoluteSignals();
    CsVector SumFDL();
    void FilterIsUpdated(CsVector2 filter);
    //==================================================================================
    WP_CeSoundFIRTraceConv_24FXParams* m_pParams;
    AK::IAkPluginMemAlloc* m_pAllocator;
    AK::IAkEffectPluginContext* m_pContext;
    //==================================================================================
    AkUInt16 bufferSize { 0 };
    const double pi{ std::acos(-1.0) };
    const CsC minus_i{ (0, -1) };
    bool filterExRunning{ false };
    INT32 dataVersion{ -1 };

    CsVector FDLBuffer;
    CsVector2C FDL_H;
    CsVector2C FDL_Htemp;
    CsVector2C FDL_X;
    CsVector2C FDL_Result;

    
};



#endif // WP_CeSoundFIRTraceConv_24FX_H
