#ifndef DANCE_H
#define DANCE_H

#include <QMainWindow>
#include <QMainWindow>
#include <QMovie>
#include <QLabel>
#include <QKeyEvent>
#include <QFrame>
#include <QTimer>
#include <QSoundEffect>
#include <QAudioOutput>
#include <QtMultimedia>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class dance; }
QT_END_NAMESPACE

class dance : public QMainWindow
{
    Q_OBJECT
public slots:
    void Odliczanie();
public:

    dance(QWidget *parent = nullptr);
    ~dance();

protected:
    QSerialPort* arduino;
    QTimer* timer;
    QTimer* timer1;
    QMediaPlayer* player;
    QSoundEffect* soundEffect;
    QAudioOutput* audioOutput;
    void standardGif();
    void keyPressEvent(QKeyEvent *event) override;
    void movieFinished();
    void PlayMusic1();
    void timerTimeout1();
    void podskok(int czas);
    void obrot(int czas);
    //void StartFreestyle();
    void changeGif(const QString& filePath);



signals:
    void resized(QResizeEvent *event);

private slots:
    void readSerial();
    void on_action_Freestyle_triggered();

    void on_Slider_Volume_valueChanged(int value);

    void on_pushButton_clicked();

private:
    Ui::dance *ui;
    QMovie *movie;
    int counter;
    bool isAnimationRunning;
    static const quint16 arduino_uno_vendor_id = 9025;
    static const quint16 arduino_uno_product_id = 67;
    QByteArray serialData;
    QString serialBuffer;
    QString parsed_data;
    void podzialDanych();


};

#endif // DANCE_H
