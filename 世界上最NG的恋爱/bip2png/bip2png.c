#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <direct.h>
#include <png.h>
#define DataOffset 0x500//ͼƬ���ݿ��ַ
#define WHOffset 0x88//�����ַ
#define PixelOffset 0x100//��ɫ���ַ

typedef unsigned char  unit8;
typedef unsigned short unit16;
typedef unsigned int   unit32;

unit32 FileNum = 0;//���ļ�������ʼ����Ϊ0

struct index
{
	char FileName[260];//�ļ���
	unit32 FileSize;//�ļ���С
}Index[2000];

unit32 process_dir(char *dname)
{
	long Handle;
	unit32 i = 0;
	struct _finddata64i32_t FileInfo;
	_chdir(dname);//��ת·��
	if ((Handle = _findfirst("*.bin", &FileInfo)) == -1L)
	{
		printf("û���ҵ�ƥ�����Ŀ���뽫ԭ�ļ���׺����Ϊ.bin\n");
		system("pause");
		return -1;
	}
	do
	{
		if (FileInfo.name[0] == '.')  //���˱���Ŀ¼�͸�Ŀ¼
			continue;
		for (i = 0; i <= 260; i++)
			Index[FileNum].FileName[i] = FileInfo.name[i];
		Index[FileNum].FileSize = FileInfo.size;
		FileNum++;
	} while (_findnext(Handle, &FileInfo) == 0);
	return FileNum;
}

void WritePng(FILE *Pngname, unit32 Width, unit32 Height, unit8* PixelData, unit8* data)
{
	png_structp png_ptr;
	png_infop info_ptr;
	png_colorp pcolor;
	unit8 *png_alpha;
	unit32 i = 0, bpp = 8;
	unit32 nplt = 256;//��ɫ����
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL)
	{
		printf("PNG��Ϣ����ʧ��!\n");
		return -1;;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		printf("info��Ϣ����ʧ��!\n");
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		return -1;;
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
		png_write_row(png_ptr, data + i*Width);
	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);
}

void WriteFile(unit32 FileNum, char *dname)
{
	unit8 *data;
	unit8* PixelData;
	unit32 i;
	unit16 Width, Height;
	FILE *OpenFile, *WriteFile;
	for (i = 0; i < FileNum; i++)
	{
		_chdir(dname);
		fopen_s(&OpenFile, Index[i].FileName, "rb");
		sprintf(Index[i].FileName, "%s.png", Index[i].FileName);
		fopen_s(&WriteFile, Index[i].FileName, "wb");
		fseek(OpenFile, WHOffset, SEEK_SET);
		fread(&Width, 2, 1, OpenFile);
		fread(&Height, 2, 1, OpenFile);
		fseek(OpenFile, PixelOffset, SEEK_SET);
		PixelData = malloc(1024);
		data = malloc(Index[i].FileSize - DataOffset);
		fread(PixelData, 1024, 1, OpenFile);
		fread(data, Index[i].FileSize - DataOffset, 1, OpenFile);
		WritePng(WriteFile, Width, Height, PixelData, data);
		free(PixelData);
		free(data);
		fclose(WriteFile);
		fclose(OpenFile);
	}
}

int main(int argc, char *argv[])
{
	process_dir(argv[1]);
	WriteFile(FileNum, argv[1]);
	printf("����ɣ����ļ���%d\n", FileNum);
	system("pause");
	return 0;
}