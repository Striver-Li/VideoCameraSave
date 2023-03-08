#ifndef READTHREAD_H
#define READTHREAD_H

#include <QThread>
#include <QTime>

class VideoDecode;
class VideoSave;
struct AVFrame;

/**
 * @brief 读取视频图像数据线程，在线程中解码视频
 */
class ReadThread : public QThread
{
    Q_OBJECT
public:
    enum PlayState  // 视频播放状态
    {
        Playing,
        Stopped
    };
public:
    explicit ReadThread(QObject *parent = nullptr);
    ~ReadThread() override;

    void open(const QString &url = QString());
    void pause(bool flag);
    void close();
    const QString &url();                       // 获取打开的视频地址
    void saveVideo(const QString &fileName);    // 录制视频
    void stop();                                // 停止录制

protected:
    void run() override;

signals:
    void repaint(AVFrame *frame);           // 重绘
    void playState(ReadThread::PlayState);  // 视频播放状态改变时触发

private:
    VideoDecode *m_videoDecode = nullptr;   // 视频解码类
    VideoSave   *m_videoSave   = nullptr;   // 视频编码保存类
    QString m_url;                          // 打开的视频地址
    bool m_play = false;                    // 播放控制
};

#endif // READTHREAD_H
