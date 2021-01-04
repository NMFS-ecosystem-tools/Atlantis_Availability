
#include "nmfSetupTab02.h"
#include "nmfConstants.h"
#include "nmfConstantsAvailability.h"
#include "nmfUtilsQt.h"
#include "nmfUtils.h"


nmfSetup_Tab2::nmfSetup_Tab2(QTabWidget* tabs,
                           nmfLogger*    logger)
{
    QUiLoader loader;

    Setup_Tabs    = tabs;
    m_Logger      = logger;

    // Load ui as a widget from disk
    QFile file(":/forms/Setup/Setup_Tab02.ui");
    file.open(QFile::ReadOnly);
    Setup_Tab2_Widget = loader.load(&file,Setup_Tabs);
    Setup_Tab2_Widget->setObjectName("Setup_Tab2_Widget");
    file.close();

    // Add the loaded widget as the new tabbed page
    Setup_Tabs->addTab(Setup_Tab2_Widget, tr("2. Project Setup"));

    m_ProjectName.clear();
    m_ProjectDir.clear();
    m_ProjectAuthor.clear();
    m_ProjectDescription.clear();
    m_NewProject = false;

    Setup_Tab2_ProjectNameLE      = Setup_Tabs->findChild<QLineEdit   *>("Setup_Tab2_ProjectNameLE");
    Setup_Tab2_ProjectDirBrowsePB = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_BrowseProjectDirPB");
    Setup_Tab2_ProjectDirLE       = Setup_Tabs->findChild<QLineEdit   *>("Setup_Tab2_ProjectDirLE");
    Setup_Tab2_ProjectAuthorLE    = Setup_Tabs->findChild<QLineEdit   *>("Setup_Tab2_ProjectAuthorLE");
    Setup_Tab2_ProjectDescLE      = Setup_Tabs->findChild<QLineEdit   *>("Setup_Tab2_ProjectDescriptionLE");
    Setup_Tab2_SaveProjectPB      = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_SaveProjectPB");
    Setup_Tab2_NewProjectPB       = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_NewProjectPB");
    Setup_Tab2_BrowseProjectPB    = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_BrowseProjectPB");
    Setup_Tab2_DelProjectPB       = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_DelProjectPB");
    Setup_Tab2_ReloadProjectPB    = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_ReloadProjectPB");
    Setup_Tab2_ProjectDataGB      = Setup_Tabs->findChild<QGroupBox   *>("Setup_Tab2_ProjectDataGB");
    Setup_Tab2_NextPB             = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_NextPB");

    Setup_Tab2_ProjectNameLE->setClearButtonEnabled(true);
    Setup_Tab2_ProjectDirLE->setClearButtonEnabled(true);
    Setup_Tab2_ProjectAuthorLE->setClearButtonEnabled(true);
    Setup_Tab2_ProjectDescLE->setClearButtonEnabled(true);
    Setup_Tab2_NextPB->setText("--\u25B7");

    connect(Setup_Tab2_ProjectDirBrowsePB, SIGNAL(clicked()),
            this,                          SLOT(callback_ProjectDirBrowsePB()));
    connect(Setup_Tab2_SaveProjectPB,      SIGNAL(clicked()),
            this,                          SLOT(callback_SaveProject()));
    connect(Setup_Tab2_NewProjectPB,       SIGNAL(clicked()),
            this,                          SLOT(callback_NewProject()));
    connect(Setup_Tab2_DelProjectPB,       SIGNAL(clicked()),
            this,                          SLOT(callback_DelProject()));
    connect(Setup_Tab2_BrowseProjectPB,    SIGNAL(clicked()),
            this,                          SLOT(callback_BrowseProject()));
    connect(Setup_Tab2_ReloadProjectPB,    SIGNAL(clicked()),
            this,                          SLOT(callback_ProjectReload()));
    connect(Setup_Tab2_ProjectNameLE,      SIGNAL(editingFinished()),
            this,                          SLOT(callback_ProjectNameAdd()));
    connect(Setup_Tab2_ProjectAuthorLE,    SIGNAL(editingFinished()),
            this,                          SLOT(callback_ProjectAuthorAdd()));
    connect(Setup_Tab2_ProjectDescLE,      SIGNAL(editingFinished()),
            this,                          SLOT(callback_ProjectDescAdd()));
    connect(Setup_Tab2_ProjectDirLE,       SIGNAL(returnPressed()),
            this,                          SLOT(callback_ProjectDirAdd()));
    connect(Setup_Tab2_NextPB,             SIGNAL(clicked()),
            this,                          SLOT(callback_NextPB()));
}

nmfSetup_Tab2::~nmfSetup_Tab2()
{
}

bool
nmfSetup_Tab2::isProjectDataValid()
{
    return (! (getProjectName().isEmpty()                             ||
               Setup_Tab2_ProjectDirLE->text().isEmpty()              ||
               Setup_Tab2_ProjectAuthorLE->text().isEmpty()           ||
               Setup_Tab2_ProjectDescLE->text().isEmpty()));
}

void
nmfSetup_Tab2::callback_SaveProject()
{
    QString fullFilename;
    std::string msg;
    QMessageBox::StandardButton reply;
    QString fileSuffix;
    std::ofstream outFile;

    if (! isProjectDataValid()) {
        QMessageBox::warning(Setup_Tabs,
                             tr("Missing Data"),
                             tr("\nPlease enter missing data above."),
                             QMessageBox::Ok);
        return;
    }

    m_ProjectName = getProjectName();
    if (! isProjectNameValid(m_ProjectName)) {
        return;
    }

    // Build the filename, add the .prj prefix if one isn't already there and
    // check to see if the file already exists.
    fullFilename = QDir(m_ProjectDir).filePath(m_ProjectName);
    fileSuffix =  QFileInfo(fullFilename).suffix();
    if (fileSuffix != "prj")
        fullFilename += ".prj";
    if (QFileInfo(fullFilename).exists()) {
        msg  = "\nThe project file already exists:\n\n" + fullFilename.toStdString() + "\n\n";
        msg += "OK to overwrite it?\n";
        reply = QMessageBox::question(Setup_Tabs,
                                      tr("File Found"),
                                      tr(msg.c_str()),
                                      QMessageBox::Yes|QMessageBox::No,
                                      QMessageBox::Yes);
        if (reply == QMessageBox::No) {
            return;
        }
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    // Write out the project file
    outFile = std::ofstream(fullFilename.toLatin1());
    outFile << "#\n";
    outFile << "# This is an Availability Calculator Project file.\n";
    outFile << "#\n";
    outFile << "# It defines the Project Name and Directory as well as other\n";
    outFile << "# information for the Project. The following data are:\n";
    outFile << "# Project directory, Author, and Description.\n";
    outFile << "#\n";
    outFile << m_ProjectDir.toStdString() << "\n";
    outFile << m_ProjectAuthor.toStdString() << "\n";
    outFile << m_ProjectDescription.toStdString() << "\n";
    outFile.close();

    emit SaveMainSettings();

    emit ProjectSaved();

    // Make sure the outputImages and outputData directories are there
    QString imagePath      = QDir(m_ProjectDir).filePath("outputImages");
    QString inputDataPath  = QDir(m_ProjectDir).filePath("inputData");
    QString outputDataPath = QDir(m_ProjectDir).filePath("outputData");
    QDir dir;
    dir.mkpath(imagePath);
    dir.mkpath(inputDataPath);
    dir.mkpath(outputDataPath);

    // Load the project
    loadProject(m_Logger,QDir(m_ProjectDir).filePath(m_ProjectName+".prj"));

    QApplication::restoreOverrideCursor();

    QString msgConfirm =  "\nSaved project: " + m_ProjectName + "\n";
    QMessageBox::information(Setup_Tabs,tr("Project Saved"),
                          tr(msgConfirm.toLatin1()),
                          QMessageBox::Ok);
}

bool
nmfSetup_Tab2::isProjectNameValid(QString projectName)
{
    std::string msg;
    bool isValid = false;
    bool isAtLeastOneChar = (! projectName.isEmpty());

    // Use regex to validate name
    QRegularExpression re("^[a-zA-Z0-9_]*$");
    QRegularExpressionMatch match = re.match(m_ProjectName);
    isValid = match.hasMatch();

    if ((! isAtLeastOneChar) || (! isValid)) {
        msg = "Invalid project name: " + projectName.toStdString();
        m_Logger->logMsg(nmfConstants::Error,msg);
        msg += "\n\nA project name must contain at least one character and consist";
        msg += "\nonly of letters, numbers, and underscore characters.";
        QMessageBox::warning(Setup_Tabs,"Invalid Project Name",
                             QString::fromStdString("\n"+msg+"\n"),
                             QMessageBox::Ok);
    }

    return (isAtLeastOneChar && isValid);
}

void
nmfSetup_Tab2::clearProject()
{
    setProjectAuthor("");
    setProjectDescription("");
    setProjectDirectory("");
    setProjectName("");
}

void
nmfSetup_Tab2::callback_ProjectDirBrowsePB()
{
    // Launch file dialog
    QString dir = QFileDialog::getExistingDirectory(
                Setup_Tabs,
                tr("Open Project Directory"),
                m_ProjectDir+"/..",
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (! dir.isEmpty()) {
        setProjectDirectory(dir);
        m_ProjectDir = dir;
    }
}

void
nmfSetup_Tab2::callback_NextPB()
{
    int nextPage = Setup_Tabs->currentIndex()+1;
    Setup_Tabs->setCurrentIndex(nextPage);
}

void
nmfSetup_Tab2::callback_ProjectDirAdd()
{
    m_ProjectDir = Setup_Tab2_ProjectDirLE->text();
}

void
nmfSetup_Tab2::callback_ProjectNameAdd()
{
    m_ProjectName = getProjectName();
    m_NewProject = true;
}

void
nmfSetup_Tab2::callback_ProjectAuthorAdd()
{
    m_ProjectAuthor = Setup_Tab2_ProjectAuthorLE->text();
}

void
nmfSetup_Tab2::callback_ProjectDescAdd()
{
    m_ProjectDescription = Setup_Tab2_ProjectDescLE->text();
}

void
nmfSetup_Tab2::enableProjectData()
{
    Setup_Tab2_ProjectDataGB->setChecked(true);
}

void
nmfSetup_Tab2::enableSetupTabs(bool enable)
{
    for (int i=2; i<Setup_Tabs->count(); ++i) {
        Setup_Tabs->setTabEnabled(i,enable);
    }
}

bool
nmfSetup_Tab2::loadProject(nmfLogger *logger, QString fileName)
{    
    std::string msg;

    if (fileName.isEmpty() || (fileName == ".")) {
        QMessageBox::information(Setup_Tabs,tr("No Project Loaded"),
                              tr("\nPlease create/load a Project.\n"),
                              QMessageBox::Ok);
        return false;
    }

    // If the file doesn't have an extension, add .prj
    if (QFileInfo(fileName).suffix().isEmpty()) {
        fileName += ".prj";
    }

    // Read file and load data
    QFile file(fileName);
    if (! file.open(QIODevice::ReadOnly)) {
        msg = "Error nmfSetupTab2::loadProject: Cannot open file for reading: "+fileName.toStdString();
        logger->logMsg(nmfConstants::Normal,msg);
        msg = "\nError nmfSetupTab2::loadProject: Cannot open file for reading:\n\n"+fileName.toStdString();
        QMessageBox::critical(Setup_Tabs,tr("Project Error"),
                              tr(msg.c_str()),
                              QMessageBox::Ok);
        return false;
    }
    QTextStream in(&file);
    logger->logMsg(nmfConstants::Normal,"Reading Project File: " + fileName.toStdString());

    // Skip initial comments
    QString line = in.readLine();
    while (line.trimmed()[0] == "#") {
        line = in.readLine();
    }

    m_ProjectDir         = line;
    m_ProjectAuthor      = in.readLine();
    m_ProjectDescription = in.readLine();

    Setup_Tab2_ProjectNameLE->clear();
    Setup_Tab2_ProjectDirLE->clear();
    Setup_Tab2_ProjectAuthorLE->clear();
    Setup_Tab2_ProjectDescLE->clear();

    QFileInfo filenameNoPath(fileName);
    setProjectName(filenameNoPath.baseName());
    setProjectDirectory(m_ProjectDir);
    setProjectAuthor(m_ProjectAuthor);
    setProjectDescription(m_ProjectDescription);
    m_ProjectName = filenameNoPath.baseName();

    file.close();

    saveSettings();
    emit LoadProject();
    readSettings();

    logger->logMsg(nmfConstants::Normal,"loadProject end");

    return true;
}

void
nmfSetup_Tab2::callback_BrowseProject()
{
    QString fileName = QFileDialog::getOpenFileName(Setup_Tabs,
        tr("Load a Project"), m_ProjectDir, tr("Project Files (*.prj)"));

    if (! fileName.isEmpty()) {
        loadProject(m_Logger,fileName);
        m_NewProject = true;
    }

   Setup_Tab2_BrowseProjectPB->clearFocus();
}

void
nmfSetup_Tab2::callback_NewProject()
{
    Setup_Tab2_ProjectNameLE->clear();
    Setup_Tab2_ProjectAuthorLE->clear();
    Setup_Tab2_ProjectDescLE->clear();
}

void
nmfSetup_Tab2::callback_DelProject()
{
    std::string msg;
    QString projectPrefix = getProjectName();
    std::string ProjectToDelete = projectPrefix.toStdString()+".prj";
    QString fileToDelete = QDir(m_ProjectDir).filePath(QString::fromStdString(ProjectToDelete));

    if (! isProjectNameValid(projectPrefix)) {
        return;
    }
    if (! QFileInfo(fileToDelete).exists())
    {
        msg = "Project file doesn't exist:\n\n" + fileToDelete.toStdString();
        QMessageBox::warning(Setup_Tabs,"Delete Error",
                             QString::fromStdString("\n"+msg+"\n"),
                             QMessageBox::Ok);
        return;
    }

    msg = "\nOK to delete the Project file: " + ProjectToDelete + " ?\n";
    QMessageBox::StandardButton reply = QMessageBox::question(Setup_Tabs, tr("Delete Project"),
         tr(msg.c_str()), QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        fileToDelete = QDir(m_ProjectDir).filePath(QString::fromStdString(ProjectToDelete));
        QFile file(fileToDelete);
        if (file.remove()) {
            QMessageBox::information(Setup_Tabs,
                                     tr("Remove Project"),
                                     tr("\nProject file removed successfully."),
                                     QMessageBox::Ok);
            callback_NewProject(); // Clear Project fields
        } else {
            msg = "\nCouldn't remove Project file: " + ProjectToDelete;
            QMessageBox::critical(Setup_Tabs,
                                  tr("Remove Project"),
                                  tr(msg.c_str()),
                                  QMessageBox::Ok);
        }
    }
}

void
nmfSetup_Tab2::callback_ProjectReload()
{
    readSettings();
    if (loadProject(m_Logger,QDir(m_ProjectDir).filePath(m_ProjectName+".prj"))) {
        QMessageBox::information(Setup_Tabs,"Info","\nProject reloaded.\n",QMessageBox::Ok);
    }
}

QString
nmfSetup_Tab2::getProjectName()
{
    return Setup_Tab2_ProjectNameLE->text();
}

QString
nmfSetup_Tab2::getProjectDir()
{
    return Setup_Tab2_ProjectDirLE->text();
}

QString
nmfSetup_Tab2::getProjectAuthor()
{
    return Setup_Tab2_ProjectAuthorLE->text();
}

QString
nmfSetup_Tab2::getProjectDescription()
{
    return Setup_Tab2_ProjectDescLE->text();
}

void
nmfSetup_Tab2::readSettings()
{
    // Read the settings and load into class variables.
    QSettings* settings = nmfUtilsQt::createSettings(
                nmfConstantsAvailability::SettingsDirWindows,QApplication::applicationName());

    settings->beginGroup("SetupTab");
    m_ProjectName        = settings->value("ProjectName","").toString();
    m_ProjectDir         = settings->value("ProjectDir","").toString();
    m_ProjectAuthor      = settings->value("ProjectAuthor","").toString();
    m_ProjectDescription = settings->value("ProjectDescription","").toString();
    settings->endGroup();

    // Load class variables into appropriate widgets.
    setProjectName(m_ProjectName);
    setProjectDirectory(m_ProjectDir);
    setProjectAuthor(m_ProjectAuthor);
    setProjectDescription(m_ProjectDescription);

    delete settings;
}

void
nmfSetup_Tab2::saveSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(
                nmfConstantsAvailability::SettingsDirWindows,
                QApplication::applicationName());

    settings->beginGroup("SetupTab");
    settings->setValue("ProjectName",        getProjectName());
    settings->setValue("ProjectDir",         getProjectDir());
    settings->setValue("ProjectAuthor",      getProjectAuthor());
    settings->setValue("ProjectDescription", getProjectDescription());
    settings->endGroup();

    delete settings;
}

void
nmfSetup_Tab2::setProjectName(QString name)
{
    Setup_Tab2_ProjectNameLE->setText(name);
}

void
nmfSetup_Tab2::setProjectDirectory(QString dir)
{
    Setup_Tab2_ProjectDirLE->setText(dir);
}

void
nmfSetup_Tab2::setProjectAuthor(QString author)
{
    Setup_Tab2_ProjectAuthorLE->setText(author);
}
void
nmfSetup_Tab2::setProjectDescription(QString desc)
{
    Setup_Tab2_ProjectDescLE->setText(desc);
}

void
nmfSetup_Tab2::loadWidgets()
{
    m_Logger->logMsg(nmfConstants::Normal,"nmfSetup_Tab2::loadWidgets()");

    readSettings();
}
