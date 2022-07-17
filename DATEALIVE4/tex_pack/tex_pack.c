/*
���ڽ��tex�ļ�
made by Darkness-TX
2022.07.12
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <direct.h>
#include <Windows.h>
#include <png.h>

typedef unsigned char  unit8;
typedef unsigned short unit16;
typedef unsigned int   unit32;
typedef unsigned __int64 unit64;

unit32 FileNum = 0;//���ļ�������ʼ����Ϊ0

typedef struct texture_s
{
	unit8 magic[20];
	unit32 block_length;//���С
	unit64 flag;//ͼƬ����
	unit32 pic_length;//ͼƬ��С
	unit16 width;
	unit16 height;
}texture_t;

typedef struct parts_s
{
	unit8 magic[20];
	unit32 length;//����
	unit32 frame_num;//����
}parts_t;

typedef struct frame_s
{
	unit32 unk1;
	unit32 unk2;
	unit32 frameWidth;
	unit32 frameHeight;
	unit32 frameXScale;
	unit32 frameYScale;
	unit32 frameWidthScale;
	unit32 frameHeightScale;
}frame_t;

typedef struct anime_s
{
	unit8 magic[20];
	unit32 length;//����
	unit32 info_num;//����
}anime_t;

struct index
{
	char FileName[MAX_PATH];//�ļ���
}Index[5000];

unit32 process_dir(char* dname)
{
	long Handle;
	unit32 i = 0;
	struct _finddata64i32_t FileInfo;
	_chdir(dname);//��ת·��
	if ((Handle = _findfirst("*.tex", &FileInfo)) == -1L)
	{
		printf("û���ҵ�ƥ�����Ŀ���뽫ԭ�ļ���׺����Ϊ.tex\n");
		system("pause");
		return -1;
	}
	do
	{
		if (FileInfo.name[0] == '.')  //���˱���Ŀ¼�͸�Ŀ¼
			continue;
		sprintf(Index[FileNum].FileName, FileInfo.name);
		FileNum++;
	} while (_findnext(Handle, &FileInfo) == 0);
	return FileNum;
}

unit32 CheckString(char* buff)
{
	if (strncmp(buff, "0x", 2) == 0 || strncmp(buff, "0X", 2) == 0)
		return strtoul(buff + 2, NULL, 16);
	else
		return strtoul(buff, NULL, 10);
}

unit8* ReadPng8bit(FILE* Pngfile, unit32 *width, unit32 *height)
{
	png_structp png_ptr;
	png_infop info_ptr, end_ptr;
	png_bytep *rows;
	unit32 i = 0, bpp = 0, format = 0;
	unit8 *data = NULL;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
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
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		system("pause");
		exit(0);
	}
	end_ptr = png_create_info_struct(png_ptr);
	if (end_ptr == NULL)
	{
		printf("end��Ϣ����ʧ��!\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		system("pause");
		exit(0);
	}
	png_init_io(png_ptr, Pngfile);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, width, height, &bpp, &format, NULL, NULL, NULL); 
	unit8 *udata = malloc(*width * *height);
	if (format == PNG_COLOR_TYPE_RGB_ALPHA)
	{
		data = malloc(*width * *height * 4);
		rows = (png_bytep*)malloc(*height * sizeof(char*));
		for (i = 0; i < *height; i++)
			rows[i] = (png_bytep)(data + *width * i * 4);
		png_read_image(png_ptr, rows);
		free(rows);
		for (i = 0; i < *width * *height; i++)
			udata[i] = data[i * 4 + 3];
		free(data);
	}
	else
	{
		printf("��֧�ֵ�bppģʽ!");
		free(udata);
		system("pause");
		exit(0);
	}
	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_ptr);
	return udata;
}

unit8* ReadPng32bit(FILE* Pngfile, unit32 *width, unit32 *height)
{
	png_structp png_ptr;
	png_infop info_ptr, end_ptr;
	png_bytep *rows;
	unit32 i = 0, bpp = 0, format = 0;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
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
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		system("pause");
		exit(0);
	}
	end_ptr = png_create_info_struct(png_ptr);
	if (end_ptr == NULL)
	{
		printf("end��Ϣ����ʧ��!\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		system("pause");
		exit(0);
	}
	png_init_io(png_ptr, Pngfile);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, width, height, &bpp, &format, NULL, NULL, NULL); 
	unit8* data = malloc(*width * *height * 4);
	if (format == PNG_COLOR_TYPE_RGB_ALPHA)
	{
		rows = (png_bytep*)malloc(*height * sizeof(char*));
		for (i = 0; i < *height; i++)
			rows[i] = (png_bytep)(data + *width * i * 4);
		png_read_image(png_ptr, rows);
		free(rows);
	}
	else
	{
		printf("��֧�ֵ�bppģʽ!");
		free(data);
		system("pause");
		exit(0);
	}
	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_ptr);
	return data;
}

void ReadPng(FILE* Pngfile, unit32 *width, unit32 *height)
{
	png_structp png_ptr;
	png_infop info_ptr, end_ptr;
	unit32 i = 0, bpp = 0, format = 0;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
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
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		system("pause");
		exit(0);
	}
	end_ptr = png_create_info_struct(png_ptr);
	if (end_ptr == NULL)
	{
		printf("end��Ϣ����ʧ��!\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		system("pause");
		exit(0);
	}
	png_init_io(png_ptr, Pngfile);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, width, height, &bpp, &format, NULL, NULL, NULL);
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_ptr);
}


void Pack(char* dname)
{
	unit8 *data = NULL, namebuff[MAX_PATH], zero = '\0', buff[20];
	FILE *src = NULL, *dst = NULL, *pngfile = NULL;
	unit32 i = 0, j = 0, block_length = 0;
	_chdir(dname);
	for (i = 0; i < FileNum; i++)
	{
		src = fopen(Index[i].FileName, "rb");
		if (src == NULL)
		{
			printf("��%sʧ��", Index[i].FileName);
			system("pause");
			exit(0);
		}
		texture_t Texture;
		fread(Texture.magic, 20, 1, src);
		if (strncmp(Texture.magic, "Texture ", 8) != 0)
		{
			fseek(src, 0, SEEK_SET);
			Texture.block_length = 0;
		}
		else
			fread(&Texture.block_length, 4, 1, src);
		block_length = Texture.block_length;
		fread(&Texture.flag, 8, 1, src);
		fread(&Texture.pic_length, 4, 1, src);
		fread(&Texture.width, 2, 1, src);
		fread(&Texture.height, 2, 1, src);
		if (Texture.flag == 0x0210000000000080)//8λ�Ҷ�ͼ
		{
			sprintf(namebuff, "%s.png", Index[i].FileName);
			pngfile = fopen(namebuff, "rb");
			unit32 width = 0;
			unit32 height = 0;
			data = ReadPng8bit(pngfile, &width, &height);
			Texture.width = width;
			Texture.height = height;
			Texture.pic_length = width * height;
			fclose(pngfile);
			if (block_length != 0)
				Texture.block_length = Texture.pic_length + sizeof(texture_t);
		}
		else if (Texture.flag == 0x0810000000004000)//32λrgba
		{
			sprintf(namebuff, "%s.png", Index[i].FileName);
			pngfile = fopen(namebuff, "rb");
			unit32 width = 0;
			unit32 height = 0;
			data = ReadPng32bit(pngfile, &width, &height);
			Texture.width = width;
			Texture.height = height;
			Texture.pic_length = width * height * 4;
			fclose(pngfile);
			if (block_length != 0)
				Texture.block_length = Texture.pic_length + sizeof(texture_t);
		}
		else if (Texture.flag == 0x0810000100004000)//png
		{
			sprintf(namebuff, "%s.png", Index[i].FileName);
			pngfile = fopen(namebuff, "rb");
			unit32 width = 0;
			unit32 height = 0;
			ReadPng(pngfile, &width, &height);
			Texture.width = width;
			Texture.height = height;
			fseek(pngfile, 0, SEEK_END);
			Texture.pic_length = ftell(pngfile);
			fseek(pngfile, 0, SEEK_SET);
			data = malloc(Texture.pic_length);
			fread(data, Texture.pic_length, 1, pngfile);
			fclose(pngfile);
			if (block_length != 0)
				Texture.block_length = Texture.pic_length + sizeof(texture_t);
		}
		else
		{
			printf("�ݲ�֧�ֵ��ļ����ͣ�flag:0x%llX\n", Texture.flag);
			system("pause");
			exit(0);
		}
		printf("%s size:0x%X width:%d height:%d\n", Index[i].FileName, Texture.pic_length, Texture.width, Texture.height);
		sprintf(namebuff, "%s.new", Index[i].FileName);
		dst = fopen(namebuff, "wb");
		if (block_length == 0)
		{
			fwrite(&Texture.flag, 8, 1, dst);
			fwrite(&Texture.pic_length, 4, 1, dst);
			fwrite(&Texture.width, 2, 1, dst);
			fwrite(&Texture.height, 2, 1, dst);
			fwrite(data, Texture.pic_length, 1, dst);
			free(data);
			fclose(dst);
		}
		else
		{
			fwrite(Texture.magic, 20, 1, dst);
			fwrite(&Texture.block_length, 4, 1, dst);
			fwrite(&Texture.flag, 8, 1, dst);
			fwrite(&Texture.pic_length, 4, 1, dst);
			fwrite(&Texture.width, 2, 1, dst);
			fwrite(&Texture.height, 2, 1, dst);
			fwrite(data, Texture.pic_length, 1, dst);
			free(data);
			fseek(src, block_length + 7 & ~7, SEEK_SET);
			parts_t Parts;
			fread(Parts.magic, 20, 1, src);
			if (strncmp(Parts.magic, "Parts   ", 8) != 0)
			{
				printf("��֧�ֵ��ļ����ͣ������Ƿ���Parts��\n");
				system("pause");
				exit(0);
			}
			fread(&Parts.length, 4, 1, src);
			fread(&Parts.frame_num, 4, 1, src);
			for (j = (Texture.block_length + 7 & ~7) - ftell(dst); j > 0; j--)
				fwrite(&zero, 1, 1, dst);
			sprintf(namebuff, "%s\\%s.ini", dname, Index[i].FileName);
			if (_access(namebuff, 4) == -1)
			{
				printf("��ʼ��ʧ�ܣ���ȷ��Ŀ¼���Ƿ���%s\n", namebuff);
				system("pause");
				exit(0);
			}
			unit32 frame_num = 0;
			frame_num = GetPrivateProfileIntA("Parts", "frame_num", 0, namebuff);
			if (frame_num != Parts.frame_num)
			{
				printf("Parts���frame_num����һ�£�frame_num=%d Parts.frame_num=%d\n", frame_num, Parts.frame_num);
				system("pause");
			}
			Parts.frame_num = frame_num;
			Parts.length = frame_num * 0x20 + 0x1C;
			printf("\tframe_num=%d length=0x%X\n", Parts.frame_num, Parts.length);
			fwrite(Parts.magic, 20, 1, dst);
			fwrite(&Parts.length, 4, 1, dst);
			fwrite(&Parts.frame_num, 4, 1, dst);
			for (j = 1; j <= Parts.frame_num; j++)
			{
				frame_t Frame;
				char keybuff[20];
				sprintf(keybuff, "unk1_%d", j);
				GetPrivateProfileStringA("Parts", keybuff, "0", buff, 20, namebuff);
				Frame.unk1 = CheckString(buff);
				sprintf(keybuff, "unk2_%d", j);
				GetPrivateProfileStringA("Parts", keybuff, "0", buff, 20, namebuff);
				Frame.unk2 = CheckString(buff);
				sprintf(keybuff, "frameWidth_%d", j);
				GetPrivateProfileStringA("Parts", keybuff, "0", buff, 20, namebuff);
				Frame.frameWidth = CheckString(buff);
				sprintf(keybuff, "frameHeight_%d", j);
				GetPrivateProfileStringA("Parts", keybuff, "0", buff, 20, namebuff);
				Frame.frameHeight = CheckString(buff);
				sprintf(keybuff, "frameXScale_%d", j);
				GetPrivateProfileStringA("Parts", keybuff, "0", buff, 20, namebuff);
				Frame.frameXScale = CheckString(buff);
				sprintf(keybuff, "frameYScale_%d", j);
				GetPrivateProfileStringA("Parts", keybuff, "0", buff, 20, namebuff);
				Frame.frameYScale = CheckString(buff);
				sprintf(keybuff, "frameWidthScale_%d", j);
				GetPrivateProfileStringA("Parts", keybuff, "0", buff, 20, namebuff);
				Frame.frameWidthScale = CheckString(buff);
				sprintf(keybuff, "frameHeightScale_%d", j);
				GetPrivateProfileStringA("Parts", keybuff, "0", buff, 20, namebuff);
				Frame.frameHeightScale = CheckString(buff);
				fwrite(&Frame, sizeof(frame_t), 1, dst);
			}
			for (j = (ftell(dst) + 7 & ~7) - ftell(dst); j > 0; j--)
				fwrite(&zero, 1, 1, dst);
			for (j = 1; j <= Parts.frame_num; j++)
			{
				frame_t Frame;
				fread(&Frame, sizeof(frame_t), 1, src);
			}
			fseek(src, ftell(src) + 7 & ~7, SEEK_SET);
			anime_t Anime;
			fread(Anime.magic, 20, 1, src);
			if (strncmp(Anime.magic, "Anime   ", 8) != 0)
			{
				printf("��֧�ֵ��ļ����ͣ������Ƿ���Anime��\n");
				system("pause");
				exit(0);
			}
			fread(&Anime.length, 4, 1, src);
			fread(&Anime.info_num, 4, 1, src);
			if (Anime.info_num != 0)
			{
				printf("Anime�������ݣ�info_num=%d\n", Anime.info_num);
				system("pause");
			}
			unit32 info_num = 0;
			info_num = GetPrivateProfileIntA("Anime", "info_num", 0, namebuff);
			if (info_num != Anime.info_num)
			{
				printf("Anime���info_num����һ�£�info_num=%d Anime.info_num=%d\n", info_num, Anime.info_num);
				system("pause");
			}
			Anime.info_num = info_num;
			Anime.length = info_num * 0x20 + 0x1C;
			fwrite(Anime.magic, 20, 1, dst);
			fwrite(&Anime.length, 4, 1, dst);
			fwrite(&Anime.info_num, 4, 1, dst);
			for (j = (ftell(dst) + 0x0F & ~0x0F) - ftell(dst); j > 0; j--)
				fwrite(&zero, 1, 1, dst);
		}
		fclose(dst);
		fclose(src);
	}
}

int main(int argc, char* argv[])
{
	printf("project��Helheim-DATE A LIVE ��Dystopia\n���ڷ��tex�ļ�����tex�ļ��Ž��ļ��У������ļ��е�������\nby Darkness-TX 2022.07.12\n\n");
	process_dir(argv[1]);
	Pack(argv[1]);
	printf("����ɣ����ļ���%d\n", FileNum);
	system("pause");
	return 0;
}