/**
 * @file nmfMainWindow.h
 * @brief GUI definition for the main nmfMainWindow Availability Tool Application class
 *
 * This file contains the Main Window Application class. All of the menu
 * interactions are done through this class.
 *
 * @copyright
 * Public Domain Notice\n
 *
 * National Oceanic And Atmospheric Administration\n\n
 *
 * This software is a "United States Government Work" under the terms of the
 * United States Copyright Act.  It was written as part of the author's official
 * duties as a United States Government employee/contractor and thus cannot be copyrighted.
 * This software is freely available to the public for use. The National Oceanic
 * And Atmospheric Administration and the U.S. Government have not placed any
 * restriction on its use or reproduction.  Although all reasonable efforts have
 * been taken to ensure the accuracy and reliability of the software and data,
 * the National Oceanic And Atmospheric Administration and the U.S. Government
 * do not and cannot warrant the performance or results that may be obtained
 * by using this software or data. The National Oceanic And Atmospheric
 * Administration and the U.S. Government disclaim all warranties, express
 * or implied, including warranties of performance, merchantability or fitness
 * for any particular purpose.\n\n
 *
 * Please cite the author(s) in any work or product based on this material.
 */

#ifndef NMFMAINWINDOW_H
#define NMFMAINWINDOW_H

#include <QBitmap>
#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QWhatsThis>
#include <QWidget>

#include "nmfConstantsAvailability.h"
#include "nmfChartBar.h"
#include "nmfLogWidget.h"
#include "nmfSetupTab01.h"
#include "nmfSetupTab02.h"
#include "nmfSetupTab03.h"
#include "nmfSetupTab04.h"
#include "nmfStructsQt.h"
#include "nmfUtilsQt.h"
#include "nmfUtilsStatistics.h"

namespace Ui {
    class nmfMainWindow;
}


/**
 * @brief The Atlantis Availability Tool Main Window
 *
 *
 */
class nmfMainWindow : public QMainWindow
{

    Q_OBJECT

private:
    Ui::nmfMainWindow* m_UI;

    QChart*        m_ChartWidget;
    QChartView*    m_ChartView2d;
    nmfLogger*     m_Logger;
    nmfLogWidget*  m_LogWidget;
    std::string    m_ProjectDir;
    std::string    m_ProjectName;
    std::string    m_ProjectSettingsConfig;
    int            m_SetupFontSize;
    QWidget*       m_NavigatorTreeWidget;
    QTreeWidget*   m_NavigatorTree;
    QDialog*       m_PreferencesDlg;
    QWidget*       m_PreferencesWidget;
    QDialog*       m_AvailabilityDlg;
    nmfSetup_Tab1* Setup_Tab1_ptr;
    nmfSetup_Tab2* Setup_Tab2_ptr;
    nmfSetup_Tab3* Setup_Tab3_ptr;
    nmfSetup_Tab4* Setup_Tab4_ptr;

    void        closeEvent(QCloseEvent *event);
    void        enableAutoChecks(
                   QTableView* tv,
                   const bool& enable);
    void        enableCalculateTab(bool isEnabled);
    void        enableLastNavigatorItem(bool state);
    QTableView* findTableInFocus();
    QString     getCurrentStyle();
    void        initConnections();
    void        initGUIs();
    void        initNavigatorTree();
    void        initPostGuiConnections();
    void        initPreferencesDlg();
    bool        isTableReadOnly(QTableView* tv);
    void        loadGuis();
    bool        okSplashScreen();
    bool        queryUserPreviousProject();
    void        readSettings();
    void        readSettingsGuiOrientation(bool alsoResetPosition);
    void        restoreInitialColumns(
                   QStringList& selectedPredators,
                   QStringList& selectedPreyOnly,
                   QTableView*  tableview);
    bool        saveRankDataFile(QString filename);
    bool        saveScreenshot(QString &outputfile, QPixmap &pm);
    void        saveSettings();
    void        saveDefaultSettings();
    void        initLogWidget();
    void        updateWindowTitle();

public:
    /**
     * @brief Main class for the Availability Calculator application
     * @param parent : unused parent widget
     */
    explicit nmfMainWindow(QWidget *parent = nullptr);
    ~nmfMainWindow();

public slots:
    /**
     * @brief Callback invoked when the Calculate Availability tab should be enabled
     * @param enable : new state of Calculate Availability tab
     */
    void callback_EnableCalculateTab(bool enable);
    /**
     * @brief Callback invoked when user wants to load a project
     */
    void callback_LoadProject();
    /**
     * @brief Callback invoked when the user changes an item in the Navigation window
     */
    void callback_NavigatorSelectionChanged();
    /**
     * @brief Callback invoked when the user changes the window style in the Preferences dialog
     * @param style : the desired style of the application (dark or light)
     */
    void callback_PreferencesSetStyleSheet(QString style);
    /**
     * @brief Callback invoked after a project has been saved
     */
    void callback_ProjectSaved();
    /**
     * @brief Callback invoked when the user wants to reset the GUI position
     */
    void callback_ResetGUI();
    /**
     * @brief Callback invoked when the user wants to reset the Application settings
     */
    void callback_ResetSettings();
    /**
     * @brief Callback invoked when user changes to a different current tab and
     * wants to update the Navigator tree
     * @param tab : number of tab clicked on
     */
    void callback_SetupTabChanged(int tab);
    /**
     * @brief Callback invoked when user wants to view the output Diet Composition chart
     * @param predatorNames : names of predator species
     * @param preyNames : names of prey species
     * @param ChartData : matrix containing the data for the chart
     */
    void callback_ShowOutputChart(
            QStringList& predatorNames,
            QStringList& preyNames,
            boost::numeric::ublas::matrix<double>& ChartData);
    /**
     * @brief Callback invoked when the user wants to either show or hide the output chart
     * @param state : Boolean state of the output tab (false=hidden, true=visible)
     */
    void callback_ShowOutputTab(bool state);
    /**
     * @brief Callback invoked when user clicks the X button in the top right of the
     * splash screen. This will close the splash screen and the main GUI will not appear.
     */
    void callback_SplashScreenRejected();
    /**
     * @brief Raise the About dialog with application information
     */
    void menu_about();
    /**
     * @brief Copy current selection in current table
     */
    void menu_copy();
    /**
     * @brief Clear the current selection in current table
     */
    void menu_clear();
    /**
     * @brief Clears all cells in current table
     */
    void menu_clearAll();
    /**
     * @brief Deselect all cells in current table
     */
    void menu_deselectAll();
    /**
     * @brief Paste what's been copied to the currently selected table cell
     */
    void menu_paste();
    /**
     * @brief Paste what's been copied to every cell in current table
     */
    void menu_pasteAll();
    /**
     * @brief Raise the Preferences dialog
     */
    void menu_preferences();
    /**
     * @brief Exit the application without saving
     */
    void menu_quit();
    /**
     * @brief Take a screenshot of the current chart and save to project directory
     */
    void menu_screenShot();
    /**
     * @brief Select every cell in current table
     */
    void menu_selectAll();
    /**
     * @brief Start WhatsThis mode (cursor changes and if the user clicks on a
     * widget that has WhatsThis text defined, a popup displaying that text will appear)
     */
    void menu_whatsThis();
};

#endif
