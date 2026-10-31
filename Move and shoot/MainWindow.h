#pragma once
#include <QWidget>//引入工具箱
#include <QPixmap>//图片库与绘图
#include<QPaintEvent>//绘制事件头文件
#include <QKeyEvent>//接受键盘事件
#include<QTimer>//闹钟
#include <QElapsedTimer>//加入高精度计时器
#include <QMediaPlayer>//播放器
#include <QAudioOutput>//输出声音
#include <QSet>//集合的建立
#include <QMouseEvent>//接受鼠标事件
#include <QTextDocument>//文字编辑
#include <QRect>
#include<cmath>
#include<vector>
#include <algorithm>
#include<random>

class MainWindow : public QWidget//MainWindow是基于QWidget库的一个空白画板
{
    Q_OBJECT//这是一个QT专属的宏，信号槽，时间定时触发函数，事件响应都要用到宏内定义的操作

public:
    MainWindow(QWidget* parent = nullptr);//构造函数，可以指定一个窗口和上级窗口，如果parent为空指针，那么该窗口作为一个顶部窗口使用，非空则将窗口嵌入parent里面（作为父对象的子对象）
    //主窗口构造函数的parent默认parent为空指针因为它就是底层
    ~MainWindow();
protected:
    void paintEvent(QPaintEvent* event) override; //告诉系统重写绘图事件
    void keyPressEvent(QKeyEvent* event) override; //重写键盘事件
    void keyReleaseEvent(QKeyEvent* event) override;//键松开事件
    void mousePressEvent(QMouseEvent* event) override;//鼠标事件
    void resetgame();//重置游戏
private:
    QPixmap background;//背景图片
    QPixmap black_area;//射击框图片
    QPixmap black_area1;//文本框

    bool game_over = false;

    QPointF m_pos;//物体的位置（x,y）
    int m_speed;//速度
    QTimer* m_timer;//闹钟
    int score;
    int self_blood=10;//自己血量
    QElapsedTimer m_elapsed;//一个高精度的秒表
    QMediaPlayer* m_player;
    QMediaPlayer* m_player_boss_1;
    QAudioOutput* m_audioOutput;
    QAudioOutput* m_audioOutput_boss_1;
    bool input_lock = false;

    QSet<int> m_pressedKeys;//记录当前所按下的所有按键
    int speedstatus;//高速/低速切换

    QList<QPointF> m_bullets;    // 存所有子弹当前的坐标
    int m_bulletSpeed = 600;    // 子弹飞行速度（像素/秒），比主角快
    int m_fireCooldown = 40;   // 冷却时间：40毫秒发一颗
    double m_fireAccumulator = 0.0; // 记录累积的时间（毫秒）
    QPixmap m_bulletImg; // 存放子弹图片
    bool m_isPaused = false; // 暂停状态标志，默认是 false（运行中）

    int m_skill_1_status = 0;//一技能的状态
    int m_skill_1_cooldown = 5000;//一技能冷却时间
    QList<QPointF>m_bullets_1;
    int m_bulletSpeed_1 = 600;
    double m_alltime_1 = 0.0;
    double m_fireAccumulator_1 = 0.0;
    QPixmap m_bulletImg_1;
    int rest_cool_1=0;//显示在频幕上的一技能冷却时间

    //二技能，每个转30度，一共12个为一组，分别是0，30，60，....
    int m_skill_2_status = 0;
    int m_skill_2_cooldown = 10000;
    int m_fireCooldown_2 = 200;
    QList<QVector<QPointF>>m_bullets_2;
    std::vector<std::vector<int>>list;
    int m_bulletSpeed_2 = 200;
    double m_alltime_2 = 0.0;
    double m_fireAccumulator_2 = 0.0;
    QPixmap m_bulletImg_2;
    QVector<QPointF>start_point;
    int rest_cool_2 = 0;//显示在频幕上的一技能冷却时间
    //三技能，扇形范围内的高速弹幕
    int m_skill_3_status = 0;
    int m_skill_3_cooldown = 10000;
    int m_fireCooldown_3 = 50;
    QList<QVector<QPointF>>m_bullets_3;
    std::vector<std::vector<int>>list_3;
    int m_bulletSpeed_3 = 800;
    double m_alltime_3 = 0.0;
    double m_fireAccumulator_3 = 0.0;
    QPixmap m_bulletImg_3;
    int rest_cool_3 = 0;//显示在频幕上的一技能冷却时间
    //自动敌机的运行
    std::mt19937 rng;
    QList<QPointF> auto_enemy;
    int enemy_total = 0;
    std::vector<double> angular_speed;
    std::vector<double> enemy_radius;
    std::vector<int> blood;
    int enemy_speed = 300;
    int radius = 300;
    //omega=1
    std::vector<double>theta;//储存转角
    QPixmap enemy_Img;
    int enemy_cooltime = 1000;//每一秒生成一个敌机
    double enemy_accumulator = 0.0;//最开始积累值为0

    QList<QVector<QPointF>>enemy_bullet;//自机狙
    //给每一个生成的敌机赋值一个随机数，在随机数所规定的位置，生成一段狙击自机的弹幕
    std::vector<double>random_position;//记录范围内的发射位置
    QPixmap enemy_bullet_Img;
    int enemy_bullet_speed = 800;//非常高速
    std::vector<std::vector<double>>direction;//记录发射方向
    std::vector<std::vector<int>>list_e;
    std::vector<bool>enemy_fire_cooldown;//敌机发射冷却防止反复发射
    //设计的每一组弹幕一共5发，只和当前敌机位置和自机位置有关，同时发射，速度从720-800分布

    QPointF boss_1;
    int boss_1_blood = 10000;
    QPixmap boss_1_Img;
    int boss_1_speed = 150;
    QList<QPointF> Wheel;
    int arm_length = 150;
    double theta_arm = 0;
    double angular_speed_arm = 10.0;
    double delta_vx = 0;
    double delta_vy = 0;
    int boss_1_init = 0;//表示一面boss战是否初始化
    int boss_1_status = 0;//表示一面boss战是否正式开始
    bool vol_status = false;//上一首bgm渐弱开启
    double vol = 0.5;//上一首bgm音量
    bool boss_1_end = false;//表示一面boss战完结

    QList<QPointF> boss_1_bullets;//在一个张角内随机分布的一个弹幕，设这个张角是（-15°~+15°）
    std::vector<double>boss_1_angle;//随机生成角度的列表
    std::vector<double>boss_1_bullets_speed;//随机生成的速度分布
    int boss_1_cooldown = 100;//敌机开火冷却
    double boss_1_accumulation;//敌机开火计时器
private slots://这是独有的语法，槽函数，专门接收信号的窗口
    void onTimerTick(); //定时器每次触发的函数，槽函数
};

