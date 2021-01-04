
#include "nmfSetupTab03.h"
#include "nmfUtilsQt.h"
#include "nmfUtils.h"
#include "nmfConstants.h"

#include <QCheckBox>

nmfSetup_Tab3::nmfSetup_Tab3(QTabWidget*  tabs,
                             nmfLogger*   logger,
                             std::string& projectDir)
{
    QUiLoader loader;

    Setup_Tabs   = tabs;
    m_Logger     = logger;
    m_ProjectDir = projectDir;
    m_smodelDietComposition = nullptr;
    m_smodelPredator = nullptr;
    m_smodelPreyOnly = nullptr;
    m_ProjectName.clear();
    m_colLabelsPredatorTable.clear();
    m_colLabelsPreyOnlyTable.clear();
    m_colLabelsPredatorTableTooltips.clear();
    m_colLabelsPreyOnlyTableTooltips.clear();
    m_colLabelsPredatorTableWhatsThis.clear();
    m_colLabelsPreyOnlyTableWhatsThis.clear();
    m_Taxa.clear();
    m_Desc.clear();
    m_Fished.clear();
    m_Predator.clear();
    m_UserPressedViewOnce   = false;
    m_loadedDietComposition = false;
    m_savedDietComposition  = false;
    m_loadedSpecies         = false;
    m_savedSpecies          = false;
    m_valueClicked          = 0;

    readSettings();

    // Load ui as a widget from disk
    QFile file(":/forms/Setup/Setup_Tab03.ui");
    file.open(QFile::ReadOnly);
    Setup_Tab3_Widget = loader.load(&file,Setup_Tabs);
    Setup_Tab3_Widget->setObjectName("Setup_Tab3_Widget");
    file.close();

    // Add the loaded widget as the new tabbed page
    Setup_Tabs->addTab(Setup_Tab3_Widget, tr("3. Species Setup"));

    Setup_Tab3_SpeciesTabW        = Setup_Tabs->findChild<QTabWidget     *>("Setup_Tab3_SpeciesTabW");
    Setup_Tab3_PotentialSpeciesLV = Setup_Tabs->findChild<QListView      *>("Setup_Tab3_PotentialSpeciesLV");
    Setup_Tab3_SelectedSpeciesLV  = Setup_Tabs->findChild<QListView      *>("Setup_Tab3_SelectedSpeciesLV");
    Setup_Tab3_PredatorTV         = Setup_Tabs->findChild<QTableView     *>("Setup_Tab3_PredatorTV");
    Setup_Tab3_DietCompositionTV  = Setup_Tabs->findChild<QTableView     *>("Setup_Tab3_DietCompositionTV");
    Setup_Tab3_MoveToSelectedPB   = Setup_Tabs->findChild<QPushButton    *>("Setup_Tab3_MoveToSelectedPB");
    Setup_Tab3_MoveToPotentialPB  = Setup_Tabs->findChild<QPushButton    *>("Setup_Tab3_MoveToPotentialPB");
    Setup_Tab3_BrowsePB           = Setup_Tabs->findChild<QPushButton    *>("Setup_Tab3_BrowsePB");
    Setup_Tab3_ReloadPB           = Setup_Tabs->findChild<QPushButton    *>("Setup_Tab3_ReloadPB");
    Setup_Tab3_LoadPB             = Setup_Tabs->findChild<QPushButton    *>("Setup_Tab3_LoadPB");
    Setup_Tab3_ViewPB             = Setup_Tabs->findChild<QPushButton    *>("Setup_Tab3_ViewPB");
    Setup_Tab3_SavePB             = Setup_Tabs->findChild<QPushButton    *>("Setup_Tab3_SavePB");
    Setup_Tab3_PrevPB             = Setup_Tabs->findChild<QPushButton    *>("Setup_Tab3_PrevPB");
    Setup_Tab3_NextPB             = Setup_Tabs->findChild<QPushButton    *>("Setup_Tab3_NextPB");
    Setup_Tab3_CodesFileLE        = Setup_Tabs->findChild<QLineEdit      *>("Setup_Tab3_CodesFileLE");
    Setup_Tab3_CodeLE             = Setup_Tabs->findChild<QLineEdit      *>("Setup_Tab3_CodeLE");
    Setup_Tab3_TaxaLE             = Setup_Tabs->findChild<QLineEdit      *>("Setup_Tab3_TaxaLE");
    Setup_Tab3_DescLE             = Setup_Tabs->findChild<QLineEdit      *>("Setup_Tab3_DescLE");
    Setup_Tab3_FishedCB           = Setup_Tabs->findChild<QCheckBox      *>("Setup_Tab3_FishedCB");
    Setup_Tab3_PredatorCB         = Setup_Tabs->findChild<QCheckBox      *>("Setup_Tab3_PredatorCB");
    Setup_Tab3_ShowZeroColumnsCB  = Setup_Tabs->findChild<QCheckBox      *>("Setup_Tab3_ShowZeroColumnsCB");
    Setup_Tab3_ShowZerosCB        = Setup_Tabs->findChild<QCheckBox      *>("Setup_Tab3_ShowZerosCB");
    Setup_Tab3a_QuickPasteCB      = Setup_Tabs->findChild<QCheckBox      *>("Setup_Tab3a_QuickPasteCB");
    Setup_Tab3a_QuickPasteSB      = Setup_Tabs->findChild<QDoubleSpinBox *>("Setup_Tab3a_QuickPasteSB");
    Setup_Tab3b_QuickPasteCB      = Setup_Tabs->findChild<QCheckBox      *>("Setup_Tab3b_QuickPasteCB");
    Setup_Tab3b_QuickPasteSB      = Setup_Tabs->findChild<QDoubleSpinBox *>("Setup_Tab3b_QuickPasteSB");
    Setup_Tab3_MoveToSelectedPreyOnlyPB      = Setup_Tabs->findChild<QPushButton *>("Setup_Tab3_MoveToSelectedPreyOnlyPB");
    Setup_Tab3_MoveToPotentialFromPreyOnlyPB = Setup_Tabs->findChild<QPushButton *>("Setup_Tab3_MoveToPotentialFromPreyOnlyPB");
    Setup_Tab3_PreyOnlyTV         = Setup_Tabs->findChild<QTableView     *>("Setup_Tab3_PreyOnlyTV");
    Setup_Tab3_EqualizeCB         = Setup_Tabs->findChild<QCheckBox      *>("Setup_Tab3_EqualizeCB");
    Setup_Tab3_EqualizePB         = Setup_Tabs->findChild<QPushButton    *>("Setup_Tab3_EqualizePB");

    setCheckBoxReadOnly(Setup_Tab3_FishedCB,true);
    setCheckBoxReadOnly(Setup_Tab3_PredatorCB,true);
    enableDietCompositionTab(false);

    Setup_Tab3_PrevPB->setText("\u25C1--");
    Setup_Tab3_NextPB->setText("--\u25B7");
    Setup_Tab3_NextPB->setEnabled(false);
    Setup_Tab3_SavePB->setEnabled(true);
    Setup_Tab3_LoadPB->setEnabled(true);

    // Setup list view model
    QStringListModel* smodel = new QStringListModel();
    Setup_Tab3_PotentialSpeciesLV->setModel(smodel);
    Setup_Tab3_PotentialSpeciesLV->setSelectionMode(QListView::ExtendedSelection);

    setupConnections();
    setupHelp();
}

nmfSetup_Tab3::~nmfSetup_Tab3()
{
}

void
nmfSetup_Tab3::setupConnections()
{
    connect(Setup_Tab3_SpeciesTabW,        SIGNAL(currentChanged(int)),
            this,                          SLOT(callback_SpeciesTabWChanged(int)));
    connect(Setup_Tab3_SavePB,             SIGNAL(clicked()),
            this,                          SLOT(callback_SavePB()));
    connect(Setup_Tab3_ViewPB,             SIGNAL(clicked()),
            this,                          SLOT(callback_ViewPB()));
    connect(Setup_Tab3_LoadPB,             SIGNAL(clicked()),
            this,                          SLOT(callback_LoadPB()));
    connect(Setup_Tab3_PrevPB,             SIGNAL(clicked()),
            this,                          SLOT(callback_PrevPB()));
    connect(Setup_Tab3_NextPB,             SIGNAL(clicked()),
            this,                          SLOT(callback_NextPB()));
    connect(Setup_Tab3_MoveToSelectedPB,   SIGNAL(clicked()),
            this,                          SLOT(callback_MoveToSelectedPB()));
    connect(Setup_Tab3_MoveToPotentialPB,  SIGNAL(clicked()),
            this,                          SLOT(callback_MoveToPotentialPB()));
    connect(Setup_Tab3_BrowsePB,           SIGNAL(clicked()),
            this,                          SLOT(callback_BrowsePB()));
    connect(Setup_Tab3_ReloadPB,           SIGNAL(clicked()),
            this,                          SLOT(callback_ReloadPB()));
    connect(Setup_Tab3_PotentialSpeciesLV->selectionModel(),SIGNAL(currentRowChanged(const QModelIndex&,const QModelIndex&)),
            this,                          SLOT(callback_CurrentRowChanged(const QModelIndex&,const QModelIndex&)));
    connect(Setup_Tab3_CodesFileLE,        SIGNAL(returnPressed()),
            this,                          SLOT(callback_FilenameReturnPressed()));
    connect(Setup_Tab3_MoveToSelectedPreyOnlyPB, SIGNAL(clicked()),
            this,                                SLOT(callback_MoveToSelectedPreyOnlyPB()));
    connect(Setup_Tab3_MoveToPotentialFromPreyOnlyPB, SIGNAL(clicked()),
            this,                                     SLOT(callback_MoveToPotentialFromPreyOnlyPB()));
    connect(Setup_Tab3_EqualizeCB,         SIGNAL(stateChanged(int)),
            this,                          SLOT(callback_EqualizeCB(int)));
    connect(Setup_Tab3_EqualizePB,         SIGNAL(clicked()),
            this,                          SLOT(callback_EqualizePB()));

    setupPredatorTableConnections();
    setupPreyOnlyTableConnections();
    setupDietCompositionTableConnections();
}

void
nmfSetup_Tab3::setupPredatorTableConnections()
{
    disconnect(Setup_Tab3a_QuickPasteCB,   SIGNAL(stateChanged(int)),
            this,                          SLOT(callback_QuickPaste3aCB(int)));
    disconnect(Setup_Tab3_PredatorTV,      SIGNAL(activated(const QModelIndex&)),
            this,                          SLOT(callback_CurrentCellChanged(const QModelIndex&)));
    disconnect(Setup_Tab3_PredatorTV->horizontalHeader(), SIGNAL(sectionClicked(int)),
            this,                          SLOT(callback_PredatorHorizontalSectionClicked(int)));
    disconnect(Setup_Tab3_PredatorTV,      SIGNAL(clicked(const QModelIndex&)),
            this,                          SLOT(callback_PredatorTableClicked(const QModelIndex&)));

    connect(Setup_Tab3a_QuickPasteCB,      SIGNAL(stateChanged(int)),
            this,                          SLOT(callback_QuickPaste3aCB(int)));
    connect(Setup_Tab3_PredatorTV,         SIGNAL(activated(const QModelIndex&)),
            this,                          SLOT(callback_CurrentCellChanged(const QModelIndex&)));
    connect(Setup_Tab3_PredatorTV->horizontalHeader(), SIGNAL(sectionClicked(int)),
            this,                          SLOT(callback_PredatorHorizontalSectionClicked(int)));
    connect(Setup_Tab3_PredatorTV,         SIGNAL(clicked(const QModelIndex&)),
            this,                          SLOT(callback_PredatorTableClicked(const QModelIndex&)));
}

void
nmfSetup_Tab3::setupPreyOnlyTableConnections()
{
    disconnect(Setup_Tab3_PreyOnlyTV->horizontalHeader(), SIGNAL(sectionClicked(int)),
            this,                          SLOT(callback_PreyOnlyHorizontalSectionClicked(int)));
    disconnect(Setup_Tab3_PreyOnlyTV,      SIGNAL(clicked(const QModelIndex&)),
            this,                          SLOT(callback_PreyOnlyTableClicked(const QModelIndex&)));

    connect(Setup_Tab3_PreyOnlyTV->horizontalHeader(), SIGNAL(sectionClicked(int)),
            this,                          SLOT(callback_PreyOnlyHorizontalSectionClicked(int)));
    connect(Setup_Tab3_PreyOnlyTV,         SIGNAL(clicked(const QModelIndex&)),
            this,                          SLOT(callback_PreyOnlyTableClicked(const QModelIndex&)));
}

void
nmfSetup_Tab3::setupDietCompositionTableConnections()
{
    auto TopLeftCornerButton = Setup_Tab3_DietCompositionTV->findChild<QAbstractButton*>(QString(), Qt::FindDirectChildrenOnly);

    disconnect(Setup_Tab3_ShowZeroColumnsCB, SIGNAL(stateChanged(int)),
            this,                            SLOT(callback_ShowZeroColumnsCB(int)));
    disconnect(Setup_Tab3_ShowZerosCB,       SIGNAL(stateChanged(int)),
            this,                            SLOT(callback_ShowZerosCB(int)));
    disconnect(Setup_Tab3b_QuickPasteCB,     SIGNAL(stateChanged(int)),
            this,                            SLOT(callback_QuickPaste3bCB(int)));
    disconnect(Setup_Tab3_DietCompositionTV->verticalHeader(), SIGNAL(sectionClicked(int)),
            this,                            SLOT(callback_EqualizeRow(int)));
    disconnect(Setup_Tab3_DietCompositionTV, SIGNAL(clicked(const QModelIndex&)),
            this,                            SLOT(callback_DietCompositionTableClicked(const QModelIndex&)));
    disconnect(Setup_Tab3_DietCompositionTV, SIGNAL(activated(const QModelIndex&)),
            this,                            SLOT(callback_DietCompositionTableClicked(const QModelIndex&)));
    disconnect(Setup_Tab3_DietCompositionTV->horizontalHeader(), SIGNAL(sectionClicked(int)),
            this,                            SLOT(callback_DietCompositionHorizontalSectionClicked(int)));
    disconnect(Setup_Tab3_DietCompositionTV->verticalHeader(),   SIGNAL(sectionClicked(int)),
            this,                            SLOT(callback_DietCompositionVerticalSectionClicked(int)));
    disconnect(TopLeftCornerButton,          SIGNAL(clicked()),
               this,                         SLOT(callback_TopLeftCornerButtonPressed()));

    connect(Setup_Tab3_ShowZeroColumnsCB,  SIGNAL(stateChanged(int)),
            this,                          SLOT(callback_ShowZeroColumnsCB(int)));
    connect(Setup_Tab3_ShowZerosCB,        SIGNAL(stateChanged(int)),
            this,                          SLOT(callback_ShowZerosCB(int)));
    connect(Setup_Tab3b_QuickPasteCB,      SIGNAL(stateChanged(int)),
            this,                          SLOT(callback_QuickPaste3bCB(int)));
    connect(Setup_Tab3_DietCompositionTV->verticalHeader(), SIGNAL(sectionClicked(int)),
            this,                          SLOT(callback_EqualizeRow(int)));
    connect(Setup_Tab3_DietCompositionTV,  SIGNAL(clicked(const QModelIndex&)),
            this,                          SLOT(callback_DietCompositionTableClicked(const QModelIndex&)));
    connect(Setup_Tab3_DietCompositionTV,  SIGNAL(activated(const QModelIndex&)),
            this,                          SLOT(callback_DietCompositionTableClicked(const QModelIndex&)));
    connect(Setup_Tab3_DietCompositionTV->horizontalHeader(), SIGNAL(sectionClicked(int)),
            this,                          SLOT(callback_DietCompositionHorizontalSectionClicked(int)));
    connect(Setup_Tab3_DietCompositionTV->verticalHeader(),   SIGNAL(sectionClicked(int)),
            this,                          SLOT(callback_DietCompositionVerticalSectionClicked(int)));
    connect(TopLeftCornerButton,           SIGNAL(clicked()),
            this,                          SLOT(callback_TopLeftCornerButtonPressed()));
}

void
nmfSetup_Tab3::setCheckBoxReadOnly(QCheckBox* checkBox,
                                   const bool& readOnly)
{
    checkBox->setAttribute(Qt::WA_TransparentForMouseEvents, readOnly);
    checkBox->setFocusPolicy(readOnly ? Qt::NoFocus : Qt::StrongFocus);
}

void
nmfSetup_Tab3::clearSpeciesWidgets()
{
}

void
nmfSetup_Tab3::loadWidgets()
{
    m_Logger->logMsg(nmfConstants::Normal,"nmfSetup_Tab3::loadWidgets()");
    loadTab(0);
    loadTab(1);
    readSettings();
}

int
nmfSetup_Tab3::numColumnsPredator()
{
    return m_colLabelsPredatorTable.size();
}

int
nmfSetup_Tab3::numColumnsPreyOnly()
{
    return m_colLabelsPreyOnlyTable.size();
}

void
nmfSetup_Tab3::populateAPredatorTableRow(const QString& species)
{
    int nextRow = m_smodelPredator->rowCount();
    QComboBox* cbox;

    m_smodelPredator->setRowCount(nextRow+1);
    QStandardItem *item;
    QModelIndex index;
    for (int col=0;col<numColumnsPredator(); ++col) {
        item = new QStandardItem();
        item->setTextAlignment(Qt::AlignCenter);
        if (col == nmfConstantsAvailability::Predator_Column_Name) {
            item->setText(species);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            m_smodelPredator->setItem(nextRow,col,item);
        } else if (col == nmfConstantsAvailability::Predator_Column_AgeStructure) {
            cbox = new QComboBox();
            cbox->addItems(nmfConstantsAvailability::AgeStructureValues);
            index = m_smodelPredator->index(nextRow,col);
            Setup_Tab3_PredatorTV->setIndexWidget(index,cbox);
        } else if (col == nmfConstantsAvailability::Predator_Column_FunctionalResponse) {
            cbox = new QComboBox();
            cbox->addItems(nmfConstantsAvailability::FunctionalResponseValues);
            index = m_smodelPredator->index(nextRow,col);
            Setup_Tab3_PredatorTV->setIndexWidget(index,cbox);
        } else {
            item->setText("0.0");
            m_smodelPredator->setItem(nextRow,col,item);
        }
    }
    Setup_Tab3_PredatorTV->resizeColumnsToContents();
}

void
nmfSetup_Tab3::populateAPreyOnlyTableRow(const QString& species)
{
    int nextRow     = m_smodelPreyOnly->rowCount();
    int numCols     = numColumnsPreyOnly();

    m_smodelPreyOnly->setRowCount(nextRow+1);
    m_smodelPreyOnly->setColumnCount(numCols);
    m_smodelPreyOnly->setHorizontalHeaderLabels(m_colLabelsPreyOnlyTable);
    QStandardItem *item;
    for (int col=0; col<numCols; ++col) {
        item = new QStandardItem();
        item->setTextAlignment(Qt::AlignCenter);
        if (col == nmfConstantsAvailability::Predator_Column_Name) {
            item->setText(species);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            m_smodelPreyOnly->setItem(nextRow,col,item);
        } else {
            item->setText("0.0");
            m_smodelPreyOnly->setItem(nextRow,col,item);
        }
    }
    Setup_Tab3_PreyOnlyTV->resizeColumnsToContents();
}

void
nmfSetup_Tab3::readSettings(QString attribute)
{
    QSettings* settings = nmfUtilsQt::createSettings(
                nmfConstantsAvailability::SettingsDirWindows,QApplication::applicationName());

    settings->beginGroup("SetupTab");
    Setup_Tab3_CodesFileLE->setText(settings->value(attribute,"").toString());
    settings->endGroup();

    delete settings;
}

void
nmfSetup_Tab3::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(
                nmfConstantsAvailability::SettingsDirWindows,QApplication::applicationName());

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();
    settings->beginGroup("SetupTab");
    m_ProjectDir  = settings->value("ProjectDir", "").toString().toStdString();
    m_ProjectName = settings->value("ProjectName","").toString().toStdString();
    settings->endGroup();

    delete settings;
}

void
nmfSetup_Tab3::removeFromProjectFiles(QString species)
{
    std::string msg;
    QString projectName = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(m_ProjectName));
    QString filename;
    QString line;
    QStringList parts;
    QStringList newLines;

    msg = "Removing all references to: " + species.toStdString();
    m_Logger->logMsg(nmfConstants::Normal,msg);

    // Remove from .species file
    filename = projectName + ".species";
    newLines.clear();
    QFile fileIn(filename);
    if (fileIn.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream stream(&fileIn);
        line = stream.readLine();
        while (! line.isNull()) {
            parts = line.split(",");
            if (species != parts[0]) {
                newLines << line;
            }
            line = stream.readLine();
        }
        fileIn.close();
    } else {
        msg = "Error: Couldn't open file for reading: " + filename.toStdString();
        m_Logger->logMsg(nmfConstants::Error,msg);
        return;
    }
    QFile fileOut(filename);
    if (fileOut.open(QIODevice::WriteOnly|QIODevice::Text)) {
        QTextStream stream(&fileOut);
        for (QString line : newLines) {
            stream << line << "\n";
        }
        fileOut.close();
    }  else {
        msg = "Error: Couldn't open file for writing: " + filename.toStdString();
        m_Logger->logMsg(nmfConstants::Error,msg);
        return;
    }

    // Remove species file from [project].[pred] file
    filename = projectName + "." + species;
    QFile predFile(filename);
    if (predFile.exists()) {
        if (! predFile.remove()) {
            msg = "Error: Couldn't remove file: " + filename.toStdString();
            m_Logger->logMsg(nmfConstants::Error,msg);
            return;
        }
    }

    // Remove all occurrences in .inter file and copy edited lines to newLines
    filename = projectName + ".inter";
    QFile fileInterIn(filename);
    newLines.clear();
    if (fileInterIn.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream stream(&fileInterIn);
        line = stream.readLine();
        while (! line.isNull()) {
            parts = line.split(",");
            if (species != parts[0]) {
                line = line.replace(","+species,"");
                line = line.replace(species+",","");
                newLines << line;
            }
            line = stream.readLine();
        }
        fileInterIn.close();
    } else {
        msg = "Error: Couldn't open file for reading: " + filename.toStdString();
        m_Logger->logMsg(nmfConstants::Error,msg);
        return;
    }
    // Copy newLines back into the .inter file
    QFile fileInterOut(filename);
    if (fileInterOut.open(QIODevice::WriteOnly|QIODevice::Text)) {
        QTextStream stream(&fileInterOut);
        for (QString line : newLines) {
            stream << line << "\n";
        }
        fileInterOut.close();
    } else {
        msg = "Error: Couldn't open file for writing: " + filename.toStdString();
        m_Logger->logMsg(nmfConstants::Error,msg);
        return;
    }
}

void
nmfSetup_Tab3::saveSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(
                nmfConstantsAvailability::SettingsDirWindows,QApplication::applicationName());

    settings->beginGroup("SetupTab");
    settings->setValue("CodesFilename", getSpeciesCodesFile());
    settings->endGroup();

    delete settings;
}

void
nmfSetup_Tab3::savePredatorData()
{
    QString msg;
    QComboBox* cmbox;
    QString predatorFile;
    QModelIndex index;
    int numRows = m_smodelPredator->rowCount();

    if (numRows > 0) {
        predatorFile = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsAvailability::InputDataDir));
        predatorFile = QDir(predatorFile).filePath(QString::fromStdString(nmfConstantsAvailability::PredatorFile));
        std::ofstream outputFileSpecies(predatorFile.toLatin1());
        outputFileSpecies << "Species codes filename," << getSpeciesCodesFile().toStdString() << std::endl;
        for (int row=0; row<numRows;++row) {
            if (row == 0) {
                outputFileSpecies << m_smodelPredator->horizontalHeaderItem(0)->text().toStdString();
                for (int col=nmfConstantsAvailability::Predator_Column_GrowthRate;
                         col<m_smodelPredator->columnCount();++col) {
                    outputFileSpecies << "," << m_smodelPredator->horizontalHeaderItem(col)->text().toStdString();
                }
                outputFileSpecies << std::endl;
            }
             for (int col=0; col<m_smodelPredator->columnCount();++col) {
                if (col > nmfConstantsAvailability::Predator_Column_Name) {
                    outputFileSpecies << ",";
                }
                if (col <= nmfConstantsAvailability::Predator_Column_Exponent) {
                    outputFileSpecies << m_smodelPredator->item(row,col)->text().toStdString();
                } else {
                    index = m_smodelPredator->index(row,col);
                    cmbox = qobject_cast<QComboBox *>(Setup_Tab3_PredatorTV->indexWidget(index));
                    outputFileSpecies << cmbox->currentText().toStdString();
                }
            }
            outputFileSpecies << std::endl;
        }
        outputFileSpecies.close();

        msg = "\nPredator species data saved as:\n\n" + predatorFile + "\n";
        QMessageBox::information(Setup_Tabs, "File Saved", msg);
    }
}

void
nmfSetup_Tab3::savePreyOnlyData()
{
    QString msg;
    QString preyOnlyFile;
    int numRows = m_smodelPreyOnly->rowCount();

    preyOnlyFile = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsAvailability::InputDataDir));
    preyOnlyFile = QDir(preyOnlyFile).filePath(QString::fromStdString(nmfConstantsAvailability::PreyOnlyFile));
    std::ofstream outputFileSpecies(preyOnlyFile.toLatin1());
    outputFileSpecies << "Species codes filename," << getSpeciesCodesFile().toStdString() << std::endl;
    for (int row=0; row<numRows;++row) {
        if (row == 0) {
            outputFileSpecies << m_smodelPreyOnly->horizontalHeaderItem(0)->text().toStdString();
            for (int col=nmfConstantsAvailability::PreyOnly_Column_InitialBiomass;
                     col<m_smodelPreyOnly->columnCount();
                   ++col) {
                outputFileSpecies << "," << m_smodelPreyOnly->horizontalHeaderItem(col)->text().toStdString();
            }
            outputFileSpecies << std::endl;
        }
        for (int col=nmfConstantsAvailability::PreyOnly_Column_Name;
                 col<m_smodelPreyOnly->columnCount(); ++col) {
            if (col > nmfConstantsAvailability::PreyOnly_Column_Name) {
                outputFileSpecies << ",";
            }
            if (col <= nmfConstantsAvailability::PreyOnly_Column_InitialBiomass) {
                outputFileSpecies << m_smodelPreyOnly->item(row,col)->text().toStdString();
            }
        }
        outputFileSpecies << std::endl;
    }
    outputFileSpecies.close();
    if (numRows > 0) {
        msg = "\nPrey Only species data saved as:\n\n" + preyOnlyFile + "\n";
        QMessageBox::information(Setup_Tabs, "File Saved", msg);
    }
}

void
nmfSetup_Tab3::initializeDietCompositionTable()
{
    int m = 0;
    QStandardItem* item;
    std::vector<std::pair<int,QString> > subRowVector;

    // Find number of rows needed in the Diet Composition table
    std::vector<int> numSubRowVector; // how many subrows for every row
    int totalRows = getNumberDietCompositionRows(subRowVector);
    int rowInc = 0;
    QStringList allSpecies = {"Species","Predator Type","Prey Type"};
    // allSpecies += getAllSpecies();
    allSpecies += getAllSelectedPredatorAndPreyOnlySpecies();
    QStringList selectedPredators = getSelectedPredators();

    m_smodelDietComposition = new QStandardItemModel(totalRows, allSpecies.size());
    m_smodelDietComposition->setHorizontalHeaderLabels(allSpecies);

    for (int k=0;k<int(subRowVector.size());++k) {
        for (int row=0; row<subRowVector[k].first; ++row) {
            for (int col=nmfConstantsAvailability::DietComposition_Column_Name;
                     col<m_smodelDietComposition->columnCount(); ++col) {
                item = new QStandardItem();
                item->setTextAlignment(Qt::AlignCenter);
                if (col == nmfConstantsAvailability::DietComposition_Column_Name) {
                    if (row == 0) {
                        item->setText(selectedPredators[m++]);
                    } else {
                        item->setText("");
                    }
                    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                } else if (col == nmfConstantsAvailability::DietComposition_Column_PredatorType) {
                    if (subRowVector[k].second == "All Ages") {
                        if (row == 0) {
                            item->setText("Juvenile");
                        } else if (row == 2) {
                            item->setText("Adult");
                        }
                    } else {
                        if (row == 0) {
                            item->setText(subRowVector[k].second);
                        }
                    }
                    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                } else if (col == nmfConstantsAvailability::DietComposition_Column_PreyType) {
                    if (row % 2) { // row is odd
                        item->setText("Adult");
                    } else {
                        item->setText("Juvenile");
                    }
                    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                } else if (col > nmfConstantsAvailability::DietComposition_Column_PreyType) {
                    item->setText("0.0");
                }
                m_smodelDietComposition->setItem(rowInc,col,item);
            }
            ++rowInc;
        }
    }
    Setup_Tab3_DietCompositionTV->setModel(m_smodelDietComposition);
    Setup_Tab3_DietCompositionTV->resizeColumnsToContents();

    disconnect(m_smodelDietComposition, SIGNAL(itemChanged(QStandardItem*)),
            this,     SLOT(callback_DietCompositionItemEdited(QStandardItem*)));
    connect(m_smodelDietComposition,    SIGNAL(itemChanged(QStandardItem*)),
            this,     SLOT(callback_DietCompositionItemEdited(QStandardItem*)));

    // Adjust for the zero checkboxes
    callback_ShowZeroColumnsCB(Setup_Tab3_ShowZeroColumnsCB->checkState());
    callback_ShowZerosCB(Setup_Tab3_ShowZerosCB->checkState());
}

void
nmfSetup_Tab3::enableDietCompositionItemEdited(const bool& enable)
{
    disconnect(m_smodelDietComposition, SIGNAL(itemChanged(QStandardItem*)),
            this, SLOT(callback_DietCompositionItemEdited(QStandardItem*)));
    if (enable) {
        connect(m_smodelDietComposition, SIGNAL(itemChanged(QStandardItem*)),
                this, SLOT(callback_DietCompositionItemEdited(QStandardItem*)));
    }
}

void
nmfSetup_Tab3::saveDietCompositionData()
{
    QString msg;
    QString value;
    QString lastSpecies;
    QString lastPredatorType;
    QString dietCompositionFile = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsAvailability::InputDataDir));
    dietCompositionFile = QDir(dietCompositionFile).filePath(QString::fromStdString(nmfConstantsAvailability::DietCompositionFile));
    std::ofstream outputFileDietComposition(dietCompositionFile.toLatin1());

    // Write out header
    for (int col=nmfConstantsAvailability::DietComposition_Column_Name;
             col<m_smodelDietComposition->columnCount(); ++col) {
        if (col > nmfConstantsAvailability::DietComposition_Column_Name) {
            outputFileDietComposition << ",";
        }
        outputFileDietComposition << m_smodelDietComposition->horizontalHeaderItem(col)->text().toStdString();
    }
    outputFileDietComposition << std::endl;

    // Write out data
    for (int row=0; row<m_smodelDietComposition->rowCount(); ++row) {
        for (int col=nmfConstantsAvailability::DietComposition_Column_Name;
                 col<m_smodelDietComposition->columnCount(); ++col) {
            value = m_smodelDietComposition->item(row,col)->text();
            if (col > nmfConstantsAvailability::DietComposition_Column_Name) {
                outputFileDietComposition << ",";
            }
            if (! value.isEmpty()) {
                if (col == nmfConstantsAvailability::DietComposition_Column_Name) {
                    lastSpecies = value;
                } else if (col == nmfConstantsAvailability::DietComposition_Column_PredatorType) {
                    lastPredatorType = value;
                }
            } else {
                if (col == nmfConstantsAvailability::DietComposition_Column_Name) {
                    value = lastSpecies;
                } else if (col == nmfConstantsAvailability::DietComposition_Column_PredatorType) {
                    value = lastPredatorType;
                }
            }
            outputFileDietComposition << value.toStdString();
        }
        outputFileDietComposition << std::endl;
    }
    outputFileDietComposition.close();

    msg  = "\nDiet Composition data saved as:\n\n" + dietCompositionFile + "\n";
    QMessageBox::information(Setup_Tabs, "File Saved", msg);
}

void
nmfSetup_Tab3::setupHelp()
{
    m_colLabelsPredatorTable << "Species" << "Growth Rate"
                             << "Initial Biomass" << "Efficiency" << "Clearance Rate"
                             << "Exponent"
                             << "Age Structure" << "Functional Response";
    m_colLabelsPredatorTableTooltips << "Biological Item Name"
                                     << "Growth rate (typically between 0.0 and 1.0)"
                                     << "Feeding Predator Biomass in a Given Cell (mgN/m³)"
                                     << "Assimilation Efficiency (must be between 0.0 and 1.0)"
                                     << "Volume of Water Searched (m³ /mgN /day) (must be between 0.0 and 1.0)"
                                     << "Used only for the Standard Holling Type II½ Functional Response"
                                     << "Age Category (Juvenile or Adult)"
                                     << "Feeding Functional Responses";
    m_colLabelsPreyOnlyTable << "Species" << "Initial Biomass";
    m_colLabelsPreyOnlyTableTooltips << "Biological Item Name"
                                     << "Prey Biomass (mgN/m³)";
    m_colLabelsPredatorTableWhatsThis << ""
                                      << ""
                                      << ""
                                      << ""
                                      << ""
                                      << "<strong><center>Exponent</center></strong>"\
                                         "<p>The Standard Holling II½ functional response is equivalent to the  "\
                                         "Standard Holling III functional response equation with the only change being "\
                                         "that instead of a fixed exponent of 2, the exponent can be supplied by the user. "\
                                         "Acceptable exponent values are between " +
                                         QString::number(nmfConstantsAvailability::MinExponent,'f',1) + " and " +
                                         QString::number(nmfConstantsAvailability::MaxExponent,'f',1) +
                                         ", inclusive.</p>"
                                      << "<strong><center>Age Structure</center></strong>"\
                                         "<p>Currently there are 3 Age Structure groups.<br><br>They are:</p>"\
                                         "<p><ol>"\
                                         "<li>Juvenile Only</li>"\
                                         "<li>Adult Only</li>"\
                                         "<li>All Ages</li>"
                                      << "<strong><center>Functional Response</center></strong>"\
                                         "<p>Currently there are 4 Functional Response equations implemented.<br><br>They are:</p>"\
                                         "<p><ol>"\
                                         "<li>  Type I</li>"\
                                         "<br>D(pi) = B(p)C(p)a(pi)B(i) / ∑B(prey)<br>"\
                                         "<li>Standard Holling Type II</li>"\
                                         "<br>D(pi) = B(p)C(p)a(pi)B(i) / [1 + (C(p)E(p)/g(p)) x ∑B(prey)]<br>"\
                                         "<li>Modified Holling Type II</li>"\
                                         "<br>D(pi) = B(p)C(p)a(pi)B(i) / [1 + (C(p)E(p)/g(p)) x ∑(a(prey)B(prey))]<br>"\
                                         "<li>Standard Holling Type III</li>"\
                                         "<br>D(pi) = B(p)C(p)a²(pi)B²(i) / [1 + (C(p)E(p)/g(p)) x ∑B²(prey)]<br>"\
                                         "<li>Modified Holling Type III</li>"\
                                         "<br>D(pi) = B(p)C(p)a²(pi)B²(i) / [1 + (C(p)E(p)/g(p)) x ∑(a²(prey)B²(prey))]<br>"\
                                         "</ol>where</p><br>"\
                                         "D = Diet Composition Fraction<br>"\
                                         "B = Biomass<br>"\
                                         "C = Clearance Rate<br>"\
                                         "E = Assimilation Efficiency<br>"\
                                         "g = Growth Rate<br>"
                                         "a = Availability Value<br>"\
                                         "<br>and<br><br>"\
                                         "index p refers to a predator<br>"\
                                         "index i refers to a prey species<br>"
                                         "∑ is over all prey species";
    m_colLabelsPreyOnlyTableWhatsThis << ""
                                      << "";
}

void
nmfSetup_Tab3::callback_MoveToSelectedPB()
{
    QStringListModel* smodel     = qobject_cast<QStringListModel*>(Setup_Tab3_PotentialSpeciesLV->model());
    QStringList potentialSpecies = smodel->stringList();
    QStringList newList;
    QStringList selectedItems;
    QString species;
    std::vector<nmfConstantsAvailability::PredatorStruct> currentlySelectedItems;
    nmfConstantsAvailability::PredatorStruct predatorItem;
    std::map<QString,nmfConstantsAvailability::PredatorStruct> selectedItemMap;
    QComboBox* cmbox;
    QModelIndex index;

    // Store current items that have been moved to Selected table
    for (int row=0; row<m_smodelPredator->rowCount(); ++row) {
        species                         = m_smodelPredator->item(row,nmfConstantsAvailability::Predator_Column_Name)->text();
        predatorItem.growthRate         = m_smodelPredator->item(row,nmfConstantsAvailability::Predator_Column_GrowthRate)->text();
        predatorItem.initialBiomass     = m_smodelPredator->item(row,nmfConstantsAvailability::Predator_Column_InitialBiomass)->text();
        predatorItem.efficiency         = m_smodelPredator->item(row,nmfConstantsAvailability::Predator_Column_Efficiency)->text();
        predatorItem.clearanceRate      = m_smodelPredator->item(row,nmfConstantsAvailability::Predator_Column_ClearanceRate)->text();
        predatorItem.exponent           = m_smodelPredator->item(row,nmfConstantsAvailability::Predator_Column_Exponent)->text();
        index = m_smodelPredator->index(row,nmfConstantsAvailability::Predator_Column_AgeStructure);
        cmbox = qobject_cast<QComboBox *>(Setup_Tab3_PredatorTV->indexWidget(index));
        predatorItem.ageStructure       = cmbox->currentText();
        index = m_smodelPredator->index(row,nmfConstantsAvailability::Predator_Column_FunctionalResponse);
        cmbox = qobject_cast<QComboBox *>(Setup_Tab3_PredatorTV->indexWidget(index));
        predatorItem.functionalResponse = cmbox->currentText();
        selectedItemMap[species] = predatorItem;
    }

    // Find selected rows in potential species list
    QModelIndexList indexes = Setup_Tab3_PotentialSpeciesLV->selectionModel()->selectedIndexes();
    for (QModelIndex index : indexes) {
        species = index.data().toString();
        selectedItems << species;
        populateAPredatorTableRow(species);
    }

    // Now remove selectedItems from potential species list and add a row to the species table
    for (int i=0; i<potentialSpecies.size(); ++i) {
        if (! selectedItems.contains(potentialSpecies[i])) {
            newList << potentialSpecies[i];
        }
    }
    smodel->setStringList(newList);

    // Sort selected species according to order in file and re-organize the table accordingly
    sortSelectedTable(m_smodelPredator,selectedItems);

    // Write back out what was in the species cells prior to adding more rows
    for (int row=0; row<m_smodelPredator->rowCount(); ++row) {
        species = m_smodelPredator->item(row,0)->text();
        if (selectedItemMap.count(species)) {
            predatorItem = selectedItemMap[species];
            m_smodelPredator->item(row,nmfConstantsAvailability::Predator_Column_GrowthRate)->setText(predatorItem.growthRate);
            m_smodelPredator->item(row,nmfConstantsAvailability::Predator_Column_InitialBiomass)->setText(predatorItem.initialBiomass);
            m_smodelPredator->item(row,nmfConstantsAvailability::Predator_Column_Efficiency)->setText(predatorItem.efficiency);
            m_smodelPredator->item(row,nmfConstantsAvailability::Predator_Column_ClearanceRate)->setText(predatorItem.clearanceRate);
            m_smodelPredator->item(row,nmfConstantsAvailability::Predator_Column_Exponent)->setText(predatorItem.exponent);
            index = m_smodelPredator->index(row,nmfConstantsAvailability::Predator_Column_AgeStructure);
            cmbox = qobject_cast<QComboBox *>(Setup_Tab3_PredatorTV->indexWidget(index));
            cmbox->setCurrentText(predatorItem.ageStructure);
            index = m_smodelPredator->index(row,nmfConstantsAvailability::Predator_Column_FunctionalResponse);
            cmbox = qobject_cast<QComboBox *>(Setup_Tab3_PredatorTV->indexWidget(index));
            cmbox->setCurrentText(predatorItem.functionalResponse);
        } else {
            m_smodelPredator->item(row,nmfConstantsAvailability::Predator_Column_GrowthRate)->setText("0");
            m_smodelPredator->item(row,nmfConstantsAvailability::Predator_Column_InitialBiomass)->setText("0");
            m_smodelPredator->item(row,nmfConstantsAvailability::Predator_Column_Efficiency)->setText("0");
            m_smodelPredator->item(row,nmfConstantsAvailability::Predator_Column_ClearanceRate)->setText("0");
            m_smodelPredator->item(row,nmfConstantsAvailability::Predator_Column_Exponent)->setText("1");
            index = m_smodelPredator->index(row,nmfConstantsAvailability::Predator_Column_AgeStructure);
            cmbox = qobject_cast<QComboBox *>(Setup_Tab3_PredatorTV->indexWidget(index));
            cmbox->setCurrentIndex(0);
            index = m_smodelPredator->index(row,nmfConstantsAvailability::Predator_Column_FunctionalResponse);
            cmbox = qobject_cast<QComboBox *>(Setup_Tab3_PredatorTV->indexWidget(index));
            cmbox->setCurrentIndex(0);
        }
    }
    setupPredatorTableConnections();
}

void
nmfSetup_Tab3::callback_MoveToPotentialPB()
{
    QStringList selectedSpecies;
    QStringList nonselectedSpecies;
    QStandardItem* item;
    QSet<int> selectedRows;

    // Find selected rows in table
    QModelIndexList selectedList = Setup_Tab3_PredatorTV->selectionModel()->selectedRows();
    for (QModelIndex index : selectedList) {
        selectedRows.insert(index.row());
    }
    // Find the predator names of those selected rows
    QList<int> selectedRowsList = selectedRows.values();
    for (int i=0; i<selectedRowsList.size(); ++i) {
        item = m_smodelPredator->item(selectedRowsList[i],0);
        selectedSpecies << item->text();
    }

    // Find non selected species names in predator table
    for (int i=0; i<m_smodelPredator->rowCount(); ++i) {
        item = m_smodelPredator->item(i,0);
        if (! selectedSpecies.contains(item->text())) {
            nonselectedSpecies << item->text();
        }
    }
    // Add to the nonselectedSpecies all of the species in the Prey Only table
    for (int i=0; i<m_smodelPreyOnly->rowCount(); ++i) {
        item = m_smodelPreyOnly->item(i,0);
        nonselectedSpecies << item->text();
    }

    // Remove selectedRows
    std::sort(selectedRowsList.begin(),selectedRowsList.end());
    for (int i=selectedRows.size()-1; i>=0; --i) {
        // Need to do the following check as Qt will crash when deleting the last QTableWidget row if not checked.
        if ((i == 0) && (m_smodelPredator->rowCount() == 1)) {
            Setup_Tab3_PredatorTV->clearSelection();
            Setup_Tab3_PredatorTV->disconnect();
        }
        m_smodelPredator->removeRow(selectedRowsList[i]);
    }

    // Reload codes except for the names still in the table
    QString filename = getSpeciesCodesFile();
    if (! filename.isEmpty()) {
        loadCodesFile(filename,nonselectedSpecies);
    }
}

void
nmfSetup_Tab3::callback_MoveToSelectedPreyOnlyPB()
{
    QStringListModel* smodel     = qobject_cast<QStringListModel*>(Setup_Tab3_PotentialSpeciesLV->model());
    QStringList potentialSpecies = smodel->stringList();
    QStringList newList;
    QStringList selectedItems;
    QString species;
    std::vector<nmfConstantsAvailability::PreyOnlyStruct> currentlySelectedItems;
    nmfConstantsAvailability::PreyOnlyStruct preyItem;
    std::map<QString,nmfConstantsAvailability::PreyOnlyStruct> selectedItemMap;

    // Store current items that have been moved to Selected Prey Only table
    for (int row=0; row<m_smodelPreyOnly->rowCount(); ++row) {
        species                  = m_smodelPreyOnly->item(row,0)->text();
        preyItem.initialBiomass  = m_smodelPreyOnly->item(row,1)->text();
        selectedItemMap[species] = preyItem;
    }

    // Find selected rows in potential species list
    QModelIndexList indexes = Setup_Tab3_PotentialSpeciesLV->selectionModel()->selectedIndexes();
    for (QModelIndex index : indexes) {
        species = index.data().toString();
        selectedItems << species;
        populateAPreyOnlyTableRow(species);
    }

    // Now remove selectedItems from potential species list and add a row to the species table
    for (int i=0; i<potentialSpecies.size(); ++i) {
        if (! selectedItems.contains(potentialSpecies[i])) {
            newList << potentialSpecies[i];
        }
    }
    smodel->setStringList(newList);

    // Sort selected species according to order in file and re-organize the table accordingly
    sortSelectedTable(m_smodelPreyOnly,selectedItems);

    // Write back out what was in the species cells prior to adding more rows
    for (int row=0; row<m_smodelPreyOnly->rowCount(); ++row) {
        species = m_smodelPreyOnly->item(row,0)->text();
        if (selectedItemMap.count(species)) {
            preyItem = selectedItemMap[species];
            m_smodelPreyOnly->item(row,1)->setText(preyItem.initialBiomass);
        } else {
            m_smodelPreyOnly->item(row,1)->setText("0.0");
        }
    }

    setupPreyOnlyTableConnections();
}

void
nmfSetup_Tab3::callback_MoveToPotentialFromPreyOnlyPB()
{
    QStringList selectedSpecies;
    QStringList nonselectedSpecies;
    QStandardItem* item;
    QSet<int> selectedRows;

    // Find selected rows in table
    QModelIndexList selectedList = Setup_Tab3_PreyOnlyTV->selectionModel()->selectedRows();
    for (QModelIndex index : selectedList) {
        selectedRows.insert(index.row());
    }
    // Find the names of those selected rows
    QList<int> selectedRowsList = selectedRows.values();
    for (int i=0; i<selectedRowsList.size(); ++i) {
        item = m_smodelPreyOnly->item(selectedRowsList[i],0);
        selectedSpecies << item->text();
    }

    // Find non selected species names in prey only table
    for (int i=0; i<m_smodelPreyOnly->rowCount(); ++i) {
        item = m_smodelPreyOnly->item(i,0);
        if (! selectedSpecies.contains(item->text())) {
            nonselectedSpecies << item->text();
        }
    }
    // Add to the nonselectedSpecies all of the species in the Predator table
    for (int i=0; i<m_smodelPredator->rowCount(); ++i) {
        item = m_smodelPredator->item(i,0);
        nonselectedSpecies << item->text();
    }

    // Remove selectedRows
    std::sort(selectedRowsList.begin(),selectedRowsList.end());
    for (int i=selectedRows.size()-1; i>=0; --i) {
        // Need to do the following check as Qt will crash when deleting the last QTableWidget row if not checked.
        if ((i == 0) && (m_smodelPreyOnly->rowCount() == 1)) {
            Setup_Tab3_PreyOnlyTV->selectionModel()->clearSelection();
            Setup_Tab3_PreyOnlyTV->disconnect();
        }
        m_smodelPreyOnly->removeRow(selectedRowsList[i]);
    }

    // Reload codes except for the names still in the table
    QString filename = getSpeciesCodesFile();
    if (! filename.isEmpty()) {
        loadCodesFile(filename,nonselectedSpecies);
    }
}

QStringList
nmfSetup_Tab3::orderTheSpeciesList(const QStringList& speciesList)
{
    QFile file(getSpeciesCodesFile());
    QStringList parts;
    QStringList orderedSpeciesList;
    QStringList currentSelectedSpecies;
    QStringList totalSelectedSpecies;
    QString species;
    QString msg;

    bool firstLine = true;
    if ( file.open(QIODevice::ReadOnly|QIODevice::Text) )
    {
        QTextStream stream(&file);
        QString line = stream.readLine();
        while (! line.isNull()) {
            if (! firstLine) {
                species = line.split(",")[0];
                if (speciesList.contains(species)) {
                    orderedSpeciesList << species;
                }
            }
            line = stream.readLine();
            firstLine = false;
        }

        file.close();
        saveSettings();
    }
    return orderedSpeciesList;
}

void
nmfSetup_Tab3::sortSelectedTable(QStandardItemModel* smodel,
                                 QStringList newSelectedSpecies)
{
    QFile file(getSpeciesCodesFile());
    QStringList parts;
    QStringList speciesList;
    QStringList currentSelectedSpecies;
    QStringList totalSelectedSpecies;
    QString species;
    QString msg;
    int row = 0;

    // Add current items in selected table to selectedSpecies
    for (int row=0; row<smodel->rowCount(); ++row) {
        currentSelectedSpecies << smodel->item(row,0)->text();
    }
    totalSelectedSpecies = newSelectedSpecies + currentSelectedSpecies;

    bool firstLine = true;
    if ( file.open(QIODevice::ReadOnly|QIODevice::Text) )
    {
        QTextStream stream(&file);
        QString line = stream.readLine();
        while (! line.isNull()) {
            if (! firstLine) {
                species = line.split(",")[0];
                if (totalSelectedSpecies.contains(species)) {
                    smodel->item(row++,0)->setText(species);
                }
            }
            line = stream.readLine();
            firstLine = false;
        }

        file.close();
        saveSettings();
    }
}

void
nmfSetup_Tab3::callback_BrowsePB()
{
    QString filename = QFileDialog::getOpenFileName(Setup_Tabs,
        tr("Open Codes File"), "", tr("Codes Files (*.csv)"));
    if (! filename.isEmpty()) {
        Setup_Tab3_CodesFileLE->setText(filename);
        loadCodesFile(filename,{});
        initializeSpeciesTables();
    }
}

void
nmfSetup_Tab3::callback_FilenameReturnPressed()
{
    loadCodesFile(getSpeciesCodesFile(),{});
    initializeSpeciesTables();
}

void
nmfSetup_Tab3::callback_ReloadPB()
{
    readSettings("CodesFilename");
    loadCodesFile(getSpeciesCodesFile(),{});
    initializeSpeciesTables();
}

void
nmfSetup_Tab3::loadCodesFile(const QString& filename,
                             const QStringList& exceptForList)
{
    // Read filename and load potential species listview
    // Ignore the first line and parse the remaining lines
    QString allSpeciesFile = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsAvailability::InputDataDir));
    allSpeciesFile = QDir(allSpeciesFile).filePath(QString::fromStdString(nmfConstantsAvailability::AllSpeciesFile));
    std::ofstream outputFile(allSpeciesFile.toLatin1());

    QFile file(filename);
    QStringList parts;
    QStringList speciesList;
    QString species;
    QString msg;

    m_Taxa.clear();
    m_Desc.clear();
    m_Fished.clear();
    m_Predator.clear();

    bool firstLine = true;
    if ( file.open(QIODevice::ReadOnly|QIODevice::Text) )
    {
        QTextStream stream(&file);
        QString line = stream.readLine();
        while (! line.isNull()) {
            if (! firstLine) {
                parts = line.split(",");
                species = parts[0];
                outputFile << species.toStdString() << std::endl;
                if (! exceptForList.contains(parts[0])) {
                    speciesList << species;
                }
                m_Taxa[species]     =  parts[3];
                m_Desc[species]     =  parts[4];
                m_Fished[species]   = (parts[13] == "1");
                m_Predator[species] = (parts[17] == "1");
            }
            line = stream.readLine();
            firstLine = false;
        }
        file.close();
        outputFile.close();
        saveSettings();
    } else {
        msg = "\nCouldn't open file: " + filename + "\n";
        QMessageBox::information(Setup_Tabs, "Error", msg);
    }

    QStringListModel* smodel = qobject_cast<QStringListModel*>(Setup_Tab3_PotentialSpeciesLV->model());
    smodel->setStringList(speciesList);
}

void
nmfSetup_Tab3::initializeSpeciesTables()
{
    m_smodelPredator = new QStandardItemModel(0, m_colLabelsPredatorTable.size());
    m_smodelPredator->setHorizontalHeaderLabels(m_colLabelsPredatorTable);
    Setup_Tab3_PredatorTV->setModel(m_smodelPredator);
    loadTooltips("Predator");

    m_smodelPreyOnly = new QStandardItemModel(0, m_colLabelsPreyOnlyTable.size());
    m_smodelPreyOnly->setHorizontalHeaderLabels(m_colLabelsPreyOnlyTable);
    Setup_Tab3_PreyOnlyTV->setModel(m_smodelPreyOnly);
    loadTooltips("PreyOnly");
}

void
nmfSetup_Tab3::loadTooltips(const QString& table)
{
    if (table == "Predator") {
        for (int i=0; i<m_colLabelsPredatorTableTooltips.size(); ++i) {
            m_smodelPredator->horizontalHeaderItem(i)->setToolTip(m_colLabelsPredatorTableTooltips[i]);
            m_smodelPredator->horizontalHeaderItem(i)->setWhatsThis(m_colLabelsPredatorTableWhatsThis[i]);
        }
    } else if (table == "PreyOnly") {
        for (int i=0; i<m_colLabelsPreyOnlyTableTooltips.size(); ++i) {
            m_smodelPreyOnly->horizontalHeaderItem(i)->setToolTip(m_colLabelsPreyOnlyTableTooltips[i]);
            m_smodelPredator->horizontalHeaderItem(i)->setWhatsThis(m_colLabelsPreyOnlyTableWhatsThis[i]);
        }
    }
}

void
nmfSetup_Tab3::callback_NextPB()
{
    if (Setup_Tab3_SpeciesTabW->currentIndex() == 0) {
        Setup_Tab3_SpeciesTabW->setCurrentIndex(1);
    } else {
        int nextPage = Setup_Tabs->currentIndex()+1;
        Setup_Tabs->setCurrentIndex(nextPage);
    }
}

void
nmfSetup_Tab3::callback_PrevPB()
{
    if (Setup_Tab3_SpeciesTabW->currentIndex() == 0) {
        int prevPage = Setup_Tabs->currentIndex()-1;
        Setup_Tabs->setCurrentIndex(prevPage);
    } else {
        Setup_Tab3_SpeciesTabW->setCurrentIndex(0);
    }
}

QStringList
nmfSetup_Tab3::getAllSpecies()
{
    QStringListModel* smodel = qobject_cast<QStringListModel*>(Setup_Tab3_PotentialSpeciesLV->model());
    return smodel->stringList();
}

QStringList
nmfSetup_Tab3::getAllSelectedPredatorAndPreyOnlySpecies()
{
    QString line;
    QStringList species = {};

    // Read predator and prey only species names from files
    QString base = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsAvailability::InputDataDir));
    QStringList speciesFiles = {
        QDir(base).filePath(QString::fromStdString(nmfConstantsAvailability::PredatorFile)),
        QDir(base).filePath(QString::fromStdString(nmfConstantsAvailability::PreyOnlyFile))};
    for (QString speciesFile : speciesFiles) {
        QFile fileIn(speciesFile);
        if (fileIn.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QTextStream stream(&fileIn);
            // Skip first 2 lines
            for (int i=0; i<3; ++i) {
                line = stream.readLine();
            }
            while (! line.isNull()) {
                species << line.split(",")[0];
                line = stream.readLine();
            }
            fileIn.close();
        }
    }

    // Order these as they're found in the original species code file
    return orderTheSpeciesList(species);
}

QString
nmfSetup_Tab3::getSpeciesCodesFile()
{
    return Setup_Tab3_CodesFileLE->text();
}

QStringList
nmfSetup_Tab3::getSelectedPredators()
{
    QStringList predatorList;
    for (int i=0; i<m_smodelPredator->rowCount(); ++i) {
        predatorList << m_smodelPredator->item(i,0)->text();
    }
    return predatorList;
}

QStringList
nmfSetup_Tab3::getSelectedPreyOnly()
{
    QStringList preyOnlyList;
    for (int i=0; i<m_smodelPreyOnly->rowCount(); ++i) {
        preyOnlyList << m_smodelPreyOnly->item(i,0)->text();
    }
    return preyOnlyList;
}

int
nmfSetup_Tab3::getNumberDietCompositionRows(std::vector<std::pair<int,QString> >& subRowVector)
{
    QComboBox* ageCMB;
    QString ageStr;
    QModelIndex index;
    int totalRows = 0;

    // Add up the predator rows
    for (int row=0; row<m_smodelPredator->rowCount(); ++row) {
        index = m_smodelPredator->index(row,nmfConstantsAvailability::Predator_Column_AgeStructure);
        ageCMB = qobject_cast<QComboBox *>(Setup_Tab3_PredatorTV->indexWidget(index));
        ageStr = ageCMB->currentText();
        if (ageStr == "All Ages") {
            subRowVector.push_back(std::make_pair(4,ageStr));
            totalRows += 4;
        } else {
            subRowVector.push_back(std::make_pair(2,ageStr.split(" ")[0]));
            totalRows += 2;
        }
    }

    return totalRows;
}

void
nmfSetup_Tab3::restoreInitialPredatorColumns(QStringList& selectedPredators)
{
    int m = 0;
    QStandardItem* item;

    Setup_Tabs->setCursor(Qt::WaitCursor);

    m_smodelPredator = qobject_cast<QStandardItemModel* >(Setup_Tab3_PredatorTV->model());

    for (int row=0; row<m_smodelPredator->rowCount(); ++row) {
        item = new QStandardItem();
        item->setTextAlignment(Qt::AlignCenter);
        item->setText(selectedPredators[m++]);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        m_smodelPredator->setItem(row,0,item);
    }

    Setup_Tab3_PredatorTV->setModel(m_smodelPredator);
    Setup_Tab3_PredatorTV->resizeColumnsToContents();

    Setup_Tabs->setCursor(Qt::ArrowCursor);
}

void
nmfSetup_Tab3::restoreInitialPreyOnlyColumns(QStringList& selectedPreyOnly)
{
    int m = 0;
    QStandardItem* item;

    Setup_Tabs->setCursor(Qt::WaitCursor);

    m_smodelPreyOnly = qobject_cast<QStandardItemModel* >(Setup_Tab3_PreyOnlyTV->model());

    for (int row=0; row<m_smodelPreyOnly->rowCount(); ++row) {
        item = new QStandardItem();
        item->setTextAlignment(Qt::AlignCenter);
        item->setText(selectedPreyOnly[m++]);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        m_smodelPreyOnly->setItem(row,0,item);
    }

    Setup_Tab3_PreyOnlyTV->setModel(m_smodelPreyOnly);
    Setup_Tab3_PreyOnlyTV->resizeColumnsToContents();

    Setup_Tabs->setCursor(Qt::ArrowCursor);
}

void
nmfSetup_Tab3::restoreInitialDietCompositionColumns()
{
    int m = 0;
    QStandardItem* item;
    Setup_Tabs->setCursor(Qt::WaitCursor);
    std::vector<std::pair<int,QString> > subRowVector;

    // Find number of rows needed in the Diet Composition table
    std::vector<int> numSubRowVector; // how many subrows for every row
    getNumberDietCompositionRows(subRowVector);
    int rowInc = 0;
    QStringList allSpecies = {"Species","Predator Type","Prey Type"};
    allSpecies += getAllSpecies();
    QStringList selectedPredators = getSelectedPredators();

    m_smodelDietComposition = qobject_cast<QStandardItemModel* >(Setup_Tab3_DietCompositionTV->model());

    for (int k=0;k<int(subRowVector.size());++k) {
        for (int row=0; row<subRowVector[k].first; ++row) {
            for (int col=nmfConstantsAvailability::DietComposition_Column_Name;
                     col<=nmfConstantsAvailability::DietComposition_Column_PreyType; ++col) {
                item = new QStandardItem();
                item->setTextAlignment(Qt::AlignCenter);
                if (col == nmfConstantsAvailability::DietComposition_Column_Name) {
                    if (row == 0) {
                        item->setText(selectedPredators[m++]);
                    } else {
                        item->setText("");
                    }
                    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                } else if (col == nmfConstantsAvailability::DietComposition_Column_PredatorType) {
                    if (subRowVector[k].second == "All Ages") {
                        if (row == 0) {
                            item->setText("Juvenile");
                        } else if (row == 2) {
                            item->setText("Adult");
                        }
                    } else {
                        if (row == 0) {
                            item->setText(subRowVector[k].second);
                        }
                    }
                    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                } else if (col == nmfConstantsAvailability::DietComposition_Column_PreyType) {
                    if (row % 2) { // row is odd
                        item->setText("Adult");
                    } else {
                        item->setText("Juvenile");
                    }
                    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                }
                m_smodelDietComposition->setItem(rowInc,col,item);
            }
            ++rowInc;
        }
    }
    Setup_Tab3_DietCompositionTV->setModel(m_smodelDietComposition);
    Setup_Tab3_DietCompositionTV->resizeColumnsToContents();
    Setup_Tabs->setCursor(Qt::ArrowCursor);
}


bool
nmfSetup_Tab3::loadSelectedPredators()
{
    QString predatorFile = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsAvailability::InputDataDir));
    predatorFile = QDir(predatorFile).filePath(QString::fromStdString(nmfConstantsAvailability::PredatorFile));
    QFile file(predatorFile);
    QStringList parts;
    QStandardItem *item;
    QString line;
    QComboBox* cbox;
    bool dataLoaded = false;

    // Clear table
    m_smodelPredator = new QStandardItemModel(0,m_colLabelsPredatorTable.size());
    m_smodelPredator->setHorizontalHeaderLabels(m_colLabelsPredatorTable);
    Setup_Tab3_PredatorTV->setModel(m_smodelPredator);

    // Populate table from data in file
    if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream stream(&file);
        line = stream.readLine(); // Ignore 1st two header lines
        line = stream.readLine();
        int numCols = line.split(",").size();
        int numRows = 0;
        line = stream.readLine();
        while (! line.isNull()) {
            ++numRows;
            line = stream.readLine();
        }
        file.close();
        m_smodelPredator = new QStandardItemModel(numRows, numCols);
        m_smodelPredator->setHorizontalHeaderLabels(m_colLabelsPredatorTable);
        Setup_Tab3_PredatorTV->setModel(m_smodelPredator);
        loadTooltips("Predator");
    }

    if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QModelIndex index;
        QTextStream stream(&file);
        line = stream.readLine(); // Ignore  1st two header lines
        line = stream.readLine();
        int row = 0;
        line = stream.readLine();
        while (! line.isNull()) {
            parts = line.split(",");
            for (int col=0; col<parts.size(); ++col) {
                if (col <= nmfConstantsAvailability::Predator_Column_Exponent) {
                    item = new QStandardItem();
                    item->setTextAlignment(Qt::AlignCenter);
                    if (col > nmfConstantsAvailability::Predator_Column_Name) {
                        item->setText(QString::number(parts[col].toDouble()));
                    } else {
                        item->setText(parts[col]);
                        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                    }
                    m_smodelPredator->setItem(row,col,item);
                } else if (col == nmfConstantsAvailability::Predator_Column_AgeStructure) {
                    cbox = new QComboBox();
                    cbox->addItems(nmfConstantsAvailability::AgeStructureValues);
                    cbox->setCurrentText(parts[col]);
                    index = m_smodelPredator->index(row,col);
                    Setup_Tab3_PredatorTV->setIndexWidget(index,cbox);
                } else if (col == nmfConstantsAvailability::Predator_Column_FunctionalResponse) {
                    cbox = new QComboBox();
                    cbox->addItems(nmfConstantsAvailability::FunctionalResponseValues);
                    cbox->setCurrentText(parts[col]);
                    index = m_smodelPredator->index(row,col);
                    Setup_Tab3_PredatorTV->setIndexWidget(index,cbox);
                }
            }
            ++row;
            line = stream.readLine();
        }
        dataLoaded = true;
        file.close();
    }
    Setup_Tab3_PredatorTV->resizeColumnsToContents();

    return dataLoaded;
}

bool
nmfSetup_Tab3::loadSelectedPreyOnly()
{
    QString preyOnlyFile = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsAvailability::InputDataDir));
    preyOnlyFile = QDir(preyOnlyFile).filePath(QString::fromStdString(nmfConstantsAvailability::PreyOnlyFile));
    QFile file(preyOnlyFile);
    QStringList parts;
    QStandardItem *item;
    QString line;
    bool dataLoaded = false;

    // Clear table
    m_smodelPreyOnly = new QStandardItemModel(0,m_colLabelsPreyOnlyTable.size());
    m_smodelPreyOnly->setHorizontalHeaderLabels(m_colLabelsPreyOnlyTable);
    Setup_Tab3_PreyOnlyTV->setModel(m_smodelPreyOnly);

    if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream stream(&file);
        line = stream.readLine(); // Ignore 1st two header lines
        line = stream.readLine();
        int numCols = line.split(",").size();
        int numRows = 0;
        line = stream.readLine();
        while (! line.isNull()) {
            ++numRows;
            line = stream.readLine();
        }
        file.close();
        m_smodelPreyOnly = new QStandardItemModel(numRows, numCols);
        m_smodelPreyOnly->setHorizontalHeaderLabels(m_colLabelsPreyOnlyTable);
        Setup_Tab3_PreyOnlyTV->setModel(m_smodelPreyOnly);
        loadTooltips("PreyOnly");
    }

    if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream stream(&file);
        line = stream.readLine(); // Ignore  1st two header lines
        line = stream.readLine();
        int row = 0;
        line = stream.readLine();
        while (! line.isNull()) {
            parts = line.split(",");
            for (int col=0; col<parts.size(); ++col) {
                item = new QStandardItem();
                item->setTextAlignment(Qt::AlignCenter);
                if (col > nmfConstantsAvailability::Predator_Column_Name) {
                    item->setText(QString::number(parts[col].toDouble()));
                } else {
                    item->setText(parts[col]);
                    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                }
                m_smodelPreyOnly->setItem(row,col,item);
            }
            ++row;
            line = stream.readLine();
        }
        dataLoaded = true;
        file.close();
    }
    Setup_Tab3_PreyOnlyTV->resizeColumnsToContents();

    return dataLoaded;
}

bool
nmfSetup_Tab3::loadDietCompositionData()
{
    QString dietCompositionFile = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsAvailability::InputDataDir));
    dietCompositionFile = QDir(dietCompositionFile).filePath(QString::fromStdString(nmfConstantsAvailability::DietCompositionFile));
    QFile file(dietCompositionFile);
    QStringList parts;
    QStandardItem *item;
    QString line;
    QString headerLine;
    bool dataLoaded = false;
    QStringList allSpecies = {"Species","Predator Type","Prey Type"};
    allSpecies += getAllSelectedPredatorAndPreyOnlySpecies();

    // Clear table
    m_smodelDietComposition = new QStandardItemModel(0, allSpecies.size());
    m_smodelDietComposition->setHorizontalHeaderLabels(allSpecies);
    Setup_Tab3_DietCompositionTV->setModel(m_smodelDietComposition);

    if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream stream(&file);
        headerLine = stream.readLine(); // Ignore 1st header line
        line = stream.readLine();
        int numCols = line.split(",").size();
        int numRows = 0;
        while (! line.isNull()) {
            ++numRows;
            line = stream.readLine();
        }
        file.close();
        m_smodelDietComposition = new QStandardItemModel(numRows,numCols);
    }

    if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        m_smodelDietComposition->setHorizontalHeaderLabels(headerLine.split(","));
        QTextStream stream(&file);
        std::array<QString,3> strValue;
        std::array<QString,3> lastStrValue;
        line = stream.readLine(); // Ignore  1st header line
        int row = 0;
        line = stream.readLine();
        while (! line.isNull()) {
            parts = line.split(",");
            for (int col=0; col<parts.size(); ++col) {
                if (col > nmfConstantsAvailability::DietComposition_Column_PreyType) {
                    item = new QStandardItem();
                    item->setTextAlignment(Qt::AlignCenter);
                    item->setText(QString::number(parts[col].toDouble()));
                    m_smodelDietComposition->setItem(row,col,item);
                }
            }
            ++row;
            line = stream.readLine();
        }
        dataLoaded = true;
        file.close();
        Setup_Tab3_DietCompositionTV->setModel(m_smodelDietComposition);
        Setup_Tab3_DietCompositionTV->resizeColumnsToContents();

        connect(m_smodelDietComposition, SIGNAL(itemChanged(QStandardItem*)),
                this,     SLOT(callback_DietCompositionItemEdited(QStandardItem*)));

        restoreInitialDietCompositionColumns();
    }

    return dataLoaded;
}

void
nmfSetup_Tab3::updateSpeciesCodes()
{
    // Reload codes except for the names still in tables
    QString filename = getSpeciesCodesFile();
    if (! filename.isEmpty()) {
        loadCodesFile(filename,getAllSelectedPredatorAndPreyOnlySpecies());
    }
}

void
nmfSetup_Tab3::loadTab(int tabNum)
{
    bool dataLoaded;
    bool predatorDataLoaded;
    bool preyOnlyDataLoaded;
    bool dietCompositionDataLoaded;

    if (tabNum == 0) {
        callback_ReloadPB();
        predatorDataLoaded = loadSelectedPredators();
        preyOnlyDataLoaded = loadSelectedPreyOnly();
        updateSpeciesCodes();
        dataLoaded = predatorDataLoaded || preyOnlyDataLoaded;
        enableDietCompositionTab(dataLoaded);
        m_loadedSpecies = dataLoaded;
    } else if (tabNum == 1) {
        dietCompositionDataLoaded = loadDietCompositionData();
        emit EnableCalculateTab(dietCompositionDataLoaded);
        m_loadedDietComposition = true;
    }
}

void
nmfSetup_Tab3::callback_LoadPB()
{
    Setup_Tabs->setCursor(Qt::WaitCursor);

    loadTab(Setup_Tab3_SpeciesTabW->currentIndex());

    Setup_Tab3_NextPB->setEnabled(true);

    Setup_Tabs->setCursor(Qt::ArrowCursor);
}

void
nmfSetup_Tab3::enableDietCompositionTab(bool isEnabled)
{
    Setup_Tab3_SpeciesTabW->setTabEnabled(1,isEnabled);
}

void
nmfSetup_Tab3::setLoadPBState(bool state)
{
   Setup_Tab3_LoadPB->setEnabled(state);
}

bool
nmfSetup_Tab3::viewingDietCompositionTab()
{
    return (Setup_Tab3_SpeciesTabW->currentIndex()==1);
}

void
nmfSetup_Tab3::callback_SavePB()
{
    bool thereIsPredatorData = false;
    bool thereIsPreyOnlyData = false;
    Setup_Tabs->setCursor(Qt::WaitCursor);

    if (Setup_Tab3_SpeciesTabW->currentIndex() == 0) {
        if ((m_smodelPredator != nullptr) && (m_smodelPredator->rowCount() > 0)) {
            if (passedPredatorDataChecks()) {
                savePredatorData();
                removeDietCompositionTable();
                thereIsPredatorData = true;
            } else {
                Setup_Tabs->setCursor(Qt::ArrowCursor);
                return;
            }
        }
        if ((m_smodelPreyOnly != nullptr) && (m_smodelPreyOnly->rowCount() > 0)) {
            savePreyOnlyData();
            thereIsPreyOnlyData = true;
        }
        if (thereIsPredatorData || thereIsPreyOnlyData) {
            initializeDietCompositionTable();
            Setup_Tab3_PredatorTV->resizeColumnsToContents();
            Setup_Tab3_PreyOnlyTV->resizeColumnsToContents();
            enableDietCompositionTab(true);
            Setup_Tab3_NextPB->setEnabled(true);
            m_savedSpecies = true;
        } else {
            QMessageBox::warning(Setup_Tabs,"Warning","\nNo data to save.\n");
        }
    } else if (Setup_Tab3_SpeciesTabW->currentIndex() == 1) {
        if ((m_smodelDietComposition != nullptr) && (m_smodelDietComposition->rowCount() > 0)) {
            if (passedDietCompositionDataChecks()) {
                saveDietCompositionData();
                emit EnableCalculateTab(true);
                m_savedDietComposition = true;
                Setup_Tab3_NextPB->setEnabled(true);
                Setup_Tab3_LoadPB->setEnabled(true);
            }
            Setup_Tab3_DietCompositionTV->resizeColumnsToContents();
        } else {
            QMessageBox::warning(Setup_Tabs,"Warning","\nNo data to save.\n");
        }
    }

    Setup_Tabs->setCursor(Qt::ArrowCursor);
}

void
nmfSetup_Tab3::removeDietCompositionTable()
{
    QString dietCompositionFile = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsAvailability::InputDataDir));
    dietCompositionFile = QDir(dietCompositionFile).filePath(QString::fromStdString(nmfConstantsAvailability::DietCompositionFile));
    QFile file (dietCompositionFile);

    file.remove();
}

bool
nmfSetup_Tab3::hasUserPressedDietCompositionSave()
{
    return m_savedDietComposition;
}

bool
nmfSetup_Tab3::hasUserPressedView()
{
    return m_UserPressedViewOnce;
}

void
nmfSetup_Tab3::callback_ViewPB()
{
    if ((m_smodelDietComposition == nullptr) ||
        (m_smodelDietComposition->rowCount() == 0)) {
        QMessageBox::warning(Setup_Tabs,"Warning","\nNo data to view.\n");
        return;
    }

    int numRows = m_smodelDietComposition->rowCount();
    int numCols = m_smodelDietComposition->columnCount();
    QStringList PreyNames = {};
    QStringList PredatorNames = {};
    QString lastPredName;
    QString lastPredType;
    QString predName;
    QString predType;
    QString preyType;

    m_UserPressedViewOnce = true;

    boost::numeric::ublas::matrix<double> ChartData;
    nmfUtils::initialize(ChartData,numRows,numCols-3);

    for (int row=0; row<numRows; ++row) {
        predName = m_smodelDietComposition->item(row,0)->text();
        predType = m_smodelDietComposition->item(row,1)->text();
        preyType = m_smodelDietComposition->item(row,2)->text();
        predName = (predName.isEmpty()) ? lastPredName : predName;
        predType = (predType.isEmpty()) ? lastPredType : predType;
        PredatorNames << encodePredatorName(predName,predType,preyType);

        for (int col=nmfConstantsAvailability::DietComposition_Column_Data;
                 col<numCols; ++col) {
           if (row == 0) {
               PreyNames << m_smodelDietComposition->horizontalHeaderItem(col)->text();
           }
           ChartData(row,col-nmfConstantsAvailability::DietComposition_Column_Data) =
                   m_smodelDietComposition->item(row,col)->text().toDouble();
        }
        lastPredName = predName;
        lastPredType = predType;
    }

    emit ShowOutputChart(PredatorNames,PreyNames,ChartData);
}

void
nmfSetup_Tab3::callback_TypeCMB(QString unused)
{
    callback_ViewPB();
}

QString
nmfSetup_Tab3::encodePredatorName(QString& predName,
                                  QString& predType,
                                  QString& preyType)
{
    QString encodedName = predName + "-";
    encodedName = (predType == "Juvenile") ? encodedName+"J" : encodedName+"A";
    encodedName = (preyType == "Juvenile") ? encodedName+"J" : encodedName+"A";
    return encodedName;
}

bool
nmfSetup_Tab3::passedPredatorDataChecks()
{
    int numRows = m_smodelPredator->rowCount();
    double exponent;
    QString msg;
    QString species;

    // Check 1. Assure each exponent is within valid range
    for (int row=0; row<numRows; ++row) {
        species  = m_smodelPredator->item(row,nmfConstantsAvailability::Predator_Column_Name)->text();
        exponent = m_smodelPredator->item(row,nmfConstantsAvailability::Predator_Column_Exponent)->text().toDouble();
        if ((exponent < nmfConstantsAvailability::MinExponent) ||
            (exponent > nmfConstantsAvailability::MaxExponent)) {
            msg = "\nError (Row " + QString::number(row+1) + "): Found exponent of " + QString::number(exponent) +
                    " for species: " + species +"\n\nEvery species exponent must be between " +
                    QString::number(nmfConstantsAvailability::MinExponent,'f',1) + " and " +
                    QString::number(nmfConstantsAvailability::MaxExponent,'f',1) + ".\n";
            QMessageBox::critical(Setup_Tabs,"Error",msg);
            return false;
        }
    }
    return true;
}

bool
nmfSetup_Tab3::passedDietCompositionDataChecks()
{
    int numRows = m_smodelDietComposition->rowCount();
    int numCols = m_smodelDietComposition->columnCount();
    double total;
    QString species;
    QString msg;

    // Check 1. Assure each species' diet composition values sum to 1
    for (int row=0; row<numRows; ++row) {
        total = 0;
        species = (m_smodelDietComposition->item(row,0)->text().isEmpty()) ? species : m_smodelDietComposition->item(row,0)->text();
        for (int col=nmfConstantsAvailability::DietComposition_Column_Data;
                 col<numCols; ++col) {
            total += m_smodelDietComposition->item(row,col)->text().toDouble();
        }
        if (! nmfUtils::isNearlyZero(total-1.0)) {
            msg = "\nError (Row " + QString::number(row+1) + "): Found total of " + QString::number(total) +
                    " for species: " + species +"\n\nEvery species row total must equal 1.0.\n";
            QMessageBox::critical(Setup_Tabs,"Error",msg);
            return false;
        }
    }

    return true;
}

bool
nmfSetup_Tab3::showZeros()
{
    return Setup_Tab3_ShowZerosCB->isChecked();
}

void
nmfSetup_Tab3::callback_CurrentRowChanged(const QModelIndex& indexCurr,
                                          const QModelIndex& indexPrev)
{
    QStringListModel* smodel = qobject_cast<QStringListModel*>(Setup_Tab3_PotentialSpeciesLV->model());
    QStringList speciesList = smodel->stringList();

    QString species = speciesList[indexCurr.row()];

    updateSpeciesInfoWidgets(species);
}

void
nmfSetup_Tab3::updateSpeciesInfoWidgets(const QString& species)
{
    setCode(species);
    setTaxa(m_Taxa[species]);
    setDesc(m_Desc[species]);
    setFished(m_Fished[species]);
    setPredator(m_Predator[species]);
}

void
nmfSetup_Tab3::setCode(const QString& species)
{
    Setup_Tab3_CodeLE->setText(species);
}
void
nmfSetup_Tab3::setTaxa(const QString& taxa)
{
    Setup_Tab3_TaxaLE->setText(taxa);
}
void
nmfSetup_Tab3::setDesc(const QString& desc)
{
    Setup_Tab3_DescLE->setText(desc);
}

void
nmfSetup_Tab3::setFished(const bool& isFished)
{
    Setup_Tab3_FishedCB->setChecked(isFished);
}

void
nmfSetup_Tab3::setPredator(const bool& isPredator)
{
    Setup_Tab3_PredatorCB->setChecked(isPredator);
}

void
nmfSetup_Tab3::callback_CurrentCellChanged(const QModelIndex& index)
{
    int row = index.row();

    QString species = m_smodelPredator->item(row,0)->text();
    updateSpeciesInfoWidgets(species);
}

bool
nmfSetup_Tab3::rowSumOK(const int& row,
                        const int& colToSkip,
                        const double& newValue,
                        double& rowTotal)
{
    for (int col=nmfConstantsAvailability::DietComposition_Column_Data;
             col<m_smodelDietComposition->columnCount(); ++col) {
        if (col != colToSkip) {
            rowTotal += m_smodelDietComposition->item(row,col)->text().toDouble();
        }
    }
    rowTotal += newValue;

    return (rowTotal <= 1.0) ? true : false;
}

bool
nmfSetup_Tab3::isAllZeros(int col)
{
    for (int row=0; row<m_smodelDietComposition->rowCount();++row) {
        if (m_smodelDietComposition->item(row,col)->text().toDouble() != 0.0) {
            return false;
        }
    }
    return true;
}

void
nmfSetup_Tab3::callback_ShowZeroColumnsCB(int state)
{
    if (state == Qt::Checked) { // show all columns
        for (int col=0; col<m_smodelDietComposition->columnCount();++col) {
            Setup_Tab3_DietCompositionTV->showColumn(col);
        }
        Setup_Tab3_DietCompositionTV->resizeColumnsToContents();
    } else {
        for (int col=nmfConstantsAvailability::DietComposition_Column_Data;
                 col<m_smodelDietComposition->columnCount();++col) {
            if (isAllZeros(col)) {
                Setup_Tab3_DietCompositionTV->hideColumn(col);
            }
        }
    }
}

void
nmfSetup_Tab3::callback_ShowZerosCB(int state)
{
    if (state != Qt::Checked) {
        for (int row=0; row<m_smodelDietComposition->rowCount(); ++row) {
            for (int col=nmfConstantsAvailability::DietComposition_Column_Data;
                     col<m_smodelDietComposition->columnCount(); ++col) {
                if (m_smodelDietComposition->item(row,col)->text().toDouble() == 0.0) {
                    m_smodelDietComposition->item(row,col)->setText("");
                }
            }
        }
    } else {
        for (int row=0; row<m_smodelDietComposition->rowCount(); ++row) {
            for (int col=nmfConstantsAvailability::DietComposition_Column_Data;
                     col<m_smodelDietComposition->columnCount(); ++col) {
                if (m_smodelDietComposition->item(row,col)->text().isEmpty()) {
                    m_smodelDietComposition->item(row,col)->setText("0.0");
                }
            }
        }
    }
}

void
nmfSetup_Tab3::callback_EqualizeCB(int state)
{
    Setup_Tab3_EqualizePB->setEnabled(state == Qt::Checked);
}

void
nmfSetup_Tab3::callback_EqualizePB()
{
    int row;
    int col;
    int numRows;
    int numCols;
    int numSelectedCellsInRow;
    double value;
    double diff;
    double totalRow;
    double amountToEqualize;
    std::map<int,QModelIndexList> rowIndexMap;
    std::map<int,double> totalNonSelectedMap;

    QModelIndexList indexes = Setup_Tab3_DietCompositionTV->selectionModel()->selectedIndexes();
    if (indexes.size() > 0) {
        numRows = m_smodelDietComposition->rowCount();
        numCols = m_smodelDietComposition->columnCount();

        // Load each row's selected indexes into the rowIndexList map
        for (QModelIndex index : indexes) {
            row = index.row();
            col = index.column();
            if (col > nmfConstantsAvailability::DietComposition_Column_PreyType) { // Ignore first 3 columns if selected
                rowIndexMap[row].append(index);
            }
        }

        // Find for each row the total of all diet composition values in the non-selected cells
        for (int row=0; row<numRows; ++row) {
            totalNonSelectedMap[row] = 0;
            for (int col=nmfConstantsAvailability::DietComposition_Column_Data;
                     col<numCols; ++col) {
                totalNonSelectedMap[row] += m_smodelDietComposition->item(row,col)->text().toDouble();
            }
            for (QModelIndex index : rowIndexMap[row]) {
                totalNonSelectedMap[row] -= index.data().toDouble();
            }
        }

        // Disable value checks for each row totaling to 1.0
        enableDietCompositionItemEdited(false);

        // Equalize each row for only those cells that are selected in said row
        for (int row=0; row<numRows; ++row) {
            numSelectedCellsInRow = rowIndexMap[row].size();
            if (numSelectedCellsInRow > 0) {
                amountToEqualize = 1.0 - totalNonSelectedMap[row];
                if (amountToEqualize <= 1.0) {
                    value = nmfUtils::round(amountToEqualize/double(numSelectedCellsInRow),6);
                    for (int i=0; i<numSelectedCellsInRow; ++i) {
                        col = rowIndexMap[row][i].column();
                        m_smodelDietComposition->item(row,col)->setText(QString::number(value));
                    }
                } else {
                    QString msg = "\nCannot equalize in row: " + QString::number(row+1) +
                            ". Total is already equal to 1.0.\n";
                    QMessageBox::warning(Setup_Tabs, "Error", msg);
                }
                // Calculate any excess and subtract it from the last selected value so that each row
                // is guarantee to total to 1.0.
                totalRow = 0;
                for (int col=nmfConstantsAvailability::DietComposition_Column_Data;
                         col<numCols; ++col) {
                    totalRow += m_smodelDietComposition->item(row,col)->text().toDouble();
                }
                diff = totalRow - 1.0;
                col = rowIndexMap[row][numSelectedCellsInRow-1].column();
                m_smodelDietComposition->item(row,col)->setText(QString::number(value-diff));
            }
        }

        // Enable value checks for each row totaling to 1.0
        enableDietCompositionItemEdited(true);

        Setup_Tab3_DietCompositionTV->resizeColumnsToContents();
    }
}

void
nmfSetup_Tab3::callback_EqualizeRow(int unused)
{
    if (isEqualizeEnabled()) {
        callback_EqualizePB();
    }
}

void
nmfSetup_Tab3::callback_TopLeftCornerButtonPressed()
{
    if (isQuickPasteEnabled()) {
        for (int row=0; row<m_smodelDietComposition->rowCount(); ++row) {
            callback_DietCompositionVerticalSectionClicked(row);
        }
    } else if (isEqualizeEnabled()) {
        callback_EqualizeAllRows();
    }
}

void
nmfSetup_Tab3::callback_EqualizeAllRows()
{
    callback_EqualizeRow(0); // the parameter is unused
}

void
nmfSetup_Tab3::callback_QuickPaste3bCB(int state)
{
    Setup_Tab3b_QuickPasteSB->setEnabled(state);
}

void
nmfSetup_Tab3::callback_QuickPaste3aCB(int state)
{
    Setup_Tab3a_QuickPasteSB->setEnabled(state);
}

bool
nmfSetup_Tab3::isEqualizeEnabled()
{
    return Setup_Tab3_EqualizeCB->isChecked();
}

bool
nmfSetup_Tab3::isQuickPasteEnabled()
{
    return Setup_Tab3b_QuickPasteCB->isChecked();
}

bool
nmfSetup_Tab3::isQuickPaste3aEnabled()
{
    return Setup_Tab3a_QuickPasteCB->isChecked();
}

double
nmfSetup_Tab3::quickPasteValue()
{
    return Setup_Tab3b_QuickPasteSB->value();
}

double
nmfSetup_Tab3::quickPaste3aValue()
{
    return Setup_Tab3a_QuickPasteSB->value();
}

void
nmfSetup_Tab3::callback_SpeciesTabWChanged(int tab)
{
    bool selectedDietCompositionTab = (tab == 1);
    Setup_Tab3_ViewPB->setEnabled(selectedDietCompositionTab);

    if (selectedDietCompositionTab) {
        Setup_Tab3_NextPB->setEnabled(m_loadedDietComposition ||
                                      m_savedDietComposition);
        Setup_Tab3_LoadPB->setEnabled(m_savedDietComposition);
    } else {
        Setup_Tab3_NextPB->setEnabled(m_loadedSpecies ||
                                      m_savedSpecies);
        Setup_Tab3_LoadPB->setEnabled(true);
    }

    if (m_UserPressedViewOnce) {
        emit ShowOutputTab(selectedDietCompositionTab);
    }
}

void
nmfSetup_Tab3::callback_PredatorHorizontalSectionClicked(int col)
{
    double value = quickPaste3aValue();

    if (isQuickPaste3aEnabled()) {
       if ((col >= nmfConstantsAvailability::Predator_Column_GrowthRate) &&
           (col <= nmfConstantsAvailability::Predator_Column_Exponent)) {
           for (int row=0; row<m_smodelPredator->rowCount(); ++row) {
               m_smodelPredator->item(row,col)->setText(QString::number(value));
           }
       }
       Setup_Tab3_PredatorTV->resizeColumnsToContents();
    }
}

void
nmfSetup_Tab3::callback_PredatorTableClicked(const QModelIndex& index)
{
    int row = index.row();
    int col = index.column();
    double value = quickPaste3aValue();

    if (isQuickPaste3aEnabled()) {
        if ((col >= nmfConstantsAvailability::Predator_Column_GrowthRate) &&
            (col <= nmfConstantsAvailability::Predator_Column_Exponent)) {
           m_smodelPredator->item(row,col)->setText(QString::number(value));
       }
       Setup_Tab3_PredatorTV->resizeColumnsToContents();
    }

    updateSpeciesInfoWidgets(m_smodelPredator->item(row,0)->text());
}

void
nmfSetup_Tab3::callback_PreyOnlyHorizontalSectionClicked(int col)
{
    double value = quickPaste3aValue();

    if (isQuickPaste3aEnabled()) {
       if (col == nmfConstantsAvailability::PreyOnly_Column_InitialBiomass) {
           for (int row=0; row<m_smodelPreyOnly->rowCount(); ++row) {
               m_smodelPreyOnly->item(row,col)->setText(QString::number(value));
           }
       }
       Setup_Tab3_PreyOnlyTV->resizeColumnsToContents();
    }
}

void
nmfSetup_Tab3::callback_PreyOnlyTableClicked(const QModelIndex& index)
{
    int row = index.row();
    int col = index.column();
    double value = quickPaste3aValue();

    if (isQuickPaste3aEnabled()) {
       if (col == nmfConstantsAvailability::PreyOnly_Column_InitialBiomass) {
           m_smodelPreyOnly->item(row,col)->setText(QString::number(value));
       }
       Setup_Tab3_PreyOnlyTV->resizeColumnsToContents();
    }

    updateSpeciesInfoWidgets(m_smodelPreyOnly->item(row,0)->text());
}

void
nmfSetup_Tab3::callback_DietCompositionHorizontalSectionClicked(int col)
{
    double value = quickPasteValue();

    if (isQuickPasteEnabled()) {
       for (int row=0; row<m_smodelDietComposition->rowCount(); ++row) {
           m_smodelDietComposition->item(row,col)->setText(QString::number(value));
       }
       Setup_Tab3_DietCompositionTV->resizeColumnsToContents();
    }
}

void
nmfSetup_Tab3::callback_DietCompositionVerticalSectionClicked(int row)
{
    double value = quickPasteValue();

    if (isQuickPasteEnabled()) {
       for (int col=nmfConstantsAvailability::DietComposition_Column_Data;
                col<m_smodelDietComposition->columnCount(); ++col) {
           m_smodelDietComposition->item(row,col)->setText(QString::number(value));
       }
       Setup_Tab3_DietCompositionTV->resizeColumnsToContents();
    }
}

void
nmfSetup_Tab3::callback_DietCompositionTableClicked(const QModelIndex& index)
{
    QString msg;

    m_valueClicked = m_smodelDietComposition->item(index.row(),index.column())->text().toDouble();

    if (isQuickPasteEnabled()) {
        int row = index.row();
        int col = index.column();
        if (col > nmfConstantsAvailability::DietComposition_Column_PreyType) {
            double rowTotal = 0;
            double value = quickPasteValue();
            if (rowSumOK(row,col,value,rowTotal)) {
                m_smodelDietComposition->item(row,col)->setText(QString::number(value));
            } else {
                msg  = "\nValue of " + QString::number(value) + " would yield a diet composition total of " +
                        QString::number(rowTotal) + " for the current predator.  ";
                msg += "Each predator's diet composition total must be equal to 1.0.\n";
                QMessageBox::warning(Setup_Tabs, "Error", msg);
            }
            Setup_Tab3_DietCompositionTV->resizeColumnsToContents();
        }
    }
}

void
nmfSetup_Tab3::callback_DietCompositionItemEdited(QStandardItem* item)
{
    int row = item->row();
    int col = item->column();
    double rowTotal = 0;
    double value = m_smodelDietComposition->item(row,col)->text().toDouble();

    if (! rowSumOK(row,col,value,rowTotal)) { // 0.0 so not to count the value twice
        if (m_valueClicked == 0) {
            if (showZeros()) {
                m_smodelDietComposition->item(row,col)->setText("0.0");
            } else {
                m_smodelDietComposition->item(row,col)->setText("");
            }
        } else {
            m_smodelDietComposition->item(row,col)->setText(QString::number(m_valueClicked));
        }
        Setup_Tab3_DietCompositionTV->resizeColumnsToContents();
        QString msg  = "\nThe value of " + QString::number(value) + " would yield a diet composition total of " +
                       QString::number(rowTotal) + " for the current predator.  ";
        msg += "Each predator's diet composition total must be equal to 1.0.\n";
        QMessageBox::warning(Setup_Tabs, "Error", msg);
    }
}
