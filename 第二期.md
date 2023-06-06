# 一. 安卓容器

1. 下载我开启了安卓容器选项的内核进行安装或者下载我最新编译的系统不需要更新内核
[内核deb包](https://github.com/3026187875/armbian-rock5b-images/releases/download/20230309-1402/linux-image-legacy-rockchip-rk3588_5.10.110-Sbf8b-D3196-Pd578-Ccb0cHfe66-Bb436_arm64.deb)@微信非虫
2. 安装ppa源里的malig610固件
```bash
sudo apt install mali-g610-firmware
```
3.   首先安装好docker

```bash
curl -fsSL https://get.docker.com | bash -s docker --mirror Aliyun
```

4. 拉取安卓镜像，使用chisbread/rk3588-gaming:redroid-firefly

```bash
docker pull chisbread/rk3588-gaming:redroid-firefly
```

5. 创建容器

```bash
docker run -itd --privileged \
    --pull always \
    -v "$(pwd)"/Android:/data \
    -v /dev:/dev \
    -v /run/dbus:/run/dbus \
    -v /var/run/dbus:/var/run/dbus \
    --mount=type=bind,source=/dev/mali0,destination=/dev/mali0 \
    -p 5555:5555 \
    chisbread/rk3588-gaming:redroid-firefly \
    androidboot.redroid_width=1080 \
    androidboot.redroid_height=1920 \
    androidboot.redroid_dpi=480 \
    androidboot.redroid_fps=30 \
    androidboot.redroid_gpu_mode=host
```

6.   win使用qtscrcpy链接安卓容器即可使用

7.   5b上使用scrcpy即可链接安卓容器

### 声音使用sndcpy或者scrcpy-server2.0版本均不能在5b通过3.5mm耳机孔播放声音，win和mac音频传输正常

摄像头经测试无法在安卓容器内使用。但是安卓容器内显示有video文件------等待解决

### Armbian使用官方4k摄像头

下载[Camera](https://github.com/radxa/overlays/blob/main/arch/arm64/boot/dts/rockchip/overlays/rock-5ab-camera-imx415.dts)设备树

放置于/boot下，使用armbian-add-overly添加即可

# 二. spi点亮0.96oled屏幕

如代码所示

无法点亮，但数据收发都正常且时序正确

# 三. rknpu简单使用

## 3.1 使用x86模拟npu

1.   克隆两个demo仓库\

     ```bash
     git clone https://github.com/rockchip-linux/rknpu2.git
     git clone https://github.com/rockchip-linux/rknn-toolkit2.git
     ```

2.   rknn-toolkit2是x86使用的工具，以下操作均在x86电脑上完成

     ```bash
     sudo apt-get install libxslt1-dev zlib1g-dev libglib2.0 libsm6 libgl1-mesa-glx libprotobuf-dev gcc
     cd ./rknn-toolkit2		# 进入rknn-toolkit目录
     pip3 install -r doc/requirements_cp36-1.4.0.txt		# py3.6用这个
     pip3 install -r doc/requirements_cp38-1.4.0.txt		# py3.8用这个	其实这两个文件没有区别，任何py版本都可以用
     # py3.9也可以用
     pip3 install ./packages/rknn_toolkit2-1.4.0_22dcfef4-cp36-cp36m-linux_x86_64.whl# py3.6用这个
     pip3 install ./packages/rknn_toolkit2-1.4.0_22dcfef4-cp38-cp38-linux_x86_64.whl # py3.8用这个
     ```

3.    运行demo

      ```bash
      cd examples/onnx/yolov5
      python3 test.py
      ```

      如果运行完后，没有结果图片，修改test.py

      ```python
      # 在这段注释后添加retval = cv2.imwrite("reslult.jpg",img_1)
      # show output
      # cv2.imshow("post process result", img_1)
      # cv2.waitKey(0)
      # cv2.destroyAllWindows()
      retval = cv2.imwrite("reslult.jpg",img_1)
      ```

## 3.2 连板调试

不需要有线，没找到rknnserver源码所以必须要用到adb

### Linux系统

我没有找到办法开启adb server使win能通过adb链接linux

### 安卓系统

安卓系统没有使用过，根据文档说明需要usb链接板子和电脑

adb无线链接没有测试

### 上传rknn_server和librknnrt.so到板子

运行rknn_server

```bash
rknn_server
# 如果没有执行权限加执行权限
sudo chmod +x ./rknn_server
```

+	librknnrt.so: 是一个板端的runtime 库。
+	rknn_server: 是一个运行在板子上的后台代理服务，用于接收PC 通过USB 传输过来的协议,然后执行板端runtime 对应的接口，并返回结果给PC

**以下操作在x86电脑上进行**

使用adb devices 查看设备ID

修改脚本target 和device_id

+   rknn-toolkit2/examples/onnx/yolov5/test.py

```bash
rknn.config(mean_values=[[0, 0, 0]], std_values=[[255, 255, 255]])
修改为---->
rknn.config(mean_values=[[0, 0, 0]], std_values=[[255, 255, 255]],target_platform='rk3588')
ret = rknn.init_runtime()
修改为---->
ret = rknn.init_runtime(target='rk3588',device_id='your device id')
```

```bash
# 终端进入rknn-toolkit2/examples/onnx/yolov5目录
python3 test.py
```

## 3.3在板子上运行demo

### 安卓

查看原文档关于安卓系统下的章节

### Linux

以下在板子上操作

1.   克隆rknpu2仓库

```bash
git clone https://github.com/rockchip-linux/rknpu2.git
cd rknpu2/examples/rknn_yolov5_demo
./build-linux_RK3588.sh
cd ./install/rknn_yolov5_demo_Linux
./rknn_yolov5_demo ./model/RK3588/yolov5s-640-640.rknn ../../convert_rknn_demo/yolov5/bus.jpg
# 生成out.jpg作为结果
```

## 3.4 后续学习

建议阅读rknpu2仓库中doc中的Rockchip_RKNPU_User_Guide_RKNN_API_V1.4.0_CN.pdf文档，使用yolov5_demo练习api使用

然后使用自己的模型，阅读RKNN_Compiler_Support_Operator_List_v1.4.0.pdf针对自己模型中npu不支持的算子进行重写，修剪模型。

## 3.5其他重要命令

+   CPU 定频命令

    查看cpu频率

    ```bash
    cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq
    cat /sys/kernel/debug/clk/clk_summary | grep arm
    ```

    固定cpu频率

    ```bash
    # 查看CPU 可用频率
    cat /sys/devices/system/cpu/cpufreq/policy0/scaling_available_frequencies
    408000 600000 816000 1008000 1200000 1416000 1608000 1704000
    # 设置CPU 频率，例如，设置1.7GHz
    echo userspace > /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
    echo 1704000 > /sys/devices/system/cpu/cpufreq/policy0/scaling_setspeed
    ```

+   DDR 定频命令

    查看DDR频率

    ```bash
    cat /sys/class/devfreq/dmc/cur_freq
    cat /sys/kernel/debug/clk/clk_summary | grep ddr
    ```

    固定DDR频率

    ```bash
    # 查看DDR 可用频率
    cat /sys/class/devfreq/dmc/available_frequencies
    # 设置DDR 频率，例如，设置1560MHz
    echo userspace > /sys/class/devfreq/dmc/governor
    echo 1560000000 > /sys/class/devfreq/dmc/userspace/set_freq
    ```

    +   npu定频

        查看npu频率

        ```bash
        cat /sys/kernel/debug/clk/clk_summary | grep clk_npu_dsu0
        ```

        固定npu频率

        ```bash
        # 设置NPU 频率，例如，设置1GHz
        echo 1000000000 > /sys/kernel/debug/clk/clk_npu_dsu0/clk_rate
        ```
# 四.部署自己的模型到板子上 
> **建议先用python部署，跑通流程后再用c++**
> 我使用的是自己训练的烟支识别模型，单类别输出
> **yolov5** **v7**版本
## 4.1 pt转onnx
```python
python export.py --weights xxxx.pt --include onnx
```
## 4.2使用netron查看转化后的模型输入和输出
```bash
https://github.com/lutzroeder/netron
# 提供win和linux的包
```
我的模型输入是1x3x640x640,输出是1x25200x6
实际验证输入为1x640x640x3也可以
## 4.3使用模拟器验证
> 删除推理后对outputs进行处理的所有部分，需要自己写结果处理
> yolov5输出的结果1x25200x6
> 25200是特征数量   6是 x1 y1 x2 y2 类别置信度 类别
> 处理思路是找出6中的置信度最大的那一个，在图上框出来
```python
def smoke_prosess(test):
    max_index = np.argmax(test[:, :, 4])
    return test[:, max_index]
def xywh2xyxy(x):
    # Convert [x, y, w, h] to [x1, y1, x2, y2]
    y = np.copy(x)
    y[:, 0] = x[:, 0] - x[:, 2] / 2  # top left x
    y[:, 1] = x[:, 1] - x[:, 3] / 2  # top left y
    y[:, 2] = x[:, 0] + x[:, 2] / 2  # bottom right x
    y[:, 3] = x[:, 1] + x[:, 3] / 2  # bottom right y
    return y
boxes = smoke_prosess(outputs[0])
boxes = xywh2xyxy(boxes)
top = int(boxes[:,0])
left = int(boxes[:, 1])
right = int(boxes[:, 2])
bottom = int(boxes[:, 3])
cv2.rectangle(img, (top, left), (right, bottom), (255, 0, 0), 2)
```
！！！！如果你在模拟器上得到的置信度都是0，关闭量化。我关闭量化后就不全是0了。猜测原因是量化数据只有一张不够，官方推荐20张左右
```python
ret = rknn.build(do_quantization=False, dataset=DATASET)
```
！！！！opencv默认通道是BGR，要转成RGB
smoke.py
```python
import os
import urllib
import traceback
import time
import sys
import numpy as np
import cv2
from rknn.api import RKNN

ONNX_MODEL = 'smoke.onnx'
RKNN_MODEL = 'smoke.rknn'
IMG_PATH = './2.jpg'
DATASET = './smoke.txt'
OBJ_THRESH = 0.25
NMS_THRESH = 0.45
IMG_SIZE = 640
CLASSES = ("smoke")


def xywh2xyxy(x):
    # Convert [x, y, w, h] to [x1, y1, x2, y2]
    y = np.copy(x)
    y[:, 0] = x[:, 0] - x[:, 2] / 2  # top left x
    y[:, 1] = x[:, 1] - x[:, 3] / 2  # top left y
    y[:, 2] = x[:, 0] + x[:, 2] / 2  # bottom right x
    y[:, 3] = x[:, 1] + x[:, 3] / 2  # bottom right y
    return y


def smoke_prosess(test):
    max_index = np.argmax(test[:, :, 4])
    # print(test[:,max_index])
    return test[:, max_index]


if __name__ == '__main__':
    # Create RKNN object
    rknn = RKNN(verbose=True)

    # pre-process config
    print('--> Config model')
    rknn.config(mean_values=[[0, 0, 0]], std_values=[
                [255, 255, 255]], target_platform='rk3588')
    print('done')

    # Load ONNX model
    print('--> Loading model')
    ret = rknn.load_onnx(model=ONNX_MODEL)
    if ret != 0:
        print('Load model failed!')
        exit(ret)
    print('done')

    # Build model
    print('--> Building model')
    ret = rknn.build(do_quantization=False, dataset=DATASET)
    if ret != 0:
        print('Build model failed!')
        exit(ret)
    print('done')

    # Export RKNN model
    print('--> Export rknn model')
    ret = rknn.export_rknn(RKNN_MODEL)
    if ret != 0:
        print('Export rknn model failed!')
        exit(ret)
    print('done')

    # Init runtime environment
    print('--> Init runtime environment')
    ret = rknn.init_runtime()
    # ret = rknn.init_runtime('rk3566')
    if ret != 0:
        print('Init runtime environment failed!')
        exit(ret)
    print('done')

    # Set inputs
    img = cv2.imread(IMG_PATH)
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)  # opencv默认是bgr
    img = cv2.resize(img, (IMG_SIZE, IMG_SIZE))
    print(img.shape)

    # Inference
    print('--> Running model')
    outputs = rknn.inference(inputs=[img])
    np.save('./test_0.npy', outputs[0])
    print('done')
    # 处理结果
    boxes = smoke_prosess(outputs[0])
    boxes = xywh2xyxy(boxes)
    top = int(boxes[:, 0])
    left = int(boxes[:, 1])
    right = int(boxes[:, 2])
    bottom = int(boxes[:, 3])
    cv2.rectangle(img, (top, left), (right, bottom), (255, 0, 0), 2)
    # show output
    cv2.imshow("result", img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
    rknn.release()
```
成功运行后会显示一个窗口，用红框标出烟支
## 4.4 部署到板端
+ 复制rknnapi.so到系统库路径
```bash
sudo cp ./librknn_api.so /usr/lib
sudo cp ./librknnrt.so /usr/lib
# so文件在rknpu2/runtime/RK3588/Linux/librknn_api/aarch64/librknnrt.so
pip install ./rknn_toolkit_lite2-1.4.0-cp39-cp39-linux_aarch64.whl
# whl文件在rknn-toolkit2/rknn_toolkit_lite2/packages/rknn_toolkit_lite2-1.4.0-cp39-cp39-linux_aarch64.whl
```
进入/rknn-toolkit2/rknn_toolkit_lite2/examples目录
```python
python3 test.py
# 测试环境是否正常
```
> 模拟器不需要对齐输入，板端必须对齐模型输入
somke_board.py
```python
import cv2
import numpy as np
import platform
from rknnlite.api import RKNNLite

# decice tree for rk356x/rk3588
DEVICE_COMPATIBLE_NODE = '/proc/device-tree/compatible'

def get_host():
    # get platform and device type
    system = platform.system()
    machine = platform.machine()
    os_machine = system + '-' + machine
    if os_machine == 'Linux-aarch64':
        try:
            with open(DEVICE_COMPATIBLE_NODE) as f:
                device_compatible_str = f.read()
                if 'rk3588' in device_compatible_str:
                    host = 'RK3588'
                else:
                    host = 'RK356x'
        except IOError:
            print('Read device node {} failed.'.format(DEVICE_COMPATIBLE_NODE))
            exit(-1)
    else:
        host = os_machine
    return host

RK356X_RKNN_MODEL = 'resnet18_for_rk356x.rknn'
RK3588_RKNN_MODEL = '/home/leioukupo/npu/rknn-toolkit2/rknn_toolkit_lite2/smoke.rknn'

def smoke_prosess(test):
    max_index = np.argmax(test[:, :, 4])
    # print(test[:,max_index])
    return test[:, max_index]


def xywh2xyxy(x):
    # Convert [x, y, w, h] to [x1, y1, x2, y2]
    y = np.copy(x)
    y[:, 0] = x[:, 0] - x[:, 2] / 2  # top left x
    y[:, 1] = x[:, 1] - x[:, 3] / 2  # top left y
    y[:, 2] = x[:, 0] + x[:, 2] / 2  # bottom right x
    y[:, 3] = x[:, 1] + x[:, 3] / 2  # bottom right y
    return y
if __name__ == '__main__':

    host_name = get_host()
    if host_name == 'RK356x':
        rknn_model = RK356X_RKNN_MODEL
    elif host_name == 'RK3588':
        rknn_model = RK3588_RKNN_MODEL
    else:
        print("This demo cannot run on the current platform: {}".format(host_name))
        exit(-1)

    rknn_lite = RKNNLite()

    # load RKNN model
    print('--> Load RKNN model')
    ret = rknn_lite.load_rknn(rknn_model)
    if ret != 0:
        print('Load RKNN model failed')
        exit(ret)
    print('done')

    ori_img = cv2.imread('./2.jpg')
    img = cv2.cvtColor(ori_img, cv2.COLOR_BGR2RGB)

    # init runtime environment
    print('--> Init runtime environment')
    # run on RK356x/RK3588 with Debian OS, do not need specify target.
    if host_name == 'RK3588':
        ret = rknn_lite.init_runtime(core_mask=RKNNLite.NPU_CORE_0)
    else:
        ret = rknn_lite.init_runtime()
    if ret != 0:
        print('Init runtime environment failed')
        exit(ret)
    print('done')

    # Inference
    print('--> Running model')
    img=cv2.resize(img,[640,640]) # 必须对齐模型输入
    outputs = rknn_lite.inference(inputs=img)
    print(outputs)
    boxes = smoke_prosess(outputs[0])
    boxes = xywh2xyxy(boxes)
    top = int(boxes[:,0])
    left = int(boxes[:, 1])
    right = int(boxes[:, 2])
    bottom = int(boxes[:, 3])
    cv2.rectangle(img, (top, left), (right, bottom), (255, 0, 0), 2)
    # show output
    # 正式应用时需要还原成原本的大小
    cv2.imshow("result", img)
    cv2.waitKey(1000)
    cv2.destroyAllWindows()
    print('done')

    rknn_lite.release()

```
全流程跑通了，后续根据自己的需求调试精度
视频流和图片一样