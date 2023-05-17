
#include "dance.h"
#include "ui_dance.h"

dance::dance(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::dance)
{
    ui->setupUi(this);
    standardGif();
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    isAnimationRunning = false;
    audioOutput->setVolume(ui->Slider_Volume->value());

}


dance::~dance()
{
    delete ui;
    delete soundEffect;
    delete player;
    delete audioOutput;

}
void dance::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        // zwolnienie poprzedniego obiektu QMovie
        if (movie) {
            movie->deleteLater();
        }
        movie = new QMovie("C:/Users/wajci/Downloads/opmg.gif");
        ui->labelgif->setScaledContents(true);
        int w = ui->labelgif->width();
        int h = ui->labelgif->height();
        int movieWidth = movie->scaledSize().width();
        int movieHeight = movie->scaledSize().height();
        ui->labelgif->setMovie(movie);

        connect(movie, &QMovie::finished, this, &dance::movieFinished);

        movie->start();

        if (movieWidth > w || movieHeight > h) {
            double widthRatio = (double) w / movieWidth;
            double heightRatio = (double) h / movieHeight;
            double scaleFactor = qMin(widthRatio, heightRatio);
            QSize scaledSize(scaleFactor * movieWidth, scaleFactor * movieHeight);
            movie->setScaledSize(scaledSize);
        }
    }
}

void dance::standardGif(){

    movie = new QMovie("C:/Users/wajci/Downloads/opmg1.gif");
    ui->labelgif->setScaledContents(true);
    ui->labelgif->setMovie( movie);
    int w = ui->labelgif->width();
    int h = ui->labelgif->height();
    int movieWidth = movie->scaledSize().width();
    int movieHeight = movie->scaledSize().height();

    if (movieWidth > w || movieHeight > h) {
        double widthRatio = (double) w / movieWidth;
        double heightRatio = (double) h / movieHeight;
        double scaleFactor = qMin(widthRatio, heightRatio);
        QSize scaledSize(scaleFactor * movieWidth, scaleFactor * movieHeight);
        movie->setScaledSize(scaledSize);
    }
    movie->start();
}

void dance::movieFinished() {
    // Resetowanie flagi isAnimationRunning
    isAnimationRunning = false;
    if (movie) {
        movie->deleteLater();
    }
    standardGif();
}

void dance::Odliczanie()
{

    int countdown = 3;

    // Tworzenie QTimer
    timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, [this, &countdown]() {
        if (countdown > 0) {

            ui->label_ruch->setText(QString::number(countdown));
            countdown--;
        } else {
            ui->label_ruch->setText("Start!");
            timer->stop();
            timer->deleteLater();
            PlayMusic1();
        }
    });
    // Ustawienie interwału timera na 1 sekundę (1000 ms)
    timer->start(1000);
}




void dance::on_action_Freestyle_triggered()
{
    Odliczanie();

}

void dance::PlayMusic1()
{
    QString musicFilePath = "C:/Users/wajci/Downloads/cantina.mp3"; // Plik audio powinien być w formacie WAV
    player->setSource(QUrl::fromLocalFile(musicFilePath));
    player->setAudioOutput(audioOutput);
    player->play();

    timer1 = new QTimer(this);
    connect(timer1, &QTimer::timeout, this, &dance::timerTimeout1);
    timer1->setSingleShot(true);
    timer1->start(1000);
    // Resetowanie wartości counter do 0 przy każdym wywołaniu PlayMusic1()
    counter = 0;
}

void dance::timerTimeout1()
{
    counter++;

    if (counter == 1) {
        podskok(5300);
    } else if (counter == 2) {
        obrot(9000);
    } else {
        ui->label_ruch->setText("Ruch");
        // Inne operacje po zakończeniu zmiany napisów
    }
}

void dance::on_Slider_Volume_valueChanged(int value)
{
    qreal qrealVolume = value / 100.0;
    qreal linearVolume = QAudio::convertVolume(qrealVolume, QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
    audioOutput->setVolume(linearVolume);
}

void dance::podskok(int czas)
{
        ui->label_ruch->setText("podskok");
        QTimer::singleShot(1500, this, [this, czas]() { // Dodano "czas" do listy przechwytującej lambdę
            ui->label_ruch->setText("Ruch");
            QTimer::singleShot(czas, this, &dance::timerTimeout1);
        });
}
void dance::obrot(int czas)
{
        ui->label_ruch->setText("obrot");
        QTimer::singleShot(1500, this, [this, czas]() { // Dodano "czas" do listy przechwytującej lambdę
            ui->label_ruch->setText("Ruch");
            QTimer::singleShot(czas, this, &dance::timerTimeout1);
        });
}
