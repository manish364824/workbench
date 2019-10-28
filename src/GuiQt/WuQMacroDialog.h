#ifndef __WU_Q_MACRO_DIALOG_H__
#define __WU_Q_MACRO_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#include <QDialog>
#include <QIcon>

#include "WuQMacroCommandParameter.h"
#include "WuQMacroShortCutKeyEnum.h"
#include "WuQMacroStandardItemTypeEnum.h"

class QAbstractButton;
class QCheckBox;
class QComboBox;
class QDialogButtonBox;
class QDoubleSpinBox;
class QGridLayout;
class QLabel;
class QLineEdit;
class QMenu;
class QPlainTextEdit;
class QStackedWidget;
class QStandardItem;
class QTreeView;
class QToolButton;

namespace caret {

    class CommandParameterWidget;
    class WuQMacro;
    class WuQMacroCommand;
    class WuQMacroCommandParameterWidget;
    class WuQMacroGroup;
    class WuQMacroShortCutKeyComboBox;

    class WuQMacroDialog : public QDialog {
        
        Q_OBJECT

    public:
        WuQMacroDialog(QWidget* parent = 0);
        
        virtual ~WuQMacroDialog();
        
        WuQMacroDialog(const WuQMacroDialog&) = delete;

        WuQMacroDialog& operator=(const WuQMacroDialog&) = delete;
        
        void updateDialogContents();

        void restorePositionAndSize();
        
        // ADD_NEW_METHODS_HERE

    public slots:
        void selectMacroCommand(const WuQMacro* macro,
                                const WuQMacroCommand* command);

    private slots:
        void treeViewItemClicked(const QModelIndex& modelIndex);
        
        void treeViewCustomContextMenuRequested(const QPoint& pos);

        void runOnlySelectedCommandMenuItemSelected();
        
        void runAndStartWithSelectedCommandMenuItemSelected();
        
        void runAndStartWithNoDelayDurationSelectedCommandMenuItemSelected();
        
        void runAndStopAfterSelectedCommandMenuItemSelected();
        
        void runAndStopAfterWithNoDelayDurationSelectedCommandMenuItemSelected();
        
        void macroGroupComboBoxActivated(int);
        
        void buttonBoxButtonClicked(QAbstractButton* button);
        
        void importMacroGroupActionTriggered();
        
        void exportMacroGroupActionTriggered();
        
        void macroGroupToolButtonClicked();
        
        void macroGroupResetToolButtonClicked();
        
        void macroNameLineEditTextEdited(const QString& text);
        
        void macroDescriptionTextEditChanged();
        
        void macroShortCutKeySelected(const WuQMacroShortCutKeyEnum::Enum);
        
        void runOptionMoveMouseCheckBoxClicked(bool);
        
        void runOptionLoopCheckBoxClicked(bool);
        
        void runOptionRecordMovieCheckBoxClicked(bool);
        
        void runOptionCreateMovieCheckBoxClicked(bool);
        
        void updateCreateMovieCheckBox();
        
        void runOptionIgnoreDelaysAndDurationsCheckBoxClicked(bool);
        
        void editingMoveUpToolButtonClicked();

        void editingMoveDownToolButtonClicked();

        void editingDeleteToolButtonClicked();

        void editingInsertToolButtonClicked();
        
        void pauseContinueMacroToolButtonClicked();
        
        void runMacroToolButtonClicked();

        void stopMacroToolButtonClicked();
        
        void recordMacroToolButtonClicked();
        
        void macroCommandDelaySpinBoxValueChanged(double);
        
        void macroCommandDescriptionTextEditChanged();
        
        void commandParamaterDataChanged(int);
        
        void insertMenuCopyMacroSelected();
        
        void recordAndInsertNewMacroSelected();
        
        void insertMenuNewMacroSelected();
        
        void insertMenuNewMacroCommandSelected();
        
        void insertMenuRecordNewMacroCommandSelected();
        
        void stopRecordingSelected();
        
        void addNewMacroCommand(WuQMacroCommand* command);
        
        void selectionModelRowChanged(const QModelIndex& current, const QModelIndex& previous);
        
    protected:
        virtual void closeEvent(QCloseEvent* event) override;
        
    private:
        enum class ValueIndex {
            ONE,
            TWO
        };
        
        enum class EditButton {
            DELETER,  /* "DELETE" does not compile on an operating system */
            INSERTER,
            MOVE_DOWN,
            MOVE_UP,
            PAUSE,
            RECORD_OFF,
            RECORD_ON,
            RESET,
            RUN,
            STOP
        };
        
        QWidget* createMacroAndCommandSelectionWidget();
        
        QWidget* createRunOptionsWidget();
        
        QWidget* createMacroDisplayWidget();
        
        QWidget* createCommandDisplayWidget();
        
        void updateMacroWidget(WuQMacro* macro);
        
        void updateCommandWidget(WuQMacroCommand* command);
        
        WuQMacroGroup* getSelectedMacroGroup();
        
        WuQMacro* getSelectedMacro();
        
        WuQMacroCommand* getSelectedMacroCommand();
        
        WuQMacroStandardItemTypeEnum::Enum getSelectedItemType() const;
        
        QStandardItem* getSelectedItem() const;
        
        QWidget* createHorizontalLine() const;
        
        QWidget* createMacroRunAndEditingToolButtons();
        
        QPixmap createEditingToolButtonPixmap(const QWidget* widget,
                                              const EditButton editButton);
        
        void updateEditingToolButtons();
        
        void treeItemSelected(const QModelIndex& modelIndex);
        
        void insertNewMacro(WuQMacro* macro);
        
        QWidget* getWindow();
        
        void runSelectedMacro(const WuQMacroCommand* macroCommandToStartAt,
                              const WuQMacroCommand* macroCommandToStopAfter);
        
        std::vector<WuQMacroGroup*> m_macroGroups;
        
        QComboBox* m_macroGroupComboBox;
        
        QToolButton* m_resetMacroGroupToolButton;
        
        QToolButton* m_macroGroupToolButton;
        
        QTreeView* m_treeView;
        
        QLineEdit* m_macroNameLineEdit;
        
        bool m_macroNameLineEditBlockUpdateFlag = false;
        
        WuQMacroShortCutKeyComboBox* m_macroShortCutKeyComboBox;
        
        QPlainTextEdit* m_macroDescriptionTextEdit;
        
        bool m_macroDescriptionTextEditBlockUpdateFlag = false;
        
        QWidget* m_macroWidget;
        
        QWidget* m_commandWidget;
        
        QWidget* m_emptyWidget;
        
        QStackedWidget* m_stackedWidget;
        
        QDialogButtonBox* m_dialogButtonBox;
        
        QComboBox* m_runOptionsWindowComboBox;
        
        QCheckBox* m_runOptionLoopCheckBox;
        
        QCheckBox* m_runOptionMoveMouseCheckBox;
        
        QCheckBox* m_runOptionRecordMovieWhileMacroRunsCheckBox;
        
        QCheckBox* m_runOptionCreateMovieAfterMacroRunsCheckBox;
        
        QCheckBox* m_ignoreDelaysAndDurationsCheckBox;
        
        QLabel* m_commandTitleLabel;
        
        QLabel* m_commandTypeLabel;
        
        QLabel* m_commandNameLabel;
        
        QDoubleSpinBox* m_commandDelaySpinBox;
        
        QPlainTextEdit* m_commandDescriptionTextEdit;
        
        bool m_macroDescriptionCommandTextEditBlockUpdateFlag = false;
        
        std::vector<WuQMacroCommandParameterWidget*> m_parameterWidgets;
        
        QGridLayout* m_parameterWidgetsGridLayout;
        
        QToolButton* m_pauseMacroToolButton;
        
        QToolButton* m_runMacroToolButton;
        
        QToolButton* m_stopMacroToolButton;
        
        QToolButton* m_recordMacroToolButton;
        
        QIcon m_recordMacroToolButtonIconOff;
        
        QIcon m_recordMacroToolButtonIconOn;
        
        QToolButton* m_editingMoveUpToolButton;
        
        QToolButton* m_editingMoveDownToolButton;
        
        QToolButton* m_editingDeleteToolButton;
        
        QToolButton* m_editingInsertToolButton;
        
        bool m_macroIsRunningFlag = false;
        
        bool m_blockSelectionModelRowChangedFlag = false;
        
        static QByteArray s_previousDialogGeometry;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_DIALOG_DECLARE__
    QByteArray WuQMacroDialog::s_previousDialogGeometry;
#endif // __WU_Q_MACRO_DIALOG_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_DIALOG_H__
