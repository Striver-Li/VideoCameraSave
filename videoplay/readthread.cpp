#include "readthread.h"
#include "videodecode.h"
#include "videosave.h"
#include <QEventLoop>
#include <QDebug>
#include <QTimer>

ReadThread::ReadThread(QObject *parent)
    : QThread{parent}
{
    m_videoDecode = new VideoDecode();
    m_videoSave = new VideoSave();
    qRegisterMetaType<ReadThread::PlayState>("ReadThread::PlayState");
}

ReadThread::~ReadThread()
{
    if (m_videoDecode)
        delete m_videoDecode;
    if (m_videoSave)
        delete m_videoSave;
}

/**
 * @brief 传入播放的视频地址并开启线程
 * @param url
 */
void ReadThread::open(const QString &url)
{
    if (!isRunning()) {
        m_url = url;
        this->start();
    }
}

void ReadThread::close()
{
    m_play = false;
}

const QString &ReadThread::url()
{
    return m_url;
}

void ReadThread::saveVideo(const QString &fileName)
{
    m_videoSave->open(m_videoDecode->getVideoStream(), fileName);
}

void ReadThread::stop()
{
    m_videoSave->close();
}

/**
 * @brief      非阻塞延时
 * @param msec 延时毫秒
 */
void sleepMsec(int msec)
{
    if(msec <= 0)
        return;
    QEventLoop loop; // 定义一个新的事件循环
    QTimer::singleShot(msec, &loop, SLOT(quit())); // 创建单次定时器，槽函数为事件循环的退出函数
    loop.exec(); // 事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
}

void ReadThread::run()
{
    bool ret = m_videoDecode->open(m_url);
    if (ret) {
        m_play = true;
        emit playState(ReadThread::Playing);
    } else {
        qWarning() << "打开失败！";
    }
    // 循环读取视频图像
    while (m_play) {
        AVFrame *frame = m_videoDecode->read();
        if (frame) {
            m_videoSave->write(frame);
            emit repaint(frame);
        } else {
            // 当前读取到无效图像时判断是否读取完成
            if (m_videoDecode->isEnd())
                break;
            // 这里不能使用QThread::msleep()延时，否则会很不稳定
            sleepMsec(1);
        }
    }

    qDebug() << "关闭摄像头！";
    m_videoDecode->close();
    emit playState(ReadThread::Stopped);
}
