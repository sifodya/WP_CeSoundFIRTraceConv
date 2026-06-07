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

#include "WP_CeSoundFIRTraceConv_24Plugin.h"
#include "../SoundEnginePlugin/WP_CeSoundFIRTraceConv_24FXFactory.h"

WP_CeSoundFIRTraceConv_24Plugin::WP_CeSoundFIRTraceConv_24Plugin()
{
}

WP_CeSoundFIRTraceConv_24Plugin::~WP_CeSoundFIRTraceConv_24Plugin()
{
}

bool WP_CeSoundFIRTraceConv_24Plugin::GetBankParameters(const GUID & in_guidPlatform, AK::Wwise::Plugin::DataWriter& in_dataWriter) const
{
    // Write bank data here
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "stretch"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "decay"));
    in_dataWriter.WriteBool(m_propertySet.GetBool(in_guidPlatform, "reverse"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "attack"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "lpCo"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "lpQ"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "lpDb"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "hpCo"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "hpQ"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "hpDb"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "p1Co"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "p1Q"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "p1Db"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "p2Co"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "p2Q"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "p2Db"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "freqHigh"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "freqLow"));
    in_dataWriter.WriteInt32(m_propertySet.GetInt32(in_guidPlatform, "firSelect"));

    return true;
}



AK_DEFINE_PLUGIN_CONTAINER(WP_CeSoundFIRTraceConv_24);											// Create a PluginContainer structure that contains the info for our plugin
AK_EXPORT_PLUGIN_CONTAINER(WP_CeSoundFIRTraceConv_24);											// This is a DLL, we want to have a standardized name
AK_ADD_PLUGIN_CLASS_TO_CONTAINER(                                             // Add our CLI class to the PluginContainer
    WP_CeSoundFIRTraceConv_24,        // Name of the plug-in container for this shared library
    WP_CeSoundFIRTraceConv_24Plugin,  // Authoring plug-in class to add to the plug-in container
    WP_CeSoundFIRTraceConv_24FX       // Corresponding Sound Engine plug-in class
);

DEFINE_PLUGIN_REGISTER_HOOK

DEFINEDUMMYASSERTHOOK;							// Placeholder assert hook for Wwise plug-ins using AKASSERT (cassert used by default)
