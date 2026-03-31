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

GUI::GUI() {}

// Acquire the module instance from the Microsoft linker
extern "C" IMAGE_DOS_HEADER __ImageBase;

HINSTANCE GUI::GetResourceHandle() const
{
    //return ((HINSTANCE)&__ImageBase);

    // OR using MFC:
    /*AFX_MANAGE_STATE( AfxGetStaticModuleState() );
    return AfxGetStaticModuleState()->m_hCurrentResourceHandle;*/
    return (HINSTANCE)&__ImageBase;
}

// These macros generate a table named "WoaGainProperties" to pass to GetDialog
// See https://www.audiokinetic.com/library/edge/?source=SDK&id=wwiseplugin_dialog_guide.html#wwiseplugin_dialog_guide_poptable
//
// The preprocessor turns the code below into:
// AK::Wwise::PopulateTableItem WoaGainProperties = {
//    {IDC_GAIN_SLIDER, L"Dummy"},
//    {0, NULL}
// };
AK_WWISE_PLUGIN_GUI_WINDOWS_BEGIN_POPULATE_TABLE(WoaGainProperties)
AK_WWISE_PLUGIN_GUI_WINDOWS_POP_ITEM(
    IDC_GAIN_SLIDER, /* < ID of the Win32 control in resource.h and WoaGain.rc */
    "stretch"         /* < Property Name in WoaGain.xml */
)
AK_WWISE_PLUGIN_GUI_WINDOWS_END_POPULATE_TABLE()


// Return true = Custom GUI
// Return false = Generated GUI
bool GUI::GetDialog(AK::Wwise::Plugin::eDialog in_eDialog, UINT& out_uiDialogID, AK::Wwise::Plugin::PopulateTableItem*& out_pTable) const
{
    // Which dialog type is being requested?
    switch (in_eDialog)
    {
        // Plug-in Settings: Available to all plug-ins
    case AK::Wwise::Plugin::SettingsDialog:
    {
        out_uiDialogID = IDD_WOA_DIALOG;
        out_pTable = WoaGainProperties;
        return true;
    }
    // Contents Editor: Only available to source plug-ins
    case AK::Wwise::Plugin::ContentsEditorDialog:
    default:
    {
        return false;
    }
    }
}

