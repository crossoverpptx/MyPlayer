#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QLabel> // 头文件就是一个类，与下面加 class QLabel; 功能相同

QT_BEGIN_NAMESPACE
class QVideoWidget;
class QMediaPlayer;
class QAudioOutput;
class QSlider;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QComboBox;
QT_END_NAMESPACE

class Player : public QWidget
{
    Q_OBJECT

public:
    Player(QWidget *parent = nullptr);
    ~Player();

private:
    QVideoWidget *m_videoWidget = nullptr;
    QMediaPlayer *m_player = nullptr;
    QAudioOutput *m_audioOutput = nullptr;

    QSlider *m_slider = nullptr;
    QLabel *m_labelDuration = nullptr;

    QListWidget *m_playlistView = nullptr;

    qint64 m_duration;

    QPushButton *m_fullScreenButton = nullptr;

    QComboBox *m_audioOutputCom = nullptr;

    void updateDurationInfo(qint64 currentInfo);

private slots:
    void open();
    void stop();

    void seek(int mseconds); // 滑动条变化，视频相应变化
    void durationChanged(qint64 duration); // 滑动条变化，时长相应变化
    void positionChanged(qint64 progress);

    void setFullScreen();

    void audioOutputChanged(int index);

    QUrl getUrlFromItem(QListWidgetItem *item);

    void on_listWidget_doubleClicked(const QModelIndex &index);

    void previousClicked();
    void nextClicked();
};

#endif // PLAYER_H
