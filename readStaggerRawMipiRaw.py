from glob import glob
import cv2
import numpy as np
import os
# import scipy.misc

class readHdrRaw():
    def __init__(self):
        self.cols = 4096
        self.rows = 2304 * 2
    def read(self,path):
        img = np.fromfile(path,dtype="ushort")
        img = img.reshape(self.rows,self.cols,1)
        #gamma 2.2
        # img = np.power(img / 1023, 1 / 2.2) * 1023
        img = np.uint8(img / 4)
        img = cv2.resize(img,(1152,1024))
        cv2.imshow("res",img)
        cv2.waitKey(1)
    def run(self):
        fns = glob(r"\\10.114.1.209\rwx5330461\data\base_raw\rawdata\*.raw")
        for fn in fns:
            print(fn)
            self.read(fn)

class readMipiRaw():
    def __init__(self,path):
    #custom param
        self.cols = 4096
        self.rows = 3072
        self.k = 1.25
        self.iPath = path
        fp, tempfilename = os.path.split(self.iPath)
        filename, extension = os.path.splitext(tempfilename)
        print("iPath:", self.iPath)
    #is save file
        self.isSavebayer = True
        self.isSaveBmp = True
        self.isSaveJpeg = True
    # calc res fp
        self.bmpOpath = os.path.join(fp, filename + "unPack" + ".bmp")
        self.jepgOpath = os.path.join(fp, filename + "unPack" + ".jpeg")
        self.bayerOpath = os.path.join(fp, filename + "unPack" + extension)
    #auto param
        self.mipiLen = int(self.rows * self.cols * self.k)
        self.bayerLen = self.rows * self.cols
        self.resArray = np.zeros(shape=(self.rows,self.cols),dtype=np.int16)
    def saveImg(self,bayerBuffer):
        if self.isSavebayer:
            np.array(bayerBuffer,dtype=np.uint16).tofile(self.bayerOpath)
        if self.isSaveBmp:
            bmpArray = np.reshape(bayerBuffer, newshape=(self.rows, self.cols))
            bmpArray = np.power(bmpArray / 1023, 1 / 2.2) * 1023
            bmpArray = bmpArray.astype(np.uint16)
            scipy.misc.imsave(self.bmpOpath,bmpArray)
        if self.isSaveJpeg:
            tmpBuffer = np.reshape(bayerBuffer,newshape=(self.rows,self.cols)).astype(np.int16)
            # demosaic
            # tmpBuffer = cv2.cvtColor(tmpBuffer, cv2.COLOR_BAYER_RG2BGR)
            tmpBuffer = np.array(tmpBuffer / 4,dtype=np.uint8)
            cv2.imwrite(self.jepgOpath,tmpBuffer)
            tmpBuffer = cv2.resize(tmpBuffer,(1200,900))
            cv2.imshow("res",tmpBuffer)
            cv2.waitKey(0)
    def run(self):
        mipiBuffer = np.fromfile(self.iPath,dtype= "ubyte").tolist()
        print("mipiRawLength :", len(mipiBuffer))
        bayerBuffer = [0 for i in range(self.bayerLen)]
        index = 0
        for i in range(0, self.mipiLen, 5):
            b1,b2,b3,b4,b5 = mipiBuffer[i],mipiBuffer[i + 1],mipiBuffer[i + 2],mipiBuffer[i + 3],mipiBuffer[i + 4]
            bayerBuffer[index + 0] = (b1 << 2) | ((b5 & 0x3) >> 0)
            bayerBuffer[index + 1] = (b2 << 2) | ((b5 & 0xc) >> 2)
            bayerBuffer[index + 2] = (b3 << 2) | ((b5 & 0x30) >> 4)
            bayerBuffer[index + 3] = (b4 << 2) | ((b5 & 0xc0) >> 6)
            index += 4
        self.saveImg(bayerBuffer)
if __name__ == '__main__':
    # readHdrRaw().run()
    path = r"C:\Users\l00520372\Desktop\123456\123456.raw"
    readMipiRaw(path).run()

