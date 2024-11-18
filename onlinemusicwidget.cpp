#include "onlinemusicwidget.h"
#include "ui_onlinemusicwidget.h"
#include<QPainter>
#include<QMouseEvent>
OnlineMusicWidget::OnlineMusicWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OnlineMusicWidget)
{
    ui->setupUi(this);
    //禁止窗口改变尺寸
    this->setFixedSize(this->geometry().size());
    // 去掉标题栏,去掉工具栏，窗口置顶
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);

    //光标交点定位到搜索歌曲控件当中
    ui->InputSong->setFocus();

    //初始化
    AccessManager=new QNetworkAccessManager(this);
    iPos=0;

    docTextObject=ui->OnlineSongList->document();

    //将控件设置为只读
    ui->OnlineSongList->setReadOnly(true);

    //初始化多媒体实例
    PlayerObjects=new QMediaPlayer(this);
    PlayerList=new QMediaPlaylist(this);

    //设置播放列表
    PlayerObjects->setMedia(PlayerList);
    //设置播放模式
    PlayerList->setPlaybackMode(QMediaPlaylist::Loop);

    //信号与槽处理
    connect(PlayerObjects,SIGNAL(positionChanged(qint64)),this,SLOT(HandleLCDNumberTimeChangeFunc(qint64)));
    connect(PlayerObjects,SIGNAL(positionChanged(qint64)),this,SLOT(HandelPositionChangeFunc(qint64)));
    connect(PlayerObjects,SIGNAL(durationChanged(qint64)),this,SLOT(HandleProgressTimeChangeFunc(qint64)));
    connect(AccessManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(HandleDataBackFunc(QNetworkReply*)));
}

OnlineMusicWidget::~OnlineMusicWidget()
{

    delete ui;
}

void OnlineMusicWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0,0,width(),height(),QPixmap(":/new/prefix2/image/back3.png"));

}

//鼠标释放
void OnlineMusicWidget::mousePressEvent(QMouseEvent *e)
{
    if(e->button()== Qt::LeftButton)	//鼠标左键释放
        last = e->globalPos();
}

//鼠标移动
void OnlineMusicWidget::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons()==Qt::LeftButton)
    {
        int dx=e->globalX()-last.x();
        int dy=e->globalY()-last.y();
        last=e->globalPos();
        move(x()+dx,y()+dy);
    }
}

//鼠标按下
void OnlineMusicWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton)	//鼠标左键按下
    {
        int dx = e->globalX() - last.x();
        int dy = e->globalY() - last.y();
        move(x()+dx, y()+dy);
    }

}

void OnlineMusicWidget::on_Close_clicked()
{
    this->close();
}

//添加本地MP3歌曲
void OnlineMusicWidget::on_Documents_clicked()
{
    QString strCurrentPaths=QDir::homePath();
    QString strDlgTitle="请选择音乐文件";
    QString strFilters="所有文件(*.*);;音频文件(*.mp3);;mp3文件(*.mp3)";
    //所有文件(*.*);;音频文件(*.mp3);;mp3文件(*.mps)

    QString strMp3FileList=QFileDialog::getOpenFileName(this,strDlgTitle,strCurrentPaths,strFilters);

    if(strMp3FileList.count()<1) return;
    for(int i=0;i<strMp3FileList.count();i++)
    {
        QString strFiles=strMp3FileList.at(i);
        //将音乐文件添加到播放列表
        PlayerList->addMedia(QUrl::fromLocalFile(strFiles));

        QFileInfo qFileInfo(strFiles);

        ui->OnlineSongList->append(qFileInfo.fileName());
    }

    //添加完毕立刻播放
    if(PlayerObjects->state()!=QMediaPlayer::PlayingState)
        PlayerList->setCurrentIndex(0);

    PlayerObjects->play();
}

void OnlineMusicWidget::on_Play_clicked()
{
    //如果是停止或暂停状态，则播放
    if(iPos==0)
    {
        PlayerObjects->play();
        iPos=1;
    }
    else if(PlayerList->currentIndex()<0)
    {
        PlayerList->setCurrentIndex(0);
    }
    PlayerObjects->play();
}

//暂停播放
void OnlineMusicWidget::on_Pause_clicked()
{
    if(iPos==1)
    {
        PlayerObjects->pause();
        iPos=0;
    }
}

//停止播放
void OnlineMusicWidget::on_Stop_clicked()
{
    PlayerObjects->stop();
}

//播放上一曲
void OnlineMusicWidget::on_Piror_clicked()
{
    int m_index=PlayerList->currentIndex();
    int m_indexsum=PlayerList->mediaCount();
    if(m_index>0)
    {
        m_index=m_index-1;
        PlayerList->setCurrentIndex(m_index);
    }
    else
    {
        m_index=m_indexsum-1;
        PlayerList->setCurrentIndex(m_index);
        PlayerObjects->play();
    }
}

//播放下一曲
void OnlineMusicWidget::on_next_clicked()
{
    int m_index=PlayerList->currentIndex();
    int m_indexsum=PlayerList->mediaCount();
    if(m_index<m_indexsum)
    {
        m_index=m_index+1;
        PlayerList->setCurrentIndex(m_index);
    }
    else
    {
        m_index=0;
        PlayerList->setCurrentIndex(m_index);
        PlayerObjects->play();
    }

}

//静音,图标之间切换
void OnlineMusicWidget::on_Volumn_clicked()
{
    bool state=PlayerObjects->isMuted();
    PlayerObjects->setMuted(!state);
    if(state)
    {
        ui->Volumn->setIcon(QIcon(":/new/prefix2/image/volumn.png"));
    }
    else
    {
        ui->Volumn->setIcon(QIcon(":/new/prefix2/image/silent.png"));
    }

}

void OnlineMusicWidget::on_Mini_clicked()
{

}

void OnlineMusicWidget::on_Skin_clicked()
{

}

//处理数据信息返回函数
void OnlineMusicWidget::HandleDataBackFunc(QNetworkReply *pReply)
{
    //读取网络回馈数据
    QByteArySearchInfo=pReply->readAll();

    QJsonParseError JsonPError; //定义错误信息对象

    //将json文本转换为json文件对象
    QJsonDocument RecvData=QJsonDocument::fromJson(QByteArySearchInfo,&JsonPError);
    if(JsonPError.error!=QJsonParseError::NoError)
    {
        QMessageBox::critical(this,"Prompt","搜索歌曲获取json格式错误",QMessageBox::Yes);
        return;
    }

    QJsonObject TotalJsonObject=RecvData.object();

    //列出json里面所有的key
    QStringList strKeys=TotalJsonObject.keys();

    if(strKeys.contains("result")) //有数据信息
    {
        //将带有result的数据内容提取后转换为对象
        QJsonObject resultobject=TotalJsonObject["result"].toObject();

        //存储所有keys
        QStringList strResultKeys=resultobject.keys();

        if(strResultKeys.contains("songs"))
        {
            QJsonArray array=resultobject["songs"].toArray();

            //通过for循环查找歌曲当中的字段信息
            for(auto isong:array)
            {
                QJsonObject obj1=isong.toObject();
                //获取歌曲id 歌名 歌手
                MusicID=obj1["id"].toInt();
                StrMusicName=obj1["name"].toString();

                QStringList strkeys=obj1.keys();

                if(strkeys.contains("artists"))
                {
                    QJsonArray artistsjsonarray=obj1["artists"].toArray();
                    for(auto js:artistsjsonarray)
                    {
                        QJsonObject obj2=js.toObject();
                        StrSingerName=obj2["name"].toString();
                    }
                }
            }
        }
    }

    QString strURL;
    strURL=QString("http://music.163.com/song/media/outer/url?id=%0").arg(MusicID);

    PlayerList->addMedia(QUrl(strURL));

    ui->OnlineSongList->append(StrMusicName+"-"+StrSingerName);
}

//处理LCDNumber控件时间变化
void OnlineMusicWidget::HandleLCDNumberTimeChangeFunc(qint64 SongDuration)
{
    int Second=SongDuration/1000;
    int Minute=Second/60;
    Second=Second%60;
    QString SongTime;
    if(Minute<10&&Second<10)
    {
        SongTime=QString::asprintf("0%d:0%d",Minute,Second);
    }
    else if(Minute<10&&Second>10)
    {
        SongTime=QString::asprintf("0%d:%d",Minute,Second);
    }
    else if(Minute>10&&Second<10)
    {
        SongTime=QString::asprintf("%d:0%d",Minute,Second);
    }
    else
    {
        SongTime=QString::asprintf("%d:%d",Minute,Second);
    }
    ui->lcdNumber->display(SongTime);
}
//处理进度条控件变化
void OnlineMusicWidget::HandleProgressTimeChangeFunc(qint64 dration)
{
    //设置进度条最大值
    ui->horizontalSlider->setMaximum(dration);

}
//处理播放位置变化
void OnlineMusicWidget::HandelPositionChangeFunc(qint64 position)
{
    //if(ui->horizontalSlider->isSliderDown())
    //    return;
    //}
    //设置滑块的位置
    //ui->horizontalSlider->setSliderPosition(position);

}


//搜索歌曲名称实现
void OnlineMusicWidget::on_Search_clicked()
{
    QString str1,str2;
    str1=ui->InputSong->text();

    str2="http://music.163.com/api/search/get/web?csrf_token=hlpretag=&hlposttag=&s={"+str1+"}&type=1&offset=0&total=true&limit=1";

    //定义请求对象
    QNetworkRequest networkRequest;
    //将请求格式设置给对应请求对象
    networkRequest.setUrl(str2);
    //使用AccessManager类对应的API发送GET请求并获取响应数据
    AccessManager->get(networkRequest); //请求处理
}

//音量
void OnlineMusicWidget::on_horizontalSlider_2_valueChanged(int value)
{
    PlayerObjects->setVolume(value);

}

//歌曲进度
void OnlineMusicWidget::on_horizontalSlider_valueChanged(int value)
{
    PlayerObjects->setPosition(value);
}
