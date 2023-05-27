
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

    arduino = new QSerialPort(this);
    serialBuffer = "";

    /*
     *  Testing code, prints the description, vendor id, and product id of all ports.
     *  Used it to determine the values for the arduino uno.
     *
     *
    qDebug() << "Number of ports: " << QSerialPortInfo::availablePorts().length() << "\n";
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        qDebug() << "Description: " << serialPortInfo.description() << "\n";
        qDebug() << "Has vendor id?: " << serialPortInfo.hasVendorIdentifier() << "\n";
        qDebug() << "Vendor ID: " << serialPortInfo.vendorIdentifier() << "\n";
        qDebug() << "Has product id?: " << serialPortInfo.hasProductIdentifier() << "\n";
        qDebug() << "Product ID: " << serialPortInfo.productIdentifier() << "\n";
    }
    */
    bool arduino_is_available = false;
    QString arduino_uno_port_name;
    //
    //  For each available serial port
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        //  check if the serialport has both a product identifier and a vendor identifier
        if(serialPortInfo.hasProductIdentifier() && serialPortInfo.hasVendorIdentifier()){
            //  check if the product ID and the vendor ID match those of the arduino uno
            if((serialPortInfo.productIdentifier() == arduino_uno_product_id)
                && (serialPortInfo.vendorIdentifier() == arduino_uno_vendor_id)){
                arduino_is_available = true; //    arduino uno is available on this port
                arduino_uno_port_name = serialPortInfo.portName();
            }
        }
    }

    if(arduino_is_available){
        qDebug() << "Found the arduino port...\n";
        arduino->setPortName(arduino_uno_port_name);
        arduino->open(QSerialPort::ReadOnly);
        arduino->setBaudRate(QSerialPort::Baud115200);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        QObject::connect(arduino, SIGNAL(readyRead()), this, SLOT(readSerial()));

    }else{
        qDebug() << "Couldn't find the correct port for the arduino.\n";
        ui->statusbar->showMessage("Couldn't open serial port to arduino.");
    }
    if (arduino->isOpen())
    {
        ui->statusbar->showMessage("Connection established.",5000);
    }
    else
    {
        ui->statusbar->showMessage("Failed to establish connection.",5000);
    }
}


dance::~dance()
{
    if(arduino->isOpen()){
        arduino->close(); //    Close the serial port if it's open.
    }
    delete ui;
    delete soundEffect;
    delete player;
    delete audioOutput;

}

void dance::readSerial(){
    /*serialData = arduino->readAll();
    serialBuffer += QString::fromStdString(serialData.toStdString());

    // Wyświetlenie zawartości zmiennej temp w konsoli debugowania
    qDebug() << serialBuffer;
    */
    QStringList buffer_split = serialBuffer.split(" ");
    if(buffer_split.length() < 3){
        // no parsed value yet so continue accumulating bytes from serial in the buffer.
        serialData = arduino->readAll();
        serialBuffer += QString::fromStdString(serialData.toStdString());
        serialData.clear();
    }else{
        // the second element of buffer_split is parsed correctly, update the temperature value on temp_lcdNumber
        serialBuffer = "";
        qDebug() << buffer_split;
        parsed_data = buffer_split[1];
        podzialDanych();

    }
}

void dance::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        // zwolnienie poprzedniego obiektu QMovie
        if (movie) {
            movie->deleteLater();
        }
        movie = new QMovie(":/rec/animations/Oruch.gif");
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

    movie = new QMovie(":/rec/animations/Basic.gif");
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
void dance::podzialDanych()
{
    QStringList values = parsed_data.split(","); // Podział łańcucha na trzy części

    if (values.length() == 3) {
        bool ok;
        float x = values[0].toFloat(&ok); // Konwersja pierwszej części na float
        ui->label_danex->setText(values[0]);
        ui->label_daney->setText(values[1]);
        ui->label_danez->setText(values[2]);

        if (ok) {
            float y = values[1].toFloat(&ok); // Konwersja drugiej części na float
            if (ok) {
                float z = values[2].toFloat(&ok); // Konwersja trzeciej części na float

                // Warunki zmiany obrazka w zależności od wartości x, y, i z
                if(!isAnimationRunning){
                    if (x > 8.0f && y < 3.0f && y > -3.0f) {
                        changeGif(":/rec/animations/Oruch.gif");
                    }   else if (x < -8.0f && y < 3.0f && y > -3.0f) {
                        changeGif(":/rec/animations/obrot.gif");
                    }   else if (y > 8.0f && x < 3.0f && y > -3.0f) {
                        changeGif(":/rec/animations/podskok.gif");
                    }   else if (y < -8.0f && x < 3.0f && x > -3.0f) {
                        changeGif(":/rec/animations/shuffle.gif");
                    }   else if (y < -5.0f && x > 5.0f) {
                        changeGif(":/rec/animations/fala.gif");
                    }

                }
            }
        }
    }
}

void dance::changeGif(const QString& filePath)
{
    isAnimationRunning = true;
    if (movie) {
        movie->deleteLater();
    }

    movie = new QMovie(filePath);
    ui->labelgif->setScaledContents(true);
    int w = ui->labelgif->width();
    int h = ui->labelgif->height();
    int movieWidth = movie->scaledSize().width();
    int movieHeight = movie->scaledSize().height();
    ui->labelgif->setMovie(movie);

    connect(movie, &QMovie::finished, this, &dance::movieFinished);

    movie->start();

    if (movieWidth > w || movieHeight > h) {
        double widthRatio = (double)w / movieWidth;
        double heightRatio = (double)h / movieHeight;
        double scaleFactor = qMin(widthRatio, heightRatio);
        QSize scaledSize(scaleFactor * movieWidth, scaleFactor * movieHeight);
        movie->setScaledSize(scaledSize);
    }
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

void dance::on_pushButton_clicked()
{
    changeGif(":/rec/animations/Oruch.gif");
}

