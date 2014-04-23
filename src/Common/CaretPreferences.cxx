
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#define __CARET_PREFERENCES_DECLARE__
#include "CaretPreferences.h"
#undef __CARET_PREFERENCES_DECLARE__

#include <algorithm>
#include <set>

#include <QSettings>
#include <QStringList>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ModelTransform.h"
#include "TileTabsConfiguration.h"

using namespace caret;


    
/**
 * \class caret::CaretPreferences 
 * \brief Preferences for use in Caret.
 *
 * Maintains preferences for use in Caret.  The
 * preferences are read only one time, when an 
 * instance is created.  If a preference is changed,
 * it is written.
 */
/**
 * Constructor.
 */
CaretPreferences::CaretPreferences()
: CaretObject()
{
    this->qSettings = new QSettings("brainvis.wustl.edu",
                                    "Caret7");
    this->readPreferences();
}

/**
 * Destructor.
 */
CaretPreferences::~CaretPreferences()
{
    this->removeAllCustomViews();
    
    this->removeAllTileTabsConfigurations();
    
    delete this->qSettings;
}

/**
 * Get the boolean value for the given preference name.
 * @param name
 *    Name of the preference.
 * @param defaultValue
 *    Value returned in the preference with the given name was not found.
 * @return
 *    Boolean value of preference or defaultValue if the
 *    named preference is not found.
 */
bool CaretPreferences::getBoolean(const AString& name,
                                  const bool defaultValue)
{
    bool b = this->qSettings->value(name, defaultValue).toBool();
    return b;
}

/**
 * Set the given preference name with the boolean value.
 * @param
 *    Name of the preference.
 * @param
 *    New value for preference.
 */
void CaretPreferences::setBoolean(const AString& name,
                                  const bool value)
{
    this->qSettings->setValue(name, value);
}

/**
 * Get the boolean value for the given preference name.
 * @param name
 *    Name of the preference.
 * @param defaultValue
 *    Value returned in the preference with the given name was not found.
 * @return
 *    Integer value of preference or defaultValue if the
 *    named preference is not found.
 */
int CaretPreferences::getInteger(const AString& name,
                                  const int defaultValue)
{
    int b = this->qSettings->value(name, defaultValue).toInt();
    return b;
}

/**
 * Set the given preference name with the integer value.
 * @param
 *    Name of the preference.
 * @param
 *    New value for preference.
 */
void CaretPreferences::setInteger(const AString& name,
                                  const int value)
{
    this->qSettings->setValue(name, value);
}

/**
 * Get the string value for the given preference name.
 * @param name
 *    Name of the preference.
 * @param defaultValue
 *    Value returned in the preference with the given name was not found.
 * @return
 *    String value of preference or defaultValue if the
 *    named preference is not found.
 */
AString
CaretPreferences::getString(const AString& name,
                            const AString& defaultValue)
{
    AString s = this->qSettings->value(name,
                                       defaultValue).toString();
    return s;
}

/**
 * Set the given preference name with the string value.
 * @param
 *    Name of the preference.
 * @param
 *    New value for preference.
 */
void
CaretPreferences::setString(const AString& name,
                            const AString& value)
{
    this->qSettings->setValue(name,
                              value);
}

/**
 * Remove all custom views.
 */
void
CaretPreferences::removeAllCustomViews()
{
    for (std::vector<ModelTransform*>::iterator iter = this->customViews.begin();
         iter != this->customViews.end();
         iter++) {
        delete *iter;
    }
    this->customViews.clear();
}

/**
 * @return Names of custom views sorted by name.  May want to precede this
 * method with a call to 'readCustomViews(true)' so that the custom views
 * are the latest from the settings.
 */
std::vector<AString>
CaretPreferences::getCustomViewNames() const
{
    std::vector<AString> names;
    
    for (std::vector<ModelTransform*>::const_iterator iter = this->customViews.begin();
         iter != this->customViews.end();
         iter++) {
        const ModelTransform* mt = *iter;
        names.push_back(mt->getName());
    }
    
    std::sort(names.begin(),
              names.end());
    
    return names;
}

/**
 * @return Names and comments of custom views sorted by name.  May want to precede this
 * method with a call to 'readCustomViews(true)' so that the custom views
 * are the latest from the settings.
 */
std::vector<std::pair<AString,AString> >
CaretPreferences::getCustomViewNamesAndComments() const
{
    std::vector<AString> customViewNames = getCustomViewNames();
    
    std::vector<std::pair<AString,AString> > namesAndComments;
    
    for (std::vector<AString>::const_iterator iter = customViewNames.begin();
         iter != customViewNames.end();
         iter++) {
        const AString name = *iter;
        ModelTransform modelTransform;
        if (getCustomView(name, modelTransform)) {
            const AString comment = modelTransform.getComment();
            namesAndComments.push_back(std::make_pair(name,
                                                      comment));
        }
    }
    
    return namesAndComments;
}


/**
 * Get a custom view with the given name.
 *
 * @param customViewName
 *     Name of requested custom view.
 * @param modelTransformOut
 *     Custom view will be loaded into this model transform.
 * @return true if a custom view with the name exists.  If no
 *     custom view exists with the name, false is returned and
 *     the model transform will be the identity transform.
 */
bool
CaretPreferences::getCustomView(const AString& customViewName,
                   ModelTransform& modelTransformOut) const
{
    for (std::vector<ModelTransform*>::const_iterator iter = this->customViews.begin();
         iter != this->customViews.end();
         iter++) {
        const ModelTransform* mt = *iter;
        
        if (customViewName == mt->getName()) {
            modelTransformOut = *mt;
            return true;
        }
    }
    
    modelTransformOut.setToIdentity();
    
    return false;
}

/**
 * Add or update a custom view.  If a custom view exists with the name
 * in the given model transform it is replaced.
 *
 * @param modelTransform
 *     Custom view's model transform.
 */
void
CaretPreferences::addOrReplaceCustomView(const ModelTransform& modelTransform)
{
    bool addNewCustomView = true;
    
    for (std::vector<ModelTransform*>::iterator iter = this->customViews.begin();
         iter != this->customViews.end();
         iter++) {
        ModelTransform* mt = *iter;
        if (mt->getName() == modelTransform.getName()) {
            *mt = modelTransform;
            addNewCustomView = false;
            break;
        }
    }
    
    if (addNewCustomView) {
        this->customViews.push_back(new ModelTransform(modelTransform));
    }
    this->writeCustomViews();
}

/**
 * Remove the custom view with the given name.
 *
 * @param customViewName
 *     Name of custom view.
 */
void
CaretPreferences::removeCustomView(const AString& customViewName)
{
    for (std::vector<ModelTransform*>::iterator iter = this->customViews.begin();
         iter != this->customViews.end();
         iter++) {
        ModelTransform* mt = *iter;
        if (mt->getName() == customViewName) {
            this->customViews.erase(iter);
            delete mt;
            break;
        }
    }
    
    this->writeCustomViews();
}

///**
// * Get all of the user views.
// * @return
// *    All of the user views.
// */
//std::vector<ModelTransform*> 
//CaretPreferences::getAllModelTransforms()
//{
//    std::vector<ModelTransform*> viewsOut;
//    viewsOut.insert(viewsOut.end(),
//                    this->ModelTransforms.begin(),
//                    this->ModelTransforms.end());
//    return viewsOut;
//}
//
///**
// * Set the user views to the given user views.  This class will take
// * ownership of the user views and delete them when necessary.
// *
// * @param allModelTransforms
// *     New user views.
// */
//void
//CaretPreferences::setAllModelTransforms(std::vector<ModelTransform*>& allModelTransforms)
//{
//    /*
//     * Remove any existing views that are not in the new vector of views
//     */
//    for (std::vector<ModelTransform*>::iterator iter = this->ModelTransforms.begin();
//         iter != this->ModelTransforms.end();
//         iter++) {
//        ModelTransform* uv = *iter;
//        if (std::find(allModelTransforms.begin(),
//                      allModelTransforms.end(),
//                      uv) == allModelTransforms.end()) {
//            delete uv;
//        }
//    }
//    this->ModelTransforms.clear();
//    
//    this->ModelTransforms = allModelTransforms;
//    
//    this->writeModelTransforms();
//}
//
///**
// * Get the user view with the specified name.
// * @param viewName
// *    Name of view.
// * @return
// *    Pointer to view or NULL if not found.
// */
//const ModelTransform* 
//CaretPreferences::getModelTransform(const AString& viewName)
//{
//    for (std::vector<ModelTransform*>::iterator iter = this->ModelTransforms.begin();
//         iter != this->ModelTransforms.end();
//         iter++) {
//        ModelTransform* uv = *iter;
//        if (uv->getName() == viewName) {
//            return uv;
//        }
//    }
//    
//    return NULL;
//}
//
///**
// * Add a user view.  If a view with the same name exists
// * it is replaced.
// * @param
// *    New user view.
// */
//void 
//CaretPreferences::addModelTransform(const ModelTransform& ModelTransform)
//{
//    for (std::vector<ModelTransform*>::iterator iter = this->ModelTransforms.begin();
//         iter != this->ModelTransforms.end();
//         iter++) {
//        ModelTransform* uv = *iter;
//        if (uv->getName() == ModelTransform.getName()) {
//            *uv = ModelTransform;
//            return;
//        }
//    }
//    
//    this->ModelTransforms.push_back(new ModelTransform(ModelTransform));
//    
//    this->writeModelTransforms();
//}
//
///**
// * Remove the user view with the specified name.
// */
//void 
//CaretPreferences::removeModelTransform(const AString& viewName)
//{
//    for (std::vector<ModelTransform*>::iterator iter = this->ModelTransforms.begin();
//         iter != this->ModelTransforms.end();
//         iter++) {
//        ModelTransform* uv = *iter;
//        if (uv->getName() == viewName) {
//            this->ModelTransforms.erase(iter);
//            delete uv;
//            break;
//        }
//    }
//    
//    this->writeModelTransforms();
//}

/**
 * Write the custom views.
 */
void 
CaretPreferences::writeCustomViews()
{
    /*
     * Remove "userViews" that were replaced by customView
     */
    this->qSettings->remove("userViews");
    
    this->qSettings->beginWriteArray(NAME_CUSTOM_VIEWS);
    const int32_t numViews = static_cast<int32_t>(this->customViews.size());
    for (int32_t i = 0; i < numViews; i++) {
        this->qSettings->setArrayIndex(i);
        this->qSettings->setValue(AString::number(i),
                                  this->customViews[i]->getAsString());
    }
    this->qSettings->endArray();
    this->qSettings->sync();
}


/**
 * Remove all of the tile tabs configurations.
 */
void
CaretPreferences::removeAllTileTabsConfigurations()
{
    for (std::vector<TileTabsConfiguration*>::iterator iter = this->tileTabsConfigurations.begin();
         iter != this->tileTabsConfigurations.end();
         iter++) {
        delete *iter;
    }
    this->tileTabsConfigurations.clear();
}

/**
 * Write the tile tabs configurations.
 */
void
CaretPreferences::writeTileTabsConfigurations()
{
    this->qSettings->beginWriteArray(NAME_TILE_TABS_CONFIGURATIONS);
    const int32_t numViews = static_cast<int32_t>(this->tileTabsConfigurations.size());
    for (int32_t i = 0; i < numViews; i++) {
        this->qSettings->setArrayIndex(i);
        this->qSettings->setValue(AString::number(i),
                                  this->tileTabsConfigurations[i]->encodeInXML());
    }
    this->qSettings->endArray();
    this->qSettings->sync();
}

/**
 * Read the tile tabs configuration.  Since user's may want to use them
 * in multiple instance of workbench that are running, this method allows
 * the tile tab configurations to be read without affecting other preferences.
 *
 * @param performSync
 *    Sync with preferences since preferences may have been changed
 *    by a concurrently running workbench.
 */
void
CaretPreferences::readTileTabsConfigurations(const bool performSync)
{
    if (performSync) {
        this->qSettings->sync();
    }
    
    this->removeAllTileTabsConfigurations();
    
    /*
     * Read Configurations
     */
    const int numConfigurations = this->qSettings->beginReadArray(NAME_TILE_TABS_CONFIGURATIONS);
    for (int i = 0; i < numConfigurations; i++) {
        this->qSettings->setArrayIndex(i);
        const AString configString = this->qSettings->value(AString::number(i)).toString();
        TileTabsConfiguration* ttc = new TileTabsConfiguration();
        if (ttc->decodeFromXML(configString)) {
            this->tileTabsConfigurations.push_back(ttc);
        }
        else {
            delete ttc;
        }
    }
    this->qSettings->endArray();
}

/**
 * @return The Tile tabs configurations sorted by name.
 */
std::vector<const TileTabsConfiguration*>
CaretPreferences::getTileTabsConfigurationsSortedByName() const
{
    /*
     * Copy the configurations and sort them by name.
     */
    std::vector<const TileTabsConfiguration*> configurations;
    configurations.insert(configurations.end(),
                          this->tileTabsConfigurations.begin(),
                          this->tileTabsConfigurations.end());    
    std::sort(configurations.begin(),
              configurations.end(),
              TileTabsConfiguration::lessThanComparisonByName);
    
    return configurations;
}

///**
// * @return Unique identifiers of tile tabs configurations that are sorted
// * by the names of the tile tabs configurationsMay want to precede this
// * method with a call to 'readTileTabsConfigurations(true)' so that the 
// * tile tabs are the latest from the settings.
// */
//std::vector<AString>
//CaretPreferences::getTileTabsConfigurationUniqueIdentifiers() const
//{
//    /*
//     * Copy the pointers to the tile tabs configurations and then
//     * sort them by name.
//     */
//    std::vector<TileTabsConfiguration*> configurations = this->tileTabsConfigurations;
//    std::sort(configurations.begin(),
//              configurations.end(),
//              TileTabsConfiguration::lessThanComparisonByName);
//    
//    /*
//     * Get unique IDs
//     */
//    std::vector<AString> uniqueIDs;
//    for (std::vector<TileTabsConfiguration*>::iterator iter = configurations.begin();
//         iter != configurations.end();
//         iter++) {
//        TileTabsConfiguration* ttc = *iter;
//        uniqueIDs.push_back(ttc->getUniqueIdentifier());
//    }
//    
//    return uniqueIDs;
//}

/**
 * Get the tile tabs configuration with the given unique identifier.
 *
 * @param uniqueIdentifier
 *     Unique identifier of the requested tile tabs configuration.
 * @return
 *     Pointer to tile tabs configuration with the given unique identifier
 *     or NULL is it does not exist.
 */
TileTabsConfiguration*
CaretPreferences::getTileTabsConfigurationByUniqueIdentifier(const AString& uniqueIdentifier)
{
    for (std::vector<TileTabsConfiguration*>::const_iterator iter = this->tileTabsConfigurations.begin();
         iter != this->tileTabsConfigurations.end();
         iter++) {
        TileTabsConfiguration* ttc = *iter;
        if (ttc->getUniqueIdentifier() == uniqueIdentifier) {
            return ttc;
        }
    }
    
    return NULL;
}

/**
 * Get the tile tabs configuration with the given unique identifier.
 * 
 * @param uniqueIdentifier
 *     Unique identifier of the requested tile tabs configuration.
 * @return 
 *     Pointer to tile tabs configuration with the given unique identifier
 *     or NULL is it does not exist.
 */
const TileTabsConfiguration*
CaretPreferences::getTileTabsConfigurationByUniqueIdentifier(const AString& uniqueIdentifier) const
{
    for (std::vector<TileTabsConfiguration*>::const_iterator iter = this->tileTabsConfigurations.begin();
         iter != this->tileTabsConfigurations.end();
         iter++) {
        const TileTabsConfiguration* ttc = *iter;
        if (ttc->getUniqueIdentifier() == uniqueIdentifier) {
            return ttc;
        }
    }
    
    return NULL;
}


///**
// * @return Names of tile tabs configurations sorted by name.  May want to precede this
// * method with a call to 'readTileTabsConfigurations(true)' so that the tile tabs
// * are the latest from the settings.
// */
//std::vector<AString>
//CaretPreferences::getTileTabsConfigurationNames() const
//{
//    std::vector<AString> names;
//    
//    for (std::vector<TileTabsConfiguration*>::const_iterator iter = this->tileTabsConfigurations.begin();
//         iter != this->tileTabsConfigurations.end();
//         iter++) {
//        const TileTabsConfiguration* ttc = *iter;
//        names.push_back(ttc->getName());
//    }
//    
//    std::sort(names.begin(),
//              names.end());
//    
//    return names;
//}

/**
 * Get the tile tabs configuration with the given name.
 *
 * @param name
 *     Name of the requested tile tabs configuration.
 * @return
 *     Pointer to tile tabs configuration with the given name
 *     or NULL is it does not exist.
 */
TileTabsConfiguration*
CaretPreferences::getTileTabsConfigurationByName(const AString& name) const
{
    for (std::vector<TileTabsConfiguration*>::const_iterator iter = this->tileTabsConfigurations.begin();
         iter != this->tileTabsConfigurations.end();
         iter++) {
        TileTabsConfiguration* ttc = *iter;
        
        if (name == ttc->getName()) {
            return ttc;
        }
    }
    
    return NULL;
}

///**
// * Get a tile tabs configuration with the given unique identifier.
// *
// * @param tileTabsUniqueIdentifier
// *     Unique identifier of the tile tabs configuration.
// * @param tileTabsConfigurationOut
// *     Tile tabs configuration will be loaded into this tile tabs configuration.
// * @return true if a tile tabs configuration with the unique identifier exists.  If no
// *     tile tabs configuration exists with the unique identifier, false is returned and
// *     the output tile tabs configuration will not be modified.
// */
//bool
//CaretPreferences::getTileTabsConfigurationByUniqueIdentifier(const AString& tileTabsUniqueIdentifier,
//                                                TileTabsConfiguration& tileTabsConfigurationOut) const
//{
//    for (std::vector<TileTabsConfiguration*>::const_iterator iter = this->tileTabsConfigurations.begin();
//         iter != this->tileTabsConfigurations.end();
//         iter++) {
//        const TileTabsConfiguration* ttc = *iter;
//        
//        if (tileTabsUniqueIdentifier == ttc->getUniqueIdentifier()) {
//            tileTabsConfigurationOut = *ttc;
//            return true;
//        }
//    }
//    
//    return false;
//}

/**
 * Add a new tile tabs configuration.
 * 
 * @param tileTabsConfiguration
 *    New tile tabs configuration that is added.
 */
void
CaretPreferences::addTileTabsConfiguration(TileTabsConfiguration* tileTabsConfiguration)
{
    this->tileTabsConfigurations.push_back(tileTabsConfiguration);
    this->writeTileTabsConfigurations();
}

///**
// * Add or update a tile tabs configuration.  If a tile tabs configuration exists with the name
// * in the given tile tabs configuration it is replaced.
// *
// * @param modelTransform
// *    Tile tabs configuration that is added or replaced.
// */
//void
//CaretPreferences::addOrReplaceTileTabsConfiguration(const TileTabsConfiguration& tileTabsConfiguration)
//{
//    bool addNewTileTabsConfiguration = true;
//    
//    for (std::vector<TileTabsConfiguration*>::iterator iter = this->tileTabsConfigurations.begin();
//         iter != this->tileTabsConfigurations.end();
//         iter++) {
//        TileTabsConfiguration* ttc = *iter;
//        if (ttc->getUniqueIdentifier() == tileTabsConfiguration.getUniqueIdentifier()) {
//            *ttc = tileTabsConfiguration;
//            addNewTileTabsConfiguration = false;
//            break;
//        }
//    }
//    
//    if (addNewTileTabsConfiguration) {
//        this->tileTabsConfigurations.push_back(new TileTabsConfiguration(tileTabsConfiguration));
//    }
//    this->writeTileTabsConfigurations();
//}

/**
 * Remove the tile tabs configuration with the given name.
 *
 * @param tileTabsUniqueIdentifier
 *     Unique identifier of configuration that will be removed.
 */
void
CaretPreferences::removeTileTabsConfigurationByUniqueIdentifier(const AString& tileTabsUniqueIdentifier)
{
    for (std::vector<TileTabsConfiguration*>::iterator iter = this->tileTabsConfigurations.begin();
         iter != this->tileTabsConfigurations.end();
         iter++) {
        TileTabsConfiguration* ttc = *iter;
        if (ttc->getUniqueIdentifier() == tileTabsUniqueIdentifier) {
            this->tileTabsConfigurations.erase(iter);
            delete ttc;
            break;
        }
    }
    
    this->writeTileTabsConfigurations();
}

/**
 * Get the foreground color as integer rgb components ranging in value
 * from 0 to 255.
 *
 * @param colorForeground
 *   The output into which rgb color components are loaded.
 */
void 
CaretPreferences::getColorForeground(uint8_t colorForeground[3]) const
{
    colorForeground[0] = this->colorForeground[0];
    colorForeground[1] = this->colorForeground[1];
    colorForeground[2] = this->colorForeground[2];
}

/**
 * Get the foreground color as float rgb components ranging in value
 * from 0 to 1
 *
 * @param colorForeground
 *   The output into which rgb color components are loaded.
 */
void 
CaretPreferences::getColorForeground(float colorForeground[3]) const
{
    uint8_t byteForeground[3];
    this->getColorForeground(byteForeground);
    
    colorForeground[0] = std::min((byteForeground[0] / 255.0), 255.0);
    colorForeground[1] = std::min((byteForeground[1] / 255.0), 255.0);
    colorForeground[2] = std::min((byteForeground[2] / 255.0), 255.0);
}

/**
 * Set the foreground color as integer rgb components ranging in value
 * from 0 to 255.
 *
 * @param colorForeground
 *   New values for the foreground rgb color components.
 */
void 
CaretPreferences::setColorForeground(const uint8_t colorForeground[3])
{
    this->colorForeground[0] = colorForeground[0];
    this->colorForeground[1] = colorForeground[1];
    this->colorForeground[2] = colorForeground[2];
    
    this->qSettings->beginWriteArray(NAME_COLOR_FOREGROUND);
    for (int i = 0; i < 3; i++) {
        this->qSettings->setArrayIndex(i);
        this->qSettings->setValue(AString::number(i),
                                  colorForeground[i]);
    }
    this->qSettings->endArray();
    this->qSettings->sync();
}

/**
 * Get the background color as integer rgb components ranging in value
 * from 0 to 255.
 *
 * @param colorBackground
 *   The output into which rgb color components are loaded.
 */
void 
CaretPreferences::getColorBackground(uint8_t colorBackground[3]) const
{
    colorBackground[0] = this->colorBackground[0];
    colorBackground[1] = this->colorBackground[1];
    colorBackground[2] = this->colorBackground[2];
}

/**
 * Get the background color as float rgb components ranging in value
 * from 0 to 1
 *
 * @param colorBackground
 *   The output into which rgb color components are loaded.
 */
void 
CaretPreferences::getColorBackground(float colorBackground[3]) const
{
    uint8_t byteBackground[3];
    this->getColorBackground(byteBackground);
    
    colorBackground[0] = std::min((byteBackground[0] / 255.0), 255.0);
    colorBackground[1] = std::min((byteBackground[1] / 255.0), 255.0);
    colorBackground[2] = std::min((byteBackground[2] / 255.0), 255.0);
}

/**
 * Set the background color as integer rgb components ranging in value
 * from 0 to 255.
 *
 * @param colorBackground
 *   New values for the background rgb color components.
 */
void 
CaretPreferences::setColorBackground(const uint8_t colorBackground[3])
{
    this->colorBackground[0] = colorBackground[0];
    this->colorBackground[1] = colorBackground[1];
    this->colorBackground[2] = colorBackground[2];
    this->qSettings->beginWriteArray(NAME_COLOR_BACKGROUND);
    for (int i = 0; i < 3; i++) {
        this->qSettings->setArrayIndex(i);
        this->qSettings->setValue(AString::number(i),
                                  colorBackground[i]);
    }
    this->qSettings->endArray();
    this->qSettings->sync();
}

/**
 * Get the previous spec files.
 *
 * @param previousSpecFiles
 *    Will contain previous spec files.
 */
void 
CaretPreferences::getPreviousSpecFiles(std::vector<AString>& previousSpecFiles) const
{
    previousSpecFiles = this->previousSpecFiles;
}

/**
 * Add to the previous spec files.
 * 
 * @param specFileName
 *    Spec file added to the previous spec files.
 */
void 
CaretPreferences::addToPreviousSpecFiles(const AString& specFileName)
{
    if (specFileName.isEmpty() == false) {
        this->addToPrevious(this->previousSpecFiles,
                            specFileName);
    }
    
    const int32_t num = static_cast<int32_t>(this->previousSpecFiles.size());
    this->qSettings->beginWriteArray(NAME_PREVIOUS_SPEC_FILES);
    for (int i = 0; i < num; i++) {
        this->qSettings->setArrayIndex(i);
        this->qSettings->setValue(AString::number(i),
                                  this->previousSpecFiles[i]);
    }
    this->qSettings->endArray();
    this->qSettings->sync();
}

void 
CaretPreferences::clearPreviousSpecFiles()
{
    this->previousSpecFiles.clear();
    this->addToPreviousSpecFiles("");
}

/**
 * Get the directories that were used in the Open File Dialog.
 *
 * @param previousOpenFileDirectories
 *    Will contain previous directories.
 */
void 
CaretPreferences::getPreviousOpenFileDirectories(std::vector<AString>& previousOpenFileDirectories) const
{
    previousOpenFileDirectories = this->previousOpenFileDirectories;
}

/**
 * Get the directories that were used in the Open File Dialog.
 *
 * @param previousOpenFileDirectories
 *    Will contain previous directories.
 */
void 
CaretPreferences::getPreviousOpenFileDirectories(QStringList& previousOpenFileDirectoriesList) const
{
    previousOpenFileDirectoriesList.clear();
    const int32_t numDirectories = static_cast<int32_t>(this->previousOpenFileDirectories.size());
    for (int32_t i = 0; i < numDirectories; i++) {
        previousOpenFileDirectoriesList.append(this->previousOpenFileDirectories[i]);
    }
}

/**
 * Add to the previous directories that were used in the Open File Dialog.
 * 
 * @param directoryName
 *    Directory added to the previous directories from Open File Dialog.
 */
void 
CaretPreferences::addToPreviousOpenFileDirectories(const AString& directoryName)
{
    this->addToPrevious(this->previousOpenFileDirectories,
                        directoryName);
    
    const int32_t num = static_cast<int32_t>(this->previousOpenFileDirectories.size());    
    this->qSettings->beginWriteArray(NAME_PREVIOUS_OPEN_FILE_DIRECTORIES);
    for (int i = 0; i < num; i++) {
        this->qSettings->setArrayIndex(i);
        this->qSettings->setValue(AString::number(i),
                                  this->previousOpenFileDirectories[i]);
    }
    this->qSettings->endArray();
    this->qSettings->sync();
}

/**
 * Add to a list of previous, removing any matching entries
 * and limiting the size of the list.
 *
 * @param previousDeque
 *   Deque containing the previous elements.
 * @param newName
 *   Name that is added at the front.
 */
void 
CaretPreferences::addToPrevious(std::vector<AString>& previousVector,
                                const AString& newName)
{
    /*
     * Note: remove moves duplicate elements to after 'pos' but
     * does not change the size of the container so use erase
     * to remove the duplicate elements from the container.
     */
    std::vector<AString>::iterator pos = std::remove(previousVector.begin(),
                                          previousVector.end(),
                                          newName);
    previousVector.erase(pos, 
                        previousVector.end());

    const uint64_t MAX_ELEMENTS = 10;
    if (previousVector.size() > MAX_ELEMENTS) {
        previousVector.erase(previousVector.begin() + MAX_ELEMENTS,
                            previousVector.end());
    }
    
    previousVector.insert(previousVector.begin(),
                          newName);
}

/**
 * @return  The logging level.
 */
LogLevelEnum::Enum 
CaretPreferences::getLoggingLevel() const
{
    return this->loggingLevel;
}

/**
 * Set the logging level.
 * Will also update the level in the Caret Logger.
 * @param loggingLevel
 *    New value for logging level.
 */
void 
CaretPreferences::setLoggingLevel(const LogLevelEnum::Enum loggingLevel)
{
    this->loggingLevel = loggingLevel;
    
    const AString name = LogLevelEnum::toName(this->loggingLevel);
    this->qSettings->setValue(NAME_LOGGING_LEVEL, name);
    this->qSettings->sync();
    CaretLogger::getLogger()->setLevel(loggingLevel);
}

/**
 * @return The OpenGL Drawing method.
 */
OpenGLDrawingMethodEnum::Enum
CaretPreferences::getOpenDrawingMethod() const
{
    OpenGLDrawingMethodEnum::Enum drawMethod = this->openGLDrawingMethod;

    /*
     * Disable vertex buffers for now
     */
    drawMethod = OpenGLDrawingMethodEnum::DRAW_WITH_VERTEX_BUFFERS_OFF;
    
    return drawMethod;
}

/**
 * Set the OpenGL Drawing method.
 *
 * @param openGLDrawingMethod
 *    New value for OpenGL Drawing method.
 */
void
CaretPreferences::setOpenGLDrawingMethod(const OpenGLDrawingMethodEnum::Enum openGLDrawingMethod)
{
    this->openGLDrawingMethod = openGLDrawingMethod;
    this->setString(NAME_OPENGL_DRAWING_METHOD,
                    OpenGLDrawingMethodEnum::toName(this->openGLDrawingMethod));
}

/**
 * @return The image capture method.
 */
ImageCaptureMethodEnum::Enum
CaretPreferences::getImageCaptureMethod() const
{
    return this->imageCaptureMethod;
}

/**
 * Set the image capture method.
 *
 * @param imageCaptureMethod
 *     New value for image capture method.
 */
void
CaretPreferences::setImageCaptureMethod(const ImageCaptureMethodEnum::Enum imageCaptureMethod)
{
    this->imageCaptureMethod = imageCaptureMethod;
    this->setString(NAME_IMAGE_CAPTURE_METHOD,
                    ImageCaptureMethodEnum::toName(this->imageCaptureMethod));
}


/**
 * @return Save remote login to preferences.
 */
bool
CaretPreferences::isRemoteFilePasswordSaved()
{
    return this->remoteFileLoginSaved;
}

/**
 * Set saving of remote login and password to preferences.
 *
 * @param saveRemoteLoginToPreferences
 *    New status.
 */
void
CaretPreferences::setRemoteFilePasswordSaved(const bool saveRemotePasswordToPreferences)
{
    this->remoteFileLoginSaved = saveRemotePasswordToPreferences;
    this->setBoolean(NAME_REMOTE_FILE_LOGIN_SAVED,
                     this->remoteFileLoginSaved);
    this->qSettings->sync();
}


/**
 * Get the remote file username and password
 *
 * @param userNameOut
 *    Contains username upon exit
 * @param passwordOut
 *    Contains password upon exit.
 */
void
CaretPreferences::getRemoteFileUserNameAndPassword(AString& userNameOut,
                                                   AString& passwordOut) const
{
    userNameOut = this->remoteFileUserName;
    passwordOut = this->remoteFilePassword;
}

/**
 * Set the remote file username and password
 *
 * @param userName
 *    New value for username.
 * @param passwordOut
 *    New value for password.
 */
void
CaretPreferences::setRemoteFileUserNameAndPassword(const AString& userName,
                                                   const AString& password)
{
    this->remoteFileUserName = userName;
    this->remoteFilePassword = password;
    
    this->setString(NAME_REMOTE_FILE_USER_NAME,
                    userName);
    this->setString(NAME_REMOTE_FILE_PASSWORD,
                    password);
    this->qSettings->sync();
}

/**
 * @return  Are axes crosshairs displayed?
 */
bool 
CaretPreferences::isVolumeAxesCrosshairsDisplayed() const
{
    return this->displayVolumeAxesCrosshairs;
}

/**
 * Set axes crosshairs displayed
 * @param displayed
 *   New status.
 */
void 
CaretPreferences::setVolumeAxesCrosshairsDisplayed(const bool displayed)
{
    this->displayVolumeAxesCrosshairs = displayed;
    this->setBoolean(CaretPreferences::NAME_VOLUME_AXES_CROSSHAIRS, 
                     this->displayVolumeAxesCrosshairs);
    this->qSettings->sync();
}

/**
 * @return  Are axes labels displayed?
 */
bool 
CaretPreferences::isVolumeAxesLabelsDisplayed() const
{
    return this->displayVolumeAxesLabels;
}

/**
 * Set axes labels displayed
 * @param displayed
 *   New status.
 */
void 
CaretPreferences::setVolumeAxesLabelsDisplayed(const bool displayed)
{
    this->displayVolumeAxesLabels = displayed;
    this->setBoolean(CaretPreferences::NAME_VOLUME_AXES_LABELS, 
                     this->displayVolumeAxesLabels);
    this->qSettings->sync();
}


/**
 * @return  Are montage axes coordinates displayed?
 */
bool
CaretPreferences::isVolumeMontageAxesCoordinatesDisplayed() const
{
    return this->displayVolumeAxesCoordinates;
}

/**
 * Set montage axes coordinates displayed
 * @param displayed
 *   New status.
 */
void
CaretPreferences::setVolumeMontageAxesCoordinatesDisplayed(const bool displayed)
{
    this->displayVolumeAxesCoordinates = displayed;
    this->setBoolean(CaretPreferences::NAME_VOLUME_AXES_COORDINATE,
                     this->displayVolumeAxesCoordinates);
    this->qSettings->sync();
}

/**
 * @return The volume montage gap.
 */
int32_t
CaretPreferences::getVolumeMontageGap() const
{
    return this->volumeMontageGap;
}

/**
 * Set the volume montage gap.
 *
 * @param volumeMontageGap
 *     New value for montage gap.
 */
void
CaretPreferences::setVolumeMontageGap(const int32_t volumeMontageGap)
{
    this->volumeMontageGap = volumeMontageGap;
    this->setInteger(CaretPreferences::NAME_VOLUME_MONTAGE_GAP,
                     this->volumeMontageGap);
    this->qSettings->sync();
}

/**
 * @return The volume montage coordinate precision
 */
int32_t
CaretPreferences::getVolumeMontageCoordinatePrecision() const
{
    return this->volumeMontageCoordinatePrecision;
}

/**
 * Set the volume montage coordinate precision
 *
 * @param volumeMontageCoordinatePrecision
 *     New value for montage coordinate precision
 */
void
CaretPreferences::setVolumeMontageCoordinatePrecision(const int32_t volumeMontageCoordinatePrecision)
{
    this->volumeMontageCoordinatePrecision = volumeMontageCoordinatePrecision;
    this->setInteger(CaretPreferences::NAME_VOLUME_MONTAGE_COORDINATE_PRECISION,
                     this->volumeMontageCoordinatePrecision);
    this->qSettings->sync();
}

/**
 * @return The toolbox type.
 */
int32_t 
CaretPreferences::getToolBoxType() const
{
    return this->toolBoxType;
}

/**
 * Set the toolbox type.
 * @param toolBoxType
 *    New toolbox type.
 */
void 
CaretPreferences::setToolBoxType(const int32_t toolBoxType)
{
    this->toolBoxType = toolBoxType;
    this->setInteger(CaretPreferences::NAME_TOOLBOX_TYPE, 
                     this->toolBoxType);
    this->qSettings->sync();
}


/**
 * @return Is contralateral identification enabled?
 *
bool 
CaretPreferences::isContralateralIdentificationEnabled() const
{
    return this->contralateralIdentificationEnabled;
}
*/

/**
 * Set contralateral identification enabled.
 * @param enabled
 *    New status.
 *
void 
CaretPreferences::setContralateralIdentificationEnabled(const bool enabled)
{
    this->contralateralIdentificationEnabled = enabled;
    this->setBoolean(CaretPreferences::NAME_IDENTIFICATION_CONTRALATERAL, 
                     this->contralateralIdentificationEnabled);
}
*/

/**
 * @return Is the splash screen enabled?
 */
bool 
CaretPreferences::isSplashScreenEnabled() const
{
    return this->splashScreenEnabled;
}

/**
 * Set the splash screen enabled.
 * @param enabled
 *    New status.
 */
void 
CaretPreferences::setSplashScreenEnabled(const bool enabled)
{
    this->splashScreenEnabled = enabled;
    this->setBoolean(CaretPreferences::NAME_SPLASH_SCREEN, 
                     this->splashScreenEnabled);
    this->qSettings->sync();
}

/**
 * @return Is the Develop Menu enabled?
 */
bool
CaretPreferences::isDevelopMenuEnabled() const
{
    return this->developMenuEnabled;
}

/**
 * Set the Develop Menu enabled.
 * @param enabled
 *    New status.
 */
void
CaretPreferences::setDevelopMenuEnabled(const bool enabled)
{
    this->developMenuEnabled = enabled;
    this->setBoolean(CaretPreferences::NAME_DEVELOP_MENU,
                     this->developMenuEnabled);
    this->qSettings->sync();
}


/**
 * Initialize/Read the preferences
 */
void 
CaretPreferences::readPreferences()
{
    this->colorForeground[0] = 255;
    this->colorForeground[1] = 255;
    this->colorForeground[2] = 255;
    const int numFG = this->qSettings->beginReadArray(NAME_COLOR_FOREGROUND);
    for (int i = 0; i < numFG; i++) {
        this->qSettings->setArrayIndex(i);
        colorForeground[i] = static_cast<uint8_t>(this->qSettings->value(AString::number(i)).toInt());
    }
    this->qSettings->endArray();
    
    this->colorBackground[0] = 0;
    this->colorBackground[1] = 0;
    this->colorBackground[2] = 0;
    const int numBG = this->qSettings->beginReadArray(NAME_COLOR_BACKGROUND);
    for (int i = 0; i < numBG; i++) {
        this->qSettings->setArrayIndex(i);
        colorBackground[i] = static_cast<uint8_t>(this->qSettings->value(AString::number(i)).toInt());
    }
    this->qSettings->endArray();
    
    this->previousSpecFiles.clear();    
    const int numPrevSpec = this->qSettings->beginReadArray(NAME_PREVIOUS_SPEC_FILES);
    for (int i = 0; i < numPrevSpec; i++) {
        this->qSettings->setArrayIndex(i);
        previousSpecFiles.push_back(this->qSettings->value(AString::number(i)).toString());
    }
    this->qSettings->endArray();
    
    this->previousOpenFileDirectories.clear();
    const int numPrevDir = this->qSettings->beginReadArray(NAME_PREVIOUS_OPEN_FILE_DIRECTORIES);
    for (int i = 0; i < numPrevDir; i++) {
        this->qSettings->setArrayIndex(i);
        previousOpenFileDirectories.push_back(this->qSettings->value(AString::number(i)).toString());
    }
    this->qSettings->endArray();
    
    this->readCustomViews(false);
    
    this->readTileTabsConfigurations();

    AString levelName = this->qSettings->value(NAME_LOGGING_LEVEL,
                                          LogLevelEnum::toName(LogLevelEnum::INFO)).toString();
    bool valid = false;
    LogLevelEnum::Enum logLevel = LogLevelEnum::fromName(levelName, &valid);
    if (valid == false) {
        logLevel = LogLevelEnum::INFO;
    }
    this->setLoggingLevel(logLevel);
    
    AString imageCaptureMethodName = this->qSettings->value(NAME_IMAGE_CAPTURE_METHOD,
                                                        ImageCaptureMethodEnum::toName(ImageCaptureMethodEnum::IMAGE_CAPTURE_WITH_RENDER_PIXMAP)).toString();
    bool validImageCaptureMethodName = false;
    this->imageCaptureMethod = ImageCaptureMethodEnum::fromName(imageCaptureMethodName,
                                                            &validImageCaptureMethodName);
    if ( ! validImageCaptureMethodName) {
        this->imageCaptureMethod = ImageCaptureMethodEnum::IMAGE_CAPTURE_WITH_RENDER_PIXMAP;
    }
    
    AString openGLDrawingMethodName = this->qSettings->value(NAME_OPENGL_DRAWING_METHOD,
                                                             OpenGLDrawingMethodEnum::toName(OpenGLDrawingMethodEnum::DRAW_WITH_VERTEX_BUFFERS_OFF)).toString();
    bool validDrawingMethod = false;
    this->openGLDrawingMethod = OpenGLDrawingMethodEnum::fromName(openGLDrawingMethodName,
                                                                  &validDrawingMethod);
    if ( ! validDrawingMethod) {
        this->openGLDrawingMethod = OpenGLDrawingMethodEnum::DRAW_WITH_VERTEX_BUFFERS_OFF;
    }
    
    this->displayVolumeAxesLabels = this->getBoolean(CaretPreferences::NAME_VOLUME_AXES_LABELS,
                                                     true);
    this->displayVolumeAxesCrosshairs = this->getBoolean(CaretPreferences::NAME_VOLUME_AXES_CROSSHAIRS,
                                                         true);    
    this->displayVolumeAxesCoordinates = this->getBoolean(CaretPreferences::NAME_VOLUME_AXES_COORDINATE,
                                                          true);
    
    this->volumeMontageGap = this->getInteger(CaretPreferences::NAME_VOLUME_MONTAGE_GAP,
                                              3);
    
    this->volumeMontageCoordinatePrecision = this->getInteger(CaretPreferences::NAME_VOLUME_MONTAGE_COORDINATE_PRECISION,
                                                              0);
    
    this->animationStartTime = 0.0;//this->qSettings->value(CaretPreferences::NAME_ANIMATION_START_TIME).toDouble();

    this->toolBoxType = this->getInteger(CaretPreferences::NAME_TOOLBOX_TYPE,
                                         0);
    
    this->splashScreenEnabled = this->getBoolean(CaretPreferences::NAME_SPLASH_SCREEN,
                                                 true);
    
    this->developMenuEnabled = this->getBoolean(CaretPreferences::NAME_DEVELOP_MENU,
                                                false);
    
    this->remoteFileUserName = this->getString(NAME_REMOTE_FILE_USER_NAME);
    this->remoteFilePassword = this->getString(NAME_REMOTE_FILE_PASSWORD);
    this->remoteFileLoginSaved = this->getBoolean(NAME_REMOTE_FILE_LOGIN_SAVED,
                                                  false);
    
//    this->contralateralIdentificationEnabled = this->getBoolean(CaretPreferences::NAME_IDENTIFICATION_CONTRALATERAL,
//                                                                   false);
    
}

/**
 * Read the custom views.  Since user's may want to use them
 * in multiple instance of workbench that are running, this method allows
 * the custom views to be read without affecting other preferences.
 *
 * @param performSync
 *    Sync with preferences since preferences may have been changed
 *    by a concurrently running workbench.
 */
void
CaretPreferences::readCustomViews(const bool performSync)
{
    if (performSync) {
        this->qSettings->sync();
    }
    
    this->removeAllCustomViews();
    
    /*
     * Previously had "userViews" prior to CustomViews
     */
    const int numUserViews = this->qSettings->beginReadArray("userViews");
    for (int i = 0; i < numUserViews; i++) {
        this->qSettings->setArrayIndex(i);
        const AString viewString = this->qSettings->value(AString::number(i)).toString();
        ModelTransform uv;
        if (uv.setFromString(viewString)) {
            this->customViews.push_back(new ModelTransform(uv));
        }
    }
    this->qSettings->endArray();

    /*
     * Read Custom Views
     */
    const int numCustomViews = this->qSettings->beginReadArray(NAME_CUSTOM_VIEWS);
    for (int i = 0; i < numCustomViews; i++) {
        this->qSettings->setArrayIndex(i);
        const AString viewString = this->qSettings->value(AString::number(i)).toString();
        ModelTransform uv;
        if (uv.setFromString(viewString)) {
            this->customViews.push_back(new ModelTransform(uv));
        }
    }
    this->qSettings->endArray();    
}


void CaretPreferences::getAnimationStartTime(double& time)
{  
   time = animationStartTime;
   
}

void CaretPreferences::setAnimationStartTime(const double& time)
{
   animationStartTime = time;
   //this->qSettings->setValue(CaretPreferences::NAME_ANIMATION_START_TIME, time);
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
CaretPreferences::toString() const
{
    return "CaretPreferences";
}
