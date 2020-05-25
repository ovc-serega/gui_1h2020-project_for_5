#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QTableView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QSlider>
#include <QStateMachine>

#include <QMediaPlayer>
#include <QMediaPlaylist>

#include <QStandardItemModel>
#include <QHeaderView>


class music_player : public QWidget
{
    Q_OBJECT

public:
    music_player(QWidget *parent = nullptr);
    ~music_player();

private slots:
    void btn_add_clicked();


private:
    QPushButton     *btn_add;
    QPushButton     *btn_next;
    QPushButton     *btn_previous;
    QPushButton     *btn_play;
    QPushButton     *btn_pause;
    QPushButton     *btn_stop;
    QPushButton     *btn_mode;
    QSlider         *slider_volume;
    QSlider         *slider_position;
    QLabel          *current_track;
    QLabel          *current_volume;
    QLabel          *current_position;
    QLabel          *max_position;
    QTableView      *playlist_view;
    QMediaPlayer    *m_player;
    QMediaPlaylist  *m_playlist;

    QStandardItemModel  *m_playlist_model;


};
#endif // MUSIC_PLAYER_H
