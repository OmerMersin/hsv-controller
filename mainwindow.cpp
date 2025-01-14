#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), imageLoaded(false)  // Initialize streaming to true
{
    createUI();
}


void MainWindow::createUI()
{
    // Central widget and main layout
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // Left layout for image display
    QVBoxLayout *leftLayout = new QVBoxLayout();
    imageLabel = new QLabel();
    imageLabel->setFixedSize(800, 600);
    imageLabel->setStyleSheet("QLabel { background-color : black; }");
    leftLayout->addWidget(imageLabel);

    // Right layout for controlsc
    QVBoxLayout *rightLayout = new QVBoxLayout();

    // Lower HSV controls
    QGroupBox *lowerGroup = new QGroupBox("Lower HSV Thresholds");
    QGridLayout *lowerLayout = new QGridLayout();

    lowerHSlider = new QSlider(Qt::Horizontal);
    lowerHSlider->setRange(0, 179);
    lowerHSlider->setValue(35);
    lowerHLabel = new QLabel("H: 35");

    lowerSSlider = new QSlider(Qt::Horizontal);
    lowerSSlider->setRange(0, 255);
    lowerSSlider->setValue(40);
    lowerSLabel = new QLabel("S: 40");

    lowerVSlider = new QSlider(Qt::Horizontal);
    lowerVSlider->setRange(0, 255);
    lowerVSlider->setValue(60);
    lowerVLabel = new QLabel("V: 60");

    connect(lowerHSlider, &QSlider::valueChanged, this, &MainWindow::onSliderValueChanged);
    connect(lowerSSlider, &QSlider::valueChanged, this, &MainWindow::onSliderValueChanged);
    connect(lowerVSlider, &QSlider::valueChanged, this, &MainWindow::onSliderValueChanged);

    lowerLayout->addWidget(lowerHLabel, 0, 0);
    lowerLayout->addWidget(lowerHSlider, 0, 1);
    lowerLayout->addWidget(lowerSLabel, 1, 0);
    lowerLayout->addWidget(lowerSSlider, 1, 1);
    lowerLayout->addWidget(lowerVLabel, 2, 0);
    lowerLayout->addWidget(lowerVSlider, 2, 1);

    lowerGroup->setLayout(lowerLayout);

    // Upper HSV controls
    QGroupBox *upperGroup = new QGroupBox("Upper HSV Thresholds");
    QGridLayout *upperLayout = new QGridLayout();

    upperHSlider = new QSlider(Qt::Horizontal);
    upperHSlider->setRange(0, 179);
    upperHSlider->setValue(85);
    upperHLabel = new QLabel("H: 85");

    upperSSlider = new QSlider(Qt::Horizontal);
    upperSSlider->setRange(0, 255);
    upperSSlider->setValue(255);
    upperSLabel = new QLabel("S: 255");

    upperVSlider = new QSlider(Qt::Horizontal);
    upperVSlider->setRange(0, 255);
    upperVSlider->setValue(200);
    upperVLabel = new QLabel("V: 200");

    connect(upperHSlider, &QSlider::valueChanged, this, &MainWindow::onSliderValueChanged);
    connect(upperSSlider, &QSlider::valueChanged, this, &MainWindow::onSliderValueChanged);
    connect(upperVSlider, &QSlider::valueChanged, this, &MainWindow::onSliderValueChanged);

    upperLayout->addWidget(upperHLabel, 0, 0);
    upperLayout->addWidget(upperHSlider, 0, 1);
    upperLayout->addWidget(upperSLabel, 1, 0);
    upperLayout->addWidget(upperSSlider, 1, 1);
    upperLayout->addWidget(upperVLabel, 2, 0);
    upperLayout->addWidget(upperVSlider, 2, 1);

    upperGroup->setLayout(upperLayout);

    // Area threshold
    QHBoxLayout *areaLayout = new QHBoxLayout();
    QLabel *areaLabel = new QLabel("Area Threshold:");
    areaLineEdit = new QLineEdit("3000");
    connect(areaLineEdit, &QLineEdit::textChanged, this, &MainWindow::onSliderValueChanged);
    areaLayout->addWidget(areaLabel);
    areaLayout->addWidget(areaLineEdit);


    // Add "Update HSV" button to the right layout
    QPushButton *updateHSVButton = new QPushButton("Update HSV");
    connect(updateHSVButton, &QPushButton::clicked, this, &MainWindow::updateHSVValues);

    // Add widgets to right layout
    rightLayout->addWidget(lowerGroup);
    rightLayout->addWidget(upperGroup);
    rightLayout->addLayout(areaLayout);
    rightLayout->addWidget(updateHSVButton);

    rightLayout->addStretch();

    // Add layouts to main layout
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);

    // Toolbar with Open action
    QToolBar *toolbar = addToolBar("Main Toolbar");
    QAction *openAction = toolbar->addAction("Open");
    connect(openAction, &QAction::triggered, this, &MainWindow::openImage);

    // Capture from Raspberry
    QAction *CaptureImage = toolbar->addAction("Capture Image");
    connect(CaptureImage, &QAction::triggered, this, &MainWindow::captureImage);

    // Auto HSV
    QAction *AutoHSV = toolbar->addAction("Auto HSV");
    connect(AutoHSV, &QAction::triggered, this, &MainWindow::suggestHSVValues);


    // Set window title
    setWindowTitle("HSV Threshold Controller");
    setWindowIcon(QIcon(":/icons/icon.ico"));

}


void MainWindow::openImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Image Files (*.png *.jpg *.jpeg *.bmp)");
    if (!fileName.isEmpty())
    {
        // Load the image using OpenCV
        originalImage = cv::imread(fileName.toStdString());
        if (originalImage.empty())
        {
            QMessageBox::information(this, "Error", "Could not open image.");
            return;
        }

        originalCapturedImage = originalImage.clone();  // Keep a backup of the original image
        // imageLoaded = true;  // Set image loaded flag
        imageOpened = true;

        // Convert the OpenCV image (BGR) to QImage (RGB)
        QImage qimg(originalImage.data,
                    originalImage.cols,
                    originalImage.rows,
                    static_cast<int>(originalImage.step),
                    QImage::Format_BGR888);  // OpenCV uses BGR, QImage expects RGB

        // Display the image in the imageLabel (video stream area)
        imageLabel->setPixmap(QPixmap::fromImage(qimg.scaled(imageLabel->size(),
                                                             Qt::KeepAspectRatio,
                                                             Qt::SmoothTransformation)));

        qDebug() << "Image loaded and displayed in video stream area.";
    }
}



void MainWindow::onSliderValueChanged()
{
    if (imageLoaded || imageOpened)
    {
        updateLabels();
        processImage();
    }
    else
    {
        qDebug() << "No image loaded or captured, cannot process.";
    }
}

void MainWindow::updateLabels()
{
    lowerHLabel->setText(QString("H: %1").arg(lowerHSlider->value()));
    lowerSLabel->setText(QString("S: %1").arg(lowerSSlider->value()));
    lowerVLabel->setText(QString("V: %1").arg(lowerVSlider->value()));
    upperHLabel->setText(QString("H: %1").arg(upperHSlider->value()));
    upperSLabel->setText(QString("S: %1").arg(upperSSlider->value()));
    upperVLabel->setText(QString("V: %1").arg(upperVSlider->value()));
}

void MainWindow::processImage()
{
    // Ensure that the image is not empty before processing
    if (originalImage.empty())
    {
        qDebug() << "Error: Cannot process an empty image.";
        return;
    }

    // Reset the image to the original captured state (clear previous rectangles)
    originalImage = originalCapturedImage.clone();

    // Convert the original image to HSV for plant detection
    cv::Mat hsvImage;
    cv::cvtColor(originalImage, hsvImage, cv::COLOR_BGR2HSV);  // The error happens here if the image is empty

    // Get HSV values from sliders
    cv::Scalar lowerHSV(lowerHSlider->value(), lowerSSlider->value(), lowerVSlider->value());
    cv::Scalar upperHSV(upperHSlider->value(), upperSSlider->value(), upperVSlider->value());

    // Create a mask based on the current HSV thresholds
    cv::Mat mask;
    cv::inRange(hsvImage, lowerHSV, upperHSV, mask);

    // Apply Gaussian blur for noise reduction
    int blurSize = 1;
    if (blurSize > 0) {
        cv::GaussianBlur(mask, mask, cv::Size(blurSize, blurSize), 0);
    }

    // Morphological operations for noise removal
    int morphSize = 5;  // You can make this adjustable like blur if needed
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(morphSize, morphSize));
    int morphIterations = 2;  // Adjust this if you want more morphological iterations
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel, cv::Point(-1, -1), morphIterations);
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel, cv::Point(-1, -1), morphIterations);

    // Divide the image into 4 sections for nozzle control
    int numSections = 4;
    int sectionWidth = mask.cols / numSections;
    std::vector<std::vector<cv::Point>> contours;

    // Loop through each section
    for (int i = 0; i < numSections; ++i)
    {
        cv::Rect sectionRect(i * sectionWidth, 0, sectionWidth, mask.rows);
        cv::Mat sectionMask = mask(sectionRect);  // Extract section from the mask

        // Find contours in the section
        cv::findContours(sectionMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        // Analyze section areas and draw the results on the original image
        double totalArea = 0;
        for (size_t j = 0; j < contours.size(); j++)
        {
            double area = cv::contourArea(contours[j]);
            if (area > areaLineEdit->text().toInt())
            {
                totalArea += area;
                cv::Rect rect = cv::boundingRect(contours[j]);
                rect.x += i * sectionWidth;  // Adjust X for the section's offset
                cv::rectangle(originalImage, rect, cv::Scalar(0, 255, 0), 2);
                cv::putText(originalImage, "Plant", cv::Point(rect.x, rect.y - 10),
                            cv::FONT_HERSHEY_SIMPLEX, 0.9, cv::Scalar(36, 255, 12), 2);
            }
        }
        // Debug text overlay showing the total area in each section (displayed at the top)
        cv::putText(originalImage, "Area: " + std::to_string((int)totalArea),
                    cv::Point(i * sectionWidth + 10, 30),  // Position at the top of the section
                    cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 255), 1);
    }

    // Display the processed image with contours
    QImage qimg(originalImage.data, originalImage.cols, originalImage.rows, static_cast<int>(originalImage.step), QImage::Format_BGR888);
    imageLabel->setPixmap(QPixmap::fromImage(qimg.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)));
}

void MainWindow::onUpload()
{
    if (imageLoaded)
    {
        updateLabels();
        processImage();
    }
}



void MainWindow::suggestHSVValues()
{
    if (imageLoaded){
        return;
    }

    // Convert to HSV
    cv::Mat hsvImage;
    cv::cvtColor(originalImage, hsvImage, cv::COLOR_BGR2HSV);

    // Split HSV channels
    std::vector<cv::Mat> hsvChannels;
    cv::split(hsvImage, hsvChannels);

    // Compute histograms for H, S, V channels
    int histSize = 180;  // H channel ranges from 0 to 179
    float hRange[] = {0, 180};  // H range
    float sRange[] = {0, 256};  // S range
    float vRange[] = {0, 256};  // V range
    const float* hRanges = hRange;
    const float* sRanges = sRange;
    const float* vRanges = vRange;

    cv::Mat hHist, sHist, vHist;
    cv::calcHist(&hsvChannels[0], 1, 0, cv::Mat(), hHist, 1, &histSize, &hRanges, true, false);
    cv::calcHist(&hsvChannels[1], 1, 0, cv::Mat(), sHist, 1, &histSize, &sRanges, true, false);
    cv::calcHist(&hsvChannels[2], 1, 0, cv::Mat(), vHist, 1, &histSize, &vRanges, true, false);

    // Normalize the histograms
    cv::normalize(hHist, hHist, 0, 255, cv::NORM_MINMAX);
    cv::normalize(sHist, sHist, 0, 255, cv::NORM_MINMAX);
    cv::normalize(vHist, vHist, 0, 255, cv::NORM_MINMAX);

    // Define a flexible green hue range
    int greenStart = 30;  // Make this a bit wider to detect a range of greens
    int greenEnd = 90;    // Extended end for more flexibility in upper green

    double maxH = 0, maxS = 0, maxV = 0;
    int hue = 0, saturation = 0, value = 0;

    // Check histogram size before accessing
    if (hHist.rows > 0 && sHist.rows > 0 && vHist.rows > 0) {

        // Find the dominant hue within the green range using minMaxLoc for safer access
        cv::Point maxIdxH;
        cv::minMaxLoc(hHist.rowRange(greenStart, greenEnd), nullptr, &maxH, nullptr, &maxIdxH);
        hue = maxIdxH.y + greenStart;  // Adjust index by starting point

        // Analyze S and V histograms for dominant saturation and value
        cv::Point maxIdxS, maxIdxV;
        cv::minMaxLoc(sHist, nullptr, &maxS, nullptr, &maxIdxS);
        cv::minMaxLoc(vHist, nullptr, &maxV, nullptr, &maxIdxV);

        saturation = maxIdxS.y;
        value = maxIdxV.y;

        // Now, instead of a fixed range, we allow the upper hue to expand more dynamically
        int lowerH = std::max(0, hue - 15);  // Adjust lower hue slightly more
        int upperH = std::min(179, hue + 20);  // Allow higher flexibility for upper hue

        int lowerS = std::max(0, saturation - 50);  // Maintain some flexibility in saturation
        int upperS = std::min(255, saturation + 80);  // Allow more flexibility in upper saturation
        upperS = std::min(255, upperS+100);  // Allow more flexibility in upper saturation

        int lowerV = std::max(0, value - 40);  // Tighter range on value
        int upperV = std::min(255, value + 80);  // Allow more flexibility on upper value

        // Optionally blend manual HSV values with auto-calculated ones
        lowerH = (lowerH + lowerHSlider->value()) / 2;
        upperH = std::min(179, (upperH + upperHSlider->value()) / 2);
        lowerS = (lowerS + lowerSSlider->value()) / 2;
        upperS = (upperS + upperSSlider->value()) / 2;
        lowerV = (lowerV + lowerVSlider->value()) / 2;
        upperV = (upperV + upperVSlider->value()) / 2;

        // Display the suggested HSV ranges in the console or as a message
        qDebug() << "Suggested HSV ranges for green masking:";
        qDebug() << "Hue range: " << lowerH << " to " << upperH;
        qDebug() << "Saturation range: " << lowerS << " to " << upperS;
        qDebug() << "Value range: " << lowerV << " to " << upperV;

        // Optionally, set the sliders to the suggested values
        lowerHSlider->setValue(lowerH);
        upperHSlider->setValue(upperH);
        lowerSSlider->setValue(lowerS);
        upperSSlider->setValue(upperS);
        lowerVSlider->setValue(lowerV);
        upperVSlider->setValue(upperV);
    } else {
        qDebug() << "Histogram is empty, cannot suggest HSV values.";
    }
}

void MainWindow::captureImage()
{
    QString fileName = "capture.jpg";

    // Ensure the URL is valid
    QUrl imageUrl(url);
    if (!imageUrl.isValid())
    {
        QMessageBox::warning(this, "Error", "Invalid URL specified.");
        return;
    }

    // Create network manager and request
    QNetworkAccessManager manager;
    QNetworkRequest request(imageUrl); // Properly construct QNetworkRequest with QUrl
    QNetworkReply *reply = manager.get(request); // Correct usage of manager.get()

    // Use an event loop to wait for the reply
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec(); // Wait until the request is complete

    if (reply->error() == QNetworkReply::NoError)
    {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly))
        {
            file.write(reply->readAll());
            file.close();
            qDebug() << "Image captured and saved as" << fileName;

            // Load and display the captured image
            cv::Mat capturedImage = cv::imread(fileName.toStdString());
            if (!capturedImage.empty())
            {
                originalImage = capturedImage.clone(); // Save the captured image for further processing
                originalCapturedImage = originalImage.clone();
                imageOpened = true;

                QImage qimg(capturedImage.data,
                            capturedImage.cols,
                            capturedImage.rows,
                            static_cast<int>(capturedImage.step),
                            QImage::Format_BGR888);

                imageLabel->setPixmap(QPixmap::fromImage(qimg.scaled(imageLabel->size(),
                                                                     Qt::KeepAspectRatio,
                                                                     Qt::SmoothTransformation)));

                qDebug() << "Captured image displayed.";
            }
            else
            {
                QMessageBox::warning(this, "Error", "Could not load the captured image.");
            }
        }
        else
        {
            QMessageBox::warning(this, "Error", "Could not save the captured image.");
        }
    }
    else
    {
        QMessageBox::warning(this, "Error", "Failed to capture image from the server: " + reply->errorString());
    }

    reply->deleteLater();
}


void MainWindow::updateHSVValues()
{
    // Collect HSV values from the sliders
    int lowerH = lowerHSlider->value();
    int lowerS = lowerSSlider->value();
    int lowerV = lowerVSlider->value();
    int upperH = upperHSlider->value();
    int upperS = upperSSlider->value();
    int upperV = upperVSlider->value();
    bool ok;
    int areaThreshold = areaLineEdit->text().toInt(&ok);

    // Prepare the JSON payload
    QJsonObject hsvData;
    hsvData["lower"] = QJsonArray{lowerH, lowerS, lowerV};
    hsvData["upper"] = QJsonArray{upperH, upperS, upperV};
    if (ok) {
        hsvData["area"] = areaThreshold; // Add area threshold to JSON
    } else {
        qDebug() << "Invalid area threshold value entered.";
    }
    QJsonDocument jsonDoc(hsvData);
    QByteArray jsonData = jsonDoc.toJson();

    // Create network manager and request
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request((QUrl(url))); // Correct initialization
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Send the POST request
    QNetworkReply *reply = manager->post(request, jsonData);

    // Wait for the reply using a lambda slot
    connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError)
        {
            QMessageBox::information(this, "Success", "HSV values updated successfully.");
        }
        else
        {
            QMessageBox::warning(this, "Error", "Failed to update HSV values: " + reply->errorString());
        }
        reply->deleteLater();
    });
}

