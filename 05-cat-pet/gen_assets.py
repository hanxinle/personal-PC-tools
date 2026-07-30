# 生成桌面宠物所需的动画帧素材.
# 流程: 抠图 -> 裁剪 -> 生成眨眼/喝水/摇头/睡觉序列帧 -> 统一画布缩放输出.
import os
from rembg import remove, new_session
from PIL import Image, ImageDraw, ImageFont

SRC = "cat.jpg"
OUT = "assets"
TARGET_H = 420          # 最终画布高度(像素).
os.makedirs(OUT, exist_ok=True)

# 原图(1532x2048)中两只眼睛的中心坐标, 已人工核对.
print("加载模型并抠图...")
session = new_session("u2netp")
src = Image.open(SRC).convert("RGB")
cut = remove(src, session=session)          # RGBA.
bbox = cut.getbbox()
cut = cut.crop(bbox)
ox, oy = bbox[0], bbox[1]
cw, ch = cut.size
print(f"抠图完成, 尺寸: {cw}x{ch}")


def paste_on(canvas, img, pos):
    canvas.paste(img, pos, img)
    return canvas


# 统一画布: 喝水帧需要额外空间放置水盆, 其他帧底部对齐粘贴.
CANVAS_W = int(cw * 1.38)
CANVAS_H = int(ch * 1.16)
CAT_POS = (0, CANVAS_H - ch)    # 猫固定于画布左下角.

BOWL_CX = int(cw * 0.62)        # 水盆圆心, 放在猫头右下方.
BOWL_CY = int(CANVAS_H * 0.90)
BOWL_RX = int(cw * 0.15)
BOWL_RY = int(BOWL_RX * 0.42)

PIVOT = (int(cw * 0.35), int(ch * 0.80))    # 喝水时身体前倾的旋转支点(颈部).


def base_canvas():
    return Image.new("RGBA", (CANVAS_W, CANVAS_H), (0, 0, 0, 0))


def draw_bowl(canvas, water_ratio):
    # 绘制水盆并按水位比例绘制水面.
    d = ImageDraw.Draw(canvas)
    d.ellipse([BOWL_CX - BOWL_RX, BOWL_CY - BOWL_RY, BOWL_CX + BOWL_RX, BOWL_CY + BOWL_RY],
              fill=(90, 130, 200, 255), outline=(50, 80, 140, 255), width=6)
    wr = 0.82 * (0.4 + 0.6 * water_ratio)
    d.ellipse([BOWL_CX - BOWL_RX * wr, BOWL_CY - BOWL_RY * wr * 0.9,
               BOWL_CX + BOWL_RX * wr, BOWL_CY + BOWL_RY * wr * 0.9],
              fill=(150, 210, 255, 255))


def tilt(img, angle, center=None):
    # 绕支点旋转, 透明填充.
    return img.rotate(angle, resample=Image.BICUBIC, expand=False,
                      fillcolor=(0, 0, 0, 0), center=center)


frames = {}

# 待机.
frames["idle"] = paste_on(base_canvas(), cut, CAT_POS)

# 墨镜: 从头顶落下戴在眼睛上, 停留后飞走.
LENS = [((487 - ox, 935 - oy), 120, 98), ((885 - ox, 1000 - oy), 114, 100)]


def glasses_frame(y_off):
    # 在透明层上绘制墨镜(双镜片+鼻梁+镜腿), 整体纵向偏移后合成到猫上.
    layer = Image.new("RGBA", cut.size, (0, 0, 0, 0))
    d = ImageDraw.Draw(layer)
    centers = []
    for (cx, cy), rx, ry in LENS:
        cy += y_off
        centers.append((cx, cy, rx, ry))
        d.ellipse([cx - rx, cy - ry, cx + rx, cy + ry], fill=(18, 18, 24, 255),
                  outline=(60, 60, 70, 255), width=8)
        # 镜片高光.
        d.polygon([(cx - rx * 0.55, cy - ry * 0.55), (cx - rx * 0.1, cy - ry * 0.55),
                   (cx - rx * 0.55, cy + ry * 0.1)], fill=(255, 255, 255, 70))
    (lx, ly, lrx, _), (rx2, ry2, rrx, _) = centers
    # 鼻梁连接左右镜片内上沿.
    d.line([(lx + lrx * 0.8, ly - 40), (rx2 - rrx * 0.8, ry2 - 50)], fill=(15, 15, 18, 255), width=26)
    # 镜腿伸向两侧耳朵方向.
    d.line([(lx - lrx, ly - 20), (lx - lrx - 150, ly - 150)], fill=(15, 15, 18, 255), width=24)
    d.line([(rx2 + rrx, ry2 - 20), (rx2 + rrx + 150, ry2 - 160)], fill=(15, 15, 18, 255), width=24)
    return Image.alpha_composite(cut, layer)


for i, off in enumerate([-360, -150, 0, 0, 0, 0, -150, -360]):
    frames[f"cool_{i}"] = paste_on(base_canvas(), glasses_frame(off), CAT_POS)

# 喝水: 身体绕颈部支点前倾并向水盆移动, 水位逐渐下降, 水盆画在猫前方.
angles = [-5, -14, -22, -22, -14, -5]
waters = [1.0, 0.85, 0.6, 0.4, 0.25, 0.2]
moves = [(20, 30), (80, 110), (140, 190), (140, 190), (80, 110), (20, 30)]
for i, (a, w, (mx, my)) in enumerate(zip(angles, waters, moves)):
    c = base_canvas()
    paste_on(c, tilt(cut, a, center=PIVOT), (CAT_POS[0] + mx, CAT_POS[1] + my))
    draw_bowl(c, w)
    frames[f"drink_{i}"] = c

# 摇头(开心): 左右轻摆.
for i, a in enumerate([4, -4, 3, -3, 0]):
    frames[f"shake_{i}"] = paste_on(base_canvas(), tilt(cut, a), CAT_POS)

# 睡觉: 戴眼罩 + 微倾 + Zzz.
def mask_frame():
    # 在透明层上绘制睡眠眼罩(横跨双眼的圆角罩面+两侧松紧带), 合成到猫上.
    layer = Image.new("RGBA", cut.size, (0, 0, 0, 0))
    d = ImageDraw.Draw(layer)
    x1 = 487 - ox - 165
    x2 = 885 - ox + 165
    yc = 967 - oy
    d.rounded_rectangle([x1, yc - 115, x2, yc + 115], radius=100,
                        fill=(88, 70, 160, 255), outline=(58, 44, 118, 255), width=10)
    d.line([(x1 + 10, yc - 30), (x1 - 120, yc - 100)], fill=(58, 44, 118, 255), width=32)
    d.line([(x2 - 10, yc - 30), (x2 + 120, yc - 110)], fill=(58, 44, 118, 255), width=32)
    return Image.alpha_composite(cut, layer)


sleep_base = tilt(mask_frame(), -6)
try:
    font1 = ImageFont.truetype("arialbd.ttf", 90)
    font2 = ImageFont.truetype("arialbd.ttf", 60)
except OSError:
    font1 = font2 = ImageFont.load_default()
for i in range(4):
    c = paste_on(base_canvas(), sleep_base, CAT_POS)
    d = ImageDraw.Draw(c)
    zx, zy = int(CANVAS_W * 0.72), int(CANVAS_H * 0.30)
    if i >= 1:
        d.text((zx, zy), "Z", font=font2, fill=(120, 120, 220, 255))
    if i >= 2:
        d.text((zx + 55, zy - 60), "Z", font=font1, fill=(120, 120, 220, 255))
    if i >= 3:
        d.text((zx + 130, zy - 140), "Z", font=font1, fill=(120, 120, 220, 255))
    frames[f"sleep_{i}"] = c

# 统一缩放到目标高度并保存.
scale = TARGET_H / CANVAS_H
tw, th = int(CANVAS_W * scale), int(CANVAS_H * scale)
for name, img in frames.items():
    img.resize((tw, th), Image.LANCZOS).save(os.path.join(OUT, name + ".png"))
print(f"共生成 {len(frames)} 帧, 画布 {tw}x{th}, 保存到 {OUT}/")
