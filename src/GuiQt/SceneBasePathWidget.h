#ifndef __SCENE_BASE_PATH_WIDGET_H__
#define __SCENE_BASE_PATH_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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



#include <memory>

#include <QWidget>

class QAbstractButton;
class QComboBox;
class QLineEdit;
class QRadioButton;

#include "AString.h"

namespace caret {

    class SceneFile;
    
    class SceneBasePathWidget : public QWidget {
        
        Q_OBJECT

    public:
        SceneBasePathWidget(QWidget* parent = 0);
        
        virtual ~SceneBasePathWidget();
        
        void updateWithSceneFile(SceneFile* sceneFile);

        bool isValid(AString& errorMessageOut) const;
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void customBrowseButtonClicked();
        
        void customPathComboBoxActivated(int index);
        
        void copyAutoBasePathToClipboard();
        
        void basePathTypeButtonGroupClicked(QAbstractButton* button);
        
        void whatsThisBasePath();
        
    private:
        SceneBasePathWidget(const SceneBasePathWidget&);

        SceneBasePathWidget& operator=(const SceneBasePathWidget&);
        
        SceneFile* m_sceneFile = NULL;
        
        QComboBox* m_customBasePathComboBox;
        
        QRadioButton* m_automaticRadioButton;
        
        QRadioButton* m_customRadioButton;
        
        QLineEdit* m_automaticBasePathLineEdit;
        
        static std::vector<AString> s_userCustomBasePaths;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_BASE_PATH_WIDGET_DECLARE__
    std::vector<AString> SceneBasePathWidget::s_userCustomBasePaths;
#endif // __SCENE_BASE_PATH_WIDGET_DECLARE__

} // namespace
#endif  //__SCENE_BASE_PATH_WIDGET_H__
