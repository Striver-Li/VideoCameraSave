#include "widget.h"
#include "playimage.h"
#include "qfiledialog.h"
#include "readthread.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QCameraInfo>
#include <QDir>

extern "C"
{
    #include <libavcodec/avcodec.h>
}

Q_DECLARE_METATYPE(AVFrame)

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    setWindowTitle("Qt+ffmpeg打开本地摄像头录像Demo");

    // 使用QOpenGLWindow绘制
    playImage = new PlayImage;
#if USE_WINDOW
    ui->verticalLayout->addWidget(QWidget::createWindowContainer(playImage)); // 这一步加载速度要比OpenGLWidget慢一点
#else
    ui->verticalLayout->addWidget(playImage);
#endif

    m_readThread = new ReadThread();
    connect(m_readThread, &ReadThread::repaint, playImage, &PlayImage::repaint, Qt::BlockingQueuedConnection);
    connect(m_readThread, &ReadThread::playState, this, &Widget::on_playState);

    // 获取可用摄像头列表
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    for (auto camera : cameras) {
#if defined(Q_OS_WIN)
        ui->com_url->addItem("video=" + camera.description()); // ffmpeg在Window下要使用video=description()
#elif defined(Q_OS_LINUX)
        ui->com_url->addItem(camera.deviceName());             // ffmpeg在linux下要使用deviceName()
#elif defined(Q_OS_MAC)
#endif
    }
}

Widget::~Widget()
{
    if (m_readThread) {
        // 由于使用了BlockingQueuedConnection，所以在退出时如果信号得不到处理就会卡死，所以需要取消绑定
        disconnect(m_readThread, &ReadThread::repaint, playImage, &PlayImage::repaint);
        m_readThread->close();
        m_readThread->wait();
        delete m_readThread;
    }
    delete ui;
}

void Widget::on_but_open_clicked()
{
    if (ui->but_open->text() == "开始播放") {
        m_readThread->open(ui->com_url->currentText());
    } else {
        m_readThread->close();
    }
}

void Widget::on_playState(ReadThread::PlayState state)
{
    if (state == ReadThread::Playing) {
        this->setWindowTitle(QString("正在播放：%1").arg(m_readThread->url()));
        ui->but_open->setText("停止播放");
    } else {
        ui->but_open->setText("开始播放");
        this->setWindowTitle(QString("Qt+ffmpeg打开本地摄像头录像Demo"));
    }
}

/**
 * @brief 录制视频保存到本地
 */
void Widget::on_but_save_clicked()
{
    if (ui->but_save->text() == "开始录制") {
        if (m_fileName.isEmpty())
            m_fileName = QDir::currentPath() + "/";
        m_readThread->saveVideo(m_fileName + QString("%1.mp4").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss")));
        ui->but_save->setText("停止");
    } else {
        m_readThread->stop();
        ui->but_save->setText("开始录制");
    }
}

void Widget::on_btn_select_clicked()
{
    if (m_fileName.isEmpty())
        m_fileName = QDir::currentPath();
    m_fileName = QFileDialog::getExistingDirectory(this, "打开目录",
                                                   m_fileName,
                                                   QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks) + "/";
    ui->path->setText(m_fileName);
}

void Widget::on_btn_clear_clicked()
{
    m_fileName.clear();
    ui->path->setText("");
}

