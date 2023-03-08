#ifndef VIDEOSAVE_H
#define VIDEOSAVE_H

#include <QMutex>

struct AVCodecParameters;
struct AVFormatContext;
struct AVCodecContext;
struct AVStream;
struct AVFrame;
struct AVPacket;
struct AVOutputFormat;

/**
 * @brief 将视频编码后保存到文件中
 */
class VideoSave
{
public:
    VideoSave();
    ~VideoSave();

    bool open(AVStream *inStream, const QString &fileName);
    void write(AVFrame *frame);
    void close();

private:
    void showError(int err);

private:
    AVFormatContext *m_formatContext = nullptr;
    AVCodecContext  *m_codecContext  = nullptr; // 编码器上下文
    AVStream        *m_videoStream   = nullptr;
    AVPacket        *m_packet        = nullptr;
    int m_index = 0;
    bool m_writeHeader = false;                 // 是否写入头
    QMutex m_mutex;
};

#endif // VIDEOSAVE_H
