/**
 * @file nmfSetupTab04.h
 * @brief GUI definition for the Availability Calculator Run GUI
 *
 * This file contains the GUI definitions for the Availability Calculator Run GUI. From
 * this page the user may run the calculator and either export the calculated values
 * into an existing Atlantis .prm file or to a .csv file.
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

#ifndef NMFSETUPTAB4_H
#define NMFSETUPTAB4_H

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QHeaderView>
#include <QLabel>
#include <QLine>
#include <QObject>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QStringList>
#include <QTabWidget>
#include <QTableView>
#include <QTextEdit>
#include <QUiLoader>
#include <QWidget>

#include <set>

#include "nmfAvailabilityModelAPI.h"
#include "nmfConstantsAvailability.h"
#include "nmfExportDialog.h"
#include "nmfLogger.h"

/**
 * @brief The Setup Tab 4 allows the user to run the Availability Calculator
 *
 * This page contains the GUI definitions for the Availability Calculator Run GUI. From
 * this page the user may run the calculator and either export the calculated values
 * into an existing Atlantis .prm file or to a .csv file.
 */
class nmfSetup_Tab4: public QObject
{
    Q_OBJECT

    std::string         m_ProjectDir;
    std::string         m_ProjectSettingsConfig;
    std::string         m_ProjectName;
    nmfLogger*          m_Logger;
    QStandardItemModel* m_smodel;
    QStandardItemModel* m_smodel_Original;

    QTabWidget*  Setup_Tabs;
    QTableView*  Setup_Tab4_AvailabilityDataTV;
    QPushButton* Setup_Tab4_CalculatePB;
    QWidget*     Setup_Tab4_Widget;
    QPushButton* Setup_Tab4_SavePB;
    QPushButton* Setup_Tab4_ExportPB;
    QPushButton* Setup_Tab4_PrevPB;
    QCheckBox*   Setup_Tab4_ShowZeroColumnsCB;
    QCheckBox*   Setup_Tab4_ShowZerosCB;
    QComboBox*   Setup_Tab4_DecimalPlacesCMB;

    void getHighlightColors(QString& growthHighlightColor,
                            QString& harvestHighlightColor,
                            QString& competitionHighlightColor,
                            QString& predationHighlightColor,
                            QString& growthColorName,
                            QString& harvestColorName,
                            QString& competitionColorName,
                            QString& predationColorName);
    bool isAllZeros(int col);
    void loadSystem();
    bool saveSettingsConfiguration(bool verbose,
                                   std::string CurrentSettingsName);
    void setModelName(std::string modelName);
    bool showZeros();
    void updateOutputWidget();

public:
    /**
     * @brief nmfSetup_Tab4 : Class constructor for the Run GUI
     * @param tabs : the tab widget into which this tab will be placed
     * @param logger : pointer to the application logger
     * @param projectDir : the project directory
     */
    nmfSetup_Tab4(QTabWidget*  tabs,
                  nmfLogger*   logger,
                  std::string& projectDir);
    virtual ~nmfSetup_Tab4();

    /**
     * @brief Get the number of decimal places the user wants to set the Availability Data to
     * @return The number of desired decimal places for the Availability Data
     */
    int getNumberDecimalPlaces();
    /**
     * @brief Read the application settings from disk
     */
    void readSettings();
    /**
     * @brief Saves Availability Data to a csv file
     */
    void saveAvailabilityData();
    /**
     * @brief Raises the Export Availability Data Gui to allow the user to export the
     * Availability Data into the appropriate Atlantis biological .prm file
     */
    void showExportAvailabilityDataGUI();

public Q_SLOTS:
    /**
     * @brief Callback invoked when the user modifies the number of decimal places combo box
     * @param numberOfDecimalPlaces : number of decimal places to set Availability Data to
     */
    void callback_DecimalPlacesCMB(int numberOfDecimalPlaces);
    /**
     * @brief Callback invoked when the Export button is pressed. Export allows the user to
     * place the newly calculated Availability Data inside an Atlantis biological .prm file.
     */
    void callback_ExportPB();
    /**
     * @brief Callback invoked when the user clicks the Previous Page button
     */
    void callback_PrevPB();
    /**
     * @brief Callback invoked when the user clicks the Run button
     */
    void callback_RunPB();
    /**
     * @brief Callback invoked when the user clicks the Save button. The
     * Availability data are saved to a .csv file.
     */
    void callback_SavePB();
    /**
     * @brief Callback invoked when the user checks the Show Zero Columns checkbox. This allows
     * the user to show or hide columns that consist of all 0 values.
     * @param state : state of the checkbox (checked or not)
     */
    void callback_ShowZeroColumnsCB(int state);
    /**
     * @brief Callback invoked with the user checks the Show Zeros checkbox. This allows
     * the user to view blanks instead of 0 values. This may be useful if the Availability table
     * has lots of 0 values in it.
     * @param state : state of the checkbox (checked or not)
     */
    void callback_ShowZerosCB(int state);
};

#endif
