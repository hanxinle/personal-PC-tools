# CatPet 桌面宠物

把一张猫咪照片变成 Windows 桌面宠物：无边框透明窗口、置顶显示、可随意拖动，右键菜单可播放动作或退出程序。

## 功能特性

- 无边框、背景透明、窗口置顶
- 左键按住拖动，可移动到屏幕任意位置
- 右键菜单选择动作：墨镜、喝水、摇头、睡觉、退出
- 待机时每 5 到 10 秒随机自动播放动作
- 可打包为单文件 exe，双击即用，无需安装 Python

## 动作说明

| 动作 | 效果 |
| ---- | ---- |
| 墨镜 | 一副墨镜从头顶落下戴到眼睛上，停留后飞走 |
| 喝水 | 身体绕颈部支点前倾靠近水盆，盆中水位逐渐下降 |
| 摇头 | 身体左右轻轻摇摆 |
| 睡觉 | 戴上睡眠眼罩，身体微倾，旁边飘出 Zzz |

## 环境要求

- Windows 10 或 11
- Python 3.10 及以上

## 复现步骤

1. 安装依赖。

   ```bash
   pip install -r requirements.txt
   ```

2. 生成动画素材。首次运行会自动下载 rembg 的 u2netp 抠图模型（约 4.6MB），生成的序列帧保存在 `assets/` 目录。

   ```bash
   python gen_assets.py
   ```

3. 运行桌面宠物。

   ```bash
   python pet.py
   ```

4. 打包为单文件 exe（可选），产物位于 `dist/CatPet.exe`。

   ```bash
   pyinstaller --noconsole --onefile --name CatPet --add-data "assets;assets" pet.py
   ```

   如需发布给没有 Python 环境的用户，建议将 exe 通过 GitHub Release 附件发布，而不是提交到仓库。

## 文件结构

```text
CatPet/
├── cat.jpg           # 原始宠物照片
├── gen_assets.py     # 素材生成脚本: 抠图并生成全部动作序列帧
├── pet.py            # 桌面宠物主程序
├── requirements.txt  # Python 依赖清单
└── README.md
```

以下内容为生成物，已在 `.gitignore` 中排除，无需提交。

- `assets/`：动作序列帧，由 `gen_assets.py` 生成
- `build/`、`dist/`、`*.spec`：PyInstaller 打包产物

## 自定义指南

- 更换宠物照片：替换 `cat.jpg` 后，需要重新核对 `gen_assets.py` 中眼睛坐标（墨镜与眼罩依赖该坐标）。可用任意看图工具量取两眼中心在原图中的像素位置，修改 `LENS` 与 `mask_frame` 中的坐标后重新生成素材。
- 调整宠物大小：修改 `gen_assets.py` 中的 `TARGET_H`（默认 420 像素）。
- 调整动作节奏：修改 `pet.py` 中 `ACTIONS` 字典里每个动作的每帧毫秒数。
- 新增动作：在 `gen_assets.py` 中生成新的序列帧，并在 `pet.py` 的 `ACTIONS` 中登记即可。

## 实现原理

- 抠图：rembg（u2netp 模型）去除照片背景，得到透明底 RGBA 图像
- 动画帧：Pillow 在透明画布上绘制墨镜、眼罩、水盆、Zzz 等元素并与宠物图合成
- 桌面程序：PyQt5 实现无边框透明置顶窗口、拖动、右键菜单和定时器帧播放
- 打包：PyInstaller 将程序与素材打包为单文件 exe
