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

#include "WP_CeSoundFIRTraceConv_24FXParams.h"

#include <AK/Tools/Common/AkBankReadHelpers.h>

WP_CeSoundFIRTraceConv_24FXParams::WP_CeSoundFIRTraceConv_24FXParams()
{
}

WP_CeSoundFIRTraceConv_24FXParams::~WP_CeSoundFIRTraceConv_24FXParams()
{
}

WP_CeSoundFIRTraceConv_24FXParams::WP_CeSoundFIRTraceConv_24FXParams(const WP_CeSoundFIRTraceConv_24FXParams& in_rParams)
{
    RTPC = in_rParams.RTPC;
    NonRTPC = in_rParams.NonRTPC;
    m_paramChangeHandler.SetAllParamChanges();
}

AK::IAkPluginParam* WP_CeSoundFIRTraceConv_24FXParams::Clone(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, WP_CeSoundFIRTraceConv_24FXParams(*this));
}

AKRESULT WP_CeSoundFIRTraceConv_24FXParams::Init(AK::IAkPluginMemAlloc* in_pAllocator, const void* in_pParamsBlock, AkUInt32 in_ulBlockSize)
{
    if (in_ulBlockSize == 0)
    {
        // Initialize default parameters here
        RTPC.fstretch = 0.0f;
        RTPC.fdecay = 0.0f;
        RTPC.breverse = false;
        RTPC.fattack = 0.0f;
        RTPC.flpCo = 20.0f;
        RTPC.flpQ = 1.0f;
        RTPC.flpDb = 0.0f;
        RTPC.fhpCo = 18000.0f;
        RTPC.fhpQ = 1.0f;
        RTPC.fhpDb = 0.0f;
        RTPC.fp1Co = 1000.0f;
        RTPC.fp1Q = 1.0f;
        RTPC.fp1Db = 0.0f;
        RTPC.fp2Co = 4000.0f;
        RTPC.fp2Q = 1.0f;
        RTPC.fp2Db = 0.0f;
        RTPC.ffreqHigh = 20.0f;
        RTPC.ffreqLow = 20.0f;
        RTPC.ifirSelect = 0;
        
        m_paramChangeHandler.SetAllParamChanges();
        return AK_Success;
    }

    return SetParamsBlock(in_pParamsBlock, in_ulBlockSize);
}

AKRESULT WP_CeSoundFIRTraceConv_24FXParams::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}

AKRESULT WP_CeSoundFIRTraceConv_24FXParams::SetParamsBlock(const void* in_pParamsBlock, AkUInt32 in_ulBlockSize)
{
    AKRESULT eResult = AK_Success;
    AkUInt8* pParamsBlock = (AkUInt8*)in_pParamsBlock;

    // Read bank data here
    RTPC.fstretch = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fdecay = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.breverse = READBANKDATA(bool, pParamsBlock, in_ulBlockSize);
    RTPC.fattack = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.flpCo = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.flpQ = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.flpDb = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fhpCo = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fhpQ = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fhpDb = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fp1Co = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fp1Q = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fp1Db = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fp2Co = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fp2Q = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fp2Db = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.ffreqHigh = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.ffreqLow = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.ifirSelect = READBANKDATA(AkInt32, pParamsBlock, in_ulBlockSize);
    CHECKBANKDATASIZE(in_ulBlockSize, eResult);
    m_paramChangeHandler.SetAllParamChanges();

    return eResult;
}

AKRESULT WP_CeSoundFIRTraceConv_24FXParams::SetParam(AkPluginParamID in_paramID, const void* in_pValue, AkUInt32 in_ulParamSize)
{
    AKRESULT eResult = AK_Success;

    // Handle parameter change here
    switch (in_paramID)
    {
    case PARAM_STRETCH_ID:
        RTPC.fstretch = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_STRETCH_ID);
        break;
    case PARAM_DECAY_ID:
        RTPC.fdecay = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_DECAY_ID);
        break;
    case PARAM_REVERSE_ID:
        RTPC.breverse = *((bool*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_REVERSE_ID);
        break;
    case PARAM_LPCO_ID:
        RTPC.flpCo = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_LPCO_ID);
        break;
    case PARAM_LPQ_ID:
        RTPC.flpQ = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_LPQ_ID);
        break;
    case PARAM_LPDB_ID:
        RTPC.flpDb = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_LPDB_ID);
        break;
    case PARAM_HPCO_ID:
        RTPC.fhpCo = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_HPCO_ID);
        break;
    case PARAM_HPQ_ID:
        RTPC.fhpQ = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_HPQ_ID);
        break;
    case PARAM_HPDB_ID:
        RTPC.fhpDb = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_HPDB_ID);
        break;
    case PARAM_P1CO_ID:
        RTPC.fp1Co = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_P1CO_ID);
        break;
    case PARAM_P1Q_ID:
        RTPC.fp1Q = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_P1Q_ID);
        break;
    case PARAM_P1DB_ID:
        RTPC.fp1Db = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_P1DB_ID);
        break;
    case PARAM_P2CO_ID:
        RTPC.fp2Co = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_P2CO_ID);
        break;
    case PARAM_P2Q_ID:
        RTPC.fp2Q = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_P2Q_ID);
        break;
    case PARAM_P2DB_ID:
        RTPC.fp2Db = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_P2DB_ID);
        break;
    case PARAM_FREQHIGH_ID:
        RTPC.ffreqHigh = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_FREQHIGH_ID);
        break;
    case PARAM_FREQLOW_ID:
        RTPC.ffreqLow = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_FREQLOW_ID);
        break;
    case PARAM_FIRSELECT_ID:
    {
        RTPC.ifirSelect = static_cast<AkInt32>(*((AkReal32*)in_pValue));
        m_paramChangeHandler.SetParamChange(PARAM_FIRSELECT_ID);

        break;
    }
    default:
        eResult = AK_InvalidParameter;
        break;
    }

    return eResult;
}
