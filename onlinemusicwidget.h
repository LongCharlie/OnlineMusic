#ifndef ONLINEMUSICWIDGET_H
#define ONLINEMUSICWIDGET_H

#include <QWidget> // 用户界面基类
#include<QtNetwork/QNetworkRequest> //HTTP的URL请求管理类
#include<QtNetwork/QNetworkAccessManager> // 处理网络请求的主要类，管理网络操作
#include<QtNetwork/QNetworkReply>// 用于处理网络请求后的响应，包含响应的数据和相关信息
#include<QRect>// 处理矩形的位置和大小的类
#include<QEventLoop>// 提供事件循环，用于等待和处理事件
#include<QMediaPlayer>// 处理多媒体内容，支持音频和视频播放
#include<QMediaPlaylist>// 用于管理多媒体播放列表
#include<QtSql/QSqlDatabase>// 数据库连接的类，提供数据库访问功能
#include<QSqlQuery>// 执行 SQL 查询的类
#include<QSqlError>// 包含数据库错误信息的类
#include<QMessageBox>// 弹出消息框的类
#include<QTimer>// 提供定时器功能，用于定时触发事件
#include<math.h>// 数学函数库
#include<QTextBlock>// 文本块的处理类
#include<QFileDialog>// 提供对话框，用于选择文件或目录
#include<QJsonParseError>// JSON 解析错误类，用于处理 JSON 解析时的错误信息
#include<QJsonArray>// JSON 数组类，用于处理 JSON 数据中的数组
#include<QJsonObject>// JSON 对象类，用于处理 JSON 数据中的对象
#include<QLCDNumber>// 显示数字的 LCD 样式小部件
#include<QTextCursor>// 文本光标的处理类
QT_BEGIN_NAMESPACE
namespace Ui { class OnlineMusicWidget; }
QT_END_NAMESPACE

class OnlineMusicWidget : public QWidget
{
    Q_OBJECT

public:
    OnlineMusicWidget(QWidget *parent = nullptr);
    ~OnlineMusicWidget();
    //处理播放器背景
    void paintEvent(QPaintEvent *e);

    QMediaPlayer *PlayerObjects; //多媒体播放器对象
    QMediaPlaylist *PlayerList; //定义多媒体播放列表
    QByteArray QByteArySearchInfo; //存储搜索数据信息
    QString StreLocalMusicPath; //导入本地音乐路径
    QTextDocument *docTextObject; //处理富文本内容
    QTextBlock *qTextLine; //处理文本块指针
    QNetworkAccessManager *AccessManager;//访问网页
    //歌曲id
    int MusicID;
    //歌曲名称 歌手
    QString StrMusicName,StrSingerName;
    //标记
    int iPos;

public slots:
    //处理数据信息返回函数
    void HandleDataBackFunc(QNetworkReply *pReply);
    //处理LCDNumber控件时间变化
    void HandleLCDNumberTimeChangeFunc(qint64 SongDuration);
    //处理进度条控件变化
    void HandleProgressTimeChangeFunc(qint64 dration);
    //处理播放位置变化
    void HandelPositionChangeFunc(qint64 position);
    //处理播放歌曲


private slots:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    void on_Close_clicked();

    void on_Documents_clicked();

    void on_Play_clicked();

    void on_Pause_clicked();

    void on_Stop_clicked();

    void on_Piror_clicked();

    void on_next_clicked();

    void on_Volumn_clicked();

    void on_Mini_clicked();

    void on_Skin_clicked();

    void on_Search_clicked();

    void on_horizontalSlider_2_valueChanged(int value);

    void on_horizontalSlider_valueChanged(int value);

private:
    Ui::OnlineMusicWidget *ui;
    QPoint last;

protected:
    //音乐歌曲下载和播放
    void DownloadPlayer(QString alnum_id,QString hash);

    //访问HTTP网页
    void HttpAccessFunc(QString);

    //音乐的hash和ablum_id值解析(JSON)
    void HashJsonAnalysis(QByteArray);

    //解析出真正的音乐文件和歌词
    QString MusicJsonAnalysis(QByteArray);

};

#endif // ONLINEMUSICWIDGET_H
