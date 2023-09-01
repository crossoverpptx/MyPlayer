#include "player.h"
#include "videowidget.h"
#include "playcontrols.h"

#include <QMediaPlayer>
#include <QAudioOutput>
#include <QtWidgets>
#include <QString>
#include <QVariant>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QListWidgetItem>

Player::Player(QWidget *parent)
    : QWidget(parent)
{
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput); // 播放器的音频输出设备

    m_videoWidget = new VideoWidget(this);
    m_videoWidget->resize(1280, 720);
    m_player->setVideoOutput(m_videoWidget); // 播放器的视频输出设备

    connect(m_player, &QMediaPlayer::durationChanged, this, &Player::durationChanged);
    connect(m_player, &QMediaPlayer::positionChanged, this, &Player::positionChanged);

    QBoxLayout *layout = new QVBoxLayout(this);

    QBoxLayout *displayLayout = new QHBoxLayout;
    displayLayout->addWidget(m_videoWidget, 2);
    m_playlistView = new QListWidget();
    displayLayout->addWidget(m_playlistView);
    connect(m_playlistView, &QListWidget::doubleClicked, this, &Player::on_listWidget_doubleClicked);
    layout->addLayout(displayLayout);

    QPushButton *openButton = new QPushButton("open", this);
//    layout->addWidget(openButton);
    connect(openButton, &QPushButton::clicked, this, &Player::open);

//    QPushButton *stopButton = new QPushButton("stop", this);
//    layout->addWidget(stopButton);
//    connect(stopButton, &QPushButton::clicked, this, &Player::stop);

    // slider
    QHBoxLayout *hLayout = new QHBoxLayout;
    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setRange(0, m_player->duration());
    connect(m_slider, &QSlider::sliderMoved, this, &Player::seek);
//    layout->addWidget(m_slider);
    hLayout->addWidget(m_slider);

    // 进度条标签
    m_labelDuration = new QLabel();
    m_labelDuration->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
//    layout->addWidget(m_labelDuration);
    hLayout->addWidget(m_labelDuration);

    layout->addLayout(hLayout);

    // controls
    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setContentsMargins(0, 0, 0, 0);
    PlayControls *controls = new PlayControls();
    controls->setState(m_player->playbackState());
    controls->setVolume(m_audioOutput->volume());
    controls->setMuted(controls->isMuted());

    connect(controls, &PlayControls::stop, m_player, &QMediaPlayer::stop);
//    connect(controls, &PlayControls::stop, this, &Player::stop); // 与上一行功能相同
//    layout->addWidget(openButton);
//    layout->addWidget(controls);

    controlLayout->addWidget(openButton);
    controlLayout->addStretch(1); // 空格
    controlLayout->addWidget(controls);
    controlLayout->addStretch(1);

//    connect(controls, &PlayControls::stop, this, &Player::stop);
    connect(controls, &PlayControls::play, m_player, &QMediaPlayer::play);
    connect(controls, &PlayControls::pause, m_player, &QMediaPlayer::pause);
    connect(m_player, &QMediaPlayer::playbackStateChanged, controls, &PlayControls::setState);

    connect(controls, &PlayControls::next, this, &Player::nextClicked);
    connect(controls, &PlayControls::previous, this, &Player::previousClicked);

    connect(controls, &PlayControls::changeMuting, m_audioOutput, &QAudioOutput::setMuted);
    connect(m_audioOutput, &QAudioOutput::mutedChanged, controls, &PlayControls::setMuted);

    connect(controls, &PlayControls::changeVolume, m_audioOutput, &QAudioOutput::setVolume);
    connect(m_audioOutput, &QAudioOutput::volumeChanged, controls, &PlayControls::setVolume);

    connect(controls, &PlayControls::changeRate, m_player, &QMediaPlayer::setPlaybackRate);

    m_fullScreenButton = new QPushButton(tr("FullScreen"), this);
    m_fullScreenButton->setCheckable(true);
    controlLayout->addWidget(m_fullScreenButton);
    connect(m_fullScreenButton, &QPushButton::clicked, this, &Player::setFullScreen);

    m_audioOutputCom = new QComboBox(this);
    m_audioOutputCom->addItem(QString::fromUtf8("Default"), QVariant::fromValue(QAudioDevice()));
    for (auto &deviceInfo : QMediaDevices::audioOutputs())
        m_audioOutputCom->addItem(deviceInfo.description(), QVariant::fromValue(deviceInfo));
    controlLayout->addWidget(m_audioOutputCom);
    connect(m_audioOutputCom, QOverload<int>::of(&QComboBox::activated), this, &Player::audioOutputChanged);

    layout->addLayout(controlLayout);
}

//void Player::open()
//{
//    QString curPath = QDir::homePath();
//    QString dlgTitle = "选择视频文件";
//    QString filter = "视频文件(*.wmv *.mp4);;所有文件(*.*)";
//    QString aFile = QFileDialog::getOpenFileName(this, dlgTitle, curPath, filter);
//    if (aFile.isEmpty())
//        return;
//    m_player->setSource(QUrl::fromLocalFile(aFile));
//    m_player->play();
//}

void Player::open()
{
    QString curPath = QDir::homePath();
    QString dlgTitle = "选择视频文件";
    QString filter = "视频文件(*.mp4 *.avi);;所有文件(*.*)";
    QStringList fileList = QFileDialog::getOpenFileNames(this, dlgTitle, curPath, filter);
    if (fileList.count() < 0)
        return;
    for (int i = 0; i < fileList.count(); ++i)
    {
        QString aFile = fileList.at(i);
        QFileInfo fileInfo(aFile);
        QListWidgetItem *aItem = new QListWidgetItem(fileInfo.fileName());
        aItem->setData(Qt::UserRole, QUrl::fromLocalFile(aFile));
        m_playlistView->addItem(aItem);
    }
    if (m_player->playbackState() != QMediaPlayer::PlayingState)
    {
        m_playlistView->setCurrentRow(0); // 如果不是播放状态，则播放第一个视频
        QUrl source = getUrlFromItem(m_playlistView->currentItem());
        m_player->setSource(source);
    }
//    m_player->setSource(QUrl::fromLocalFile(aFile));
//    m_player->play();
}

void Player::stop()
{
    m_player->stop();
}

void Player::seek(int mseconds)
{
    m_player->setPosition(mseconds);
}

void Player::durationChanged(qint64 duration)
{
    m_duration = duration / 1000;
    m_slider->setMaximum(duration);
}

void Player::positionChanged(qint64 progress)
{
    if (!m_slider->isSliderDown())
        m_slider->setValue(progress);
    updateDurationInfo(progress/1000);
}

void Player::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    if (currentInfo || m_duration) {
        QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60,
                          currentInfo % 60, (currentInfo * 1000) % 1000);
        QTime totalTime((m_duration / 3600) % 60, (m_duration / 60) % 60,
                        m_duration % 60, (m_duration * 1000) % 1000);
        QString format = "mm:ss";
        if (m_duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    m_labelDuration->setText(tStr);
}

void Player::setFullScreen()
{
    m_videoWidget->setFullScreen(true);
}

void  Player::audioOutputChanged(int index)
{
    auto device = m_audioOutputCom->itemData(index).value<QAudioDevice>();
    m_player->audioOutput()->setDevice(device);
}

QUrl Player::getUrlFromItem(QListWidgetItem *item)
{
    QVariant itemData = item->data(Qt::UserRole);
    QUrl source = itemData.value<QUrl>();
    return source;
}

void Player::on_listWidget_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    m_player->setSource(getUrlFromItem(m_playlistView->currentItem()));
    m_player->play();
}

void Player::previousClicked()
{
    int curRow = m_playlistView->currentRow();
    curRow--;
    curRow = curRow < 0 ? 0 : curRow;
    m_playlistView->setCurrentRow(curRow);
    m_player->setSource(getUrlFromItem(m_playlistView->currentItem()));
    m_player->play();
}

void Player::nextClicked()
{
    int curRow = m_playlistView->currentRow();
    curRow++;
    int count = m_playlistView->count();
    curRow = curRow >= count ? 0 : curRow;
    m_playlistView->setCurrentRow(curRow);
    m_player->setSource(getUrlFromItem(m_playlistView->currentItem()));
    m_player->play();
}

Player::~Player()
{

}
