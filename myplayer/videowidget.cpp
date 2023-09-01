#include "videowidget.h"

#include <QKeyEvent>
#include <QMouseEvent>

VideoWidget::VideoWidget(QWidget *parent) :
    QVideoWidget(parent)
{
    // 设置窗体尺寸：默认
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    // 设置调色板
    setAutoFillBackground(true);
    QPalette p = palette();
    p.setColor(QPalette::Window, Qt::black); // 黑色
    setPalette(p);

    // 设置窗口尺寸
    setMinimumSize(640, 320);
}

void VideoWidget::keyPressEvent(QKeyEvent *event)
{
    if ((event->key() == Qt::Key_Escape || event->key() == Qt::Key_Back) && isFullScreen() )
    {
        setFullScreen(false);
        event->accept();
    }
    else if (event->key() == Qt::Key_A && event->modifiers() & Qt::ControlModifier)
    {
        setFullScreen(false);
        event->accept();
    }
    else
    {
        QVideoWidget::keyPressEvent(event);
    }
}

void VideoWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    setFullScreen(!isFullScreen());
    event->accept();
}

void VideoWidget::mousePressEvent(QMouseEvent *event)
{
    QVideoWidget::mousePressEvent(event);
}

VideoWidget::~VideoWidget()
{

}
