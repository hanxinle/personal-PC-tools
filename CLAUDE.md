# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 仓库总览

本仓库是个人桌面/浏览器工作流工具的集合，包含五个自研小工具，以及一个作为独立子项目嵌入的 `Math-To-Manim`。

- `01-cat-clock-h5/`：小猫闹钟（H5）。单 HTML 文件浏览器应用，可直接用浏览器打开。
- `02-pc-clock-cpp/`：PC 原生闹钟。C++/Qt/Visual Studio 桌面应用。
- `03-no-screen-lock/`：拒绝锁屏。C++/Qt/Visual Studio 桌面应用，定时模拟按键以防止系统锁屏。
- `04-cat-clock-pc/`：小猫闹钟桌面封装。C++/Qt WebView/Visual Studio 应用，用系统原生浏览器引擎加载并运行 `01-cat-clock-h5/小猫闹钟.html`（Windows 上为 Edge WebView2）。
- `05-cat-pet/`：桌面宠物猫。Python + PyQt5 应用，把猫咪照片变成无边框透明置顶的桌面宠物。
- `Math-To-Manim/`：教育动画生成流水线（Python + Manim），有独立的 `AGENTS.md`、`README.md` 和 `docs/ARCHITECTURE.md`。

仓库没有统一的根构建脚本；各子项目独立开发、独立运行。

## 项目 1：小猫闹钟（H5）

- 入口文件：`01-cat-clock-h5/小猫闹钟.html`
- 运行方式：直接用浏览器打开该 HTML 文件，无需构建。
- 资源文件：同级目录下的 `cat_icon.png`、`sing_picture.gif`、默认铃声 `bell3.mp3` 等必须和 HTML 在同一目录。
- 需求文档：`01-cat-clock-h5/小猫闹钟-需求描述.md` 记录了完整的 UI、交互和数据模型。

## 项目 2：PC 原生闹钟

- 解决方案：`02-pc-clock-cpp/lex_clock.sln`
- 项目文件：`02-pc-clock-cpp/lex_clock/lex_clock.vcxproj`
- 开发环境：Visual Studio 2022（工具集 v143），Qt 6.9.1_msvc2022_64，Windows SDK 10.0.26100.0。
- 目标平台：`x64`
- 常用构建配置：`Release|x64`
- 额外链接库：`winmm.lib`（用于 `PlaySound`）。
- 构建方式：在 Visual Studio 中打开 `lex_clock.sln`，选择 Release/x64 后生成。
- 资源：GIF 通过 `lex_clock.qrc` 嵌入；铃声 `bell.wav` 需放在输出目录同级。

## 项目 3：拒绝锁屏

- 解决方案：`03-no-screen-lock/no_screen_lock.sln`
- 项目文件：`03-no-screen-lock/no_screen_lock/no_screen_lock.vcxproj`
- 开发环境：Visual Studio 2022（工具集 v143），Qt 6.9.1_msvc2022_64，Windows SDK 10.0。
- 目标平台：`x64`
- 常用构建配置：`Release|x64`
- 构建方式：在 Visual Studio 中打开 `no_screen_lock.sln`，选择 Release/x64 后生成。
- 资源：GIF 通过 `no_screen_lock.qrc` 嵌入。

## 项目 4：小猫闹钟桌面封装

- 解决方案：`04-cat-clock-pc/Cat-Clock-PC/Cat-Clock-PC.sln`
- 项目文件：`04-cat-clock-pc/Cat-Clock-PC/Cat-Clock-PC/Cat-Clock-PC.vcxproj`
- 开发环境：Visual Studio 2022（工具集 v143），Qt 6.9.1_msvc2022_64，Windows SDK 10.0。
- 目标平台：`x64`
- 常用构建配置：`Release|x64`
- 依赖 Qt 模块：`core;gui;widgets;quick;quickwidgets;webview`。
- 构建方式：在 Visual Studio 中打开 `Cat-Clock-PC.sln`，选择 Release/x64 后生成。
- 资源复制：项目的 PostBuildEvent 会在生成后将 `01-cat-clock-h5/` 整个目录复制到输出目录的 `01-cat-clock-h5/` 下，程序启动时优先加载该副本。
- 运行说明：程序使用 `QWebView` 以本地文件方式打开 `01-cat-clock-h5/小猫闹钟.html`，尽量保持与 Chrome 打开该 HTML 相同的行为（JavaScript、文件选择器、音频解锁等）。Windows 平台依赖系统 Edge WebView2 Runtime。
- 部署注意：发布时需要随程序一起部署 Qt WebView 运行依赖，建议使用 `windeployqt` 处理；目标 Windows 机器需安装 WebView2 Runtime。

## 项目 5：桌面宠物猫

- 目录：`05-cat-pet/`，纯 Python 脚本，无解决方案或工程文件。
- 运行环境：Windows 10/11，Python 3.10+，依赖见 `05-cat-pet/requirements.txt`（PyQt5、rembg[cpu]、Pillow、pyinstaller）。
- 复现步骤：`pip install -r requirements.txt` → `python gen_assets.py`（rembg 抠图并生成 `assets/` 动作序列帧，首次运行自动下载 u2netp 模型）→ `python pet.py`。
- 打包（可选）：`pyinstaller --noconsole --onefile --name CatPet --add-data "assets;assets" pet.py`，产物为 `dist/CatPet.exe`。
- 生成物不提交：`assets/`、`build/`、`dist/`、`*.spec` 已在 `05-cat-pet/.gitignore` 中排除。
- 自定义要点：更换 `cat.jpg` 后需重新核对 `gen_assets.py` 中的眼睛坐标（`LENS` 与 `mask_frame`），再重新生成素材。

## Math-To-Manim 子项目

该子项目有完整独立的开发规范，优先参考：

- `Math-To-Manim/AGENTS.md`：Agent 操作原则、目录结构、测试规范、安全与提交规则。
- `Math-To-Manim/README.md`：安装、运行、CLI 命令。
- `Math-To-Manim/docs/ARCHITECTURE.md`：流水线架构和各阶段产物。

常用命令（在 `Math-To-Manim/` 目录下执行）：

```bash
# 创建并激活虚拟环境
python -m venv .venv
. .venv/bin/activate        # Windows PowerShell: .\.venv\Scripts\Activate.ps1

# 安装开发依赖
python -m pip install -U pip
python -m pip install -e ".[dev]"

# 运行测试
python -m pytest

# 无模型调用的冒烟测试
python -m math_to_manim.cli generate "Explain why derivatives are slopes" --deterministic --no-render

# 需要真实渲染时的安装
python -m pip install -e ".[dev,render]"
```

## 全局约定

- 源码注释使用简体中文，每行注释以英文句号结尾。
- 文档文件（README.md、CLAUDE.md、commit message 等）使用简体中文。
- 程序文件保留其原始语言，但注释和说明使用简体中文。
- 仓库根目录下的 `.gitignore` 以 Visual Studio 构建产物为主，已包含 Python `__pycache__`。
