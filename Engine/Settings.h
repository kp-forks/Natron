//  Natron
//
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/*
*Created by Alexandre GAUTHIER-FOICHAT on 6/1/2012. 
*contact: immarespond at gmail dot com
*
*/

#ifndef NATRON_ENGINE_SETTINGS_H_
#define NATRON_ENGINE_SETTINGS_H_

#include <string>
#include <map>
#include <vector>

#include "Global/Macros.h"
#include "Global/GlobalDefines.h"

#include "Engine/Knob.h"

/*The current settings in the preferences menu.
 @todo Move this class to QSettings instead*/


namespace Natron {
    class LibraryBinary;
}

class File_Knob;
class Page_Knob;
class Double_Knob;
class Int_Knob;
class Bool_Knob;
class Choice_Knob;
class Path_Knob;

class Settings : public KnobHolder
{
public:

    Settings(AppInstance* appInstance);
    
    virtual ~Settings(){}
        
    virtual void evaluate(KnobI* /*knob*/,bool /*isSignificant*/) OVERRIDE FINAL {}
    
    virtual void onKnobValueChanged(KnobI* k,Natron::ValueChangedReason reason) OVERRIDE FINAL;
    
    int getViewersBitDepth() const;
    
    int getViewerTilesPowerOf2() const;
    
    double getRamMaximumPercent() const;
    
    double getRamPlaybackMaximumPercent() const;
    
    U64 getMaximumDiskCacheSize() const;
    
    bool getColorPickerLinear() const;
    
    int getNumberOfThreads() const;
    
    void setNumberOfThreads(int threadsNb);
    
    const std::string& getReaderPluginIDForFileType(const std::string& extension);
    
    const std::string& getWriterPluginIDForFileType(const std::string& extension);
    
    void populateReaderPluginsAndFormats(const std::map<std::string,std::vector<std::string> >& rows);
    
    void populateWriterPluginsAndFormats(const std::map<std::string,std::vector<std::string> >& rows);
    
    void getFileFormatsForReadingAndReader(std::map<std::string,std::string>* formats);
    
    void getFileFormatsForWritingAndWriter(std::map<std::string,std::string>* formats);
   
    ///save the settings to the application's settings
    void saveSettings();
    
    ///restores the settings from disk
    void restoreSettings();
    
    bool wereChangesMadeSinceLastSave() const { return _wereChangesMadeSinceLastSave; }
    
    void resetWereChangesMadeSinceLastSave() { _wereChangesMadeSinceLastSave = false; }
    
    bool isAutoPreviewOnForNewProjects() const;
    
    QStringList getPluginsExtraSearchPaths() const;
    
    bool isRenderInSeparatedProcessEnabled() const;
    
    void restoreDefault();
    
    int getMaximumUndoRedoNodeGraph() const;
    
    int getAutoSaveDelayMS() const;
    
    bool isSnapToNodeEnabled() const;
    
    bool isCheckForUpdatesEnabled() const;
    
private:
    
    virtual void initializeKnobs() OVERRIDE FINAL;

    void setDefaultValues();
        
    bool tryLoadOpenColorIOConfig();

    
    boost::shared_ptr<Page_Knob> _generalTab;
    boost::shared_ptr<Bool_Knob> _checkForUpdates;
    boost::shared_ptr<Int_Knob> _autoSaveDelay;
    boost::shared_ptr<Int_Knob> _maxUndoRedoNodeGraph;
    boost::shared_ptr<Bool_Knob> _linearPickers;
    boost::shared_ptr<Int_Knob> _numberOfThreads;
    boost::shared_ptr<Bool_Knob> _renderInSeparateProcess;
    boost::shared_ptr<Bool_Knob> _autoPreviewEnabledForNewProjects;
    boost::shared_ptr<Bool_Knob> _snapNodesToConnections;
    boost::shared_ptr<Path_Knob> _extraPluginPaths;
    
    boost::shared_ptr<Choice_Knob> _ocioConfigKnob;
    boost::shared_ptr<File_Knob> _customOcioConfigFile;
    
    boost::shared_ptr<Page_Knob> _cachingTab;
    boost::shared_ptr<Int_Knob> _maxPlayBackPercent;
    boost::shared_ptr<Int_Knob> _maxRAMPercent;
    boost::shared_ptr<Int_Knob> _maxDiskCacheGB;
    
    boost::shared_ptr<Page_Knob> _viewersTab;
    boost::shared_ptr<Choice_Knob> _texturesMode;
    boost::shared_ptr<Int_Knob> _powerOf2Tiling;
    
    boost::shared_ptr<Page_Knob> _readersTab;
    std::vector< boost::shared_ptr<Choice_Knob> > _readersMapping;
    
    boost::shared_ptr<Page_Knob> _writersTab;
    std::vector< boost::shared_ptr<Choice_Knob> >  _writersMapping;
    
    bool _wereChangesMadeSinceLastSave;
    
};

#endif // NATRON_ENGINE_SETTINGS_H_
