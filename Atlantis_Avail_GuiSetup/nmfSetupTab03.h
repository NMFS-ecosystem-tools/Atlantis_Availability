/**
 * @file nmfSetupTab03.h
 * @brief GUI definition for the Setup Species page class nmfSetup_Tab3
 *
 * This file contains the GUI definitions for the Setup Species page. This
 * page contains the GUI widgets that allow the user to enter and modify
 * Species and Diet Composition data.
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

#ifndef NMFSETUPTAB3_H
#define NMFSETUPTAB3_H

#include <QCheckBox>
#include <QComboBox>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QListView>
#include <QListWidgetItem>
#include <QModelIndex>
#include <QModelIndexList>
#include <QObject>
#include <QPushButton>
#include <QSpinBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTabWidget>
#include <QTextEdit>
#include <QUiLoader>
#include <QWidget>

#include "nmfLogger.h"
#include "nmfConstantsAvailability.h"

#include <boost/numeric/ublas/matrix.hpp>

#include <set>


/**
 * @brief The Setup Tab 3 allows the user to enter and modify Species data
 *
 * This tab allows the user to define Species and Diet Composition data. It consists of 2 sub-tabs.
 * The 1st sub-tab allows the user to select the species from an Atlantis Species Codes file that are
 * to be used in the Availability Calculator.  After selecting the desired species, the user then enters
 * species-specific meta-data. After the species meta-data have been saved, the user may then go to the
 * Diet Composition sub-tab. This sub-tab allows the user to enter in diet composition values for each predator
 * previously defined in the Species sub-tab. Each row in the Diet Composition table must sum to 1.0.
 */
class nmfSetup_Tab3: public QObject
{
    Q_OBJECT

    nmfLogger*                m_Logger;
    std::string               m_ProjectDir;
    std::string               m_ProjectName;
    std::string               m_ProjectSettingsConfig;
    QStandardItemModel*       m_smodelSpecies;
    QStandardItemModel*       m_smodelOtherPredSpecies;
    QStringList               m_colLabelsPredatorTable;
    QStringList               m_colLabelsPredatorTableTooltips;
    QStringList               m_colLabelsPredatorTableWhatsThis;
    QStringList               m_colLabelsPreyOnlyTable;
    QStringList               m_colLabelsPreyOnlyTableTooltips;
    QStringList               m_colLabelsPreyOnlyTableWhatsThis;
    std::vector<std::string>  m_ModelPresetNames;
    std::map<std::string,std::vector<std::string> > m_ModelPresets;
    std::map<QString,QString> m_Taxa;
    std::map<QString,QString> m_Desc;
    std::map<QString,bool>    m_Fished;
    std::map<QString,bool>    m_Predator;
    QStandardItemModel*       m_smodelDietComposition;
    QStandardItemModel*       m_smodelPredator;
    QStandardItemModel*       m_smodelPreyOnly;
    bool                      m_UserPressedViewOnce;
    bool                      m_loadedDietComposition;
    bool                      m_savedDietComposition;
    bool                      m_loadedSpecies;
    bool                      m_savedSpecies;
    double                    m_valueClicked;

    QCheckBox*      Setup_Tab3_FishedCB;
    QCheckBox*      Setup_Tab3_PredatorCB;
    QCheckBox*      Setup_Tab3_ShowZerosCB;
    QCheckBox*      Setup_Tab3_ShowZeroColumnsCB;
    QCheckBox*      Setup_Tab3a_QuickPasteCB;
    QCheckBox*      Setup_Tab3b_QuickPasteCB;
    QCheckBox*      Setup_Tab3_EqualizeCB;
    QDoubleSpinBox* Setup_Tab3a_QuickPasteSB;
    QDoubleSpinBox* Setup_Tab3b_QuickPasteSB;
    QLineEdit*      Setup_Tab3_CodesFileLE;
    QLineEdit*      Setup_Tab3_CodeLE;
    QLineEdit*      Setup_Tab3_DescLE;
    QLineEdit*      Setup_Tab3_TaxaLE;
    QListView*      Setup_Tab3_PotentialSpeciesLV;
    QListView*      Setup_Tab3_SelectedSpeciesLV;
    QPushButton*    Setup_Tab3_BrowsePB;
    QPushButton*    Setup_Tab3_LoadPB;
    QPushButton*    Setup_Tab3_MoveToPotentialPB;
    QPushButton*    Setup_Tab3_MoveToPotentialFromPreyOnlyPB;
    QPushButton*    Setup_Tab3_MoveToSelectedPB;
    QPushButton*    Setup_Tab3_MoveToSelectedPreyOnlyPB;
    QPushButton*    Setup_Tab3_NextPB;
    QPushButton*    Setup_Tab3_PrevPB;
    QPushButton*    Setup_Tab3_ReloadPB;
    QPushButton*    Setup_Tab3_SavePB;
    QPushButton*    Setup_Tab3_EqualizePB;
    QPushButton*    Setup_Tab3_ViewPB;
    QTableView*     Setup_Tab3_PredatorTV;
    QTableView*     Setup_Tab3_PreyOnlyTV;
    QTableView*     Setup_Tab3_DietCompositionTV;
    QTabWidget*     Setup_Tabs;
    QTabWidget*     Setup_Tab3_SpeciesTabW;
    QWidget*        Setup_Tab3_Widget;

    void clearSpeciesWidgets();
    void enableDietCompositionTab(bool isEnabled);
    QString encodePredatorName(QString& predName, QString& predType, QString& preyType);
    QStringList getAllSelectedPredatorAndPreyOnlySpecies();
    QStringList getAllSpecies();
    int getNumberDietCompositionRows(std::vector<std::pair<int,QString> >& subRowVector);
    QString getSpeciesCodesFile();
    void initializeDietCompositionTable();
    void initializeSpeciesTables();
    bool isAllZeros(int col);
    bool isEqualizeEnabled();
    bool isQuickPaste3aEnabled();
    bool isQuickPasteEnabled();
    void loadCodesFile(const QString& filename,
                       const QStringList& exceptForList);
    bool loadDietCompositionData();
    bool loadSelectedPredators();
    bool loadSelectedPreyOnly();
    void loadTab(int tabNum);
    void loadTooltips(const QString& table);
    int  numColumnsGuilds();
    int  numColumnsPredator();
    int  numColumnsPreyOnly();
    QStringList orderTheSpeciesList(const QStringList& speciesList);
    bool passedDietCompositionDataChecks();
    bool passedPredatorDataChecks();
    void populateAPredatorTableRow(const QString& species);
    void populateAPreyOnlyTableRow(const QString& species);
    double quickPasteValue();
    double quickPaste3aValue();
    void readSettings(QString attribute);
    void removeDietCompositionTable();
    void removeFromProjectFiles(QString species);
    bool rowSumOK(const int& row,
                  const int& col,
                  const double& newValue,
                  double& rowTotal);
    void saveDietCompositionData();
    void savePredatorData();
    void savePreyOnlyData();
    void saveSettings();
    void setCheckBoxReadOnly(QCheckBox* checkBox,
                             const bool& readOnly);
    void setCode(const QString& species);
    void setDesc(const QString& desc);
    void setFished(const bool& isFished);
    void setPredator(const bool& isPredator);
    void setTaxa(const QString& taxa);
    void setupConnections();
    void setupDietCompositionTableConnections();
    void setupHelp();
    void setupPredatorTableConnections();
    void setupPreyOnlyTableConnections();
    bool showZeros();
    void sortSelectedTable(QStandardItemModel* smodel,
                           QStringList selectedSpecies);
    void updateSpeciesCodes();
    void updateSpeciesInfoWidgets(const QString& species);

public:
    /**
     * @brief nmfSetup_Tab3 : Class constructor for the Species and Diet Composition Setup GUIs
     * @param tabs : the tab widget into which this Setup tab will be placed
     * @param logger : pointer to the application logger
     * @param projectDir : the project directory
     */
    nmfSetup_Tab3(QTabWidget*  tabs,
                  nmfLogger*   logger,
                  std::string& projectDir);
    virtual ~nmfSetup_Tab3();

    /**
     * @brief Connects and/or disconnects the Item Edited callback for the Diet Composition table
     * @param enable : Boolean to determine if the callback should be enabled or just disabled
     */
    void enableDietCompositionItemEdited(const bool& enable);
    /**
     * @brief Get the list of selected predators from the Selected Predators table
     * @return QStringList representing the list of selected predators
     */
    QStringList getSelectedPredators();
    /**
     * @brief Get the list of selected prey only species from the Selected Prey Only table
     * @return QStringList representing the list of selected prey only species
     */
    QStringList getSelectedPreyOnly();
    /**
     * @brief Checks to see if the user has pressed the Diet Composition Save button once
     * @return Boolean signifying if the Diet Composition Save button has been pressed
     */
    bool hasUserPressedDietCompositionSave();
    /**
     * @brief Checks if the user has pressed the View button at least once. Necessary so that the Output window
     * will not be redrawn and possibly change its visible state already set once by the user.
     * @return Boolean signifying if the user has pressed the View button at least once
     */
    bool hasUserPressedView();
    /**
     * @brief Loads the widgets in the two sub-tabs
     */
    void loadWidgets();
    /**
     * @brief Read Qt system settings from disk
     */
    void readSettings();
    /**
     * @brief Loads model data into the Diet Composition table. Loading is a bit wonky since the format
     * of the Diet Composition table, specifically the first 3 columns, is non-standard (i.e., there are
     * blank cells instead of repeating species names).
     */
    void restoreInitialDietCompositionColumns();
    /**
     * @brief Loads model data into the predator table. Need a method for this because the first column
     * of the table must be read-only as it contains the species name.
     * @param selectedPredators : list of the selected predator names
     */
    void restoreInitialPredatorColumns(QStringList& selectedPredators);
    /**
     * @brief Loads model data into the prey only table. Need a method for this because the first column
     * of the table must be read-only as it contains the species name.
     * @param selectedPreyOnly : list of the selected prey only names
     */
    void restoreInitialPreyOnlyColumns(QStringList& selectedPreyOnly);
    /**
     * @brief Sets the state of the Load button
     * @param state : state of load button (true=enabled, false=disabled)
     */
    void setLoadPBState(bool state);
    /**
     * @brief Determines whether the user is viewing the Diet Composition tab. Useful because the
     * Diet Composition chart should only be visible if the user is viewing the Diet Composition
     * tab and hidden when the user switches to a different tab.
     * @return Boolean determining if the user is viewing the Diet Composition tab.
     */
    bool viewingDietCompositionTab();

signals:
    /**
     * @brief Signal emitted to notify main to enable the Calculation tab
     * @param enable : Boolean signifying whether the Calculation tab should be enabled
     */
    void EnableCalculateTab(bool enable);
    /**
     * @brief Signal emitted when the user wants to view the output chart
     * @param PredatorNames : names of predators in plot
     * @param PreyNames : names of prey only species in plot
     * @param ChartData : data to be represented on plot
     */
    void ShowOutputChart(
            QStringList& PredatorNames,
            QStringList& PreyNames,
            boost::numeric::ublas::matrix<double>& ChartData);
    /**
     * @brief Signal emitted when the user wants to view the output tab
     * @param state : Boolean representing state of output tab (visible or not)
     */
    void ShowOutputTab(bool state);

public Q_SLOTS:
    /**
     * @brief Callback invoked when the user presses the Species Codes file browse button
     */
    void callback_BrowsePB();
    /**
     * @brief Callback invoked when the user changes a cell in the Selected Predator table
     * @param index : QModelIndex of current cell
     */
    void callback_CurrentCellChanged(const QModelIndex& index);
    /**
     * @brief Callback invoked when the user selects a row in the Selected Predator table
     * @param indexCurr : current selected item's QModelIndex
     * @param indexPrev : previous selected item's QModelIndex
     */
    void callback_CurrentRowChanged(const QModelIndex& indexCurr,
                                    const QModelIndex& indexPrev);
    /**
     * @brief Callback invoked when the user selects a horizontal section
     * heading in the Diet Composition table
     * @param section : Selected section column number
     */
    void callback_DietCompositionHorizontalSectionClicked(int section);
    /**
     * @brief Callback invoked when the user edits a Diet Composition cell
     * @param item : the edited item
     */
    void callback_DietCompositionItemEdited(QStandardItem* item);
    /**
     * @brief Callback invoked when the user clicks in the Diet Composition table
     * @param index : QModelIndex of item clicked
     */
    void callback_DietCompositionTableClicked(const QModelIndex& index);
    /**
     * @brief Callback invoked when the user selects a vertical section
     * heading in the Diet Composition table
     * @param section : Selected section row number
     */
    void callback_DietCompositionVerticalSectionClicked(int section);
    /**
     * @brief Callback invoked when the user has pressed the top left Diet Composition button
     * and the Equalize checkbox is checked. All cells in all rows will be equalized, meaning that
     * each row's cells will contain the same fractional value and that the sum of each row's
     * values will equal 1.0.
     */
    void callback_EqualizeAllRows();
    /**
     * @brief Callback invoked when the user checks the Equalize checkbox.
     * @param state : state of the Equalize checkbox (checked or not)
     */
    void callback_EqualizeCB(int state);
    /**
     * @brief Callback invoked when the user presses the Equalize button.
     */
    void callback_EqualizePB();
    /**
     * @brief Callback invoked when the user selects a row with the Equalize checkbox checked.
     * @param row : selected row to equalize (currently unused - embedded logic determines the row via the current selection)
     */
    void callback_EqualizeRow(int row);
    /**
     * @brief Callback invoked when user enters a Species Codes filename and presses Enter
     */
    void callback_FilenameReturnPressed();
    /**
     * @brief Callback invoked when user presses the Load button and the appropriate table is loaded
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when the user clicks the << Prey Only table button
     */
    void callback_MoveToPotentialFromPreyOnlyPB();
    /**
     * @brief Callback invoked when the user clicks the << Predator table button
     */
    void callback_MoveToPotentialPB();
    /**
     * @brief Callback invoked when the user clicks the >> Predator table button
     */
    void callback_MoveToSelectedPB();
    /**
     * @brief Callback invoked when the user clicks the >> Prey Only table button
     */
    void callback_MoveToSelectedPreyOnlyPB();
    /**
     * @brief Callback invoked when the user clicks the Next page button
     */
    void callback_NextPB();
    /**
     * @brief Callback invoked when the user clicks the Previous page button
     */
    void callback_PrevPB();
    /**
     * @brief Callback invoked when the user clicks a horizontal header section in the Predator table
     * @param column : column number of section clicked
     */
    void callback_PredatorHorizontalSectionClicked(int column);
    /**
     * @brief Callback invoked whenever a cell in the Predator table is clicked
     * @param index : QModelIndex index of the cell clicked
     */
    void callback_PredatorTableClicked(const QModelIndex& index);
    /**
     * @brief Callback invoked when the user clicks a horizontal header section in the Prey Only table
     * @param column : column number of section clicked
     */
    void callback_PreyOnlyHorizontalSectionClicked(int column);
    /**
     * @brief Callback invoked whenever a cell in the Prey Only table is clicked
     * @param index : QModelIndex index of the cell clicked
     */
    void callback_PreyOnlyTableClicked(const QModelIndex& index);
    /**
     * @brief Callback invoked when the user checks the QuickPaste checkbox in Tab 3a (Species Tab)
     * @param state : the state of the checkbox (checked or not)
     */
    void callback_QuickPaste3aCB(int state);
    /**
     * @brief Callback invoked when the user checks the QuickPaste checkbox in Tab 3b (Diet Composition Tab)
     * @param state : the state of the checkbox (checked or not)
     */
    void callback_QuickPaste3bCB(int state);
    /**
     * @brief Callback invoked when user clicks the Species Codes Reload (rel) button
     */
    void callback_ReloadPB();
    /**
     * @brief Callback invoked when the user clicks the Save button
     */
    void callback_SavePB();
    /**
     * @brief Callback invoked when the user checks the Show Zero Columns checkbox
     * @param state : state of the checkbox (checked or not)
     */
    void callback_ShowZeroColumnsCB(int state);
    /**
     * @brief Callback invoked when the user checks the Show Zeros checkbox
     * @param state : state of the checkbox (checked or not)
     */
    void callback_ShowZerosCB(int state);
    /**
     * @brief Callback invoked when the user changes sub-tabs. Necessary so specific
     * buttons enabled-ness can be properly set.
     * @param tab : number of current sub-tab
     */
    void callback_SpeciesTabWChanged(int tab);
    /**
     * @brief Callback invoked when the top left button of the Diet Composition table is pressed
     */
    void callback_TopLeftCornerButtonPressed();
    /**
     * @brief Callback invoked when the user changes the application theme type
     * @param unused : unused argument
     */
    void callback_TypeCMB(QString unused);
    /**
     * @brief Callback invoked when the user presses the Chart View button. A chart of Diet Composition values is drawn.
     */
    void callback_ViewPB();
};

#endif
