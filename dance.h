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
    QAudioOutput* audioOutput;
    QString musicFilePath;
    void standardGif();
    void startGif();
    //void keyPressEvent(QKeyEvent *event) override;
    void movieFinished();
    void PlayMusic1(QString musicFilePath);
    //void wczytajRuch();
    void timerTimeout1();
    void timerTimeout2();
    void podskok(int czas);
    void obrot(int czas);
    void fala(int czas);
    void rondo(int czas);
    void shuffle(int czas);
    //void StartFreestyle();
    void changeGif(const QString& filePath);




signals:
    void resized(QResizeEvent *event);

private slots:
    void readSerial();

    void on_Slider_Volume_valueChanged(int value);

    void on_cantina_Freesty_triggered();
    void on_akcent_freestyle_triggered();

private:

    Ui::dance *ui;
    QMovie *movie;
    int wruch;
    int hruch;
    int counter;
    int numCalls;
    QVector<int> times;
    QVector<int> functions;
    bool isAnimationRunning;
    bool isGameRunning;
    static const quint16 arduino_uno_vendor_id = 9025;
    static const quint16 arduino_uno_product_id = 67;
    QByteArray serialData;
    QString serialBuffer;
    QString parsed_data;
    void podzialDanych();
    void moves(float x, float y,float z);



};

#endif // DANCE_H

