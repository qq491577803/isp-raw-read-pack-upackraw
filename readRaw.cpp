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
void bayer2unpack(char *iPath, char *oPath)
{

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
		fwrite(&tmpH1, sizeof(unsigned char), 1, oFile);

		tmp2 = (iBuffer[pixlIndex + 1]) & 0x3ff;//10bit
		tmpH2 = (tmp2 & 0x3fc) >> 2;//h8bit
		tmpL2 = (tmp2 & 0x3) << 2;
		fwrite(&tmpH2, sizeof(unsigned char), 1, oFile);

		tmp3 = (iBuffer[pixlIndex + 2]) & 0x3ff;//10bit
		tmpH3 = (tmp3 & 0x3fc) >> 2;//h8bit
		tmpL3 = (tmp3 & 0x3) << 4;
		fwrite(&tmpH3, sizeof(unsigned char), 1, oFile);

		tmp4 = (iBuffer[pixlIndex + 3]) & 0x3ff;//10bit
		tmpH4 = (tmp4 & 0x3fc) >> 2;//h8bit
		tmpL4 = (tmp4 & 0x3) << 6;
		fwrite(&tmpH4, sizeof(unsigned char), 1, oFile);

		tmp5 = tmpL1 | tmpL2 | tmpL3 | tmpL4;
		fwrite(&tmp5, sizeof(unsigned char), 1, oFile);
		pixlIndex += 4;
	}
	fclose(oFile);
}


int w = 4096;
int h = 3072;
unsigned char unpackBuff[4096 * 3072 * 2];
void unpack2bayer(char *iPath, char *oPath)
{

	FILE *iFile = fopen(iPath, "rb");
	FILE *oFile = fopen(oPath, "wb");
	fread(unpackBuff, 1, w*h*1.25, iFile);
	//每次读取5个8bit，写出4个10bit
	short tmpH1, tmpH2, tmpH3, tmpH4, tmpL1, tmpL2, tmpL3, tmpL4;
	short tmp1, tmp2, tmp3, tmp4;
	cout << "前5个" << endl;
	for (int xx = 0; xx < 25; xx++)
		printf("%d,", unpackBuff[xx]);
	cout << endl;

	for (int pixlIndex = 0; pixlIndex < 4096 * 3072 * 1.25 - 5;)
	{
		tmpH1 = (unpackBuff[pixlIndex] << 2) & 0x03FC;
		tmpL1 = unpackBuff[pixlIndex + 4] & 0x0003;
		tmp1 = tmpH1 | tmpL1;
		fwrite(&tmp1, sizeof(short), 1, oFile);
		tmpH2 = (unpackBuff[pixlIndex + 1] << 2) & 0x03FC;
		tmpL2 = ((unpackBuff[pixlIndex + 4]) >> 2) & 0x0003;
		tmp2 = tmpH2 | tmpL2;
		fwrite(&tmp2, sizeof(short), 1, oFile);
		tmpH3 = (unpackBuff[pixlIndex + 2] << 2) & 0x03FC;
		tmpL3 = ((unpackBuff[pixlIndex + 4]) >> 4) & 0x0003;
		tmp3 = tmpH3 | tmpL3;
		fwrite(&tmp3, sizeof(short), 1, oFile);
		tmpH4 = (unpackBuff[pixlIndex + 3] << 2) & 0x03FC;
		tmpL4 = ((unpackBuff[pixlIndex + 4]) >> 6) & 0x0003;
		tmp4 = tmpH4 | tmpL4;
		fwrite(&tmp4, sizeof(short), 1, oFile);
		pixlIndex += 5;
	}
	fclose(oFile);
}


//refer to unpack raw https://blog.csdn.net/superhubin/article/details/101361101
char raw_array[4096 * 3072 * 2];//实际上会使用40*6*3072*1.25个
short pixel_array[4096 * 3072 * 2];
int readPackedRaw1()
{
	char iPath[256] = "C:\\Users\\l00520372\\Desktop\\Q\\Bayer2Yuv_image_4096x3072_0.raw";
	char oPath[256] = "C:\\Users\\l00520372\\Desktop\\Q\\upack2bayer.raw";
	int width = 4096;
	int height = 3072;
	FILE* fp = fopen(iPath, "rb");
	FILE* fo = fopen(oPath, "wb");
	int bufferSize = sizeof(char) * width * height * 1.25;

	fread(raw_array, 1, bufferSize, fp);
	int byte_index, pixel_index;
	for (byte_index = 0, pixel_index = 0; byte_index < (bufferSize - 5); )
	{
		pixel_array[pixel_index + 0] = (((short)(raw_array[byte_index + 0])) << 2) & 0x03FC;
		pixel_array[pixel_index + 0] = pixel_array[pixel_index + 0] | (short)((raw_array[byte_index + 4] >> 0) & 0x0003);

		pixel_array[pixel_index + 1] = (((short)(raw_array[byte_index + 1])) << 2) & 0x03FC;
		pixel_array[pixel_index + 1] = pixel_array[pixel_index + 1] | (short)((raw_array[byte_index + 4] >> 2) & 0x0003);

		pixel_array[pixel_index + 2] = (((short)(raw_array[byte_index + 2])) << 2) & 0x03FC;
		pixel_array[pixel_index + 2] = pixel_array[pixel_index + 2] | (short)((raw_array[byte_index + 4] >> 4) & 0x0003);

		pixel_array[pixel_index + 3] = (((short)(raw_array[byte_index + 3])) << 2) & 0x03FC;
		pixel_array[pixel_index + 3] = pixel_array[pixel_index + 3] | (short)((raw_array[byte_index + 4] >> 6) & 0x0003);

		byte_index = byte_index + 5;
		pixel_index = pixel_index + 4;
	}
	int out_size = width * height * 2;
	fwrite(pixel_array, 1, out_size, fo);

	fclose(fp);
	fclose(fo);
	system("pause");
	return 0;
}

int main()
{
	//bayer2unpack();
	unpack2bayer();
}
