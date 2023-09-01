#include "playcontrols.h"

#include <QToolButton>
#include <QStyle>
#include <QSlider>
#include <QComboBox>
#include <QBoxLayout>
#include <QAudio>

PlayControls::PlayControls(QWidget *parent) :
    QWidget(parent)
{
    m_playButton = new QToolButton(this);
    m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(m_playButton, &QAbstractButton::clicked, this, &PlayControls::playClicked);

    m_stopButton = new QToolButton(this);
    m_stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    connect(m_stopButton, &QAbstractButton::clicked, this, &PlayControls::stop);

    m_previousButton = new QToolButton(this);
    m_previousButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    connect(m_previousButton, &QAbstractButton::clicked, this, &PlayControls::previous);

    m_nextButton = new QToolButton(this);
    m_nextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    connect(m_nextButton, &QAbstractButton::clicked, this, &PlayControls::next);

    m_muteButton = new QToolButton(this);
    m_muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    connect(m_muteButton, &QAbstractButton::clicked, this, &PlayControls::muteClicked);

    m_volumSlider = new QSlider(Qt::Horizontal, this);
    m_volumSlider->setRange(0, 100);
    connect(m_volumSlider, &QSlider::valueChanged, this, &PlayControls::onVolumeSliderValueChanged);


    m_rateBox = new QComboBox(this);
    m_rateBox->addItem("0.5x", QVariant(0.5));
    m_rateBox->addItem("1.0x", QVariant(1.0));
    m_rateBox->addItem("2.0x", QVariant(2.0));
    m_rateBox->setCurrentIndex(1);
    connect(m_rateBox, QOverload<int>::of(&QComboBox::activated), this, &PlayControls::updateRate);

    QBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0); // 设置边界
    layout->addWidget(m_stopButton);
    layout->addWidget(m_previousButton);
    layout->addWidget(m_playButton);
    layout->addWidget(m_nextButton);
    layout->addWidget(m_muteButton);
    layout->addWidget(m_volumSlider);
    layout->addWidget(m_rateBox);

    this->setLayout(layout);
}

void PlayControls::playClicked()
{
    switch (m_playerState) {
    case QMediaPlayer::StoppedState:
    case QMediaPlayer::PausedState:
        emit play();
        break;
    case QMediaPlayer::PlayingState:
        emit pause();
        break;
    default:
        break;
    }
}


void PlayControls::setState(QMediaPlayer::PlaybackState state)
{
    if (state != m_playerState) {
        m_playerState = state;

        switch (state) {
        case QMediaPlayer::StoppedState:
            m_stopButton->setEnabled(false);
            m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            break;
        case QMediaPlayer::PlayingState:
            m_stopButton->setEnabled(true);
            m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
            break;
        case QMediaPlayer::PausedState:
            m_stopButton->setEnabled(true);
            m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            break;
        }
    }
}

void PlayControls::muteClicked()
{
    emit changeMuting(!m_playerMuted);
}

void PlayControls::setMuted(bool muted)
{
    if (muted != m_playerMuted)
    {
        m_playerMuted = muted;
        m_muteButton->setIcon(style()->standardIcon(muted ? QStyle::SP_MediaVolumeMuted : QStyle::SP_MediaVolume));
    }
}

void PlayControls::onVolumeSliderValueChanged()
{
    emit changeVolume(volume());
}

float PlayControls::volume() const
{
    qreal linearVolume = QAudio::convertVolume(m_volumSlider->value() / qreal(100),
                                               QAudio::LogarithmicVolumeScale,
                                               QAudio::LinearVolumeScale);
    return linearVolume;
}

void PlayControls::setVolume(float volume)
{
    qreal logarithmicVolume = QAudio::convertVolume(volume,
                                                    QAudio::LinearVolumeScale,
                                                    QAudio::LogarithmicVolumeScale);
    m_volumSlider->setValue(qRound(logarithmicVolume*100));
}

bool PlayControls::isMuted() const
{
    return m_playerMuted;
}

qreal PlayControls::playbackRate() const
{
    return m_rateBox->itemData(m_rateBox->currentIndex()).toDouble();
}

void PlayControls::updateRate()
{
    emit changeRate(playbackRate());
}

PlayControls::~PlayControls()
{

}
