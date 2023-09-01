#ifndef PLAYCONTROLS_H
#define PLAYCONTROLS_H

#include <QWidget>
#include <QMediaPlayer>

QT_BEGIN_NAMESPACE
class QAbstractButton;
class QComboBox;
class QAbstractSlider;
QT_END_NAMESPACE

class PlayControls : public QWidget
{
    Q_OBJECT

public:
    explicit PlayControls(QWidget *parent = nullptr);
    void setVolume(float volume);
    bool isMuted() const;
    ~PlayControls();

public slots:
    void setMuted(bool muted);
    void updateRate();

signals:
    void stop();
    void play();
    void pause();
    void changeMuting(bool muting);
    void changeVolume(float volume);
    void changeRate(qreal rate);

    void next();
    void previous();

private slots:
    void playClicked();
    void muteClicked();
    void onVolumeSliderValueChanged();

private:
    QMediaPlayer::PlaybackState m_playerState = QMediaPlayer::StoppedState; // 播放状态：初始化为停止状态

    QAbstractButton *m_stopButton = nullptr; // 停止按钮
    QAbstractButton *m_playButton = nullptr; // 播放按钮
    QAbstractButton *m_previousButton = nullptr; // 向前
    QAbstractButton *m_nextButton = nullptr; // 向后
    QAbstractButton *m_muteButton = nullptr; // 静音
    QAbstractSlider *m_volumSlider = nullptr; // 声音滑动条
    QComboBox *m_rateBox = nullptr; // 速率

    bool m_playerMuted = false;

    float volume() const;
    qreal playbackRate() const;

public slots:
    void setState(QMediaPlayer::PlaybackState state);
};

#endif // PLAYCONTROLS_H
