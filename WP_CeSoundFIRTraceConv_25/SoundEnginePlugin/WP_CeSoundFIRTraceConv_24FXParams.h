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

#ifndef WP_CeSoundFIRTraceConv_24FXParams_H
#define WP_CeSoundFIRTraceConv_24FXParams_H

#include <AK/SoundEngine/Common/IAkPlugin.h>
#include <AK/Plugin/PluginServices/AkFXParameterChangeHandler.h>

// Add parameters IDs here, those IDs should map to the AudioEnginePropertyID
// attributes in the xml property definition.
static const AkPluginParamID PARAM_STRETCH_ID = 0;
static const AkPluginParamID PARAM_DECAY_ID = 1;
static const AkPluginParamID PARAM_REVERSE_ID = 2;
static const AkPluginParamID PARAM_LPCO_ID = 3;
static const AkPluginParamID PARAM_LPQ_ID = 4;
static const AkPluginParamID PARAM_LPDB_ID = 5;
static const AkPluginParamID PARAM_HPCO_ID = 6;
static const AkPluginParamID PARAM_HPQ_ID = 7;
static const AkPluginParamID PARAM_HPDB_ID = 8;
static const AkPluginParamID PARAM_P1CO_ID = 9;
static const AkPluginParamID PARAM_P1Q_ID = 10;
static const AkPluginParamID PARAM_P1DB_ID = 11;
static const AkPluginParamID PARAM_P2CO_ID = 12;
static const AkPluginParamID PARAM_P2Q_ID = 13;
static const AkPluginParamID PARAM_P2DB_ID = 14;
static const AkPluginParamID PARAM_FREQHIGH_ID = 15;
static const AkPluginParamID PARAM_FREQLOW_ID = 16;
static const AkPluginParamID PARAM_FIRSELECT_ID = 17;
static const AkUInt32 NUM_PARAMS = 18;

struct WP_CeSoundFIRTraceConv_24RTPCParams
{
    AkReal32 fstretch;
    AkReal32 fdecay;
    bool breverse;
    AkReal32 fattack;
    AkReal32 flpCo;
    AkReal32 flpQ;
    AkReal32 flpDb;
    AkReal32 fhpCo;
    AkReal32 fhpQ;
    AkReal32 fhpDb;
    AkReal32 fp1Co;
    AkReal32 fp1Q;
    AkReal32 fp1Db;
    AkReal32 fp2Co;
    AkReal32 fp2Q;
    AkReal32 fp2Db;
    AkReal32 ffreqHigh;
    AkReal32 ffreqLow;
    AkInt32 ifirSelect;
};

struct WP_CeSoundFIRTraceConv_24NonRTPCParams
{
};

struct WP_CeSoundFIRTraceConv_24FXParams
    : public AK::IAkPluginParam
{
    WP_CeSoundFIRTraceConv_24FXParams();
    WP_CeSoundFIRTraceConv_24FXParams(const WP_CeSoundFIRTraceConv_24FXParams& in_rParams);

    ~WP_CeSoundFIRTraceConv_24FXParams();

    /// Create a duplicate of the parameter node instance in its current state.
    IAkPluginParam* Clone(AK::IAkPluginMemAlloc* in_pAllocator) override;

    /// Initialize the plug-in parameter node interface.
    /// Initializes the internal parameter structure to default values or with the provided parameter block if it is valid.
    AKRESULT Init(AK::IAkPluginMemAlloc* in_pAllocator, const void* in_pParamsBlock, AkUInt32 in_ulBlockSize) override;

    /// Called by the sound engine when a parameter node is terminated.
    AKRESULT Term(AK::IAkPluginMemAlloc* in_pAllocator) override;

    /// Set all plug-in parameters at once using a parameter block.
    AKRESULT SetParamsBlock(const void* in_pParamsBlock, AkUInt32 in_ulBlockSize) override;

    /// Update a single parameter at a time and perform the necessary actions on the parameter changes.
    AKRESULT SetParam(AkPluginParamID in_paramID, const void* in_pValue, AkUInt32 in_ulParamSize) override;

    AK::AkFXParameterChangeHandler<NUM_PARAMS> m_paramChangeHandler;

    WP_CeSoundFIRTraceConv_24RTPCParams RTPC;
    WP_CeSoundFIRTraceConv_24NonRTPCParams NonRTPC;
};

#endif // WP_CeSoundFIRTraceConv_24FXParams_H
