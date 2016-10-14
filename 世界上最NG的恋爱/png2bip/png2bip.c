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
}Index[10000];

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

void ReadPng(FILE *OpenPng, unit32 Width, unit32 Height, unit8* PixelData, unit8* TexData)
{
	png_structp png_ptr;
	png_infop info_ptr, end_ptr;
	png_colorp pcolor;
	png_bytep *rows;
	unit32 i = 0, bpp = 8;
	unit32 nplt = 256;//��ɫ����
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL)
	{
		printf("PNG��Ϣ����ʧ��!\n");
		exit(0);
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		printf("info��Ϣ����ʧ��!\n");
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		exit(0);
	}
	end_ptr = png_create_info_struct(png_ptr);
	if (end_ptr == NULL)
	{
		printf("end��Ϣ����ʧ��!\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		exit(0);
	}
	png_init_io(png_ptr, OpenPng);
	png_read_info(png_ptr, info_ptr);
	png_set_IHDR(png_ptr, info_ptr, Width, Height, bpp, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	png_get_PLTE(png_ptr, info_ptr, &pcolor, &nplt);
	for (i = 0; i < nplt; i++)
	{
		PixelData[i * 4 + 0] = pcolor[i].red;
		PixelData[i * 4 + 1] = pcolor[i].green;
		PixelData[i * 4 + 2] = pcolor[i].blue;
		PixelData[i * 4 + 3] = 0xFF;
	}
	rows = (png_bytep*)malloc(Height*sizeof(char*));
	for (i = 0; i < Height; i++)
	{
		rows[i] = (png_bytep)(TexData + Width*i);
	}
	png_read_image(png_ptr, rows);
	free(rows);
	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_ptr);
}

void WriteFile(unit32 FileNum, char *dname)
{
	unit32 i = 0, Width = 480, Height = 360;
	FILE *OpenFile, *WriteFile, *OpenPng;
	unit8* PixelData, *TexData, *Data;
	char OutName[260];
	_chdir(dname);
	_mkdir("import");
	for (i = 0; i < FileNum; i++)
	{
		sprintf(OutName, Index[i].FileName);
		fopen_s(&OpenFile, Index[i].FileName, "rb");
		if (OpenFile == NULL)
		{
			printf("��ԭ�ļ�ʧ��\n");
			system("pause");
		}
		else
		{
			fopen_s(&OpenPng, strcat(Index[i].FileName, ".png"), "rb");
			if (OpenPng == NULL)
			{
				printf("��PNG�ļ�ʧ��\n");
				system("pause");
			}
			PixelData = malloc(1024);
			TexData = malloc(Index[i].FileSize - 1280);
			Data = malloc(256);
			fseek(OpenFile, 0, SEEK_SET);
			fread(Data, 256, 1, OpenFile);
			ReadPng(OpenPng, Width, Height, PixelData, TexData);
			_chdir("import");
			fopen_s(&WriteFile, OutName, "wb");
			if (WriteFile == NULL)
			{
				printf("д���ļ�ʧ��\n");
				system("pause");
			}
			fseek(WriteFile, 0, SEEK_SET);
			fwrite(Data, 256, 1, WriteFile);
			fwrite(PixelData, 1024, 1, WriteFile);
			fwrite(TexData, Index[i].FileSize - 1280, 1, WriteFile);
			free(PixelData);
			free(TexData);
			free(Data);
			fclose(OpenFile);
			fclose(OpenPng);
			fclose(WriteFile);
			_chdir("..");
		}
		printf("Size:0x%X %s \n", Index[i].FileSize, Index[i].FileName);
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