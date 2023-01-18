#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qrencode.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_lineEdit_Pass_1_textChanged(const QString &arg1);
    void on_lineEdit_Pass_2_textChanged(const QString &arg1);

    void on_toolButton_clear_encoding_clicked();

    void on_toolButton_clear_decoding_clicked();

    void on_pushButton_Decode_clicked();

    void on_actionCopy_QR_to_Clipboard_triggered();

    void on_actionPrint_triggered();

    void on_plainTextEdit_PlainTextToEncode_textChanged();

    void on_lineEdit_description_textChanged(const QString &arg1);

    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_comboBox_textSize_currentIndexChanged(int index);

    void on_comboBox_errorCorrectionLevel_currentIndexChanged(int index);

    void on_actionAbout_triggered();

    void on_pushButton_encrypt_clicked();

    void on_spinBox_iterations_valueChanged(int arg1);


    void on_checkBox_visiblePass_clicked();

private:

    QRecLevel getErrorCorrectionLevel();
    void encode();
    void clearResults();
    void setQRCode(const QString &text);
    bool isPassPhraseOK();
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
