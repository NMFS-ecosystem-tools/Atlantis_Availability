
#include "nmfMainWindow.h"
#include "ui_nmfMainWindow.h"

#include "nmfConstants.h"
#include "nmfConstantsAvailability.h"

// This is needed since a signal is passing a std::string type
Q_DECLARE_METATYPE (std::string)

nmfMainWindow::nmfMainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_UI(new Ui::nmfMainWindow)
{
    QVBoxLayout* vlayt = new QVBoxLayout();

    if (! okSplashScreen()) {
        close();
    }

    m_UI->setupUi(this);

    m_UI->OutputDockWidget->hide();
    m_ChartWidget = nullptr;
    m_ChartView2d = nullptr;
    m_ProjectDir.clear();
    m_ProjectName.clear();
    m_ProjectSettingsConfig.clear();

    m_PreferencesDlg  = new QDialog(this);
    m_AvailabilityDlg = nullptr;
    m_ChartWidget     = new QChart();
    m_ChartView2d     = new QChartView(m_ChartWidget);
    m_UI->OutputChartTab->setLayout(vlayt);
    vlayt->addWidget(m_ChartView2d);

    // Check for and make if necessary hidden dirs for program usage: logs, data
    nmfUtilsQt::checkForAndCreateDirectories(nmfConstantsAvailability::HiddenDir,
                                 nmfConstantsAvailability::HiddenDataDir,
                                 nmfConstantsAvailability::HiddenLogDir);

    // Check for log files
    nmfUtilsQt::checkForAndDeleteLogFiles(
                QApplication::applicationName(),
                nmfConstantsAvailability::HiddenLogDir,
                nmfConstantsAvailability::LogFilter);

    m_Logger = new nmfLogger();
    m_Logger->initLogger(QApplication::applicationName().toStdString());
    m_Logger->logMsg(nmfConstants::Normal,"--- Start ---");

    readSettingsGuiOrientation(nmfConstantsAvailability::ResetPositionAlso);
    readSettings();

    bool usePreviousProject = queryUserPreviousProject();

    initPreferencesDlg();
    initLogWidget();
    initGUIs();
    initConnections();

    this->addDockWidget(Qt::BottomDockWidgetArea, m_UI->LogDockWidget);
    m_UI->LogDockWidget->hide();

    initPostGuiConnections();

    m_UI->OutputDockWidget->hide();

    if (usePreviousProject) {
        QString projectFile = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(m_ProjectName));
        Setup_Tab2_ptr->loadProject(m_Logger,projectFile);
    }

    enableCalculateTab(false);
}

nmfMainWindow::~nmfMainWindow()
{
    delete m_UI;
}

bool
nmfMainWindow::queryUserPreviousProject()
{
    QMessageBox::StandardButton reply;
    std::string msg  = "\nLast Project worked on:  " + m_ProjectName + "\n\nContinue working with this Project?\n";
    reply = QMessageBox::question(this, tr("Open"), tr(msg.c_str()),
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::Yes);
    return (reply == QMessageBox::Yes);
}

void
nmfMainWindow::closeEvent(QCloseEvent *event)
{
    m_Logger->logMsg(nmfConstants::Normal,"--- End ---");
    saveSettings();
}

QTableView*
nmfMainWindow::findTableInFocus()
{
    QTableView *retv = nullptr;

    if (m_UI->SetupInputTabWidget->findChild<QTableView *>("Setup_Tab4_AvailabilityDataTV")->hasFocus()) {
        return m_UI->SetupInputTabWidget->findChild<QTableView *>("Setup_Tab4_AvailabilityDataTV");
    } else if (m_UI->SetupInputTabWidget->findChild<QTableView *>("Setup_Tab3_DietCompositionTV")->hasFocus()) {
        return m_UI->SetupInputTabWidget->findChild<QTableView *>("Setup_Tab3_DietCompositionTV");
    } else if (m_UI->SetupInputTabWidget->findChild<QTableView *>("Setup_Tab3_PredatorTV")->hasFocus()) {
        return m_UI->SetupInputTabWidget->findChild<QTableView *>("Setup_Tab3_PredatorTV");
    } else if (m_UI->SetupInputTabWidget->findChild<QTableView *>("Setup_Tab3_PreyOnlyTV")->hasFocus()) {
        return m_UI->SetupInputTabWidget->findChild<QTableView *>("Setup_Tab3_PreyOnlyTV");
    }

    else {
        std::cout << "Error: No table found to cut, copy, or paste." << std::endl;
        return retv;
    }
}

QString
nmfMainWindow::getCurrentStyle()
{
    if (qApp->styleSheet().isEmpty())
        return "Light";
    else
        return "Dark";
}

void
nmfMainWindow::initConnections()
{
    connect(m_UI->actionAbout,                SIGNAL(triggered()),
            this,                             SLOT(menu_about()));
    connect(m_UI->actionClear,                SIGNAL(triggered()),
            this,                             SLOT(menu_clear()));
    connect(m_UI->actionCopy,                 SIGNAL(triggered()),
            this,                             SLOT(menu_copy()));
    connect(m_UI->actionDeselectAll,          SIGNAL(triggered()),
            this,                             SLOT(menu_deselectAll()));
    connect(m_UI->actionPaste,                SIGNAL(triggered()),
            this,                             SLOT(menu_paste()));
    connect(m_UI->actionPasteAll,             SIGNAL(triggered()),
            this,                             SLOT(menu_pasteAll()));
    connect(m_UI->actionPreferences,          SIGNAL(triggered()),
            this,                             SLOT(menu_preferences()));
    connect(m_UI->actionQuit,                 SIGNAL(triggered()),
            this,                             SLOT(menu_quit()));
    connect(m_UI->actionScreenShot,           SIGNAL(triggered()),
            this,                             SLOT(menu_screenShot()));
    connect(m_UI->actionSelectAll,            SIGNAL(triggered()),
            this,                             SLOT(menu_selectAll()));
    connect(m_UI->actionWhatsThis,            SIGNAL(triggered()),
            this,                             SLOT(menu_whatsThis()));
    connect(m_UI->actionWhatsThisTB,          SIGNAL(triggered()),
            this,                             SLOT(menu_whatsThis()));
    connect(m_NavigatorTree,                  SIGNAL(itemSelectionChanged()),
            this,                             SLOT(callback_NavigatorSelectionChanged()));
    connect(Setup_Tab2_ptr,                   SIGNAL(ProjectSaved()),
            this,                             SLOT(callback_ProjectSaved()));   
    connect(Setup_Tab3_ptr,                   SIGNAL(EnableCalculateTab(bool)),
            this,                             SLOT(callback_EnableCalculateTab(bool)));
    connect(Setup_Tab3_ptr,                   SIGNAL(ShowOutputTab(bool)),
            this,                             SLOT(callback_ShowOutputTab(bool)));
    connect(m_UI->OutputControlChartThemeCMB, SIGNAL(currentTextChanged(QString)),
            Setup_Tab3_ptr,                   SLOT(callback_TypeCMB(QString)));
    connect(m_UI->SetupInputTabWidget,        SIGNAL(currentChanged(int)),
            this,                             SLOT(callback_SetupTabChanged(int)));
    connect(Setup_Tab3_ptr,                   SIGNAL(ShowOutputChart(QStringList&, QStringList&,
                                                                     boost::numeric::ublas::matrix<double>&)),
            this,                             SLOT(callback_ShowOutputChart(QStringList&, QStringList&,
                                                                            boost::numeric::ublas::matrix<double>&)));
}

void
nmfMainWindow::initGUIs()
{
    QUiLoader loader;

    // Initialize Navigator Tree
    QFile file(":/forms/Main/Main_NavigatorTreeWidget.ui");
    file.open(QFile::ReadOnly);
    m_NavigatorTreeWidget = loader.load(&file,this);
    file.close();
    m_UI->NavigatorDockWidget->setWidget(m_NavigatorTreeWidget);
    m_NavigatorTree = m_UI->NavigatorDockWidget->findChild<QTreeWidget *>("NavigatorTree");
    QModelIndex index = m_NavigatorTree->model()->index(0,0);
    m_NavigatorTree->setCurrentIndex(index);
    initNavigatorTree();

    // Load up Setup pages
    Setup_Tab1_ptr      = new nmfSetup_Tab1(m_UI->SetupInputTabWidget);
    Setup_Tab2_ptr      = new nmfSetup_Tab2(m_UI->SetupInputTabWidget,m_Logger);
    Setup_Tab3_ptr      = new nmfSetup_Tab3(m_UI->SetupInputTabWidget,m_Logger,m_ProjectDir);
    Setup_Tab4_ptr      = new nmfSetup_Tab4(m_UI->SetupInputTabWidget,m_Logger,m_ProjectDir);

    // Select first item in Navigator Tree
    callback_SetupTabChanged(0);
}

void
nmfMainWindow::initNavigatorTree()
{
    QTreeWidgetItem *item;
    QTreeWidgetItem *lastItem;

    m_NavigatorTree->clear();
    item = nmfUtilsQt::addTreeRoot(m_NavigatorTree,"Availability Model");
    nmfUtilsQt::addTreeItem(item, "1. Getting Started");
    nmfUtilsQt::addTreeItem(item, "2. Project Setup");
    nmfUtilsQt::addTreeItem(item, "3. Species Setup");
    nmfUtilsQt::addTreeItem(item, "4. Calculate Availability");
    item->setExpanded(true);

    enableLastNavigatorItem(false);
}

void
nmfMainWindow::enableLastNavigatorItem(bool state)
{
    QTreeWidgetItem *topItem  = m_NavigatorTree->topLevelItem(0);
    QTreeWidgetItem *lastItem = topItem->child(3);

    if (state) {
        lastItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
    } else {
        lastItem->setFlags(Qt::NoItemFlags);
    }
}

void
nmfMainWindow::initPostGuiConnections()
{
    connect(Setup_Tab2_ptr, SIGNAL(LoadProject()),
            this,           SLOT(callback_LoadProject()));
}

void
nmfMainWindow::initPreferencesDlg()
{
    QUiLoader loader;
    QFile file(":/forms/Main/PreferencesDlg.ui");
    file.open(QFile::ReadOnly);
    m_PreferencesWidget = loader.load(&file,this);
    file.close();

    QComboBox*   styleCMB = m_PreferencesWidget->findChild<QComboBox*>("PrefAppStyleCMB");
    QPushButton* closePB  = m_PreferencesWidget->findChild<QPushButton*>("PrefClosePB");
    QVBoxLayout* layt = new QVBoxLayout();
    layt->addWidget(m_PreferencesWidget);
    m_PreferencesDlg->setLayout(layt);
    m_PreferencesDlg->setWindowTitle("Preferences");

    connect(styleCMB,         SIGNAL(currentTextChanged(QString)),
            this,             SLOT(callback_PreferencesSetStyleSheet(QString)));
    connect(closePB,          SIGNAL(clicked()),
            m_PreferencesDlg, SLOT(close()));
}

void
nmfMainWindow::loadGuis()
{
//  Setup_Tab2_ptr->loadWidgets();
    Setup_Tab3_ptr->loadWidgets();
}

bool
nmfMainWindow::okSplashScreen()
{
   QLabel* splashLBL            = new QLabel();
   m_AvailabilityDlg            = new QDialog();
   QPushButton* resetGUIPB      = new QPushButton("Reset GUI");
   QPushButton* startPB         = new QPushButton("Start");
   QPushButton* resetSettingsPB = new QPushButton("Reset Settings");
   QVBoxLayout* vlayt           = new QVBoxLayout();
   QHBoxLayout* hlayt           = new QHBoxLayout();
   QPixmap splashPM(":/icons/atlantis2v2.png");
   startPB->setFixedWidth(100);
   startPB->setFixedHeight(50);
   resetGUIPB->setToolTip("Move main GUI back to (0,0). (Useful if GUI gets lost.)");
   resetGUIPB->setStatusTip("Move main GUI back to (0,0). (Useful if GUI gets lost.)");
   resetSettingsPB->setToolTip("Remove Settings file. (Useful if App won't start.)");
   resetSettingsPB->setStatusTip("Remove Settings file. (Useful if App won't start.)");

   splashLBL->setPixmap(splashPM);
   splashLBL->setMask(splashPM.mask());
   hlayt->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
   hlayt->addWidget(resetGUIPB);
   hlayt->addWidget(startPB);
   hlayt->addWidget(resetSettingsPB);
   hlayt->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
   vlayt->addWidget(splashLBL);
   vlayt->addLayout(hlayt);
   m_AvailabilityDlg->setLayout(vlayt);
   connect(m_AvailabilityDlg,SIGNAL(rejected()),
           this,             SLOT(callback_SplashScreenRejected()));
   connect(startPB,          SIGNAL(clicked()),
           m_AvailabilityDlg,SLOT(hide()));
   connect(resetGUIPB,       SIGNAL(clicked()),
           this,             SLOT(callback_ResetGUI()));
   connect(resetSettingsPB,  SIGNAL(clicked()),
           this,             SLOT(callback_ResetSettings()));
   m_AvailabilityDlg->setWindowTitle("Availability Calculator");
   m_AvailabilityDlg->exec();

   return true;
}

void
nmfMainWindow::callback_SplashScreenRejected()
{
    close();
}

void
nmfMainWindow::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(
                nmfConstantsAvailability::SettingsDirWindows,QApplication::applicationName());

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("SetupTab");
    m_ProjectName     = settings->value("ProjectName","").toString().toStdString();
    m_ProjectDir      = settings->value("ProjectDir","").toString().toStdString();
    m_SetupFontSize   = settings->value("FontSize",9).toInt();
    settings->endGroup();

    delete settings;

    updateWindowTitle();
}

void
nmfMainWindow::readSettingsGuiOrientation(bool alsoResetPosition)
{
    QSettings* settings = nmfUtilsQt::createSettings(
                nmfConstantsAvailability::SettingsDirWindows,QApplication::applicationName());

    settings->beginGroup("MainWindow");
    resize(settings->value("size", QSize(400, 400)).toSize());
    if (alsoResetPosition) {
        move(settings->value("pos", QPoint(200, 200)).toPoint());
    }

    // Resize the dock widgets
    int NavDockWidth      = settings->value("NavigatorDockWidgetWidth",200).toInt();
    int OutputDockWidth   = settings->value("OutputDockWidgetWidth",200).toInt();
    this->resizeDocks({m_UI->NavigatorDockWidget,m_UI->OutputDockWidget},
                      {NavDockWidth,OutputDockWidth},
                      Qt::Horizontal);
    settings->endGroup();

    delete settings;
}

bool
nmfMainWindow::saveScreenshot(QString &outputImageFile, QPixmap &pm)
{
    QString msg;
    QString pathImage;
    QString pathData;
    QMessageBox::StandardButton reply;
    QString outputImageFileWithPath;
    QString outputDataFileWithPath;
    QString outputDataFile = outputImageFile;

    nmfUtilsQt::switchFileExtensions(outputDataFile,".csv",{".jpg",".png"});

    pathImage = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsAvailability::OutputImagesDir));
    pathData  = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsAvailability::OutputDataDir));

    // If path doesn't exist make it
    for (QString path : {pathImage,pathData}) {
        QDir pathDir(path);
        if (! pathDir.exists()) {
            pathDir.mkpath(path);
        }
    }
    outputDataFileWithPath  = QDir(pathData).filePath(outputDataFile);
    outputImageFileWithPath = QDir(pathImage).filePath(outputImageFile);
    if (QFileInfo(outputImageFileWithPath).exists()) {
        msg   = "\nFile exists. OK to overwrite?\n";
        reply = QMessageBox::question(this, tr("File Exists"), tr(msg.toLatin1()),
                                      QMessageBox::No|QMessageBox::Yes,
                                      QMessageBox::Yes);
        if (reply == QMessageBox::No)
            return false;
    }

    // Save the image
    pm.save(outputImageFileWithPath);

    // Notify user image has been saved
    msg  = "\nOutput image saved to file:\n\n" + outputImageFileWithPath;
    QMessageBox::information(this,
                             tr("Image and Data Saved"),
                             tr(msg.toLatin1()),
                             QMessageBox::Ok);

    m_Logger->logMsg(nmfConstants::Normal,"menu_screenshot: Image saved: "+ outputImageFile.toStdString());

    return true;
}

void
nmfMainWindow::saveSettings() {

    QSettings* settings = nmfUtilsQt::createSettings(
                nmfConstantsAvailability::SettingsDirWindows,QApplication::applicationName());

    settings->beginGroup("MainWindow");
    settings->setValue("pos", pos());
    settings->setValue("size", size());
    settings->setValue("style",getCurrentStyle());
    settings->setValue("NavigatorDockWidgetWidth",   m_UI->NavigatorDockWidget->width());
    settings->setValue("OutputDockWidgetWidth",      m_UI->OutputDockWidget->width());
    settings->setValue("LogDockWidgetIsVisible",     m_UI->LogDockWidget->isVisible());
    settings->setValue("OutputDockWidgetIsVisible",  m_UI->OutputDockWidget->isVisible());
    settings->setValue("OutputDockWidgetIsFloating", m_UI->OutputDockWidget->isFloating());
    settings->setValue("OutputDockWidgetPos",        m_UI->OutputDockWidget->pos());
    settings->setValue("OutputDockWidgetSize",       m_UI->OutputDockWidget->size());
//  settings->setValue("CentralWidgetSize",          centralWidget()->size());
    settings->endGroup();

    settings->beginGroup("SetupTab");
    settings->setValue("ProjectName",               QString::fromStdString(m_ProjectName));
    settings->setValue("ProjectDir",                QString::fromStdString(m_ProjectDir));
//  settings->setValue("ProjectAuthor",             ProjectAuthor);
//  settings->setValue("ProjectDescription",        ProjectDescription);
    settings->endGroup();

    delete settings;

    // Save other pages' settings
    Setup_Tab2_ptr->saveSettings();
}

void
nmfMainWindow::saveDefaultSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(
                nmfConstantsAvailability::SettingsDirWindows,QApplication::applicationName());

    settings->beginGroup("MainWindow");
    settings->setValue("pos", QPoint(0,0));
    settings->endGroup();

    delete settings;
}

void
nmfMainWindow::initLogWidget()
{
    m_LogWidget = new nmfLogWidget(m_Logger,nmfConstantsAvailability::LogDir);
    m_UI->LogWidget->setLayout(m_LogWidget->vMainLayt);
}

void
nmfMainWindow::updateWindowTitle()
{
    QSettings* settings = nmfUtilsQt::createSettings(
                nmfConstantsAvailability::SettingsDirWindows,QApplication::applicationName());

    settings->beginGroup("SetupTab");
    m_ProjectName     = settings->value("ProjectName","").toString().toStdString();
    m_ProjectDir      = settings->value("ProjectDir","").toString().toStdString();
    m_SetupFontSize   = settings->value("FontSize",9).toInt();
    settings->endGroup();
    std::string winTitle = "Availability Tool (" + m_ProjectName + ")";
    setWindowTitle(QString::fromStdString(winTitle));

    delete settings;
}

void
nmfMainWindow::callback_LoadProject()
{
  readSettings();

  // Must also call readSettings for the other tab pages since some of the
  // project settings (i.e. dir and name) may have changed
  Setup_Tab3_ptr->readSettings();
  Setup_Tab4_ptr->readSettings();

  disconnect(Setup_Tab2_ptr, SIGNAL(LoadProject()),
             this,           SLOT(callback_LoadProject()));

  loadGuis();

  connect(Setup_Tab2_ptr, SIGNAL(LoadProject()),
          this,           SLOT(callback_LoadProject()));
}

void
nmfMainWindow::callback_NavigatorSelectionChanged()
{
    QString parentStr;
    QString itemSelected;
    int pageNum;

    // Handle the case if the user hasn't selected anything yet.
    QList<QTreeWidgetItem *> selectedItems = m_NavigatorTree->selectedItems();

    if (selectedItems.count() > 0) {
        itemSelected = selectedItems[0]->text(0);
        pageNum = itemSelected.at(0).digitValue();
        if (pageNum < 1)
            pageNum = 1;

        parentStr.clear();
        if (selectedItems[0]->parent()) {
            parentStr = selectedItems[0]->parent()->text(0);
        }

        if ((itemSelected == "Availability Model") || (parentStr == "Availability Model")) {
            if (pageNum > 0) {
                m_UI->SetupInputTabWidget->blockSignals(true);
                m_UI->SetupInputTabWidget->setCurrentIndex(pageNum-1);
                m_UI->SetupInputTabWidget->blockSignals(false);
            }
        }
    }
}

void
nmfMainWindow::callback_PreferencesSetStyleSheet(QString style)
{
    // Update GUI widget colors for the Light or Dark theme
    if (style == "Dark") {
        QFile fileStyle(":qdarkstyle/style.qss");
        if (! fileStyle.exists()) {
            std::cout << "Error: Unable to set stylesheet, file not found: qdarkstyle/style.qss\n" << std::endl;;
        } else {
            fileStyle.open(QFile::ReadOnly | QFile::Text);
            QTextStream stream(&fileStyle);
            qApp->setStyleSheet(stream.readAll());
        }
    } else {
        qApp->setStyleSheet("");
    }

    // Necessary to redraw the chart with either the Light or Dark theme
    m_UI->OutputControlChartThemeCMB->setCurrentText(style);
}

void
nmfMainWindow::callback_ShowOutputTab(bool state)
{
    m_UI->OutputDockWidget->setVisible(state);
}

void
nmfMainWindow::callback_EnableCalculateTab(bool enable)
{
    enableCalculateTab(enable);
}

void
nmfMainWindow::enableCalculateTab(bool isEnabled)
{
    enableLastNavigatorItem(isEnabled);
    m_UI->SetupInputTabWidget->setTabEnabled(3,isEnabled);
}

void
nmfMainWindow::callback_ProjectSaved()
{
    updateWindowTitle();
}

void
nmfMainWindow::callback_ResetGUI()
{
    QMessageBox::StandardButton reply =
            QMessageBox::question(m_AvailabilityDlg,
                                  tr("Reset GUI"),
                                  tr("\nOK to reset GUI position?\n"),
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::Yes);
    if (reply == QMessageBox::Yes) {
        saveDefaultSettings();
    }
}

void
nmfMainWindow::callback_ResetSettings()
{
    QMessageBox::StandardButton reply =
            QMessageBox::question(m_AvailabilityDlg,
                                  tr("Reset Settings"),
                                  tr("\nOK to reset application settings?\n"),
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::Yes);
    if (reply == QMessageBox::Yes) {
        nmfUtilsQt::removeSettingsFile();
        QMessageBox::information(m_AvailabilityDlg,
                                 tr("Reset Settings"),
                                 tr("\nSettings reset.\n\nUser interface settings will be set again upon application exit.\n"),
                                 QMessageBox::Ok);
    }
}

void
nmfMainWindow::callback_SetupTabChanged(int tab)
{
    QModelIndex topLevelIndex = m_NavigatorTree->model()->index(0,0); // first 0 is Setup group in NavigatorTree
    QModelIndex childIndex    = topLevelIndex.model()->index(tab,0,topLevelIndex);
    m_NavigatorTree->blockSignals(true);
    m_NavigatorTree->setCurrentIndex(childIndex);
    m_NavigatorTree->blockSignals(false);

    if (tab == 2) {
        Setup_Tab3_ptr->setLoadPBState(true);//Setup_Tab3_ptr->hasUserPressedDietCompositionSave());
        callback_ShowOutputTab(Setup_Tab3_ptr->hasUserPressedView() &&
                               Setup_Tab3_ptr->viewingDietCompositionTab());
    } else {
        callback_ShowOutputTab(false);
    }
}

void
nmfMainWindow::callback_ShowOutputChart(
        QStringList& predatorNames,
        QStringList& preyNames,
        boost::numeric::ublas::matrix<double>& ChartData)
{
    std::string MainTitle;
    std::string XTitle;
    std::string YTitle;
    double MaxYVal = 1.0;
    QStringList ColumnLabels = preyNames;
    QStringList RowLabels    = predatorNames;

    m_UI->OutputDockWidget->show();

    m_ChartWidget->removeAllSeries();

    QString ChartType = m_UI->OutputControlChartTypeCMB->currentText();
    int ChartTheme    = m_UI->OutputControlChartThemeCMB->currentIndex();

    if (ChartType == "Bar") {
        nmfChartBar* barChart = new nmfChartBar();
        MainTitle = "Diet Composition Data";
        XTitle    = "Predator Species";
        YTitle    = "Diet Composition";
        ChartType = "StackedBar";
        barChart->populateChart(m_ChartWidget, ChartType.toStdString(), MaxYVal,
                                ChartData, RowLabels, ColumnLabels,
                                MainTitle, XTitle, YTitle,
                                nmfConstantsAvailability::DataAreNotPercentages,
                                {true,true}, ChartTheme);
    }
}

void
nmfMainWindow::menu_about()
{
    QString name    = "Tool for calculating predator food availability.";
    QString version = "Availability Tool v0.9.1 (beta)";
    QString specialAcknowledgement = "";
    QString cppVersion   = "C++??";
    QString boostVersion = "?";
    QString qdarkLink;
    QString linuxDeployLink;
    QString boostLink;
    QString qtLink;
    QString msg;
    QString os = QString::fromStdString(nmfUtils::getOS());

    // Define Qt link
    qtLink = QString("<a href='https://www.qt.io'>https://www.qt.io</a>");

    // Find C++ version in case you want it later
    if (__cplusplus == 201103L)
        cppVersion = "C++11";
    else if (__cplusplus == 201402L)
        cppVersion = "C++14";
    else if (__cplusplus == 201703L)
        cppVersion = "C++17";

    // Boost version and link
    boostVersion = QString::number(BOOST_VERSION / 100000) + "." +
                   QString::number(BOOST_VERSION / 100 % 1000) + "." +
                   QString::number(BOOST_VERSION / 100);
    boostLink = QString("<a href='https://www.boost.org'>https://www.boost.org</a>");

    // QDarkStyle link
    qdarkLink = QString("<a href='https://github.com/ColinDuquesnoy/QDarkStyleSheet'>https://github.com/ColinDuquesnoy/QDarkStyleSheet</a>");

    // linuxdeployqt link
    linuxDeployLink = QString("<a href='https://github.com/probonopd/linuxdeployqt'>https://github.com/probonopd/linuxdeployqt</a>");

    // Build About message
  //msg += QString("<li>")+cppVersion+QString("</li>");
    msg += QString("<li>")+QString("Qt ")+QString::fromUtf8(qVersion())+QString("<br>")+qtLink+QString("</li>");
    msg += QString("<li>")+QString("Boost ")+boostVersion+QString("<br>")+boostLink+QString("</li>");
    msg += QString("<li>")+QString("QDarkStyleSheet 2.6.5 - Colin Duquesnoy (MIT License)<br>")+qdarkLink+QString("</li>");
    msg += QString("<li>")+QString("linuxdeployqt 6 (January 27, 2019)<br>")+linuxDeployLink+QString("</li>");
    msg += QString("</ul>");

    nmfUtilsQt::showAboutWidget(this,name,os,version,specialAcknowledgement,msg);
}

bool
nmfMainWindow::isTableReadOnly(QTableView* tv)
{
    return (tv->objectName() == "Setup_Tab4_AvailabilityDataTV");
}

void
nmfMainWindow::menu_clear()
{
    QTableView* tv = findTableInFocus();

    if (tv != nullptr) {
        if (! isTableReadOnly(tv)) {
            QString retv = nmfUtilsQt::clear(qApp,findTableInFocus());
            if (! retv.isEmpty()) {
                QMessageBox::question(this,tr("Clear"),retv,QMessageBox::Ok);
            }
        } else {
            QMessageBox::information(this,"Info","\nCurrent table is read-only.\n");
        }
    } else {
        QMessageBox::information(this,"Info","\nClear not available.\n");
    }
}

void
nmfMainWindow::menu_copy()
{
    try {
        findTableInFocus();
        // RSK - this causes a hard crash if user copies cells one of which has a combobox
    } catch (...) {
        std::cout << "\nError: Invalid cells found to copy." << std::endl;
        return;
    }

    QString retv = nmfUtilsQt::copy(qApp,findTableInFocus());
    if (! retv.isEmpty()) {
        QMessageBox::question(this,tr("Copy"),retv,QMessageBox::Ok);
    }
}

void
nmfMainWindow::menu_clearAll()
{
    QTableView* tv = findTableInFocus();
    if (tv != nullptr) {
        if (! isTableReadOnly(tv)) {
            QString retv = nmfUtilsQt::clearAll(findTableInFocus());
            if (! retv.isEmpty()) {
                QMessageBox::question(this,tr("Clear All"),retv,QMessageBox::Ok);
            }
        } else {
            QMessageBox::information(this,"Info","\nCurrent table is read-only.\n");
        }
    } else {
        QMessageBox::information(this,"Info","\nClear not available.\n");
    }
}

void
nmfMainWindow::menu_deselectAll()
{
    QString retv = nmfUtilsQt::deselectAll(findTableInFocus());
    if (! retv.isEmpty()) {
        QMessageBox::question(this,tr("Deselect All"),retv,QMessageBox::Ok);
    }
}

void
nmfMainWindow::menu_paste()
{
    QStringList selectedPredators = Setup_Tab3_ptr->getSelectedPredators();
    QStringList selectedPreyOnly  = Setup_Tab3_ptr->getSelectedPreyOnly();

    QTableView* tv = findTableInFocus();
    if (tv != nullptr) {
        if (! isTableReadOnly(tv)) {
            QString retv = nmfUtilsQt::paste(qApp,tv);
            if (! retv.isEmpty()) {
                QMessageBox::question(this,tr("Paste"),retv,QMessageBox::Ok);
            }
            restoreInitialColumns(selectedPredators,selectedPreyOnly,tv);
        } else {
            QMessageBox::information(this,"Info","\nCurrent table is read-only.\n");
        }
    } else {
        QMessageBox::information(this,"Info","\nPaste not available.\n");
    }
}

void
nmfMainWindow::restoreInitialColumns(
        QStringList& selectedPredators,
        QStringList& selectedPreyOnly,
        QTableView* tv)
{
    if (tv->objectName() == "Setup_Tab3_PredatorTV") {
        Setup_Tab3_ptr->restoreInitialPredatorColumns(selectedPredators);
    } else if (tv->objectName() == "Setup_Tab3_PreyOnlyTV") {
        Setup_Tab3_ptr->restoreInitialPreyOnlyColumns(selectedPreyOnly);
    } else if (tv->objectName() == "Setup_Tab3_DietCompositionTV") {
        Setup_Tab3_ptr->restoreInitialDietCompositionColumns();
    }
}

void
nmfMainWindow::enableAutoChecks(
        QTableView* tv,
        const bool& enable)
{
   if (tv->objectName() == "Setup_Tab3_DietCompositionTV") {
       Setup_Tab3_ptr->enableDietCompositionItemEdited(enable);
   }
}

void
nmfMainWindow::menu_pasteAll()
{
    QStringList selectedPredators = Setup_Tab3_ptr->getSelectedPredators();
    QStringList selectedPreyOnly  = Setup_Tab3_ptr->getSelectedPreyOnly();

    QTableView* tv = findTableInFocus();
    if (tv != nullptr) {
        if (! isTableReadOnly(tv)) {
            enableAutoChecks(tv,false);
            QString retv = nmfUtilsQt::pasteAll(qApp,tv);
            if (! retv.isEmpty()) {
                QMessageBox::question(this,tr("Paste All"),retv,QMessageBox::Ok);
            }
            restoreInitialColumns(selectedPredators,selectedPreyOnly,tv);
            enableAutoChecks(tv,true);
        } else {
            QMessageBox::information(this,"Info","\nCurrent table is read-only.\n");
        }
    } else {
        QMessageBox::information(this,"Info","\nPaste not available.\n");
    }
}

void
nmfMainWindow::menu_preferences()
{
    m_PreferencesDlg->show();
}

void
nmfMainWindow::menu_quit()
{
    if (QMessageBox::Yes == QMessageBox::question(
                this,
                tr("Quit"),
                tr("\nAre you sure you want to quit?\n")))
    {
        saveSettings();
        close(); // emits closeEvent
    }
}

void
nmfMainWindow::menu_screenShot()
{
    QPixmap pm;
    QString outputFile;
    std::string msg;

    if (m_ChartView2d->isVisible())
    {
        nmfStructsQt::ChartSaveDlg *dlg = new nmfStructsQt::ChartSaveDlg(this);
        if (dlg->exec()) {
            outputFile = dlg->getFilename();
        }
        delete dlg;
        if (outputFile.isEmpty())
            return;

        // Grab the image and store in a pixmap
        if (m_ChartView2d->isVisible()) {
            m_ChartView2d->update();
            m_ChartView2d->repaint();
            pm = m_ChartView2d->grab();

        }
        saveScreenshot(outputFile,pm);
    }
}

void
nmfMainWindow::menu_selectAll()
{
    QString retv = nmfUtilsQt::selectAll(findTableInFocus());
    if (! retv.isEmpty()) {
        QMessageBox::question(this,tr("Select All"),retv,QMessageBox::Ok);
    }
}

void
nmfMainWindow::menu_whatsThis()
{
    QWhatsThis::enterWhatsThisMode();
}
