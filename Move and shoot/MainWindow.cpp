#include "MainWindow.h"//包含头文件
#include<QPainter>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent) //先调用QWidget的构造函数
    //QWidget::QWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags())这是一个QWidget的构造函数形式
    //如果parent非空，这个构造函数就会把自己加入父对象的子列表中
    //parent是更底层的窗口：对于MainWindow的情况，本来就是最底层的窗口，默认不传参（空指针）
    {
    setFixedSize(1000, 800); // 设置窗口大小
    setWindowTitle(QStringLiteral("私の東方妖々夢")); // 窗口标题，QStringLiteral允许超出常规语言（英语和本机语言）传输，但是前提是编码保存Unicode (UTF-8 带签名) - 代码页 65001

    m_pos = QPoint(300, 750); // 初始位置
    m_speed = 300;            // 每秒移动 300 像素
    speedstatus = 1;          //初始默认快速模式
    score = 0;//初始的得分为0

    setFocusPolicy(Qt::StrongFocus); // 告诉窗口：接收键盘按键事件

    m_timer = new QTimer(this);//创建闹钟
    m_timer->setInterval(8); //触发一次时间
    connect(m_timer, &QTimer::timeout, this, &MainWindow::onTimerTick);//将闹钟信号和处理函数连起来
    m_elapsed.start();//启动秒表计时
    m_timer->start();//启用闹钟

    m_audioOutput = new QAudioOutput(this);
    m_audioOutput->setVolume(0.5); // 音量 50%

    m_audioOutput_boss_1 = new QAudioOutput(this);
    m_audioOutput_boss_1->setVolume(0.5); // 音量 50%

    m_audioOutput_2 = new QAudioOutput(this);
    m_audioOutput_2->setVolume(0.5);

    m_player = new QMediaPlayer(this);
    m_player->setAudioOutput(m_audioOutput);
    m_player->setSource(QUrl("qrc:/new/prefix1/music.mp3"));
    m_player->setLoops(QMediaPlayer::Infinite);
    m_player->play();

    m_player_boss_1 = new QMediaPlayer(this);
    m_player_boss_1->setAudioOutput(m_audioOutput_boss_1);
    m_player_boss_1->setSource(QUrl("qrc:/new/prefix9/boss_1_music.mp3"));
    m_player_boss_1->setLoops(QMediaPlayer::Infinite);
    //初始不播放

    m_player_2 = new QMediaPlayer(this);
    m_player_2->setAudioOutput(m_audioOutput_2);
    m_player_2->setSource(QUrl("qrc:/new/prefix10/music_2.mp3"));
    m_player_2->setLoops(QMediaPlayer::Infinite);

    m_bullets_2.reserve(100);
    m_bullets_3.reserve(100);

    QPixmap rawBullet(":/new/prefix2/bullet.png");
    m_bulletImg = rawBullet.scaled(14, 14, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_bulletImg_1 = rawBullet.scaled(14, 14, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_fireAccumulator = -200;//沉默期

    QPixmap rawPin(":/new/prefix5/pin.png");
    m_bulletImg_2 = rawPin;

    QPixmap rawBg(":/new/prefix3/background.png");
    background = rawBg.scaled(1000, 800, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//平滑缩放，不适应边框

    QPixmap rawBlackArea(":/new/prefix4/black_area.png");
    black_area = rawBlackArea.scaled(580, 780, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    black_area1 = rawBlackArea.scaled(390, 250, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QPixmap rawEnemy(":/new/prefix6/auto_enemy.png");
    enemy_Img = rawEnemy.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPixmap rawEnemy_bullet(":/new/prefix7/enemy_bullet.png");
    enemy_bullet_Img = rawEnemy_bullet.scaled(14, 14, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPixmap rawBoss_1(":/new/prefix8/boss_1.png");
    boss_1_Img = rawBoss_1.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    boss_1 = QPointF(300, -100);
    Wheel = { QPointF(145, -200),QPointF(445, -200) };

    QPixmap rawEnemy_2(":/new/prefix11/auto_enemy_2.png");
    auto_enemy_2_Img = rawEnemy_2.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPixmap rawEnemy_bullet_2(":/new/prefix12/enemy_bullet_2.png");
    enemy_bullet_2_Img = rawEnemy_bullet_2.scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    Gun_turret = QPointF(300, -100);
    QPixmap rawGun_turret(":/new/prefix13/Gun_turret.png");
    Gun_turret_Img = rawGun_turret.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}


MainWindow::~MainWindow()
{
}

void MainWindow::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);//初始化画笔，这个this指向目前操作的对象，本情况就是绑定主窗口MainWindow
    painter.drawPixmap(0, 0, background);
    painter.drawPixmap(10, 10, black_area);
    painter.drawPixmap(605, 50, black_area1);

    // 从资源文件里加载图片
    QPixmap pixmap(":/player.png");

    // 画出来，缩放到 20*20
    QPixmap scaled = pixmap.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);//长宽比保持，平滑缩放反锯齿，强行缩小成最大宽高为 50x50 的图片
    painter.drawPixmap(qRound(m_pos.x() - 10), qRound(m_pos.y() - 10), scaled);//落笔位置（左上角坐标）

    QRect totalRect(10, 10, 590, 790);
    painter.setClipRect(totalRect);//开启裁剪
    painter.drawPixmap(qRound(boss_1.x() - 20), qRound(boss_1.y() - 20), boss_1_Img);

    QPen pen_arm;
    pen_arm.setColor(Qt::red);
    pen_arm.setWidth(4);
    painter.setPen(pen_arm);
    painter.drawLine(Wheel[0].x(), Wheel[0].y(), Wheel[0].x() + arm_length * cos(theta_arm), Wheel[0].y() + arm_length * sin(theta_arm));
    painter.drawLine(Wheel[0].x(), Wheel[0].y(), Wheel[0].x() - arm_length * cos(theta_arm), Wheel[0].y() - arm_length * sin(theta_arm));
    painter.drawLine(Wheel[0].x(), Wheel[0].y(), Wheel[0].x() - arm_length * sin(theta_arm), Wheel[0].y() + arm_length * cos(theta_arm));
    painter.drawLine(Wheel[0].x(), Wheel[0].y(), Wheel[0].x() + arm_length * sin(theta_arm), Wheel[0].y() - arm_length * cos(theta_arm));
    painter.drawLine(Wheel[1].x(), Wheel[1].y(), Wheel[1].x() + arm_length * cos(-theta_arm), Wheel[1].y() + arm_length * sin(-theta_arm));
    painter.drawLine(Wheel[1].x(), Wheel[1].y(), Wheel[1].x() - arm_length * cos(-theta_arm), Wheel[1].y() - arm_length * sin(-theta_arm));
    painter.drawLine(Wheel[1].x(), Wheel[1].y(), Wheel[1].x() + arm_length * sin(-theta_arm), Wheel[1].y() - arm_length * cos(-theta_arm));
    painter.drawLine(Wheel[1].x(), Wheel[1].y(), Wheel[1].x() - arm_length * sin(-theta_arm), Wheel[1].y() + arm_length * cos(-theta_arm));
    painter.setClipping(false);//关闭裁剪

    for (const QPointF& bullet : m_bullets) {
        // 因为 m_bullets 里存的是主角发射时的中心点，画图时要往左上角退一点（画图传入的坐标是左上角坐标）
        painter.drawPixmap(qRound(bullet.x() - 7), qRound(bullet.y() - 7), m_bulletImg);
    }

    for (const QPointF& bullet : m_bullets_1) {
        painter.drawPixmap(qRound(bullet.x() - 7), qRound(bullet.y() - 7), m_bulletImg_1);
    }

    for (int i = 0; i < m_bullets_2.size();i++) {
        const QVector<QPointF>& bullet_set= m_bullets_2[i];
        painter.save();
        if (bullet_set.size() != 0) {
            for (int j = 0; j < bullet_set.size(); j++) {
                if (list[i][j] != -1) {
                    painter.save();
                    painter.setRenderHint(QPainter::SmoothPixmapTransform);//最高画质反锯齿
                    painter.translate(bullet_set[j].x(), bullet_set[j].y());//平移坐标系
                    painter.rotate(15 * j);
                    painter.drawPixmap(-10, -10, 20, 20, m_bulletImg_2);
                    painter.restore();
                }
            }
        }
        painter.restore();
    }

    for (int i = 0; i < m_bullets_3.size(); i++) {
        const QVector<QPointF>& bullet_set_3 = m_bullets_3[i];
        painter.save();
        if (bullet_set_3.size() != 0) {
            for (int j = 0; j < bullet_set_3.size(); j++) {
                if (list_3[i][j] != -1) {
                    painter.save();
                    painter.setRenderHint(QPainter::SmoothPixmapTransform);//最高画质反锯齿
                    painter.translate(bullet_set_3[j].x(), bullet_set_3[j].y());//平移坐标系ws
                    painter.rotate(3 * (j - 5));
                    painter.drawPixmap(-10, -10, 20, 20, m_bulletImg_2);//3技能弹幕和2技能弹幕一致
                    painter.restore();
                }
            }
        }
        painter.restore();
    }

    for (int i = 0; i < auto_enemy.size();i++) {
        const QPointF enemy = auto_enemy[i];
        if (enemy.x() >= 10 && enemy.x() <= 590 && enemy.y() >= 10 && enemy.y() <= 790) {
            if (blood[i] <= 0) continue;
            painter.drawPixmap(qRound(enemy.x() - 20), qRound(enemy.y() - 20), enemy_Img);
        }
    }

    for (int i = 0; i < enemy_bullet.size(); i++) {
        for (int j = 0; j < enemy_bullet[i].size(); j++) {
            const QPointF bullet_e = enemy_bullet[i][j];
            if (list_e[i][j] == -1) continue;
            painter.drawPixmap(qRound(bullet_e.x() - 7), qRound(bullet_e.y() - 7), enemy_bullet_Img);
        }
    }

    for (const QPointF& bullet : boss_1_bullets) {
        painter.drawPixmap(qRound(bullet.x() - 7), qRound(bullet.y() - 7), enemy_bullet_Img);
    }

    for (const QPointF& enemy_2 : auto_enemy_2) {
        QRect totalRect(10, 10, 580, 780);
        painter.setClipRect(totalRect);//开启裁剪
        painter.drawPixmap(qRound(enemy_2.x() - 20), qRound(enemy_2.y() - 20), auto_enemy_2_Img);
        painter.setClipping(false);//关闭裁剪
    }

    for (int i = 0; i < enemy_bullet_2.size(); i++) {
        for (int j = 0; j < enemy_bullet_2[i].size(); j++) {
            const QPointF bullet_e = enemy_bullet_2[i][j];
            painter.drawPixmap(qRound(bullet_e.x() - 8), qRound(bullet_e.y() - 8), enemy_bullet_2_Img);
        }
    }

    painter.setClipRect(totalRect);//开启裁剪
    painter.save();
    painter.setRenderHint(QPainter::SmoothPixmapTransform);//最高画质反锯齿
    painter.translate(Gun_turret.x(), Gun_turret.y());//平移坐标系ws
    painter.rotate(-angle_now_2*180/M_PI);
    painter.drawPixmap(-20, -20, 40, 40, Gun_turret_Img);
    painter.restore();
    painter.setClipping(false);//关闭裁剪

    for (const QPointF& bullet : Gun_bullet_2) {
        painter.drawPixmap(qRound(bullet.x() - 7), qRound(bullet.y() - 7), enemy_bullet_Img);
    }

    QPen pen;//建立一个画笔
    pen.setColor(Qt::red);//画笔颜色
    pen.setWidth(4);//线宽度（单位是像素）
    painter.setPen(pen);//把画笔交给绘制器

    painter.drawLine(10, 10, 10, 790);
    painter.drawLine(10, 10, 590, 10);
    painter.drawLine(10, 790, 590, 790);
    painter.drawLine(590, 10, 590, 790);//这就是移动边界

    QPen pen1;
    pen1.setColor(Qt::blue);
    pen1.setWidth(3);
    painter.setPen(pen1);

    painter.drawLine(605, 50, 995, 50);
    painter.drawLine(605, 50, 605, 300);
    painter.drawLine(995, 50, 995, 300);
    painter.drawLine(605, 300, 995, 300);

    QRect textRect(605,50,385,250);//确定盒子模型的位置，左上角坐标-宽高
    QString s_cd1 = QString::number(ceil(1.0 * rest_cool_1 / 100) / 10);
    QString s_cd2 = QString::number(ceil(1.0 * rest_cool_2 / 100) / 10);
    QString s_cd3 = QString::number(ceil(1.0 * rest_cool_3 / 100) / 10);
    QString s_score = QString::number(score);
    QString s_blood = QString::number(self_blood);
    QString htmlCode = "<div style='color:#FFFFFF;font-family:KaiTi;font-size:70px; text-align: center;'><b>東方妖々夢</b></div>"
        "<div style='color:#FFFFFF;font-family:KaiTi;font-size:25px;text-align:left;'>"
        "<b>技能一冷却剩余：" + s_cd1 + " s</b><br>"
        "<b>技能二冷却剩余：" + s_cd2 + " s</b><br>"
        "<b>技能三冷却剩余：" + s_cd3 + " s</b><br>"
        "<b>得分：" + s_score + "</b><br>"
        "<b>剩余血量：" + s_blood + "</b><br>";
    if (boss_1_init==1&&boss_1_end==false) {
        htmlCode+= "<b>一面boss剩余血量：""</b><br>";
        htmlCode+= "<b>"+QString::number(boss_1_blood) + "</b><br>";
    }
    htmlCode += "</div>";
    qDebug() << "状态检查 -> init:" << boss_1_init << " end:" << boss_1_end;
    QTextDocument doc;
    doc.setHtml(htmlCode);//召唤QTextDocument解析器

    doc.setTextWidth(textRect.width());//设置排版宽度等于盒子宽度
    
    painter.save();//保存当前状态防止污染
    painter.translate(textRect.topLeft()); // 把画布平移到盒子位置
    doc.drawContents(&painter, QRectF(0, 0, textRect.width(), textRect.height()));  // 在平移后的原点开始渲染
    painter.restore();//恢复状态继续绘图

    if (game_over == true) {
        painter.fillRect(rect(), QColor(0, 0, 0, 180));
        QTextDocument doc_final;
        QString html = R"(
        <div style='color:#FFFFFF;font-family:KaiTi;font-size:70px; text-align: center;' ><b>GAME OVER</b></div>
        <div style='color:#FFFFFF;font-family:KaiTi;font-size:25px;text-align:left;'>
            <b>最终得分：%1 </b>
            <br>
            <b>点击重新开始</b>
        </div>
        )";
        html = html.arg(score);
        doc_final.setHtml(html);
        doc_final.setTextWidth(width());
        doc_final.drawContents(&painter, QRectF());
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    //这本来就是主窗口的一个成员函数，就不用传this绑定了，上一个需要绑定的原因是QPainter画笔本身不是成员函数，尽管绘画事件是
    if (input_lock == true) return;//如果此时出现过场动画，玩家控制权交还，锁定键盘
    if (event->isAutoRepeat()) return;//过滤长按重复
    if (event->key() == Qt::Key_Space) {//如果是空格键
        if (event->isAutoRepeat()) return;//过滤长按重复

        m_isPaused = !m_isPaused; // 切换状态：如果是暂停就变运行，如果是运行就变暂停

        if (m_isPaused) {
            m_timer->stop();  // 暂停主定时器（暂停主角移动、子弹移动、自动开火）
            m_player->pause(); // 暂停背景音乐（可选）
            qDebug() << "游戏已暂停";
        }
        else {
            m_elapsed.restart(); // 极其重要：恢复时重置时间差，防止瞬移！
            m_timer->start();    // 恢复主定时器
            m_player->play();    // 恢复音乐
            qDebug() << "游戏继续";
        }
        return;//拦截成功了，函数结束，这样就不会被加入键列表中
    }
    if (event->key() == Qt::Key_C) {//C键切换速度
        if (event->isAutoRepeat()) return;//过滤长按重复
        if (speedstatus == 1) {//高速切低速
            m_speed = 150;
        }
        else {//低速切高速
            m_speed = 300;
        }
        speedstatus = 1 - speedstatus;//改变记录的速度状态
        return;// 拦截成功了，函数结束，这样就不会被加入键列表中
    }
    if (event->key() == Qt::Key_1) {//1技能释放
        if (m_alltime_1 >= 0) {
            m_skill_1_status = 1;
            m_fireAccumulator_1 = 0;
            rest_cool_1 = 5000;
        }
        return;
    }
    if (event->key() == Qt::Key_2) {
        if (m_alltime_2 >= 0) {
            m_skill_2_status = 1;
            m_fireAccumulator_2 = 0;
            rest_cool_2 = 10000;
        }
        return;
    }
    if (event->key() == Qt::Key_3) {
        if (m_alltime_3 >= 0) {
            m_skill_3_status = 1;
            m_fireAccumulator_3 = 0;
            rest_cool_3 = 10000;
        }
        return;
    }
    m_pressedKeys.insert(event->key());
    QWidget::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    if (input_lock==true) {//如果松开，也无视，不接受任何键盘事件
        return;
    }
    if (event->isAutoRepeat()) return;
    if (event->key() == Qt::Key_C || event->key() == Qt::Key_Space||event->key()==Qt::Key_1) {
        return;//再次拦截
    }
    m_pressedKeys.remove(event->key());//如果不是空格键和C键
    QWidget::keyPressEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    if (game_over == true) {
        resetgame();//调用重置函数
        return;
    }

    if (event->button() == Qt::LeftButton) {
        m_isPaused = !m_isPaused; // 切换状态：如果是暂停就变运行，如果是运行就变暂停

        if (m_isPaused) {
            m_timer->stop();  // 暂停主定时器（暂停主角移动、子弹移动、自动开火）
            m_player->pause(); // 暂停背景音乐（可选）
            qDebug() << "游戏已暂停";
        }
        else {
            m_elapsed.restart(); // 极其重要：恢复时重置时间差，防止瞬移！
            m_timer->start();    // 恢复主定时器
            m_player->play();    // 恢复音乐
            qDebug() << "游戏继续";
        }
    }
}

void MainWindow::resetgame() {
    game_over = false;
    speedstatus = 1;
    m_speed = 300;
    self_blood = 10;
    score = 0;
    m_alltime_1 = 0;
    m_alltime_2 = 0;
    m_alltime_3 = 0;
    rest_cool_1 = 0;
    rest_cool_2 = 0;
    rest_cool_3 = 0;
    m_skill_1_status = 0;
    m_skill_2_status = 0;
    m_skill_3_status = 0;
    enemy_accumulator = 0;
    enemy_total = 0;
    theta_arm = 0;
    input_lock = false;
    delta_vx = 0;
    delta_vy = 0;
    boss_1_init = 0;
    boss_1_status = 0;
    vol_status = false;
    vol = 0.5;
    boss_1_accumulation = 0;
    boss_1_blood = 10000;
    vol_boss_1_status = false;
    vol_boss_1 = 0.5;
    enemy_accumulator_2 = 0;
    enemy_total_2 = 0;
    reverse_2 = 1;
    Gun_bullet_accumulator = 0;
    angle_now_2 = 0;
    //所有实体数组清楚
    auto_enemy.clear(); blood.clear(); theta.clear(); random_position.clear(); enemy_fire_cooldown.clear();
    angular_speed.clear(); enemy_radius.clear();
    m_bullets.clear();
    m_bullets_1.clear();
    m_bullets_2.clear(); list.clear(); start_point.clear();
    m_bullets_3.clear(); list_3.clear();
    enemy_bullet.clear(); direction.clear(); list_e.clear();
    boss_1_bullets.clear(); boss_1_angle.clear(); boss_1_bullets_speed.clear();
    auto_enemy_2.clear(); blood_2.clear(); line_speed_2.clear(); direction_2.clear(); sign.clear();
    enemy_bullet_2.clear(); direction_bullet_2.clear(); random_position_2.clear(); cooldown_status_2.clear();
    Gun_bullet_2.clear(); direction_gun_2.clear();

    m_pos = QPointF(300, 750);
    boss_1 = QPointF(300, -100);
    Wheel = { QPointF(145, -200),QPointF(445, -200) };
    Gun_turret = QPointF(300, -100);
    m_elapsed.restart();
    m_audioOutput->setVolume(vol);
    m_player->play();
}

void MainWindow::onTimerTick()
{
    if (game_over == true) {
        update();
        m_player->pause();//停止音乐
        m_player->setPosition(0);//从头开始
        m_player_boss_1->pause();
        m_player_boss_1->setPosition(0);
        m_player_2->pause();
        m_player_2->setPosition(0);
        return;//直接返回
    }

    qint64 elapsed = m_elapsed.restart();//向秒表求数据：上次闹钟事件之后到这次闹钟事件过了多长时间
    double dt = elapsed / 1000.0; // 换算成秒

    //已经将键盘时间制成了一个表，可以直接读取
    int dx = 0;
    int dy = 0;
    //如果记录本里面有这些键：
    if (m_pressedKeys.contains(Qt::Key_Left) || m_pressedKeys.contains(Qt::Key_A)) {
        dx -= 1;
    }
    if (m_pressedKeys.contains(Qt::Key_Right) || m_pressedKeys.contains(Qt::Key_D)) {
        dx += 1;
    }
    if (m_pressedKeys.contains(Qt::Key_Up) || m_pressedKeys.contains(Qt::Key_W)) {
        dy -= 1;
    }
    if (m_pressedKeys.contains(Qt::Key_Down) || m_pressedKeys.contains(Qt::Key_S)) {
        dy += 1;
    }

    double length = std::sqrt(dx * dx + dy * dy);//速度矢量长度，之后归一化

    //根据方向、速度和时间，计算新坐标
    //qRound 是四舍五入，因为像素坐标必须是整数
    if (length > 0) {
        m_pos.setX(m_pos.x() + (dx / length * m_speed * dt));
        m_pos.setY(m_pos.y() + (dy / length * m_speed * dt));
    }
    for (int i = 0; i < m_bullets.size(); ++i) {
        m_bullets[i].setY(m_bullets[i].y() - (m_bulletSpeed * dt)); // 向Y轴负方向（上）飞
    }
    if (m_skill_1_status == 0 && m_skill_2_status == 0&& m_skill_3_status==0) {//不释放技能，单列子弹
        m_fireAccumulator += dt * 1000;//累计时间
        if (m_fireAccumulator >= m_fireCooldown) {
            m_bullets.append(m_pos); // 在主角当前位置生成一颗子弹
            m_fireAccumulator -= m_fireCooldown;
        }//重置冷却时间，有一些零头，保留它们使得子弹产生结果尽量平均
    }
    for (int i = m_bullets.size() - 1; i >= 0; --i) {
        if (m_bullets[i].y() < 10) { // 飞到屏幕上方外了
            m_bullets.removeAt(i);
        }
    }
    //以下都是一技能内容
    if (m_alltime_1 >= 3000) {
        m_skill_1_status = 0;
        m_alltime_1 = -2000;
    }
    if (m_alltime_1 < 0) {
        if (m_alltime_1 + dt * 1000 >= 0) {
            m_alltime_1 = 0;
        }
        else {
            m_alltime_1 += dt * 1000;
        }
    }
    if (rest_cool_1 > 0) {
        rest_cool_1 -= dt*1000;
    }
    else {
        rest_cool_1 = 0;
    }
    for (int i = 0; i < m_bullets_1.size(); ++i) {
        m_bullets_1[i].setY(m_bullets_1[i].y() - (m_bulletSpeed_1 * dt)); // 向Y轴负方向（上）飞
    }
    if (m_skill_1_status == 1) {
        m_fireAccumulator_1 += dt * 1000;//累计时间
        if (m_fireAccumulator_1 > 0) {
            m_alltime_1 += dt * 1000;//总时间
        }
        if (m_fireAccumulator_1 >= m_fireCooldown) {
            QPointF m_pos_left = QPoint(m_pos.x() - 5, m_pos.y());
            QPointF m_pos_right = QPoint(m_pos.x() + 5, m_pos.y());
            m_bullets_1.append(m_pos_left);
            m_bullets_1.append(m_pos_right);
            m_fireAccumulator_1 -= m_fireCooldown;
        }//重置冷却时间，有一些零头，保留它们使得子弹产生结果尽量平均
    }
    for (int i = m_bullets_1.size() - 1; i >= 0; --i) {
        if (m_bullets_1[i].y() < 10) { // 飞到屏幕上方外了
            m_bullets_1.removeAt(i);
        }
    }
    //以下都是二技能内容
    if (m_alltime_2 >= 3000) {
        m_skill_2_status = 0;
        m_alltime_2 = -7000;
    }
    if (m_alltime_2 < 0) {
        if (m_alltime_2 + dt * 1000 >= 0) {
            m_alltime_2 = 0;
        }
        else {
            m_alltime_2 += dt * 1000;
        }
    }
    if (rest_cool_2 > 0) {
        rest_cool_2 -= dt*1000;
    }
    else {
        rest_cool_2 = 0;
    }
    for (int i = 0; i < m_bullets_2.size(); ++i) {
        for (int j = 0; j < m_bullets_2[i].size(); ++j) {//30度为一界限
            if (list[i][j] != -1) {
                double distance = std::sqrt((m_bullets_2[i][j].x() - start_point[i].x()) * (m_bullets_2[i][j].x() - start_point[i].x()) + (m_bullets_2[i][j].y() - start_point[i].y()) * (m_bullets_2[i][j].y() - start_point[i].y()));//从出发点到现在位置的距离
                double speed = m_bulletSpeed_2;
                if (distance > 200) {
                    speed = 200 * m_bulletSpeed_2 / distance;
                }
                double delta_x = sin((M_PI / 12) * j) * speed * dt;
                double delta_y = -cos((M_PI / 12) * j) * speed * dt;
                m_bullets_2[i][j].setX(m_bullets_2[i][j].x() + delta_x);
                m_bullets_2[i][j].setY(m_bullets_2[i][j].y() + delta_y);
            }
        }
    }
    if (m_skill_2_status == 1) {
        m_fireAccumulator_2 += dt * 1000;//累计时间
        if (m_fireAccumulator_2 > 0) {
            m_alltime_2 += dt * 1000;//总时间
        }
        if (m_fireAccumulator_2 >= m_fireCooldown_2) {
            QVector<QPointF>m_pos_t;
            for (int i = 0; i < 24; i++) {
                m_pos_t.append(QPointF(m_pos.x(), m_pos.y()));
            }
            m_bullets_2.append(m_pos_t);
            start_point.append(m_pos);//这是起始位置
            list.push_back({ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23 });//一个表
            m_fireAccumulator_2 -= m_fireCooldown_2;
        }//重置冷却时间，有一些零头，保留它们使得子弹产生结果尽量平均
    }
    for (int i = m_bullets_2.size() - 1; i >= 0; --i) {
        for (int j = 0; j < m_bullets_2[i].size(); j++)
            if (m_bullets_2[i][j].y() < 10 || m_bullets_2[i][j].y() > 790 || m_bullets_2[i][j].x() < 10 || m_bullets_2[i][j].x() > 590) { // 飞到屏幕上方外了
                list[i][j] = -1;//先暂存不显示，等这一组个全部出界再一起清除
            }
        int count = 0;
        for (int j = 0; j < m_bullets_2[i].size(); j++) {
            if (list[i][j] == -1) count++;
        }
        if (count == 24) {
            m_bullets_2.removeAt(i);
            list.erase(list.begin() + i);
            start_point.removeAt(i);
        }
    }

    //以下都是三技能内容
    if (m_alltime_3 >= 1000) {
        m_skill_3_status = 0;
        m_alltime_3 = -9000;
    }
    if (m_alltime_3 < 0) {
        if (m_alltime_3 + dt * 1000 >= 0) {
            m_alltime_3 = 0;
        }
        else {
            m_alltime_3 += dt * 1000;
        }
    }
    if (rest_cool_3 > 0) {
        rest_cool_3 -= dt*1000;
    }
    else {
        rest_cool_3 = 0;
    }
    for (int i = 0; i < m_bullets_3.size(); ++i) {
        for (int j = 0; j < m_bullets_3[i].size(); ++j) {//30度散射角，-15°~+15°，范围内每组发5发
            if (list_3[i][j] != -1) {
                double delta_x;
                double delta_y;
                if (j < 5) {
                    delta_x = sin(2 * M_PI + (M_PI / 60) * (j - 5)) * m_bulletSpeed_3 * dt;
                    delta_y = -cos(2 * M_PI + (M_PI / 60) * (j - 5)) * m_bulletSpeed_3 * dt;
                }
                else {
                    delta_x = sin((M_PI / 60) * (j - 5)) * m_bulletSpeed_3 * dt;
                    delta_y = -cos((M_PI / 60) * (j - 5)) * m_bulletSpeed_3 * dt;
                }
                m_bullets_3[i][j].setX(m_bullets_3[i][j].x() + delta_x);
                m_bullets_3[i][j].setY(m_bullets_3[i][j].y() + delta_y);
            }
        }
    }
    if (m_skill_3_status == 1) {
        m_fireAccumulator_3 += dt * 1000;//累计时间
        if (m_fireAccumulator_3 > 0) {
            m_alltime_3 += dt * 1000;//总时间
        }
        if (m_fireAccumulator_3 >= m_fireCooldown_3) {
            QVector<QPointF>m_pos_t_3;
            for (int i = 0; i < 11; i++) {
                m_pos_t_3.append(QPointF(m_pos.x(), m_pos.y()));
            }
            m_bullets_3.append(m_pos_t_3);
            list_3.push_back({ 0,1,2,3,4,5,6,7,8,9,10 });//一个表
            m_fireAccumulator_3 -= m_fireCooldown_3;
        }//重置冷却时间，有一些零头，保留它们使得子弹产生结果尽量平均
    }
    for (int i = m_bullets_3.size() - 1; i >= 0; --i) {
        for (int j = 0; j < m_bullets_3[i].size(); j++)
            if (m_bullets_3[i][j].y() < 10 || m_bullets_3[i][j].y() > 790 || m_bullets_3[i][j].x() < 10 || m_bullets_3[i][j].x() > 590) { // 飞到屏幕上方外了
                list_3[i][j] = -1;//先暂存不显示，等这一组11个全部出界再一起清除
            }
        int count_3 = 0;
        for (int j = 0; j < m_bullets_3[i].size(); j++) {
            if (list_3[i][j] == -1) count_3++;
        }
        if (count_3 == 11) {
            m_bullets_3.removeAt(i);
            list_3.erase(list_3.begin() + i);
        }
    }

    //以下是自动敌机移动，绘制与删除
    enemy_accumulator += dt * 1000;
    for (int i = 0; i < auto_enemy.size(); i++) {
        theta[i] += angular_speed[i] * dt;
        auto_enemy[i].setX(300 + enemy_radius[i] * cos(theta[i]));
        auto_enemy[i].setY(50 + enemy_radius[i] * sin(theta[i]));
    }
    if (enemy_accumulator >= enemy_cooltime && auto_enemy.size() < 10 && enemy_total < 50) {//生成一个敌机
        enemy_total++;
        std::uniform_real_distribution<double> radius_dist(0.95, 1.05);
        enemy_radius.push_back(radius * radius_dist(rng));
        auto_enemy.append(QPointF(300 + enemy_radius[enemy_radius.size() - 1], 50));
        blood.push_back(100);//初始血量为100
        theta.push_back(0);
        std::uniform_real_distribution<double> speed_dist(0.9, 1.1);
        double base_speed = enemy_speed / radius;//基本（平均）角速度
        angular_speed.push_back(base_speed * speed_dist(rng));
        std::uniform_real_distribution<double> random_pos(0.2, 0.8);
        random_position.push_back(10 + (590 - 10) * random_pos(rng));//发射自机狙的位置
        enemy_fire_cooldown.push_back(true);
        enemy_accumulator -= enemy_cooltime;
    }

    for (int i = 0; i < enemy_bullet.size(); i++) {
        for (int j = 0; j < enemy_bullet[i].size(); j++) {
            double delta_x = (enemy_bullet_speed - 20 * j) * dt * direction[i][0];
            double delta_y = (enemy_bullet_speed - 20 * j) * dt * direction[i][1];
            enemy_bullet[i][j].setX(enemy_bullet[i][j].x() + delta_x);
            enemy_bullet[i][j].setY(enemy_bullet[i][j].y() + delta_y);
        }
    }
    for (int i = 0; i < auto_enemy.size(); i++) {
        if (std::abs(auto_enemy[i].x()- random_position[i])<=10&&enemy_fire_cooldown[i]==true) {//敌机已经到位且开火冷却完成
            double delta_x = -auto_enemy[i].x() + m_pos.x();
            double delta_y = -auto_enemy[i].y() + m_pos.y();
            direction.push_back({ delta_x / std::sqrt(delta_x * delta_x + delta_y * delta_y),delta_y / std::sqrt(delta_x * delta_x + delta_y * delta_y) });
            enemy_bullet.append({ QPointF(auto_enemy[i].x(),auto_enemy[i].y()),QPointF(auto_enemy[i].x(),auto_enemy[i].y()) ,QPointF(auto_enemy[i].x(),auto_enemy[i].y()) ,QPointF(auto_enemy[i].x(),auto_enemy[i].y()) ,QPointF(auto_enemy[i].x(),auto_enemy[i].y()) });//生成5发子弹
            list_e.push_back({ 0,1,2,3,4 });
            enemy_fire_cooldown[i] = false;
        }
        if (std::abs(auto_enemy[i].x() - random_position[i]) > 10&& enemy_fire_cooldown[i] == false) {//敌机重新上膛
            enemy_fire_cooldown[i] = true;
        }
    }
    for (int i = enemy_bullet.size() - 1; i >= 0; --i) {
        for (int j = 0; j < enemy_bullet[i].size(); j++) {
            if (enemy_bullet[i][j].x() < 10 || enemy_bullet[i][j].x() > 590 || enemy_bullet[i][j].y() < 10 || enemy_bullet[i][j].y() > 790) {
                list_e[i][j] = -1;
            }
        }
        int count_e = 0;
        for (int j = 0; j < enemy_bullet[i].size(); j++) {
            if (list_e[i][j] == -1) {
                count_e++;
            }
        }
        if (count_e == 5) {
            enemy_bullet.remove(i);
            direction.erase(direction.begin() + i);
            list_e.erase(list_e.begin() + i);
        }
    }

    //碰撞定义
    std::vector<int>bullet_index;//要删除的子弹编号，起墓碑作用
    std::vector<int>bullet_index_1;
    //二技能和三技能的删除由list控制，因此只要给list赋值就可以了，相当于墓碑数组已经建立好了
    std::vector<int>enemy_del;//要删除的敌机的编号
    for (int i = 0; i < auto_enemy.size(); i++) {//对第i个敌机
        if (auto_enemy[i].x() < 10 || auto_enemy[i].x() > 590 || auto_enemy[i].y() < 10 || auto_enemy[i].y() > 790) continue;
        for (int j = 0; j < m_bullets.size(); j++) {//对通常子弹
            int square_distance =((-m_bullets[j].x() + auto_enemy[i].x()) * (-m_bullets[j].x() + auto_enemy[i].x()) + (-m_bullets[j].y() + auto_enemy[i].y()) * (-m_bullets[j].y() + auto_enemy[i].y()));//这是子弹中心和敌机中心距离
            if (square_distance >= 0 && square_distance <= 100) {//10像素以内判定碰撞
                if (blood[i] >0) {
                    bullet_index.push_back(j);
                    blood[i] -= 10;
                    if (blood[i] <= 0) {//敌机血量归0
                        enemy_del.push_back(i);
                        break;
                    }
                }
            }
        }
        for (int j = 0; j < m_bullets_1.size(); j++) {//对一技能弹幕子弹
            int square_distance = ((-m_bullets_1[j].x() + auto_enemy[i].x()) * (-m_bullets_1[j].x() + auto_enemy[i].x()) + (-m_bullets_1[j].y() + auto_enemy[i].y()) * (-m_bullets_1[j].y() + auto_enemy[i].y()));//这是子弹中心和敌机中心距离
            if (square_distance >= 0 && square_distance <= 100) {//10像素以内判定碰撞
                if (blood[i] > 0) {
                    bullet_index_1.push_back(j);
                    blood[i] -= 10;
                    if (blood[i] <= 0) {//敌机血量归0
                        enemy_del.push_back(i);
                        break;
                    }
                }
            }
        }
        for (int j = 0; j < m_bullets_2.size(); j++) {
            for (int k = 0; k < m_bullets_2[j].size(); k++) {
                if (list[j][k] != -1) {
                    int square_distance = ((-m_bullets_2[j][k].x() + auto_enemy[i].x()) * (-m_bullets_2[j][k].x() + auto_enemy[i].x()) + (-m_bullets_2[j][k].y() + auto_enemy[i].y()) * (-m_bullets_2[j][k].y() + auto_enemy[i].y()));//这是子弹中心和敌机中心距离
                    if (square_distance >= 0 && square_distance <= 100) {//10像素以内判定碰撞
                        if (blood[i] > 0) {
                            list[j][k] = -1;
                            blood[i] -= 10;
                            if (blood[i] <= 0) {//敌机血量归0
                                enemy_del.push_back(i);
                                break;
                            }
                        }
                    }
                }
            }
        }
        for (int j = 0; j < m_bullets_3.size(); j++) {
            for (int k = 0; k < m_bullets_3[j].size(); k++) {
                if (list_3[j][k] != -1) {
                    int square_distance = ((-m_bullets_3[j][k].x() + auto_enemy[i].x()) * (-m_bullets_3[j][k].x() + auto_enemy[i].x()) + (-m_bullets_3[j][k].y() + auto_enemy[i].y()) * (-m_bullets_3[j][k].y() + auto_enemy[i].y()));//这是子弹中心和敌机中心距离
                    if (square_distance >= 0 && square_distance <= 100) {//10像素以内判定碰撞
                        if (blood[i] > 0) {
                            list_3[j][k] = -1;
                            blood[i] -= 10;
                            if (blood[i] <= 0) {//敌机血量归0
                                enemy_del.push_back(i);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    std::sort(bullet_index.begin(), bullet_index.end());
    bullet_index.erase(std::unique(bullet_index.begin(), bullet_index.end()), bullet_index.end());
    std::sort(bullet_index_1.begin(), bullet_index_1.end());
    bullet_index_1.erase(std::unique(bullet_index_1.begin(), bullet_index_1.end()), bullet_index_1.end());
    std::sort(enemy_del.begin(), enemy_del.end());
    enemy_del.erase(std::unique(enemy_del.begin(), enemy_del.end()), enemy_del.end());
    for (int i = bullet_index.size() - 1; i >= 0; --i) {
        m_bullets.erase(m_bullets.begin() + bullet_index[i]);
    }
    bullet_index = {};
    for (int i = bullet_index_1.size() - 1; i >= 0; --i) {
        m_bullets_1.erase(m_bullets_1.begin() + bullet_index_1[i]);
    }
    bullet_index_1 = {};
    for (int i = enemy_del.size() - 1; i >= 0; --i) {
        auto_enemy.erase(auto_enemy.begin() + enemy_del[i]);
        blood.erase(blood.begin() + enemy_del[i]);
        theta.erase(theta.begin() + enemy_del[i]);
        angular_speed.erase(angular_speed.begin() + enemy_del[i]);
        enemy_radius.erase(enemy_radius.begin() + enemy_del[i]);
        random_position.erase(random_position.begin() + enemy_del[i]);
        enemy_fire_cooldown.erase(enemy_fire_cooldown.begin() + enemy_del[i]);
        score += 1;
    }
    enemy_del = {};

    //敌机弹幕和自机碰撞逻辑
    for (int i = 0; i < enemy_bullet.size(); i++) {
        for (int j = 0; j < enemy_bullet[i].size(); j++) {
            if (list_e[i][j] != -1) {
                int square_distance = (enemy_bullet[i][j].x() - m_pos.x()) * (enemy_bullet[i][j].x() - m_pos.x()) + (enemy_bullet[i][j].y() - m_pos.y()) * (enemy_bullet[i][j].y() - m_pos.y());
                if (square_distance >= 0 && square_distance <= 25) {
                    if (self_blood > 0) {
                        list_e[i][j] = -1;
                        self_blood -= 1;
                        if (self_blood == 0) {
                            game_over = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    //一面BOSS战初始化
    if (boss_1_init == 0 && score >= 50 && enemy_bullet.size() == 0 && m_bullets_1.size() == 0 && m_bullets_2.size() == 0 && m_bullets_3.size() == 0) {
        boss_1_init = 1;
        self_blood = 10;//奖励血量重置
        vol_status = true;//开始减弱过程
        input_lock = true;//玩家交换控制权
    }
    if (boss_1_status == 0 && boss_1_init == 1 && enemy_bullet.size() == 0 && m_bullets_1.size() == 0 && m_bullets_2.size() == 0 && m_bullets_3.size() == 0) {
        if (vol >= 0) {
            vol -= 0.125 * dt;
            m_audioOutput->setVolume(vol);
        }
        else {
            m_player->pause();
        }
        if (delta_vx <= 0 && delta_vy <= 0) {
            delta_vx = (300 - m_pos.x()) * 150 / 200.0;
            delta_vy = (750 - m_pos.y()) * 150 / 200.0;
        }
        if (boss_1.y() <= 100) {
            m_pressedKeys.clear();//清空所有键
            boss_1.setY(boss_1.y() + dt * boss_1_speed);
            m_pos.setX(m_pos.x() + delta_vx * dt);
            m_pos.setY(m_pos.y() + delta_vy * dt);
        }
        else {//boss已经就位
            for (int i = 0; i < Wheel.size(); i++) {
                delta_vx = 0;
                delta_vy = 0;
                if (Wheel[i].y() <= 790 - 145) {
                    Wheel[i].setY(Wheel[i].y() + dt * boss_1_speed);
                }
                else {
                    boss_1_status = 1;
                }
            }
        }
    }
    if (boss_1_status == 1) {//初始化完成，一面boss战开始
        //切换音乐
        m_player_boss_1->play();
        input_lock = false;//键盘控制权交还
        theta_arm += angular_speed_arm * dt;
        boss_1_accumulation += dt * 1000;
        //以下是自机和转轮臂的碰撞逻辑
        double left_x = m_pos.x() - 145;
        double right_x = m_pos.x() - 445;
        double y_diff = m_pos.y() - 790 + 145;
        if (left_x * left_x + y_diff * y_diff <= 22500 || right_x * right_x + y_diff * y_diff <= 22500) {//开始判定
            double theta_left = atan(y_diff / left_x);
            double theta_right = atan(y_diff / right_x);
            double k_left = (theta_arm - theta_left) / (M_PI / 2);
            double k_right = (theta_arm + theta_right) / (M_PI / 2);
            if (((k_left - ceil(k_left)) * ((k_left - ceil(k_left)) + angular_speed_arm * dt) < 0 || (k_left - floor(k_left)) * ((k_left - floor(k_left)) - angular_speed_arm * dt) < 0) && left_x * left_x + y_diff * y_diff <= 22500) {
                theta_arm = theta_left;
                self_blood = 0;
                game_over = true;
            }
            if (((k_right - ceil(k_right)) * ((k_right - ceil(k_right)) + angular_speed_arm * dt) < 0 || (k_right - floor(k_right)) * ((k_right - floor(k_right)) - angular_speed_arm * dt) < 0) && right_x * right_x + y_diff * y_diff <= 22500) {
                theta_arm = 2 * M_PI - theta_right;
                self_blood = 0;
                game_over = true;
            }
        }
        //以下是敌机弹幕非碰撞条件下增减逻辑
        for (int i = 0; i < boss_1_bullets.size(); i++) {
            boss_1_bullets[i].setX(boss_1_bullets[i].x() + boss_1_bullets_speed[i] * sin(boss_1_angle[i]) * dt);
            boss_1_bullets[i].setY(boss_1_bullets[i].y() + boss_1_bullets_speed[i] * cos(boss_1_angle[i]) * dt);
        }
        if (boss_1_accumulation >= boss_1_cooldown) {
            boss_1_accumulation -= boss_1_cooldown;
            std::uniform_real_distribution<double> boss_1_angle_list((-M_PI / 12), (M_PI / 12));
            std::uniform_real_distribution<double> boss_1_speed_list(150, 200);//速度在200~300之间的慢速弹幕
            boss_1_bullets.append(QPointF(boss_1.x(), boss_1.y()));
            boss_1_angle.push_back(boss_1_angle_list(rng));
            boss_1_bullets_speed.push_back(boss_1_speed_list(rng));
        }
        for (int i = boss_1_bullets.size() - 1; i >= 0; --i) {
            if (boss_1_bullets[i].x() < 10 || boss_1_bullets[i].x() > 590 || boss_1_bullets[i].y() > 790) {
                boss_1_bullets.removeAt(i);
                boss_1_angle.erase(boss_1_angle.begin() + i);
                boss_1_bullets_speed.erase(boss_1_bullets_speed.begin() + i);
            }
        }
        //以下是自机弹幕和一面boss碰撞逻辑
        for (int i = 0; i < m_bullets.size(); i++) {
            int square_distance = (m_bullets[i].x() - boss_1.x()) * (m_bullets[i].x() - boss_1.x()) + (m_bullets[i].y() - boss_1.y()) * (m_bullets[i].y() - boss_1.y());
            if (square_distance >= 0 && square_distance <= 100) {//开始判定
                if (boss_1_blood > 0) {
                    boss_1_blood -= 10;
                }
                bullet_index.push_back(i);
                if (boss_1_blood <= 0) {
                    boss_1_end = true;//boss战结束
                    break;
                }
            }
        }
        for (int i = 0; i < m_bullets_1.size(); i++) {
            int square_distance = (m_bullets_1[i].x() - boss_1.x()) * (m_bullets_1[i].x() - boss_1.x()) + (m_bullets_1[i].y() - boss_1.y()) * (m_bullets_1[i].y() - boss_1.y());
            if (square_distance >= 0 && square_distance <= 100) {//开始判定
                if (boss_1_blood > 0) {
                    boss_1_blood -= 10;
                }
                bullet_index_1.push_back(i);
                if (boss_1_blood <= 0) {
                    boss_1_end = true;//boss战结束
                    break;
                }
            }
        }
        for (int i = 0; i < m_bullets_2.size(); i++) {
            for (int j = 0; j < m_bullets_2[i].size(); j++) {
                if (list[i][j] != -1) {
                    int square_distance = (m_bullets_2[i][j].x() - boss_1.x()) * (m_bullets_2[i][j].x() - boss_1.x()) + (m_bullets_2[i][j].y() - boss_1.y()) * (m_bullets_2[i][j].y() - boss_1.y());
                    if (square_distance >= 0 && square_distance <= 100) {//开始判定
                        if (boss_1_blood > 0) {
                            boss_1_blood -= 10;
                        }
                        list[i][j] = -1;//建立墓碑
                        if (boss_1_blood <= 0) {
                            boss_1_end = true;//boss战结束
                            break;
                        }
                    }
                }
            }
        }
        for (int i = 0; i < m_bullets_3.size(); i++) {
            for (int j = 0; j < m_bullets_3[i].size(); j++) {
                if (list_3[i][j] != -1) {
                    int square_distance = (m_bullets_3[i][j].x() - boss_1.x()) * (m_bullets_3[i][j].x() - boss_1.x()) + (m_bullets_3[i][j].y() - boss_1.y()) * (m_bullets_3[i][j].y() - boss_1.y());
                    if (square_distance >= 0 && square_distance <= 100) {//开始判定
                        if (boss_1_blood > 0) {
                            boss_1_blood -= 10;
                        }
                        list_3[i][j] = -1;//建立墓碑
                        if (boss_1_blood <= 0) {
                            boss_1_end = true;//boss战结束
                            break;
                        }
                    }
                }
            }
        }
        std::sort(bullet_index.begin(), bullet_index.end());
        bullet_index.erase(std::unique(bullet_index.begin(), bullet_index.end()), bullet_index.end());
        std::sort(bullet_index_1.begin(), bullet_index_1.end());
        bullet_index_1.erase(std::unique(bullet_index_1.begin(), bullet_index_1.end()), bullet_index_1.end());
        for (int i = bullet_index.size() - 1; i >= 0; --i) {
            m_bullets.erase(m_bullets.begin() + bullet_index[i]);
        }
        bullet_index = {};
        for (int i = bullet_index_1.size() - 1; i >= 0; --i) {
            m_bullets_1.erase(m_bullets_1.begin() + bullet_index_1[i]);
        }
        bullet_index_1 = {};
        //以下是自机和敌机弹幕碰撞逻辑
        for (int i = boss_1_bullets.size() - 1; i >= 0; --i) {
            int square_distance = (boss_1_bullets[i].x() - m_pos.x()) * (boss_1_bullets[i].x() - m_pos.x()) + (boss_1_bullets[i].y() - m_pos.y()) * (boss_1_bullets[i].y() - m_pos.y());
            if (square_distance >= 0 && square_distance <= 25) {
                if (self_blood >= 0) {
                    self_blood--;
                    boss_1_bullets.removeAt(i);
                    boss_1_angle.erase(boss_1_angle.begin() + i);
                    boss_1_bullets_speed.erase(boss_1_bullets_speed.begin() + i);
                }
                if (self_blood <= 0) {
                    game_over = true;
                    break;
                }
            }
        }
        if (boss_1_end == true) {
            boss_1_status = 2;
            vol_boss_1_status = true;
        }
    }
    if (boss_1_status == 2) {//击败一面boss
        if (vol_boss_1 >= 0) {
            vol_boss_1 -= dt * 0.125;
            m_audioOutput_boss_1->setVolume(vol_boss_1);
        }
        else {
            m_player_boss_1->pause();
        }
        for (int i = 0; i < boss_1_bullets.size(); i++) {//剩下的子弹也还在运动，也还会判定碰撞
            boss_1_bullets[i].setX(boss_1_bullets[i].x() + boss_1_bullets_speed[i] * sin(boss_1_angle[i]) * dt);
            boss_1_bullets[i].setY(boss_1_bullets[i].y() + boss_1_bullets_speed[i] * cos(boss_1_angle[i]) * dt);
        }
        for (int i = boss_1_bullets.size() - 1; i >= 0; --i) {
            if (boss_1_bullets[i].x() < 10 || boss_1_bullets[i].x() > 590 || boss_1_bullets[i].y() > 790) {
                boss_1_bullets.removeAt(i);
                boss_1_angle.erase(boss_1_angle.begin() + i);
                boss_1_bullets_speed.erase(boss_1_bullets_speed.begin() + i);
            }
        }
        for (int i = boss_1_bullets.size() - 1; i >= 0; --i) {
            int square_distance = (boss_1_bullets[i].x() - m_pos.x()) * (boss_1_bullets[i].x() - m_pos.x()) + (boss_1_bullets[i].y() - m_pos.y()) * (boss_1_bullets[i].y() - m_pos.y());
            if (square_distance >= 0 && square_distance <= 25) {
                if (self_blood >= 0) {
                    self_blood -= 1;
                    boss_1_bullets.removeAt(i);
                    boss_1_angle.erase(boss_1_angle.begin() + i);
                    boss_1_bullets_speed.erase(boss_1_bullets_speed.begin() + i);
                }
                if (self_blood <= 0) {
                    game_over = true;
                    break;
                }
            }
        }
        if (boss_1.y() >= -100) {
            boss_1.setY(boss_1.y() - dt * boss_1_speed);
        }
        else {//boss已经就位
            for (int i = 0; i < Wheel.size(); i++) {
                if (Wheel[i].y() >= -200) {
                    Wheel[i].setY(Wheel[i].y() - dt * boss_1_speed);
                }
            }
        }
        if (Wheel[0].y() <= -200) {
            boss_1_status = 3;//1-3-true，一面boss战完全结束
            self_blood = 10;
        }
    }
    //二面道中
    if (boss_1_status == 3) {
        m_player_2->play();
        if (m_pos.y() >= Gun_turret.y()) {
            angle_now_2 = std::atan((m_pos.x() - Gun_turret.x()) / (m_pos.y() - Gun_turret.y()));
        }
        else {
            angle_now_2 = std::atan((m_pos.x() - Gun_turret.x()) / (m_pos.y() - Gun_turret.y())) + M_PI;
        }
        if (Gun_turret.y() < 100) {
            Gun_turret.setY(Gun_turret.y() + dt * 150);
        }
        else if (Gun_turret.y() > 100) {
            Gun_turret.setY(100);
        }
        enemy_accumulator_2 += dt * 1000;
        for (int i = 0; i < auto_enemy_2.size(); i++) {
            double delta_s_2 = dt * line_speed_2[i] * direction_2[i];
            if (sign[i] == -1) {//负轨道
                double delta_x_2 = delta_s_2 / (std::sqrt(1 + (25 / (625 - auto_enemy_2[i].x()))));
                if (auto_enemy_2[i].x() + delta_x_2 > 600 || auto_enemy_2[i].x() + delta_x_2 < 0) {//超出边界了，调头
                    if (auto_enemy_2[i].x() + delta_x_2 > 600) {
                        auto_enemy_2[i].setX(1200 - auto_enemy_2[i].x() - delta_x_2);
                    }
                    else {
                        auto_enemy_2[i].setX(-auto_enemy_2[i].x() - delta_x_2);
                    }
                    direction_2[i] = -direction_2[i];
                }
                else {
                    auto_enemy_2[i].setX(auto_enemy_2[i].x() + delta_x_2);
                }
                auto_enemy_2[i].setY(10 * std::sqrt(625 - auto_enemy_2[i].x()));
            }
            else {
                double delta_x_2 = delta_s_2 / (std::sqrt(1 + (25 / (25 + auto_enemy_2[i].x()))));
                if (auto_enemy_2[i].x() + delta_x_2 > 600 || auto_enemy_2[i].x() + delta_x_2 < 0) {//超出边界了，调头
                    if (auto_enemy_2[i].x() + delta_x_2 > 600) {
                        auto_enemy_2[i].setX(1200 - auto_enemy_2[i].x() - delta_x_2);
                    }
                    else {
                        auto_enemy_2[i].setX(-auto_enemy_2[i].x() - delta_x_2);
                    }
                    direction_2[i] = -direction_2[i];
                }
                else {
                    auto_enemy_2[i].setX(auto_enemy_2[i].x() + delta_x_2);
                }
                auto_enemy_2[i].setY(10 * std::sqrt(25 + auto_enemy_2[i].x()));
            }
        }
        if (enemy_accumulator_2 >= enemy_cooltime_2 && auto_enemy_2.size() < 10 && enemy_total_2 < 50) {
            enemy_total_2++;
            enemy_accumulator_2 -= enemy_cooltime_2;
            std::uniform_real_distribution<double>speed_list_2(150, 250);
            std::uniform_real_distribution<double>position_list_2(50, 550);
            line_speed_2.push_back(speed_list_2(rng));
            random_position_2.push_back(position_list_2(rng));
            enemy_bullet_2.append(QVector<QPointF>{});
            direction_bullet_2.push_back({});
            cooldown_status_2.push_back(true);
            blood_2.push_back(50);
            if (reverse_2 == 1) {//正轨道,y=10sqrt(x+25),(0,50)->(600,250)
                auto_enemy_2.append(QPointF(0, 50));
                direction_2.push_back(reverse_2);
                sign.push_back(1);
            }
            else {//负轨道,y=10sqrt(625-x),(600,50)->(0,250)
                auto_enemy_2.append(QPointF(600, 50));
                direction_2.push_back(reverse_2);
                sign.push_back(-1);
            }
            reverse_2 = -reverse_2;//反转态
        }
        //敌机子弹逻辑
        for (int i = 0; i < enemy_bullet_2.size(); i++) {
            for (int j = 0; j < enemy_bullet_2[i].size(); j++) {
                double delta_x;
                if (direction_bullet_2[i][j] > 0) {
                    delta_x = enemy_bullet_2_speed * dt * 1 / std::sqrt(1 + direction_bullet_2[i][j] * direction_bullet_2[i][j]);
                }
                else {
                    delta_x = -enemy_bullet_2_speed * dt * 1 / std::sqrt(1 + direction_bullet_2[i][j] * direction_bullet_2[i][j]);
                }
                double delta_y = enemy_bullet_2_speed * dt * std::abs(direction_bullet_2[i][j]) / std::sqrt(1 + direction_bullet_2[i][j] * direction_bullet_2[i][j]);
                if (enemy_bullet_2[i][j].x() + delta_x > 590) {
                    enemy_bullet_2[i][j].setX(1180 - (enemy_bullet_2[i][j].x() + delta_x));
                    direction_bullet_2[i][j] = -direction_bullet_2[i][j];
                }
                else if (enemy_bullet_2[i][j].x() + delta_x < 100) {
                    enemy_bullet_2[i][j].setX(20 - (enemy_bullet_2[i][j].x() + delta_x));
                    direction_bullet_2[i][j] = -direction_bullet_2[i][j];
                }
                else {
                    enemy_bullet_2[i][j].setX(enemy_bullet_2[i][j].x() + delta_x);
                }
                enemy_bullet_2[i][j].setY(enemy_bullet_2[i][j].y() + delta_y);
            }
        }
        for (int i = 0; i < auto_enemy_2.size(); i++) {//对所有的敌机
            if (sign[i] == 1 && direction_2[i] == 1) {//正轨道,y=10sqrt(x+25),(0,50)->(600,250),下降沿触发
                if (std::fmod(std::abs(auto_enemy_2[i].x() - random_position_2[i]), 200) <= 5 && cooldown_status_2[i] == true) {
                    cooldown_status_2[i] = false;
                    //正轨道上,导函数dy/dx=5/sqrt(x+25)
                    enemy_bullet_2[i].push_back(QPointF(auto_enemy_2[i].x(), auto_enemy_2[i].y()));
                    direction_bullet_2[i].push_back(5 / std::sqrt(25 + auto_enemy_2[i].x()));
                }
                else if (std::fmod(std::abs(auto_enemy_2[i].x() - random_position_2[i]), 200) >= 20 && cooldown_status_2[i] == false) {
                    cooldown_status_2[i] = true;
                }
            }
            else if (sign[i] == -1 && direction_2[i] == -1) {//负轨道,y=10sqrt(625-x),(600,50)->(0,250),下降沿触发
                if (std::fmod(std::abs(auto_enemy_2[i].x() - random_position_2[i]), 200) <= 5 && cooldown_status_2[i] == true) {
                    cooldown_status_2[i] = false;
                    enemy_bullet_2[i].push_back(QPointF(auto_enemy_2[i].x(), auto_enemy_2[i].y()));
                    direction_bullet_2[i].push_back(-5 / std::sqrt(625 - auto_enemy_2[i].x()));
                }
                else if (std::fmod(std::abs(auto_enemy_2[i].x() - random_position_2[i]), 200) >= 20 && cooldown_status_2[i] == false) {
                    cooldown_status_2[i] = true;
                }
            }
        }
        for (int i = 0; i < enemy_bullet_2.size(); i++) {
            for (int j = enemy_bullet_2[i].size() - 1; j >= 0; --j) {
                if (enemy_bullet_2[i][j].y() > 790) {
                    enemy_bullet_2[i].removeAt(j);
                    direction_bullet_2[i].erase(direction_bullet_2[i].begin() + j);
                }
            }
        }
        if (Gun_turret.y() == 100) {
            Gun_bullet_accumulator += dt * 1000;
        }
        for (int i = 0; i < Gun_bullet_2.size(); i++) {
            Gun_bullet_2[i].setX(Gun_bullet_2[i].x() + Gun_bullet_2_speed * dt * direction_gun_2[i][0]);
            Gun_bullet_2[i].setY(Gun_bullet_2[i].y() + Gun_bullet_2_speed * dt * direction_gun_2[i][1]);
        }
        if (Gun_bullet_accumulator >= Gun_bullet_cooldown_2) {
            Gun_bullet_accumulator -= Gun_bullet_cooldown_2;
            double delta_x = m_pos.x() - Gun_turret.x();
            double delta_y = m_pos.y() - Gun_turret.y();
            Gun_bullet_2.append(QPointF(300, 100));
            direction_gun_2.push_back({ delta_x / std::sqrt(delta_x * delta_x + delta_y * delta_y),delta_y / std::sqrt(delta_x * delta_x + delta_y * delta_y) });
        }
        for (int i = Gun_bullet_2.size() - 1; i >= 0; --i) {
            if (Gun_bullet_2[i].x() < 10 || Gun_bullet_2[i].x() > 590 || Gun_bullet_2[i].y() < 10 || Gun_bullet_2[i].y() > 790) {
                Gun_bullet_2.removeAt(i);
                direction_gun_2.erase(direction_gun_2.begin() + i);
            }
        }
        //以下是敌机与自机弹幕碰撞逻辑
        for (int i = 0; i < auto_enemy_2.size(); i++) {//对第i个敌机
            if (auto_enemy_2[i].x() < 10 || auto_enemy_2[i].x() > 590 || auto_enemy_2[i].y() < 10 || auto_enemy_2[i].y() > 790) continue;
            for (int j = 0; j < m_bullets.size(); j++) {//对通常子弹
                int square_distance = ((-m_bullets[j].x() + auto_enemy_2[i].x()) * (-m_bullets[j].x() + auto_enemy_2[i].x()) + (-m_bullets[j].y() + auto_enemy_2[i].y()) * (-m_bullets[j].y() + auto_enemy_2[i].y()));//这是子弹中心和敌机中心距离
                if (square_distance >= 0 && square_distance <= 100) {//10像素以内判定碰撞
                    if (blood_2[i] > 0) {
                        bullet_index.push_back(j);
                        blood_2[i] -= 10;
                        if (blood_2[i] <= 0) {//敌机血量归0
                            enemy_del.push_back(i);
                            break;
                        }
                    }
                }
            }
            for (int j = 0; j < m_bullets_1.size(); j++) {//对一技能弹幕子弹
                int square_distance = ((-m_bullets_1[j].x() + auto_enemy_2[i].x()) * (-m_bullets_1[j].x() + auto_enemy_2[i].x()) + (-m_bullets_1[j].y() + auto_enemy_2[i].y()) * (-m_bullets_1[j].y() + auto_enemy_2[i].y()));//这是子弹中心和敌机中心距离
                if (square_distance >= 0 && square_distance <= 100) {//10像素以内判定碰撞
                    if (blood_2[i] > 0) {
                        bullet_index_1.push_back(j);
                        blood_2[i] -= 10;
                        if (blood_2[i] <= 0) {//敌机血量归0
                            enemy_del.push_back(i);
                            break;
                        }
                    }
                }
            }
            for (int j = 0; j < m_bullets_2.size(); j++) {
                for (int k = 0; k < m_bullets_2[j].size(); k++) {
                    if (list[j][k] != -1) {
                        int square_distance = ((-m_bullets_2[j][k].x() + auto_enemy_2[i].x()) * (-m_bullets_2[j][k].x() + auto_enemy_2[i].x()) + (-m_bullets_2[j][k].y() + auto_enemy_2[i].y()) * (-m_bullets_2[j][k].y() + auto_enemy_2[i].y()));//这是子弹中心和敌机中心距离
                        if (square_distance >= 0 && square_distance <= 100) {//10像素以内判定碰撞
                            if (blood_2[i] > 0) {
                                list[j][k] = -1;
                                blood_2[i] -= 10;
                                if (blood_2[i] <= 0) {//敌机血量归0
                                    enemy_del.push_back(i);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            for (int j = 0; j < m_bullets_3.size(); j++) {
                for (int k = 0; k < m_bullets_3[j].size(); k++) {
                    if (list_3[j][k] != -1) {
                        int square_distance = ((-m_bullets_3[j][k].x() + auto_enemy_2[i].x()) * (-m_bullets_3[j][k].x() + auto_enemy_2[i].x()) + (-m_bullets_3[j][k].y() + auto_enemy_2[i].y()) * (-m_bullets_3[j][k].y() + auto_enemy_2[i].y()));//这是子弹中心和敌机中心距离
                        if (square_distance >= 0 && square_distance <= 100) {//10像素以内判定碰撞
                            if (blood_2[i] > 0) {
                                list_3[j][k] = -1;
                                blood_2[i] -= 10;
                                if (blood_2[i] <= 0) {//敌机血量归0
                                    enemy_del.push_back(i);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        std::sort(bullet_index.begin(), bullet_index.end());
        bullet_index.erase(std::unique(bullet_index.begin(), bullet_index.end()), bullet_index.end());
        std::sort(bullet_index_1.begin(), bullet_index_1.end());
        bullet_index_1.erase(std::unique(bullet_index_1.begin(), bullet_index_1.end()), bullet_index_1.end());
        std::sort(enemy_del.begin(), enemy_del.end());
        enemy_del.erase(std::unique(enemy_del.begin(), enemy_del.end()), enemy_del.end());
        for (int i = bullet_index.size() - 1; i >= 0; --i) {
            m_bullets.erase(m_bullets.begin() + bullet_index[i]);
        }
        bullet_index = {};
        for (int i = bullet_index_1.size() - 1; i >= 0; --i) {
            m_bullets_1.erase(m_bullets_1.begin() + bullet_index_1[i]);
        }
        bullet_index_1 = {};
        for (int i = enemy_del.size() - 1; i >= 0; --i) {
            auto_enemy_2.erase(auto_enemy_2.begin() + enemy_del[i]);
            blood_2.erase(blood_2.begin() + enemy_del[i]);
            random_position_2.erase(random_position_2.begin() + enemy_del[i]);
            line_speed_2.erase(line_speed_2.begin() + enemy_del[i]);
            direction_2.erase(direction_2.begin() + enemy_del[i]);
            sign.erase(sign.begin() + enemy_del[i]);
            cooldown_status_2.erase(cooldown_status_2.begin() + enemy_del[i]);
            score += 1;
        }
        enemy_del = {};
        for (int i = 0; i < enemy_bullet_2.size(); i++) {
            for (int j = enemy_bullet_2[i].size() - 1; j << enemy_bullet_2[i].size() >= 0; --j) {
                int square_distance = (enemy_bullet_2[i][j].x() - m_pos.x()) * (enemy_bullet_2[i][j].x() - m_pos.x()) + (enemy_bullet_2[i][j].y() - m_pos.y()) * (enemy_bullet_2[i][j].y() - m_pos.y());
                if (square_distance >= 0 && square_distance <= 25) {
                    if (self_blood > 0) {
                        enemy_bullet_2[i].removeAt(j);
                        direction_bullet_2[i].erase(direction_bullet_2[i].begin() + j);
                        self_blood -= 1;
                        if (self_blood == 0) {
                            game_over = true;
                            break;
                        }
                    }
                }
            }
        }
        for (int i = Gun_bullet_2.size() - 1; i >= 0; --i) {
            int square_distance = (Gun_bullet_2[i].x() - m_pos.x()) * (Gun_bullet_2[i].x() - m_pos.x()) + (Gun_bullet_2[i].y() - m_pos.y()) * (Gun_bullet_2[i].y() - m_pos.y());
            if (square_distance >= 0 && square_distance <= 25) {
                if (self_blood > 0) {
                    Gun_bullet_2.removeAt(i);
                    direction_gun_2.erase(direction_gun_2.begin() + i);
                    self_blood -= 1;
                    if (self_blood == 0) {
                        game_over = true;
                        break;
                    }
                }
            }
        }
    }

    //边界限制：
    if (m_pos.x() < 20) m_pos.setX(20);//碰到左边
    if (m_pos.y() < 20) m_pos.setY(20);//碰到上边
    if (m_pos.x() > width() - 420) m_pos.setX(width() - 420);//碰到右边
    if (m_pos.y() > height() - 20) m_pos.setY(height() - 20);//碰到下边
    update(); // 呼叫重绘，画面就会更新到新位置，此时触发绘画事件函数
}

