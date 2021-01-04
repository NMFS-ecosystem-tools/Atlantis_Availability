/**
 * @file nmfExportDialog.h
 * @brief GUI definition for the Availability Data Export Dialog
 *
 * This file contains the GUI definitions for an Export dialog which allows
 * the user to place the newly generated Availability data into an Atlantis
 * biological .prm file.
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
 *
 */

#pragma once

#include <iostream>

#include "nmfConstantsAvailability.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QObject>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTextBlock>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>

/**
 * @brief This class defines the Availability data Export dialog. It allows the user to view the current
 * biological .prm file and replace its pPrey values with the newly generated Availability (pPrey) values.
 */
class nmfExportDialog: public QDialog
{
    Q_OBJECT

private:
    int m_startLineNumber;
    int m_startCharNumber;
    int m_endCharNumber;
    QTextEdit*   m_textW;
    QPushButton* m_replacePB;
    QPushButton* m_insertPB;
    QPushButton* m_cancelPB;
    QPushButton* m_okPB;
    QPushButton* m_findPB;
    QPushButton* m_reloadPB;
    QPushButton* m_selectPB;
    QString      m_filename;
    QString      m_oldText;
    QTextCursor  m_cursor;
    QStandardItemModel* m_smodel;

    QString getNewText();
    QString readPrmFile(const QString& filename);
    void    selectExistingAvailabilityLines();
    void    setupConnections();
    void    writeParameterFile(QString& filename);

public:
    /**
     * @brief Class constructor for the Availablity data Export dialog
     * @param filename : Name of the Atlantis biological .prm file
     * @param smodel : Data model representing the Availability table data
     */
    explicit nmfExportDialog(
            const QString&      filename,
            QStandardItemModel* smodel);
    virtual ~nmfExportDialog() {}

public slots:
    /**
     * @brief Callback invoked when the user presses the Cancel button. The dialog is closed with no changes made.
     */
    void callback_cancel();
    /**
     * @brief Callback invoked when the user presses the Find button. The text edit window is scrolled
     * to the beginning of the pPrey section.
     */
    void callback_find();
    /**
     * @brief Callback invoked when the user presses the Insert button. The newly generated availability
     * values are inserted into the displayed .prm file at the current cursor position.
     */
    void callback_insert();
    /**
     * @brief Callback invoked when the user presses the Ok button. The displayed .prm file will be saved
     * over the existing file, saving any new modifications.
     */
    void callback_ok();
    /**
     * @brief Callback invoked when the user presses the Reload button. The displayed .prm file will be
     * replaced by the .prm file currently on disk. Any unsaved modifications will be lost.
     */
    void callback_reload();
    /**
     * @brief Callback invoked when the user presses the Replace button. The currently selected text will
     * be replaced by the newly calculated availability values.
     */
    void callback_replace();
    /**
     * @brief Callback invoked when the user presses the Select button. The original pPrey data will be
     * selected (i.e. highlighted) and the text scrolled so that the beginning of the selection is at the
     * top of the window.
     */
    void callback_select();
};

