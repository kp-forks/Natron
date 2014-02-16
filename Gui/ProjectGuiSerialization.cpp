//  Natron
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/*
 *Created by Alexandre GAUTHIER-FOICHAT on 6/1/2012.
 *contact: immarespond at gmail dot com
 *
 */

#include "ProjectGuiSerialization.h"

#include "Global/Macros.h"
CLANG_DIAG_OFF(deprecated)
#include <QSplitter>
CLANG_DIAG_ON(deprecated)

#include "Engine/Project.h"
#include "Engine/Node.h"
#include "Engine/ViewerInstance.h"
#include "Gui/NodeGui.h"
#include "Gui/Gui.h"
#include "Gui/TabWidget.h"
#include "Gui/ViewerTab.h"
#include "Gui/ViewerGL.h"
#include "Gui/ProjectGui.h"
#include "Gui/GuiApplicationManager.h"
#include "Gui/GuiAppInstance.h"

 void ProjectGuiSerialization::initialize(const ProjectGui* projectGui){
     std::vector<NodeGui*> activeNodes = projectGui->getVisibleNodes();
     _serializedNodes.clear();
     for (U32 i = 0; i < activeNodes.size(); ++i) {
         boost::shared_ptr<NodeGuiSerialization> state(new NodeGuiSerialization);
         activeNodes[i]->serialize(state.get());
         _serializedNodes.push_back(state);
         
         if (activeNodes[i]->getNode()->pluginID() == "Viewer") {
             ViewerInstance* viewer = dynamic_cast<ViewerInstance*>(activeNodes[i]->getNode()->getLiveInstance());
             ViewerTab* tab = projectGui->getGui()->getViewerTabForInstance(viewer);
             assert(viewer);
             ViewerData viewerData;
             viewerData.aspectRatio = 1.;
             tab->getViewer()->getProjection(viewerData.left, viewerData.bottom, viewerData.zoomFactor);
             viewerData.userRoI = tab->getViewer()->getUserRegionOfInterest();
             viewerData.userRoIenabled = tab->getViewer()->isUserRegionOfInterestEnabled();
             viewerData.isClippedToProject = tab->isClippedToProject();
             viewerData.exposure = tab->getExposure();
             viewerData.colorSpace = tab->getColorSpace();
             viewerData.channels = tab->getChannelsString();
             _viewersData.insert(std::make_pair(viewer->getName(),viewerData));
         }
     }
     
    const std::list<TabWidget*>& tabWidgets = projectGui->getGui()->getPanes();
     for (std::list<TabWidget*>::const_iterator it = tabWidgets.begin(); it!= tabWidgets.end(); ++it) {
         const QString& widgetName = (*it)->objectName();
         if(widgetName.isEmpty()){
             std::cout << "Warning: attempting to save the layout of an unnamed TabWidget, discarding." << std::endl;
             continue;
         }
         
         PaneLayout layout;
         layout.parentingCreated = false;
         const std::map<TabWidget*,bool>& userSplits = (*it)->getUserSplits();
         for (std::map<TabWidget*,bool>::const_iterator split = userSplits.begin(); split!=userSplits.end(); ++split) {
             layout.splits.push_back(split->second);
         }
         layout.floating = (*it)->isFloating();
         if(layout.floating){
             QPoint pos = (*it)->pos();
             layout.posx = pos.x();
             layout.posy = pos.y();
         }else{
             //not releveant since the tab is not floating
             layout.posx = -1;
             layout.posy = -1;
         }
         
         for (int i = 0; i < (*it)->count(); ++i) {
             QWidget* tab = (*it)->tabAt(i);
             assert(tab);
             const QString& tabName = tab->objectName();
             if(tabName.isEmpty()){
                 std::cout << "Warning: attempting to save the position of an unnamed tab, discarding." << std::endl;
                 continue;
             }
             layout.tabs.push_back(tabName.toStdString());
         }
         
         _layout.insert(std::make_pair(widgetName.toStdString(),layout));
     }
     
     ///now we do a second pass to insert for each layout what are its children splits
     for (std::map<std::string,PaneLayout>::iterator it = _layout.begin(); it!= _layout.end(); ++it) {
         if(!it->second.parentingCreated){
             createParenting(it);
         }
     }
     
     ///save application's splitters states
     const std::list<QSplitter*>& splitters = projectGui->getGui()->getSplitters();
     for (std::list<QSplitter*>::const_iterator it = splitters.begin(); it!= splitters.end(); ++it) {
         QByteArray ba = (*it)->saveState();
         ba = ba.toBase64();
         QString str(ba);
         _splittersStates.insert(std::make_pair((*it)->objectName().toStdString(),str.toStdString()));

     }
     
     
     
}

void ProjectGuiSerialization::createParenting(std::map<std::string,PaneLayout>::iterator it){
    QString nameCpy = it->first.c_str();
    int index = nameCpy.indexOf(TabWidget::splitVerticallyTag);
    if(index != -1){
        //this is a vertical split, find the parent widget and insert this widget as child
        
        ///The goal of the next lines is to erase the split tag string from the name of the tab widget
        /// to find out the name of the tab widget from whom this tab was sprout
        nameCpy = nameCpy.remove(index, TabWidget::splitVerticallyTag.size());
        //we now have the name of the parent
        std::map<std::string,PaneLayout>::iterator foundParent = _layout.find(nameCpy.toStdString());
        assert(foundParent != _layout.end());
        foundParent->second.splitsNames.push_back(it->first);
        it->second.parentName = nameCpy.toStdString();
        it->second.parentingCreated = true;
        
        //call this recursively
        createParenting(foundParent);
    }else{
        index = nameCpy.indexOf(TabWidget::splitHorizontallyTag);
        if(index != -1){
            //this is a horizontal split, find the parent widget and insert this widget as child
            nameCpy = nameCpy.remove(index, TabWidget::splitVerticallyTag.size());
            //we now have the name of the parent
            std::map<std::string,PaneLayout>::iterator foundParent = _layout.find(nameCpy.toStdString());
            assert(foundParent != _layout.end());
            foundParent->second.splitsNames.push_back(it->first);
            it->second.parentName = nameCpy.toStdString();
            it->second.parentingCreated = true;
            
            //call this recursively
            createParenting(foundParent);
        }
    }
}
