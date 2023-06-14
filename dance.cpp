/**
 * @file dance.cpp
 * @brief Implementation of the dance class
 */
#include "dance.h"
#include "ui_dance.h"
/**
 * @brief Constructs a dance object.
 * @param parent Pointer to the parent QWidget.
 */
dance::dance(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::dance)
{
    ui->setupUi(this);
    isGameRunning= false;
    startGif();
    ui->label_ruch->setScaledContents(true);
    QPixmap ruch(":/rec/moves/dance.jpg");
    int wruch = ui->label_ruch->width();
    int hruch = ui->label_ruch->height();
    ui->label_ruch->setPixmap(ruch.scaled(wruch,hruch,Qt::KeepAspectRatio));
    ui->label_danex->setText("m/s²");
    ui->label_daney->setText("m/s²");
    ui->label_danez->setText("m/s²");
    ui->lineEdit_X->setReadOnly(true);
    ui->lineEdit_Y->setReadOnly(true);
    ui->lineEdit_Z->setReadOnly(true);
    ui->lineEdit_X->setAlignment(Qt::AlignRight);
    ui->lineEdit_Y->setAlignment(Qt::AlignRight);
    ui->lineEdit_Z->setAlignment(Qt::AlignRight);


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
    dance::makePlot();
}

/**
 * @brief Destroys the dance object.
 */
dance::~dance()
{
    if(arduino->isOpen()){
        arduino->close(); //    Close the serial port if it's open.
    }
    delete ui;
    delete player;
    delete audioOutput;

}
/**
 * @brief Reads data from the serial port.
 */
void dance::readSerial(){

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

//void dance::keyPressEvent(QKeyEvent *event)
//{
//    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
//        // zwolnienie poprzedniego obiektu QMovie
//        if (movie) {
//            movie->deleteLater();
//        }
//        movie = new QMovie(":/rec/animations/Oruch.gif");
//        ui->labelgif->setScaledContents(true);
//        int w = ui->labelgif->width();
//        int h = ui->labelgif->height();
//        int movieWidth = movie->scaledSize().width();
//        int movieHeight = movie->scaledSize().height();
//        ui->labelgif->setMovie(movie);

//        connect(movie, &QMovie::finished, this, &dance::movieFinished);

//        movie->start();

//        if (movieWidth > w || movieHeight > h) {
//            double widthRatio = (double) w / movieWidth;
//            double heightRatio = (double) h / movieHeight;
//            double scaleFactor = qMin(widthRatio, heightRatio);
//            QSize scaledSize(scaleFactor * movieWidth, scaleFactor * movieHeight);
//            movie->setScaledSize(scaledSize);
//        }
//    }
//}
/**
 * @brief Starts the GIF animation.
 */
void dance::startGif(){

    movie = new QMovie(":/rec/animations/start1.gif");
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
/**
 * @brief Changes the GIF animation to the standard one.
 */
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
/**
 * @brief Handles the movie finished signal.
 */
void dance::movieFinished() {
    // Resetowanie flagi isAnimationRunning
    isAnimationRunning = false;
    if (movie) {
        movie->deleteLater();
    }
    standardGif();
}
/**
 * @brief Performs countdown and starts the music.
 */
void dance::Odliczanie()
{
    int countdown = 5;

    timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, [countdown, this]() mutable {
        if (countdown > 0) {
            QString str = QString::number(countdown);
            ui->label_ruch->setText(str);
            countdown--;
        } else {
            QPixmap ruch(":/rec/moves/dance.jpg");

            int wruch = ui->label_ruch->width();
            int hruch = ui->label_ruch->height();
            ui->label_ruch->setPixmap(ruch.scaled(wruch,hruch,Qt::KeepAspectRatio));
            timer->stop();
            timer->deleteLater();
            PlayMusic1(musicFilePath);
        }
    });

    timer->start(1000);
}
/**
 * @brief Parses the received data and updates the UI.
 */
void dance::podzialDanych()
{
    QStringList values = parsed_data.split(","); // Podział łańcucha na trzy części

    if (values.length() == 3) {
        bool ok;
        x = values[0].toFloat(&ok); // Konwersja pierwszej części na float

        ui->lineEdit_X->setText(values[0]);
        ui->lineEdit_Y->setText(values[1]);
        ui->lineEdit_Z->setText(values[2]);
        if (ok) {
            y = values[1].toFloat(&ok); // Konwersja drugiej części na float
            if (ok) {
                z = values[2].toFloat(&ok); // Konwersja trzeciej części na float

                // Warunki zmiany obrazka w zależności od wartości x, y, i z

                moves(x,y,z);
            }
        }
    }
}
/**
 * @brief Handles the dance moves based on the accelerometer data.
 * @param x The X-axis value.
 * @param y The Y-axis value.
 * @param z The Z-axis value.
 */
void dance::moves(float x, float y, float z)
{
    if (isGameRunning)
    {
        if (!isAnimationRunning)
        {
            if (x > 8.0f && y < 3.0f && y > -3.0f) {
                changeGif(":/rec/animations/Oruch.gif");
            }
            else if (x < -8.0f && y < 3.0f && y > -3.0f) {
                changeGif(":/rec/animations/obrot.gif");
            }
            else if (y > 8.0f && x < 3.0f && y > -3.0f) {
                changeGif(":/rec/animations/podskok.gif");
            }
            else if (y < -8.0f && x < 3.0f && x > -3.0f) {
                changeGif(":/rec/animations/shuffle.gif");
            }
            else if (y < -5.0f && x > 5.0f) {
                changeGif(":/rec/animations/fala.gif");
            }
        }
    }
    this->x = x;
    this->y = y;

    makePlot();
}
/**
 * @brief Creates a plot of the accelerometer data.
 */
void dance::makePlot()
{
    // Clear the existing graphs
    ui->customPlot->clearGraphs();

    // Generate some data:
    QVector<double> xi(1), yi(1); // Initialize with entries 0..100
    xi[0] = x;
    yi[0] = y;

    // Create graph and assign data to it:
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setData(xi, yi);

    // Set the pen color and style for the data line
    QPen pen;
    pen.setColor(Qt::blue);
    pen.setWidth(2);
    ui->customPlot->graph(0)->setPen(pen);

    // Set the style and size of the data points
    QCPScatterStyle scatterStyle;
    scatterStyle.setShape(QCPScatterStyle::ssCircle);
    scatterStyle.setSize(8);
    scatterStyle.setPen(QPen(Qt::blue));
    scatterStyle.setBrush(QBrush(Qt::blue));
    ui->customPlot->graph(0)->setScatterStyle(scatterStyle);

    // Give the axes some labels:
    ui->customPlot->xAxis->setLabel("x");
    ui->customPlot->yAxis->setLabel("y");

    // Set the range of the axes to fit the data:
    double xMin = std::min(xi[0], -10.0);
    double xMax = std::max(xi[0], 10.0);
    double yMin = std::min(yi[0], -10.0);
    double yMax = std::max(yi[0], 10.0);
    ui->customPlot->xAxis->setRange(xMin, xMax);
    ui->customPlot->yAxis->setRange(yMin, yMax);

    ui->customPlot->replot();
}
/**
 * @brief Changes the GIF animation to the specified file.
 * @param filePath The file path of the GIF animation.
 */
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

/**
 * @brief Plays the music file.
 * @param musicFilePath The file path of the music file.
 */
void dance::PlayMusic1(QString musicFilePath)
{
    player->setSource(QUrl::fromLocalFile(musicFilePath));
    player->setAudioOutput(audioOutput);
    player->play();

    timer1 = new QTimer(this);
    connect(timer1, &QTimer::timeout, this, &dance::timerTimeout1);

    timer1->setSingleShot(true);
    timer1->start(1000);
    // Resetowanie wartości counter do 0 przy każdym wywołaniu PlayMusic1()
    counter = 0;

    // Otwieranie i odczyt pliku tekstowego
    QFile file;

    if (musicFilePath == "C:/Users/wajci/Documents/DanceGame/music/cantina.mp3") {
        file.setFileName("C:/Users/wajci/Documents/DanceGame/moves/cantina.txt");
        qDebug() << file.fileName();
    } else if (musicFilePath == "C:/Users/wajci/Documents/DanceGame/music/KylieAkcent.mp3") {
        file.setFileName("C:/Users/wajci/Documents/DanceGame/moves/KylieAkcent.txt");
    } else {
        qDebug() << "Nieprawidłowa ścieżka do pliku.";
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Nie można otworzyć pliku tekstowego.";
        return;
    }

    QTextStream in(&file);

    numCalls = in.readLine().toInt(); // Przeniesiono do zmiennej klasy
    times.clear(); // Wyczyszczenie wektora klasy
    functions.clear(); // Wyczyszczenie wektora klasy

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(",");
        if (parts.length() == 2) {
            times.append(parts[0].toInt());
            functions.append(parts[1].toInt());
        }
    }

    file.close();
}
/**
 * @brief Timer timeout slot for dance movements.
 * This function is called when the timer times out. It increments the counter and performs the dance movement based on the current counter value.
 */
void dance::timerTimeout1()
{
    counter++;

    if (counter <= numCalls) {
        int time = times[counter - 1];
        int function = functions[counter - 1];

        switch (function) {
        case 1:
            podskok(time);
            break;
        case 2:
            obrot(time);
            break;
        case 3:
            shuffle(time);
            break;
        case 4:
            rondo(time);
            break;
        case 5:
            fala(time);
            break;
        // Dodaj inne przypadki dla innych funkcji
        default:
            break;
        }
    } else {
        QPixmap ruch(":/rec/moves/dance.jpg");

        int wruch = ui->label_ruch->width();
        int hruch = ui->label_ruch->height();
        ui->label_ruch->setPixmap(ruch.scaled(wruch,hruch,Qt::KeepAspectRatio));
        // Inne operacje po zakończeniu zmiany napisów
    }
}
/**
 * @brief Perform "podskok" dance movement.
 * @param czas The time duration for the dance movement.
 */
void dance::podskok(int czas)
{

    QPixmap ruch(":/rec/moves/podskok.jpg");

    int wruch = ui->label_ruch->width();
    int hruch = ui->label_ruch->height();
    ui->label_ruch->setPixmap(ruch.scaled(wruch,hruch,Qt::KeepAspectRatio));

    QTimer::singleShot(1500, this, [this, czas]() {
        QPixmap ruch(":/rec/moves/dance.jpg");

        int wruch = ui->label_ruch->width();
        int hruch = ui->label_ruch->height();
        ui->label_ruch->setPixmap(ruch.scaled(wruch,hruch,Qt::KeepAspectRatio));

        QTimer::singleShot(czas, this, &dance::timerTimeout1);
    });
}
/**
 * @brief Perform "obrot" dance movement.
 * @param czas The time duration for the dance movement.
 */
void dance::obrot(int czas)
{
    QPixmap ruch(":/rec/moves/lewo.jpg");

    int wruch = ui->label_ruch->width();
    int hruch = ui->label_ruch->height();
    ui->label_ruch->setPixmap(ruch.scaled(wruch,hruch,Qt::KeepAspectRatio));
    QTimer::singleShot(1500, this, [this, czas]() { // Dodano "czas" do listy przechwytującej lambdę
        QPixmap ruch(":/rec/moves/dance.jpg");

        int wruch = ui->label_ruch->width();
        int hruch = ui->label_ruch->height();
        ui->label_ruch->setPixmap(ruch.scaled(wruch,hruch,Qt::KeepAspectRatio));
        QTimer::singleShot(czas, this, &dance::timerTimeout1);
    });
}
/**
 * @brief Perform "shuffle" dance movement.
 * @param czas The time duration for the dance movement.
 */
void dance::shuffle(int czas)
{
    QPixmap ruch(":/rec/moves/dol.jpg");

    int wruch = ui->label_ruch->width();
    int hruch = ui->label_ruch->height();
    ui->label_ruch->setPixmap(ruch.scaled(wruch,hruch,Qt::KeepAspectRatio));
    QTimer::singleShot(1500, this, [this, czas]() { // Dodano "czas" do listy przechwytującej lambdę
        QPixmap ruch(":/rec/moves/dance.jpg");

        int wruch = ui->label_ruch->width();
        int hruch = ui->label_ruch->height();
        ui->label_ruch->setPixmap(ruch.scaled(wruch,hruch,Qt::KeepAspectRatio));
        QTimer::singleShot(czas, this, &dance::timerTimeout1);
    });
}

/**
 * @brief Perform "rondo" dance movement.
 * @param czas The time duration for the dance movement.
 */
void dance::rondo(int czas)
{
    QPixmap ruch(":/rec/moves/prawo.jpg");

    int wruch = ui->label_ruch->width();
    int hruch = ui->label_ruch->height();
    ui->label_ruch->setPixmap(ruch.scaled(wruch,hruch,Qt::KeepAspectRatio));
    QTimer::singleShot(1500, this, [this, czas]() { // Dodano "czas" do listy przechwytującej lambdę
        QPixmap ruch(":/rec/moves/dance.jpg");

        int wruch = ui->label_ruch->width();
        int hruch = ui->label_ruch->height();
        ui->label_ruch->setPixmap(ruch.scaled(wruch,hruch,Qt::KeepAspectRatio));
        QTimer::singleShot(czas, this, &dance::timerTimeout1);
    });
}
/**
 * @brief Perform "fala" dance movement.
 * @param czas The time duration for the dance movement.
 */
void dance::fala(int czas)
{
    QPixmap ruch(":/rec/moves/dolprawo.jpg");

    int wruch = ui->label_ruch->width();
    int hruch = ui->label_ruch->height();
    ui->label_ruch->setPixmap(ruch.scaled(wruch,hruch,Qt::KeepAspectRatio));
    QTimer::singleShot(1500, this, [this, czas]() { // Dodano "czas" do listy przechwytującej lambdę
        QPixmap ruch(":/rec/moves/dance.jpg");

        int wruch = ui->label_ruch->width();
        int hruch = ui->label_ruch->height();
        ui->label_ruch->setPixmap(ruch.scaled(wruch,hruch,Qt::KeepAspectRatio));
        QTimer::singleShot(czas, this, &dance::timerTimeout1);
    });
}
/**
 * @brief Slot for handling volume slider value changes.
 * @param value The new value of the volume slider.
 */
void dance::on_Slider_Volume_valueChanged(int value)
{
    qreal qrealVolume = value / 100.0;
    qreal linearVolume = QAudio::convertVolume(qrealVolume, QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
    audioOutput->setVolume(linearVolume);
}

/**
 * @brief Slot for handling the "cantina Freesty" action trigger.
 * This function is called when the "cantina Freesty" action is triggered. It sets the game running flag, sets the standard GIF, and initializes the music file path. Then, it calls the "Odliczanie" function.
 */

void dance::on_cantina_Freesty_triggered()
{
        isGameRunning=true;
        standardGif();
        musicFilePath = "C:/Users/wajci/Documents/DanceGame/music/cantina.mp3";
        Odliczanie();
}

/**
 * @brief Slot for handling the "akcent freestyle" action trigger.
 * This function is called when the "akcent freestyle" action is triggered. It sets the game running flag, sets the standard GIF, and initializes the music file path. Then, it calls the "Odliczanie" function.
 */
void dance::on_akcent_freestyle_triggered()
{
        isGameRunning=true;
        standardGif();
        musicFilePath = "C:/Users/wajci/Documents/DanceGame/music/KylieAkcent.mp3";
        Odliczanie();
}




