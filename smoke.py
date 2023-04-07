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
