#include "nmfExportDialog.h"

nmfExportDialog::nmfExportDialog(
        const QString& filename,
        QStandardItemModel* smodel)
    : QDialog()
{
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle("Export Dialog");

    QVBoxLayout* mainLayt  = new QVBoxLayout();
    QVBoxLayout* textLayt  = new QVBoxLayout();
    QHBoxLayout* btnLayt   = new QHBoxLayout();
    m_textW     = new QTextEdit();
    m_cancelPB  = new QPushButton("Cancel");
    m_reloadPB  = new QPushButton("Reload");
    m_insertPB  = new QPushButton("Insert");
    m_replacePB = new QPushButton("Replace");
    m_findPB    = new QPushButton("Find");
    m_selectPB  = new QPushButton("Select");
    m_okPB      = new QPushButton("OK");
    m_startLineNumber = 0;
    m_startCharNumber = 0;
    m_endCharNumber   = 0;
    m_smodel          = smodel;
    m_filename        = filename;
    m_oldText         = "";
    m_cursor = m_textW->textCursor();

    m_replacePB->setToolTip("Replace the selected text with the calculated Availability values");
    m_cancelPB->setToolTip("Cancel the Export");
    m_insertPB->setToolTip("Insert the Availability values into the prm file");
    m_findPB->setToolTip("Scroll the prm file to the Availability values section");
    m_reloadPB->setToolTip("Reload original prm file");
    m_selectPB->setToolTip("Select prm file's Availability values");
    m_okPB->setToolTip("Save above prm file");
    m_cancelPB->setWhatsThis(
         "<strong><center>Cancel</center></strong>"\
         "<p>This cancels the Export with no files written out.</p>");

    btnLayt->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Expanding));
    btnLayt->addWidget(m_cancelPB);
    btnLayt->addWidget(m_reloadPB);
    btnLayt->addWidget(m_insertPB);
    btnLayt->addWidget(m_replacePB);
    btnLayt->addWidget(m_findPB);
    btnLayt->addWidget(m_selectPB);
    btnLayt->addWidget(m_okPB);
    btnLayt->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Expanding));

    textLayt->addWidget(m_textW);
    mainLayt->addLayout(textLayt,100); // 100% stretch
    mainLayt->addLayout(btnLayt,   0); //   0% stretch
    setLayout(mainLayt);

    resize(1000,800); // resize the dialog as the file is pretty wide and long

    callback_reload();
    callback_find();
    callback_select();
    setupConnections();
}

QString
nmfExportDialog::readPrmFile(const QString& filename)
{
    int charNumber = 0;
    QString retv = "";
    QFile file(filename);

    m_startCharNumber = 0;
    m_endCharNumber   = 0;

    if (file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QTextStream stream(&file);
        QString line = stream.readLine().simplified();
        while (! line.isNull()) {
            charNumber += line.size()+1;
            line = line.toHtmlEscaped();
            if (line.contains(nmfConstantsAvailability::BeginSection_pPrey)) {
                line = "<a name=\"pPrey\" style=\"text-decoration:none;color:inherit;\" href=\"#"+line+"\">" + line + "</a>";
                m_startCharNumber = charNumber;
            } else if (line.contains(nmfConstantsAvailability::BeginSection_pPrey)) {
                m_endCharNumber = charNumber - (line.size()+1);
            }
            retv += line + "<br>";
            line = stream.readLine().simplified();
        }
    }

    return retv;
}

void
nmfExportDialog::setupConnections()
{
    connect(m_cancelPB,  SIGNAL(clicked()),
            this,        SLOT(callback_cancel()));
    connect(m_findPB,    SIGNAL(clicked()),
            this,        SLOT(callback_find()));
    connect(m_selectPB,  SIGNAL(clicked()),
            this,        SLOT(callback_select()));
    connect(m_insertPB,  SIGNAL(clicked()),
            this,        SLOT(callback_insert()));
    connect(m_reloadPB,  SIGNAL(clicked()),
            this,        SLOT(callback_reload()));
    connect(m_replacePB, SIGNAL(clicked()),
            this,        SLOT(callback_replace()));
    connect(m_okPB,      SIGNAL(clicked()),
            this,        SLOT(callback_ok()));
}

void
nmfExportDialog::selectExistingAvailabilityLines()
{
    m_cursor.setPosition(m_startCharNumber);
    m_cursor.setPosition(m_endCharNumber, QTextCursor::KeepAnchor);
    m_textW->setTextCursor(m_cursor);
}

void
nmfExportDialog::callback_ok()
{
    QString msg;
    QString fullFilename;

    msg = "\nOK to replace file:\n\n" + m_filename + " ?\n\nThis cannot be undone.\n";
    QMessageBox::StandardButton reply = QMessageBox::question(
                this,"Replace .prm File",msg,
                QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        writeParameterFile(m_filename);
    } else {
        bool ok;
        QString fullNewFilename;
        QFileInfo filepath(m_filename);
        QString absFilePath = filepath.path();
        QString newFilename = QInputDialog::getText(this, tr("New .prm File"),
                                                    tr("Enter new .prm file name:"),
                                                    QLineEdit::Normal,
                                                    "", &ok);
        if (ok && !newFilename.isEmpty()) {
            fullNewFilename = QDir(absFilePath).filePath(newFilename);
            QFileInfo checkFile(fullNewFilename);
            if (checkFile.exists() && checkFile.isFile()) {
                msg  = "Warning:\n\nThe file:\n\n" + fullNewFilename + "\n\nalready exists.\n\n";
                msg += "OK to overwrite?\n";
                QMessageBox::StandardButton reply = QMessageBox::warning(
                            this,"File Exists",msg,
                            QMessageBox::Yes|QMessageBox::No);
                if (reply == QMessageBox::Yes) {
                    writeParameterFile(fullNewFilename);
                }
            } else {
                writeParameterFile(fullNewFilename);
            }
        }
    }
}

void
nmfExportDialog::writeParameterFile(QString& filename)
{
    QString content = m_textW->document()->toPlainText();
    QFileInfo fi(filename);

    if (fi.suffix() != "prm") {
        filename += ".prm";
    }

    QFile file(filename);
    if (file.open(QIODevice::ReadWrite))
    {
        QTextStream stream( &file );
        stream << content+"\n";
    }
    file.close();

    QString msg = "\nSaved file:\n\n" + filename + "\n";
    QMessageBox::information(this,"Save",msg);
}

void
nmfExportDialog::callback_select()
{
    selectExistingAvailabilityLines();
    callback_find();
}

void
nmfExportDialog::callback_reload()
{
    this->setCursor(Qt::WaitCursor);
    m_textW->clear();
    m_textW->setHtml(readPrmFile(m_filename));
    callback_find();
    this->setCursor(Qt::ArrowCursor);
}

void
nmfExportDialog::callback_cancel()
{
    close();
}

QString
nmfExportDialog::getNewText()
{
    int numPrey = m_smodel->columnCount()-1;
    QString newText = "";
    QString valueStr;

    for (int row=0; row<m_smodel->rowCount(); ++row) {
        newText += m_smodel->item(row,0)->text() + " " + QString::number(numPrey) + "<br>";
        for (int col=1; col<m_smodel->columnCount(); ++col) {
            valueStr =  m_smodel->item(row,col)->text();
            valueStr = (valueStr.isEmpty()) ? "0 " : valueStr+" ";
            newText += valueStr;
        }
        newText += "<br><br>";
    }
    return newText;
}

void
nmfExportDialog::callback_replace()
{
    this->setCursor(Qt::WaitCursor);
    m_cursor.removeSelectedText();
    m_textW->insertHtml("<br>"+getNewText());
    callback_find();
    this->setCursor(Qt::ArrowCursor);

    // Necessary since after a replace or insert, the end marker is no longer valid
    m_endCharNumber = m_startCharNumber;
}

void
nmfExportDialog::callback_insert()
{
    this->setCursor(Qt::WaitCursor);
    m_textW->insertHtml(getNewText());
    this->setCursor(Qt::ArrowCursor);

    // Necessary since after a replace or insert, the end marker is no longer valid
    m_endCharNumber = m_startCharNumber;
}

void
nmfExportDialog::callback_find()
{
    m_textW->scrollToAnchor("pPrey");
}
