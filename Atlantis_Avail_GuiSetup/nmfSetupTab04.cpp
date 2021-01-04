#include "nmfSetupTab04.h"
#include "nmfConstantsDonut.h"
#include "nmfUtilsQt.h"
#include "nmfUtils.h"
#include "nmfConstants.h"


nmfSetup_Tab4::nmfSetup_Tab4(QTabWidget*  tabs,
                             nmfLogger*   logger,
                             std::string& projectDir)
{
    QUiLoader loader;

    Setup_Tabs    = tabs;
    m_Logger      = logger;
    m_ProjectDir  = projectDir;
    m_ProjectSettingsConfig.clear();
    m_ProjectName.clear();
    m_smodel = nullptr;

    // Load ui as a widget from disk
    QFile file(":/forms/Setup/Setup_Tab04.ui");
    file.open(QFile::ReadOnly);
    Setup_Tab4_Widget = loader.load(&file,Setup_Tabs);
    Setup_Tab4_Widget->setObjectName("Setup_Tab4_Widget");
    file.close();

    // Add the loaded widget as the new tabbed page
    Setup_Tabs->addTab(Setup_Tab4_Widget, tr("4. Calculate Availability"));
    Setup_Tab4_CalculatePB        = Setup_Tabs->findChild<QPushButton *>("Setup_Tab4_CalculatePB");
    Setup_Tab4_AvailabilityDataTV = Setup_Tabs->findChild<QTableView  *>("Setup_Tab4_AvailabilityDataTV");
    Setup_Tab4_ShowZeroColumnsCB  = Setup_Tabs->findChild<QCheckBox   *>("Setup_Tab4_ShowZeroColumnsCB");
    Setup_Tab4_ShowZerosCB        = Setup_Tabs->findChild<QCheckBox   *>("Setup_Tab4_ShowZerosCB");
    Setup_Tab4_DecimalPlacesCMB   = Setup_Tabs->findChild<QComboBox   *>("Setup_Tab4_DecimalPlacesCMB");
    Setup_Tab4_PrevPB             = Setup_Tabs->findChild<QPushButton *>("Setup_Tab4_PrevPB");
    Setup_Tab4_SavePB             = Setup_Tabs->findChild<QPushButton *>("Setup_Tab4_SavePB");
    Setup_Tab4_ExportPB           = Setup_Tabs->findChild<QPushButton *>("Setup_Tab4_ExportPB");

    connect(Setup_Tab4_CalculatePB,       SIGNAL(clicked()),
            this,                         SLOT(callback_RunPB()));
    connect(Setup_Tab4_ShowZeroColumnsCB, SIGNAL(stateChanged(int)),
            this,                         SLOT(callback_ShowZeroColumnsCB(int)));
    connect(Setup_Tab4_ShowZerosCB,       SIGNAL(stateChanged(int)),
            this,                         SLOT(callback_ShowZerosCB(int)));
    connect(Setup_Tab4_DecimalPlacesCMB,  SIGNAL(currentIndexChanged(int)),
            this,                         SLOT(callback_DecimalPlacesCMB(int)));
    connect(Setup_Tab4_PrevPB,            SIGNAL(clicked()),
            this,                         SLOT(callback_PrevPB()));
    connect(Setup_Tab4_SavePB,            SIGNAL(clicked()),
            this,                         SLOT(callback_SavePB()));
    connect(Setup_Tab4_ExportPB,          SIGNAL(clicked()),
            this,                         SLOT(callback_ExportPB()));

    Setup_Tab4_PrevPB->setText("\u25C1--");

    Setup_Tab4_SavePB->setEnabled(true);
    Setup_Tab4_ExportPB->setEnabled(true);
}

nmfSetup_Tab4::~nmfSetup_Tab4()
{
}

void
nmfSetup_Tab4::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(
                nmfConstantsDonut::SettingsDirWindows,QApplication::applicationName());

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
nmfSetup_Tab4::callback_PrevPB()
{
    int prevPage = Setup_Tabs->currentIndex()-1;
    Setup_Tabs->setCurrentIndex(prevPage);
}

int
nmfSetup_Tab4::getNumberDecimalPlaces()
{
    return Setup_Tab4_DecimalPlacesCMB->currentText().toInt();
}

void
nmfSetup_Tab4::callback_RunPB()
{
    double value;
    QStringList parts;

    Setup_Tabs->setCursor(Qt::WaitCursor);

    // Number of rows in final table = number of rows in Diet Composition table
    QString dietCompositionFile = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsAvailability::InputDataDir));
    dietCompositionFile = QDir(dietCompositionFile).filePath(QString::fromStdString(nmfConstantsAvailability::DietCompositionFile));
    QStringList finalPredators;
    QFile dcFile(dietCompositionFile);
    QString ageGroup1,ageGroup2;
    QString line;
    if ( dcFile.open(QIODevice::ReadOnly|QIODevice::Text) ) {
        QTextStream stream(&dcFile);
        line = stream.readLine(); // Skip header
        line = stream.readLine();
        while (! line.isNull()) {
            parts = line.split(',');
            ageGroup1 = (parts[1] == "Juvenile") ? "1" : "2";
            ageGroup2 = (parts[2] == "Juvenile") ? "1" : "2";
            finalPredators << ("pPREY"+ageGroup1+parts[0]+ageGroup2);
            line = stream.readLine();
        }
        dcFile.close();
    }
    int numRows = finalPredators.size();

    // Number of columns in final table = number of total species + 1 for encoded name (i.e. pPREY1FPL1)
    QString allSpeciesFilename = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsAvailability::InputDataDir));
    allSpeciesFilename = QDir(allSpeciesFilename).filePath(QString::fromStdString(nmfConstantsAvailability::AllSpeciesFile));
    QStringList allSpecies = {"Species"};
    QFile allSpeciesFile(allSpeciesFilename);
    if (allSpeciesFile.open(QIODevice::ReadOnly|QIODevice::Text) ) {
        QTextStream stream(&allSpeciesFile);
        QString line = stream.readLine();
        while (! line.isNull()) {
            allSpecies << line;
            line = stream.readLine();
        }
        allSpeciesFile.close();
    }
    int numCols = allSpecies.size();

    // Create matrix of availability values
    boost::numeric::ublas::matrix<double> AvailabilityMatrix;
    nmfUtils::initialize(AvailabilityMatrix,numRows,numCols-1); // -1 because the first column is species name
    nmfAvailabilityModelAPI modelAPI(m_Logger,m_ProjectDir);
    modelAPI.calculateAvailabilityMatrix(AvailabilityMatrix);

    // Set up rows and columns for final data table
    m_smodel          = new QStandardItemModel(numRows, numCols);
    m_smodel_Original = new QStandardItemModel(numRows, numCols);
    m_smodel->setHorizontalHeaderLabels(allSpecies);
    Setup_Tab4_AvailabilityDataTV->setModel(m_smodel);
    int row = 0;
    QStandardItem* item;
    for (QString predator : finalPredators) {
        item = new QStandardItem();
        item->setEditable(false);
        item->setTextAlignment(Qt::AlignCenter);
        item->setText(predator);
        m_smodel->setItem(row++,0,item);
    }

    // Calculate per-prey availability parameters
    for (int row=0; row<numRows; ++row) {
        for (int col=1; col<numCols; ++col) {
            item = new QStandardItem();
            item->setEditable(false);
            item->setTextAlignment(Qt::AlignCenter);
            value = AvailabilityMatrix(row,col-1);
            item->setText(QString::number(value));
            m_smodel->setItem(row,col,item);
        }
    }

    // Save copy of original smodel in case user changes the num of decimal places
    nmfUtilsQt::modelCopy(m_smodel,m_smodel_Original);

    Setup_Tab4_AvailabilityDataTV->resizeColumnsToContents();

    // Set to appropriate number of decimal places
    callback_DecimalPlacesCMB(0);

    Setup_Tabs->setCursor(Qt::ArrowCursor);
}

void
nmfSetup_Tab4::saveAvailabilityData()
{
    QString msg;
    QString lastSpecies;
    QString lastPredatorType;
    QString availabilityFile = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsAvailability::InputDataDir));
    availabilityFile = QDir(availabilityFile).filePath(QString::fromStdString(nmfConstantsAvailability::AvailabilityFile));
    std::ofstream outputFileAvailability(availabilityFile.toLatin1());

    // Write out header
    for (int col=0; col<m_smodel->columnCount(); ++col) {
        if (col > 0) {
            outputFileAvailability << ",";
        }
        outputFileAvailability << m_smodel->horizontalHeaderItem(col)->text().toStdString();
    }
    outputFileAvailability << std::endl;

    // Write out data
    for (int row=0; row<m_smodel->rowCount(); ++row) {
        for (int col=0; col<m_smodel->columnCount(); ++col) {
            if (col > 0) {
                outputFileAvailability << ",";
            }
            outputFileAvailability << m_smodel->item(row,col)->text().toStdString();
        }
        outputFileAvailability << std::endl;
    }
    outputFileAvailability.close();

    msg = "\nAvailability data saved as: \n\n" + availabilityFile + "\n";
    QMessageBox::information(Setup_Tabs, "File Saved", msg);
}

void
nmfSetup_Tab4::showExportAvailabilityDataGUI()
{
    // Raise file browser to select a .prm file
    QString filename = QFileDialog::getOpenFileName(
                Setup_Tabs, tr("Open .prm File"),
                "",tr("Parameters Files (*.prm)"));
    if (! filename.isEmpty()) {
        QGuiApplication::setOverrideCursor(Qt::WaitCursor);
        nmfExportDialog* dlg = new nmfExportDialog(filename,m_smodel);
        dlg->show();
        QGuiApplication::restoreOverrideCursor();
    }
}

void
nmfSetup_Tab4::callback_SavePB()
{
    if ((m_smodel != nullptr) && (m_smodel->rowCount() > 0)) {
        saveAvailabilityData();
    } else {
        QMessageBox::warning(Setup_Tabs,"Warning","\nNo data to Save.\n");
    }
}

void
nmfSetup_Tab4::callback_ExportPB()
{
    if ((m_smodel != nullptr) && (m_smodel->rowCount() > 0)) {
        showExportAvailabilityDataGUI();
    } else {
        QMessageBox::warning(Setup_Tabs,"Warning","\nNo data to Export.\n");
    }
}

bool
nmfSetup_Tab4::isAllZeros(int col)
{
    for (int row=0; row<m_smodel->rowCount();++row) {
        if (m_smodel->item(row,col)->text().toDouble() != 0.0) {
            return false;
        }
    }
    return true;
}

bool
nmfSetup_Tab4::showZeros()
{
    return Setup_Tab4_ShowZerosCB->isChecked();
}

void
nmfSetup_Tab4::callback_ShowZeroColumnsCB(int state)
{
    if (m_smodel) {
        if (state == Qt::Checked) { // show all columns
            for (int col=1; col<m_smodel->columnCount();++col) {
                Setup_Tab4_AvailabilityDataTV->showColumn(col);
            }
            Setup_Tab4_AvailabilityDataTV->resizeColumnsToContents();
        } else {
            for (int col=1; col<m_smodel->columnCount();++col) {
                if (isAllZeros(col)) {
                    Setup_Tab4_AvailabilityDataTV->hideColumn(col);
                }
            }
        }
    }
}

void
nmfSetup_Tab4::callback_ShowZerosCB(int state)
{
    if (m_smodel) {
        if (state != Qt::Checked) {
            for (int row=0; row<m_smodel->rowCount();++row) {
                for (int col=1; col<m_smodel->columnCount();++col) {
                    if (m_smodel->item(row,col)->text().toDouble() == 0.0) {
                        m_smodel->item(row,col)->setText("");
                    }
                }
            }
        } else {
            for (int row=0; row<m_smodel->rowCount();++row) {
                for (int col=1; col<m_smodel->columnCount();++col) {
                    if (m_smodel->item(row,col)->text().isEmpty()) {
                        m_smodel->item(row,col)->setText("0");
                    }
                }
            }
        }
    }
}

void
nmfSetup_Tab4::callback_DecimalPlacesCMB(int index)
{
    int numDecimalPlaces = getNumberDecimalPlaces();
    double value;
    QString valueStr = "";
    QString zeroStr = "";

    nmfUtilsQt::modelCopy(m_smodel_Original,m_smodel);

    if (m_smodel) {
        for (int row=0; row<m_smodel->rowCount();++row) {
            for (int col=1; col<m_smodel->columnCount();++col) {
                value = m_smodel->item(row,col)->text().toDouble();
                if (QString::number(value,'f',numDecimalPlaces).toDouble() == 0.0) {
                    zeroStr = (showZeros()) ? "0" : "";
                    m_smodel->item(row,col)->setText(zeroStr);
                } else {
                    m_smodel->item(row,col)->setText(QString::number(value,'f',numDecimalPlaces));
                }
            }
        }
        Setup_Tab4_AvailabilityDataTV->resizeColumnsToContents();
    }
}
