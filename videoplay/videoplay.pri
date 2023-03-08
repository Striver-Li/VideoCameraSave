INCLUDEPATH += $$PWD/../include

LIBS += $$PWD/../lib/avcodec.lib  \
	$$PWD/../lib/avdevice.lib  \
	$$PWD/../lib/avfilter.lib  \
	$$PWD/../lib/avformat.lib  \
	$$PWD/../lib/avutil.lib  \
	$$PWD/../lib/postproc.lib  \
	$$PWD/../lib/swresample.lib  \
	$$PWD/../lib/swscale.lib  \

HEADERS += \
    $$PWD/readthread.h \
    $$PWD/videodecode.h \
    $$PWD/videosave.h

SOURCES += \
    $$PWD/readthread.cpp \
    $$PWD/videodecode.cpp \
    $$PWD/videosave.cpp
