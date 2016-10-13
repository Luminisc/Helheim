/*
����ת���ֿ�
by Darkness-TX
2015.12.5
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <direct.h>
#include <png.h>
#define DataOffset 0x20//ͼƬ���ݿ��ַ
#define PixelOffset 0x40020//��ɫ���ַ
#define FileSize 0x40060//�ļ���С
#define DataSize 0x40000//ͼƬ���ݿ��С

typedef unsigned char  unit8;
typedef unsigned short unit16;
typedef unsigned int   unit32;

unit32 FileNum = 2;//���ļ�������ʼ����Ϊ0

void WritePng(FILE *Pngname, unit32 Width, unit32 Height, unit8* PixelData, unit8* data)
{
	png_structp png_ptr;
	png_infop info_ptr;
	png_colorp pcolor;
	unit8 *png_alpha;
	unit32 i = 0, bpp = 4;//4λ����ͼ
	unit32 nplt = 16;//��ɫ��ɫ��
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL)
	{
		printf("PNG��Ϣ����ʧ��!\n");
		system("pause");
		exit(0);
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		printf("info��Ϣ����ʧ��!\n");
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		system("pause");
		exit(0);
	}
	png_init_io(png_ptr, Pngname);
	png_set_IHDR(png_ptr, info_ptr, Width, Height, bpp, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	pcolor = (png_colorp)malloc(nplt*sizeof(png_color));
	png_alpha = (unit8 *)malloc(nplt * sizeof(unit8));
	for (i = 0; i<nplt; i++)
	{
		pcolor[i].red = PixelData[i * 4 + 0];
		pcolor[i].green = PixelData[i * 4 + 1];
		pcolor[i].blue = PixelData[i * 4 + 2];
		png_alpha[i] = PixelData[i * 4 + 3];
	}
	png_set_PLTE(png_ptr, info_ptr, pcolor, nplt);
	png_set_tRNS(png_ptr, info_ptr, (png_bytep)png_alpha, nplt, (png_color_16p)0);
	free(pcolor);
	free(png_alpha);
	png_write_info(png_ptr, info_ptr);
	for (i = 0; i < Height; i++)
		png_write_row(png_ptr, data + i*Width / 2);//4λ����ͼ������ռһ�ֽڣ����Գ���2
	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);
}

void WriteFile()
{
	unit8 *data;
	unit8* PixelData;
	unit32 i, j;
	unit16 Width = 1024, Height = 512;
	FILE *OpenFile, *WriteFile;
	char *FileName[] = { "Font00.tm2","Font01.tm2" };
	char *PngName[] = { "Font00.tm2.png","Font01.tm2.png" };
	for (i = 0;i < FileNum;i++)
	{
		OpenFile = fopen(FileName[i], "rb");
		if (OpenFile == NULL)
		{
			printf("��%sʧ��",FileName[i]);
			system("pause");
			exit(0);
		}
		WriteFile = fopen(PngName[i], "wb");
		fseek(OpenFile, 0x20, SEEK_SET);
		data = malloc(DataSize);
		fread(data, DataSize, 1, OpenFile);
		PixelData = malloc(FileSize - PixelOffset);
		fseek(OpenFile, PixelOffset, SEEK_SET);
		fread(PixelData, FileSize - PixelOffset, 1, OpenFile);
		for (j = 0;j < DataSize;j++)
			data[j] = (data[j] << 4) | (data[j] >> 4);//4λͼ�ⷽ��pc���ƻ����û��е㲻ͬ������Ҫ�ߵ�λ����
		WritePng(WriteFile, Width, Height, PixelData, data);
		free(data);
		free(PixelData);
		fclose(OpenFile);
		fclose(WriteFile);
	}
}

int main(int argc, char *argv[])
{
	printf("project��Helheim-��Թ\n����ת���ֿ⣬��Font00.tm2��Font01.tm2����ͬһĿ¼�º����г���\nby Darkness-TX 2015.12.5\nEmail��1545492547@qq.com\n");
	WriteFile();
	printf("����ɣ����ļ���%d\n", FileNum);
	system("pause");
	return 0;
}