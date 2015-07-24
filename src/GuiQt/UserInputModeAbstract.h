#ifndef __USER_INPUT_MODE_ABSTRACT_H__
#define __USER_INPUT_MODE_ABSTRACT_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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


#include "CaretObject.h"
#include "CursorEnum.h"

class QWidget;

namespace caret {

    class BrainOpenGLViewportContent;
    class BrainOpenGLWidget;
    class MouseEvent;
    
    class UserInputModeAbstract : public CaretObject {
        
    public:
        /** Enumerated type for input modes */
        enum UserInputMode {
            /** Invalid */
            INVALID,
            /** Border Operations */
            BORDERS,
            /** Foci Operations */
            FOCI,
            /** Viewing Operations */
            VIEW,
            /** Volume Edit Operations */
            VOLUME_EDIT
        };
        
        UserInputModeAbstract(const UserInputMode inputMode);
        
        virtual ~UserInputModeAbstract();
        
        
        /**
         * @return The input mode enumerated type.
         */
        UserInputMode getUserInputMode() const;
        
        /**
         * Called when 'this' user input receiver is set
         * to receive events.
         */
        virtual void initialize() = 0;
        
        /**
         * Called when 'this' user input receiver is no
         * longer set to receive events.
         */
        virtual void finish() = 0;
        
        /**
         * Called to update the input receiver for various events.
         */
        virtual void update() = 0;
        
        QWidget* getWidgetForToolBar();
        
        /**
         * @return The cursor for display in the OpenGL widget.
         */
        virtual CursorEnum::Enum getCursor() const = 0;
        
        /**
         * Process a mouse left click event.
         *
         * @param mouseEvent
         *     Mouse event information.
         */
        virtual void mouseLeftClick(const MouseEvent& /*mouseEvent*/) { }
        
        /**
         * Process a mouse left click with shift key down event.
         *
         * @param mouseEvent
         *     Mouse event information.
         */
        virtual void mouseLeftClickWithShift(const MouseEvent& /*mouseEvent*/) { }
        
        /**
         * Process a mouse left click with ctrl and shift keys down event.
         *
         * @param mouseEvent
         *     Mouse event information.
         */
        virtual void mouseLeftClickWithCtrlShift(const MouseEvent& /*mouseEvent*/) { }
        
        /**
         * Process a mouse left drag with no keys down event.
         *
         * @param mouseEvent
         *     Mouse event information.
         */
        virtual void mouseLeftDrag(const MouseEvent& /*mouseEvent*/) { }
        
        /**
         * Process a mouse left drag with only the alt key down event.
         *
         * @param mouseEvent
         *     Mouse event information.
         */
        virtual void mouseLeftDragWithAlt(const MouseEvent& /*mouseEvent*/) { }
        
        /**
         * Process a mouse left drag with ctrl key down event.
         *
         * @param mouseEvent
         *     Mouse event information.
         */
        virtual void mouseLeftDragWithCtrl(const MouseEvent& /*mouseEvent*/) { }
        
        /**
         * Process a mouse left drag with ctrl and shift keys down event.
         *
         * @param mouseEvent
         *     Mouse event information.
         */
        virtual void mouseLeftDragWithCtrlShift(const MouseEvent& /*mouseEvent*/) { }
        
        /**
         * Process a mouse left drag with shift key down event.
         *
         * @param mouseEvent
         *     Mouse event information.
         */
        virtual void mouseLeftDragWithShift(const MouseEvent& /*mouseEvent*/) { }
        
    protected:
        void setWidgetForToolBar(QWidget* widgetForToolBar);
        
    private:
        UserInputModeAbstract(const UserInputModeAbstract&);

        UserInputModeAbstract& operator=(const UserInputModeAbstract&);

        const UserInputMode m_userInputMode;
        
        QWidget* m_widgetForToolBar;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __USER_INPUT_MODE_ABSTRACT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __USER_INPUT_MODE_ABSTRACT_DECLARE__

} // namespace
#endif  //__USER_INPUT_MODE_ABSTRACT_H__
