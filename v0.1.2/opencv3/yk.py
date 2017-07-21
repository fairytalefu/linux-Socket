from time import time
from numpy import uint8
from numpy.random import rand
import cv2

xy=(512,512)
Nf = 500

def fpsopencv(dat):
    tic = time()
    for i in dat:
        cv2.imshow('test',i)
        cv2.waitKey(1) #integer milliseconds, 0 makes wait forever
    cv2.destroyAllWindows()
    return Nf / (time()-tic)


imgs = (rand(Nf,xy[0],xy[1])*255).astype(uint8)
fps = fpsopencv(imgs)

print(f'{fps} fps')


