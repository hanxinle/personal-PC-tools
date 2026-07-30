# 桌面宠物: 无边框透明窗口, 左键拖动, 右键菜单选择动作或退出.
import os
import random
import sys

from PyQt5.QtCore import Qt, QTimer
from PyQt5.QtGui import QCursor, QPixmap
from PyQt5.QtWidgets import QApplication, QLabel, QMenu, QWidget


def resource_dir():
    # 兼容 PyInstaller 打包后的资源路径.
    base = getattr(sys, "_MEIPASS", os.path.dirname(os.path.abspath(__file__)))
    return os.path.join(base, "assets")


# 动作名 -> (序列帧前缀, 帧数, 每帧毫秒).
ACTIONS = {
    "墨镜": ("cool", 8, 120),
    "喝水": ("drink", 6, 260),
    "摇头": ("shake", 5, 140),
    "睡觉": ("sleep", 4, 700),
}


class DesktopPet(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowFlags(Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool)
        self.setAttribute(Qt.WA_TranslucentBackground)

        self.frames = {}
        path = resource_dir()
        for name, (prefix, count, _) in ACTIONS.items():
            self.frames[name] = [QPixmap(os.path.join(path, f"{prefix}_{i}.png")) for i in range(count)]
        self.idle = QPixmap(os.path.join(path, "idle.png"))

        self.label = QLabel(self)
        self.label.setPixmap(self.idle)
        self.resize(self.idle.size())

        self.drag_pos = None
        self.playing = False
        self.frame_index = 0
        self.current_action = None

        # 动作播放定时器.
        self.play_timer = QTimer(self)
        self.play_timer.timeout.connect(self.next_frame)

        # 待机时随机自动播放动作.
        self.idle_timer = QTimer(self)
        self.idle_timer.timeout.connect(self.auto_play)
        self.schedule_idle()

        # 初始位置: 屏幕右下角.
        screen = QApplication.primaryScreen().availableGeometry()
        self.move(screen.right() - self.width() - 60, screen.bottom() - self.height() - 60)

    def schedule_idle(self):
        self.idle_timer.start(random.randint(5000, 10000))

    def auto_play(self):
        if not self.playing:
            self.play(random.choice(["墨镜", "摇头"]))
        self.schedule_idle()

    def play(self, name):
        # 播放指定动作, 播完回到待机图.
        self.current_action = name
        self.frame_index = 0
        self.playing = True
        _, _, interval = ACTIONS[name]
        self.play_timer.start(interval)
        self.show_frame()

    def next_frame(self):
        self.frame_index += 1
        if self.frame_index >= len(self.frames[self.current_action]):
            self.play_timer.stop()
            self.playing = False
            self.label.setPixmap(self.idle)
            return
        self.show_frame()

    def show_frame(self):
        self.label.setPixmap(self.frames[self.current_action][self.frame_index])

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.drag_pos = event.globalPos() - self.frameGeometry().topLeft()
            event.accept()

    def mouseMoveEvent(self, event):
        if self.drag_pos is not None and event.buttons() & Qt.LeftButton:
            self.move(event.globalPos() - self.drag_pos)
            event.accept()

    def mouseReleaseEvent(self, event):
        self.drag_pos = None

    def contextMenuEvent(self, event):
        menu = QMenu(self)
        action_menu = menu.addMenu("动作")
        for name in ACTIONS:
            action_menu.addAction(name, lambda n=name: self.play(n))
        menu.addSeparator()
        menu.addAction("退出", QApplication.quit)
        menu.exec_(QCursor.pos())


if __name__ == "__main__":
    app = QApplication(sys.argv)
    pet = DesktopPet()
    pet.show()
    sys.exit(app.exec_())
