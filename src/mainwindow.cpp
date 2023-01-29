#include "mainwindow.h"
#include "qmessagebox.h"
#include "qnamespace.h"
#include "qpixmap.h"
#include "ui_mainwindow.h"
#include "crypto.h"
#include "dialog_about.h"

#include <QMessageBox>
#include <QClipboard>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QPainter>

#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#include <QtPrintSupport/QPrinter>

#include <dialog_scale.h>
#include <qrencode.h>
#include <chrono>


// function to check password strength
int checkPasswordStrength(const QString &password)
{
    int score = 0;
    // check for length
    if (password.length() >= 14)
    {
        score++;
    }
    // check for uppercase letters
    if (password.contains(QRegularExpression("[A-Z]")))
    {
        score++;
    }
    // check for lowercase letters
    if (password.contains(QRegularExpression("[a-z]")))
    {
        score++;
    }
    // check for numbers
    if (password.contains(QRegularExpression("[0-9]")))
    {
        score++;
    }
    // check for special characters
    if (password.contains(QRegularExpression("[^A-Za-z0-9]")))
    {
        score++;
    }
    return score;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    on_lineEdit_Pass_1_textChanged("");

    clearResults();
    on_spinBox_iterations_valueChanged(0);

    ui->label_strength->setStyleSheet("background-color: white;");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setQRCode(const QString &text)
{
    QRcode *qr = QRcode_encodeString(text.toUtf8().constData(), 0, getErrorCorrectionLevel(), QR_MODE_8, 1);
    if (!qr)
    {
        QMessageBox::critical(this,"QR Generation", "Error ocurred during QR Generation (too large?).");
        return;
    }

    int qrWidth = qMin(ui->label_qr->width(), ui->label_qr->height());


    QImage qrImage = QImage(qr->width + 8, qr->width + 8, QImage::Format_RGB32);
    qrImage.fill(0xffffff);
    unsigned char *p = qr->data;
    for (int y = 0; y < qr->width; y++) {
        for (int x = 0; x < qr->width; x++) {
            qrImage.setPixel(x + 4, y + 4, ((*p & 1) ? 0x0 : 0xffffff));
            p++;
        }
    }
    QRcode_free(qr);

    auto px = QPixmap::fromImage(qrImage.scaled(qrWidth,qrWidth));

    QPainter painter(&px);


    // Draw Description Text:
    QFont font = QFont( "Monospace", ui->comboBox_textSize->currentText().toUInt(), QFont::DemiBold );
    font.setPointSize(font.pointSize());
    QFontMetrics metrics(font);
    painter.setPen(Qt::black);
    painter.setFont(font);
    painter.drawText(  0,  0, px.size().width(), metrics.height(), Qt::AlignCenter , ui->lineEdit_description->text());

    ui->label_qr->setPixmap(px);
}

void MainWindow::encode()
{
    if (isPassPhraseOK())
    {
        bool ok;
        auto input = ui->plainTextEdit_PlainTextToEncode->toPlainText().toStdString();
        auto key = ui->lineEdit_Pass_1->text().toStdString();
        auto out = Mantids::Helpers::Crypto::AES256EncryptB64(input, ui->spinBox_iterations->value(),  key, &ok );
        if (!ok)
        {
            QMessageBox::critical(this,"Encryption", "Failed to encrypt, unknown error");
            return;
        }
        ui->plainTextEdit_Encoded->setPlainText(QString().fromStdString(out));
        ui->plainTextEdit_Encoded->selectAll();
        setQRCode(QString().fromStdString(out));
    }
    else
    {
        QMessageBox::critical(this,"Encryption", "Can't encrypt\nPassphrase does not match or is empty.");
    }
}

void MainWindow::clearResults()
{
    ui->plainTextEdit_Encoded->clear();
    ui->label_qr->setPixmap(QPixmap());
}


void MainWindow::on_lineEdit_Pass_1_textChanged(const QString &arg1)
{
    clearResults();

    if (isPassPhraseOK())
    {
        ui->lineEdit_Pass_1->setStyleSheet("background-color: rgb(128, 255, 128);");
        ui->lineEdit_Pass_2->setStyleSheet("background-color: rgb(128, 255, 128);");

        ui->comboBox_textSize->setDisabled(false);
        ui->lineEdit_description->setDisabled(false);
        ui->plainTextEdit_PlainTextToEncode->setDisabled(false);
        ui->toolButton_loadFromFile->setDisabled(false);


        int score = checkPasswordStrength(ui->lineEdit_Pass_1->text());
           // display widget with color depending on password strength
           if (score <= 2) {
               ui->label_strength->setStyleSheet("background-color: red;");
           } else if (score == 3) {
               ui->label_strength->setStyleSheet("background-color: orange;");
           } else {
               ui->label_strength->setStyleSheet("background-color: green;");
           }
    }
    else
    {
        ui->lineEdit_Pass_1->setStyleSheet("background-color: rgb(255, 64, 64);");
        ui->lineEdit_Pass_2->setStyleSheet("background-color: rgb(255, 64, 64);");

        ui->comboBox_textSize->setDisabled(true);
        ui->lineEdit_description->setDisabled(true);
        ui->plainTextEdit_PlainTextToEncode->setDisabled(true);
        ui->toolButton_loadFromFile->setDisabled(true);

        ui->label_strength->setStyleSheet("background-color: white;");
    }
}


void MainWindow::on_lineEdit_Pass_2_textChanged(const QString &arg1)
{
    on_lineEdit_Pass_1_textChanged("");
}

bool MainWindow::isPassPhraseOK()
{
    return ui->lineEdit_Pass_1->text() == ui->lineEdit_Pass_2->text() && !ui->lineEdit_Pass_1->text().isEmpty();
}

void MainWindow::on_toolButton_clear_encoding_clicked()
{
    clearResults();
    ui->plainTextEdit_PlainTextToEncode->setFocus();
}

void MainWindow::on_toolButton_clear_decoding_clicked()
{
    ui->plainTextEdit_TextToDecode->setReadOnly(false);
    ui->plainTextEdit_TextToDecode->clear();
    ui->pushButton_Decode->setEnabled(true);
    ui->plainTextEdit_TextToDecode->setFocus();
}


void MainWindow::on_pushButton_Decode_clicked()
{
    if (isPassPhraseOK())
    {
        bool ok;
        auto input = ui->plainTextEdit_TextToDecode->toPlainText().toStdString();
        auto key = ui->lineEdit_Pass_1->text().toStdString();
        auto out = Mantids::Helpers::Crypto::AES256DecryptB64(input,ui->spinBox_iterations->value(), key, &ok );
        if (!ok)
        {
            QMessageBox::critical(this,"Decryption", "Failed to decrypt, check the password.");
            return;
        }

        ui->plainTextEdit_TextToDecode->setPlainText(QString().fromStdString(out));
        ui->plainTextEdit_TextToDecode->setReadOnly(true);
        ui->plainTextEdit_TextToDecode->selectAll();
        ui->plainTextEdit_TextToDecode->setFocus();
        ui->pushButton_Decode->setEnabled(false);
    }
    else
    {
        QMessageBox::critical(this,"Decryption", "Can't decrypt\nPassphrase does not match or is empty.");
    }
}


void MainWindow::on_actionCopy_QR_to_Clipboard_triggered()
{
    QClipboard *clipboard = QApplication::clipboard();
    QImage image = ui->label_qr->pixmap().toImage();
    clipboard->setImage(image);
    QMessageBox::information(this,"Clipboard", "The current QR code was copied to the clipboard.");

}


void MainWindow::on_actionPrint_triggered()
{
    QPrinter printer;

    Dialog_Scale scale;
    if (scale.exec() != QDialog::Accepted)
    {
        return;
    }
    auto factor = scale.divFactor();

    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, &QPrintPreviewDialog::paintRequested, [=](QPrinter *printer){
        QPainter painter;
        if (!painter.begin(printer)) {
            // error al iniciar la vista previa
        } else {
            QImage image = ui->label_qr->pixmap().toImage();
            // Viewport rectangle...
            QRect rect = painter.viewport();
            // Image Size.
            QSize imageSize = image.size();

            // Scale the image size to the viewport size.
            //imageSize.scale(rect.size(), Qt::KeepAspectRatio);
            imageSize.scale( rect.size().width()/factor ,rect.size().height()/factor, Qt::KeepAspectRatio );
            // Set the viewport using the initial painter viewport... and the current image size.
            painter.setViewport(rect.x(), rect.y(), imageSize.width(), imageSize.height());
            // Set the window as the rect image...
            painter.setWindow(image.rect());
            // Draw the image...
            painter.drawImage(0, 0, image);
            painter.end();
        }
    });
    if(preview.exec() == QDialog::Accepted)
    {
        QMessageBox::information(this, tr("Printing"), tr("Printed."));
    }
}


void MainWindow::on_plainTextEdit_PlainTextToEncode_textChanged()
{
    clearResults();
}


void MainWindow::on_lineEdit_description_textChanged(const QString &arg1)
{
    clearResults();
}


void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    clearResults();
}


void MainWindow::on_comboBox_textSize_currentIndexChanged(int index)
{
    clearResults();

}

QRecLevel MainWindow::getErrorCorrectionLevel()
{
    if (ui->comboBox_errorCorrectionLevel->currentText() == "HIGH")
        return QR_ECLEVEL_H;
    else if (ui->comboBox_errorCorrectionLevel->currentText() == "QUARTILE")
        return QR_ECLEVEL_Q;
    else if (ui->comboBox_errorCorrectionLevel->currentText() == "MEDIUM")
        return QR_ECLEVEL_M;
    else
        return QR_ECLEVEL_L;
}

void MainWindow::on_comboBox_errorCorrectionLevel_currentIndexChanged(int )
{
    clearResults();
}


void MainWindow::on_actionAbout_triggered()
{
    Dialog_About about;
    about.exec();
}

void MainWindow::on_pushButton_encrypt_clicked()
{
    encode();
}


void MainWindow::on_spinBox_iterations_valueChanged(int arg1)
{
    clearResults();
    ui->label_iterations->setText(QString("%1").arg(ui->spinBox_iterations->value()));

    bool ok;
    auto start = std::chrono::high_resolution_clock::now();
    auto out = Mantids::Helpers::Crypto::AES256EncryptB64("ANYTHING TESTING", 100000,  "12345678", &ok );
    // stop the timer
    auto stop = std::chrono::high_resolution_clock::now();
    // calculate the execution time
    std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    auto times = ui->spinBox_iterations->value()/100000;

    ui->label_iterations->setText(QString("~%1 ms").arg(duration.count()*times));

   // qDebug() << "Execution time: " << (float)duration.count()*times << " ms" ;


}


void MainWindow::on_checkBox_visiblePass_clicked()
{
    if (ui->checkBox_visiblePass->isChecked())
    {
        ui->lineEdit_Pass_1->setEchoMode(QLineEdit::EchoMode::Normal);
    }
    else
    {
        ui->lineEdit_Pass_1->setEchoMode(QLineEdit::EchoMode::Password);
    }

}


void MainWindow::on_toolButton_loadFromFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt)"));

    if (fileName.isEmpty())
        return;

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,"Open File", "Error opening file.");
        return;
    }

    QTextStream in(&file);

    ui->plainTextEdit_PlainTextToEncode->setPlainText(in.readAll());

    file.close();
}

