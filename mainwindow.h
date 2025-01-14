#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QSlider>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QToolBar>
#include <QAction>
#include <QImage>
#include <QPixmap>
#include <QTimer>
#include <QMessageBox>
#include <QThread>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>  // For cv::cvtColor, cv::calcHist
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QUrl>



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void openImage();
    void onSliderValueChanged();
    void onUpload();
    void processImage();


private:
    void createUI();
    void updateLabels();
    cv::VideoCapture cap;  // Declare cap as a member variable for continuous video capture
    QTimer *timer;         // Timer to capture frames
    QAction *captureAction;  // Reference to the capture/continue action
    cv::Mat originalCapturedImage;  // Store the clean original image
    void suggestHSVValues();
    void captureImage();
    void updateHSVValues();


    QWidget *centralWidget;
    QLabel *imageLabel;
    QSlider *lowerHSlider;
    QSlider *lowerSSlider;
    QSlider *lowerVSlider;
    QSlider *upperHSlider;
    QSlider *upperSSlider;
    QSlider *upperVSlider;
    QLineEdit *areaLineEdit;


    QLabel *lowerHLabel;
    QLabel *lowerSLabel;
    QLabel *lowerVLabel;
    QLabel *upperHLabel;
    QLabel *upperSLabel;
    QLabel *upperVLabel;

    cv::Mat originalImage;
    cv::Mat processedImage;

    bool imageLoaded;
    bool imageOpened = false;
    bool hasShownConnectionError = false;

    QString url = "http://10.42.0.1:8080";
};

#endif // MAINWINDOW_H
