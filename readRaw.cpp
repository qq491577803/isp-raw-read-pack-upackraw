#include<iostream>
#include<opencv2/opencv.hpp>
#include<math.h>
#include<time.h>
#include<vector>
using namespace cv;
using namespace std;


int width = 4096;
int height = 3072;
unsigned short iBuffer[4096 * 3072];
void bayer2unpack()
{
	char iPath[256] = "C:\\Users\\Administrator\\Desktop\\cat.raw";
	char oPath[256] = "C:\\Users\\Administrator\\Desktop\\bayer2upack.raw";
	FILE *iFile = fopen(iPath, "rb");
	FILE *oFile = fopen(oPath, "wb");
	fread(iBuffer, sizeof(unsigned short), width * height, iFile);
	cout << "iBuffer size:" << sizeof(iBuffer) << endl;
	// bayer raw to unpack raw;
	//读取4个10bit 写出5个8bit
	int pixlIndex;

	unsigned char tmpH1, tmpH2, tmpH3, tmpH4, tmpL1, tmpL2, tmpL3, tmpL4;
	unsigned short tmp1, tmp2, tmp3, tmp4, tmp5;
	for (pixlIndex = 0; pixlIndex < (width * height - 4); )
	{
		tmp1 = (iBuffer[pixlIndex]) & 0x3ff;//10bit
		tmpH1 = (tmp1 & 0x3fc) >> 2;//h8bit
		tmpL1 = tmp1 & 0x3;
		fwrite(&tmp1, sizeof(unsigned char), 1, oFile);

		tmp2 = (iBuffer[pixlIndex + 1]) & 0x3ff;//10bit
		tmpH2 = (tmp2 & 0x3fc) >> 2;//h8bit
		tmpL2 = (tmp2 & 0x3) << 2;
		fwrite(&tmp2, sizeof(unsigned char), 1, oFile);

		tmp3 = (iBuffer[pixlIndex + 2]) & 0x3ff;//10bit
		tmpH3 = (tmp3 & 0x3fc) >> 2;//h8bit
		tmpL3 = (tmp3 & 0x3) << 4;
		fwrite(&tmp3, sizeof(unsigned char), 1, oFile);

		tmp4 = (iBuffer[pixlIndex + 3]) & 0x3ff;//10bit
		tmpH4 = (tmp4 & 0x3fc) >> 2;//h8bit
		tmpL4 = (tmp4 & 0x3) << 6;
		fwrite(&tmp4, sizeof(unsigned char), 1, oFile);

		tmp5 = tmpL1 | tmpL2 | tmpL3 | tmpL4;
		fwrite(&tmp5, sizeof(unsigned char), 1, oFile);
		pixlIndex += 4;
	}
	fclose(oFile);
}

int w = 4096;
int h = 3072;
unsigned char unpackBuff[4096 * 3072 * 2];
void unpack2bayer()
{
	char iPath[256] = "C:\\Users\\Administrator\\Desktop\\bayer2upack.raw";
	char oPath[256] = "C:\\Users\\Administrator\\Desktop\\unpack2bayer.raw";
	FILE *iFile = fopen(iPath, "rb");
	FILE *oFile = fopen(oPath, "wb");
	fread(unpackBuff,1,w*h*1.25, iFile);
	//每次读取5个8bit，写出4个10bit
	short tmpH1, tmpH2, tmpH3, tmpH4, tmpL1, tmpL2, tmpL3, tmpL4;
	short tmp1, tmp2, tmp3, tmp4;
	for (int pixlIndex = 0; pixlIndex < 4096 * 3072*1.25;)
	{
		tmpH1 = unpackBuff[pixlIndex] << 2;
		tmpL1 = unpackBuff[pixlIndex + 4];
		tmp1 = tmpH1 | tmpL1;
		fwrite(&tmp1, sizeof(short), 1, oFile);
		tmpH2 = unpackBuff[pixlIndex + 1] << 2;
		tmpL2 = (unpackBuff[pixlIndex + 4] & 0xc) >> 2;
		tmp2 = tmpH2 | tmpL2;
		fwrite(&tmp2, sizeof(short), 1, oFile);
		tmpH3 = unpackBuff[pixlIndex + 2] << 2;
		tmpL3 = (unpackBuff[pixlIndex + 4] & 0x30) >> 4;
		tmp3 = tmpH3 | tmpL3;
		fwrite(&tmp3, sizeof(short), 1, oFile);
		tmpH4 = unpackBuff[pixlIndex + 3] << 2;
		tmpL4 = (unpackBuff[pixlIndex + 4] & 0xc0) >> 6;
		tmp4 = tmpH4 | tmpL4;
		fwrite(&tmp4, sizeof(short), 1, oFile);
		pixlIndex += 5;
	}
	fclose(oFile);
}

int main()
{
	//bayer2unpack();
	unpack2bayer();
}
