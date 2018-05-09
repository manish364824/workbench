#ifndef __USER_INPUT_TILE_TABS_CONTEXT_MENU_H__
#define __USER_INPUT_TILE_TABS_CONTEXT_MENU_H__

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

#include <QMenu>

class QWidget;

namespace caret {

    class BrainOpenGLViewportContent;
    
    class UserInputTileTabsContextMenu : public QMenu {
        
        Q_OBJECT

    public:
        UserInputTileTabsContextMenu(QWidget* parentWidget,
                                     BrainOpenGLViewportContent* viewportContent);
        
        virtual ~UserInputTileTabsContextMenu();

        bool isValid() const;

        // ADD_NEW_METHODS_HERE

    private slots:
        void actionTriggered(QAction* action);
        
    private:
        UserInputTileTabsContextMenu(const UserInputTileTabsContextMenu&);

        UserInputTileTabsContextMenu& operator=(const UserInputTileTabsContextMenu&);
        
        QWidget* m_parentWidget;
        
        const int32_t m_windowIndex;
        
        const int32_t m_tabIndex;
        
        QAction* m_createNewTabBeforeAction = nullptr;
        
        QAction* m_createNewTabAfterAction = nullptr;
        
        QAction* m_selectTabAction = nullptr;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __USER_INPUT_TILE_TABS_CONTEXT_MENU_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __USER_INPUT_TILE_TABS_CONTEXT_MENU_DECLARE__

} // namespace
#endif  //__USER_INPUT_TILE_TABS_CONTEXT_MENU_H__
