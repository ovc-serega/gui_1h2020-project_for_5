#include "music_player.h"

#include <QFileDialog>
#include <QDir>
#include <QStringList>
#include <QGraphicsItem>

music_player::music_player(QWidget *parent)
    : QWidget(parent)
{
    // buttons
    btn_add = new QPushButton(QIcon(":/btn/add"),"",this);
    btn_next = new QPushButton(QIcon(":/btn/next"),"",this);
    btn_previous = new QPushButton(QIcon(":/btn/prev"),"",this);
    btn_play = new QPushButton(QIcon(":/btn/play"),"",this);
    btn_pause = new QPushButton(QIcon(":/btn/pause"),"",this);
    btn_stop = new QPushButton(QIcon(":/btn/stop"),"",this);

    slider_volume = new QSlider(Qt::Horizontal,this);
    current_volume = new QLabel("",this);
    slider_volume->setMinimum(0);
    slider_volume->setMaximum(100);
    slider_volume->setSingleStep(5);
    slider_volume->setPageStep(20);

    // кнопка смены состояний
    btn_mode = new QPushButton(this);
    QStateMachine *machine = new QStateMachine(this);
    QState *s1 = new QState();
    QState *s2 = new QState();
    QState *s3 = new QState();

    s1->addTransition(btn_mode, &QAbstractButton::clicked, s2);
    s2->addTransition(btn_mode, &QAbstractButton::clicked, s3);
    s3->addTransition(btn_mode, &QAbstractButton::clicked, s1);


    connect(s1,&QState::entered,[this](){
        btn_mode->setIcon(QIcon(":/btn/playlist_loop"));
        m_playlist->setPlaybackMode(QMediaPlaylist::Loop);
    });
    connect(s2,&QState::entered,[this](){
        btn_mode->setIcon(QIcon(":/btn/random"));
        m_playlist->setPlaybackMode(QMediaPlaylist::Random);
    });
    connect(s3,&QState::entered,[this](){
        btn_mode->setIcon(QIcon(":/btn/current_loop"));
        m_playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    });


    machine->addState(s1);
    machine->addState(s2);
    machine->addState(s3);

    machine->setInitialState(s1);
    machine->start();

    // position slider
    slider_position = new QSlider(Qt::Horizontal,this);
    slider_position->setMinimum(0);
    slider_volume->setSingleStep(1);
    slider_volume->setPageStep(15);
    current_position = new QLabel("",this);
    max_position = new QLabel("",this);

    current_track = new QLabel("Музыка не запущена",this);

    // настройка layout
    setLayout(new QVBoxLayout);
    QHBoxLayout *btn_layout = new QHBoxLayout;
    btn_layout->addWidget(btn_add);
    btn_layout->addWidget(btn_previous);
    btn_layout->addWidget(btn_play);
    btn_layout->addWidget(btn_pause);
    btn_layout->addWidget(btn_stop);
    btn_layout->addWidget(btn_next);
    btn_layout->addWidget(btn_mode);
    layout()->addItem(btn_layout);


    QHBoxLayout *btn2_layout = new QHBoxLayout;
    btn2_layout->addWidget(slider_volume);
    btn2_layout->addWidget(current_volume);

    QHBoxLayout *btn3_layout = new QHBoxLayout;
    btn3_layout->addWidget(current_position);
    btn3_layout->addWidget(slider_position);
    btn3_layout->addWidget(max_position);


    layout()->addItem(btn2_layout);
    layout()->addItem(btn3_layout);
    layout()->addWidget(current_track);


    // Table View + model
    playlist_view = new QTableView(this);
    m_playlist_model = new QStandardItemModel(this);
    playlist_view->setModel(m_playlist_model);
    m_playlist_model->setHorizontalHeaderLabels(QStringList()  << tr("Audio Track")
                                            << tr("File Path"));
    playlist_view->hideColumn(1);
    playlist_view->verticalHeader()->setVisible(false);
    playlist_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    playlist_view->setSelectionMode(QAbstractItemView::SingleSelection);
    playlist_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    playlist_view->horizontalHeader()->setStretchLastSection(true);

    QGridLayout *grid_layout = new QGridLayout;
    grid_layout->addWidget(playlist_view,0,0,2,1);
    layout()->addItem(grid_layout);

    // плеер
    m_player = new QMediaPlayer(this);
    m_playlist = new QMediaPlaylist(m_player);
    m_player->setPlaylist(m_playlist);
    slider_volume->setValue(70);
    current_volume->setText("70");// регулировка

    // connect buttons + player
    connect(btn_add,SIGNAL(clicked(bool)),this,SLOT(btn_add_clicked()));
    connect(btn_previous,SIGNAL(clicked(bool)),m_playlist,SLOT(previous()));
    connect(btn_next,SIGNAL(clicked(bool)),m_playlist,SLOT(next()));
    connect(btn_play,SIGNAL(clicked(bool)),m_player,SLOT(play()));
    connect(btn_pause,SIGNAL(clicked(bool)),m_player,SLOT(pause()));
    connect(btn_stop,SIGNAL(clicked(bool)),m_player,SLOT(stop()));

    // connect volume slider
    connect(slider_volume, &QSlider::valueChanged,[this](int value){
        current_volume->setText(QString::number(value));
        m_player->setVolume(value);
    });

    // connect action table_view
    connect(playlist_view, &QTableView::doubleClicked, [this](const QModelIndex &index){
        m_playlist->setCurrentIndex(index.row());
    });
    connect(m_playlist, &QMediaPlaylist::currentIndexChanged, [this](int index){
        current_track->setText(m_playlist_model->data(m_playlist_model->index(index, 0)).toString());
        playlist_view->setCurrentIndex(m_playlist_model->index(index, 0));
    });

    // connect position slider
    connect(m_player,&QMediaPlayer::durationChanged,[this](qint64 time){
       int  min = time / 60 / 1000;
       int  sec = time / 1000 % 60;
       max_position->setText(QString::number(min)+":"+QString::number(sec));
       slider_position->setMaximum(time/1000);
    });

    connect(m_player,&QMediaPlayer::positionChanged,[this](qint64 time){
       int  min = time / 60 / 1000;
       int  sec = time / 1000 % 60;
       current_position->setText(QString::number(min)+":"+QString::number(sec));
       slider_position->setValue(time/1000);
    });
    connect(slider_position, &QAbstractSlider::sliderReleased,[this](){
        m_player->setPosition(slider_position->value() * 1000);
    });

}

music_player::~music_player()
{
    delete  m_playlist_model;
    delete m_playlist;
    delete m_player;

}

void music_player::btn_add_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(this,
                                                      tr("Chose file to load"),
                                                      "",
                                                      "Audio files (*.mp3);; (*.*)"
                                                       );

    foreach (QString path,  files) {
        QList<QStandardItem *> items;
        items.append(new QStandardItem(QDir(path).dirName()));
        items.append(new QStandardItem(path));
        m_playlist_model->appendRow(items);
        m_playlist->addMedia(QUrl("file://"+path));
    }

}



