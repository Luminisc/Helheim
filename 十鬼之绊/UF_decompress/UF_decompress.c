/*
���ڽ�ѹ�ļ�ͷΪUF���ֿ��ļ�
made by Darkness-TX
2019.05.23
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <io.h>
#include <direct.h>
#include <Windows.h>
#include <locale.h>
#include <png.h>

typedef unsigned char  unit8;
typedef unsigned short unit16;
typedef unsigned int   unit32;

unit32 FontNum = 0;

struct uf_header
{
	char magic[2];//UF
	unit16 version;
	unit32 count;
	unit8 type;
	unit8 width;
	unit8 height;
	unit8 flag;
	unit32 alpha;
	unit32 filesize;
	unit32 sjis_offset;
	unit32 index_offset;
	unit32 font_offset;
	unit32 unk1;
	unit32 unk2;
}UF_Header;

struct index
{
	unit8 width;
	unit8 height;
	unit16 size;
	unit32 offset;
}Index[10000];

struct sjis_index
{
	unit32 start;
	unit32 end;
	unit32 count;
}Sjis_Index[1000];

void Dumpsjis(FILE *src)
{
	unit32 i = 0, count = (UF_Header.index_offset - UF_Header.sjis_offset) / 0x0C;
	fseek(src, UF_Header.sjis_offset, SEEK_SET);
	for (i = 0; i < count; i++)
		fread(&Sjis_Index[i], 0x0C, 1, src);
	FILE *dst = fopen("tbl.txt", "wt,ccs=UNICODE");
	WCHAR wbuff;
	unit8 cbuff[2];
	for (i = 0; i < count; i++)
	{
		for (unit32 k = Sjis_Index[i].start; k < Sjis_Index[i].end; k++)
		{
			if (k < 0x100)
			{
				cbuff[0] = k & 0xFF;
				cbuff[1] = 0;
			}
			else
			{
				cbuff[0] = k >> 8;
				cbuff[1] = k & 0xFF;
			}
			MultiByteToWideChar(932, 0, cbuff, 2, &wbuff, 1);
			fwprintf(dst, L"%X=%lc\n", k, wbuff);
		}
	}
	fclose(dst);
}

void ReadIndex(FILE *src)
{
	fread(&UF_Header, sizeof(UF_Header), 1, src);
	if (strncmp(UF_Header.magic, "UF", 2) != 0)
	{
		printf("��֧�ֵ����ͣ��ļ�ͷ����UF��\n");
		system("pause");
		exit(0);
	}
	fseek(src, UF_Header.index_offset, SEEK_SET);
	for (unit32 i = 0; i < UF_Header.count; i++)
	{
		fread(&Index[i], 8, 1, src);
		Index[i].offset += UF_Header.font_offset;
	}
}

void tilec(int twidth, int thigh, unit8 *indata, unit8 *outdata, int width, int high, unit32 bpp)
{
	int w, h, tw, th, ofs, b;
	int wlen;
	unit8 *tempin, *tempout;
	tempin = indata;
	tempout = outdata;
	wlen = width;
	if (bpp == 8)
		b = 1;
	else if (bpp == 4)
		b = 2;
	for (h = 0; h < high; h += thigh)
	{
		for (w = 0; w < width; w += twidth)
		{
			for (th = 0, ofs = 0; th < thigh; th++)
			{
				for (tw = 0; tw < twidth; tw += b)
				{
					if (bpp == 8)
						tempout[ofs + tw] = tempin[0];
					else if (bpp == 4)
						tempout[ofs + tw / 2] = tempin[0];
					tempin++;
				}
				ofs += wlen*bpp / 8;
			}
			tempout += twidth*bpp / 8;
		}
		tempout += wlen*bpp / 8 * (thigh - 1);
	}
}

unit32 WritePng(FILE *Pngname, unit32 Width, unit32 Height, unit8* PaletteData, unit8* data, unit32 bpp)
{
	png_structp png_ptr;
	png_infop info_ptr;
	png_colorp pcolor;
	unit8 *png_alpha;
	unit32 i = 0;
	unit32 nplt;//��ɫ����
	if (bpp == 8)
		nplt = 256;
	else if (bpp == 4)
		nplt = 16;
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL)
	{
		printf("PNG��Ϣ����ʧ��!\n");
		return -1;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		printf("info��Ϣ����ʧ��!\n");
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		return -1;
	}
	png_init_io(png_ptr, Pngname);
	png_set_IHDR(png_ptr, info_ptr, Width, Height, bpp, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	pcolor = (png_colorp)malloc(nplt * sizeof(png_color));
	png_alpha = (unit8 *)malloc(nplt * sizeof(unit8));
	for (i = 0; i<nplt; i++)
	{
		pcolor[i].red = PaletteData[i * 4 + 0];
		pcolor[i].green = PaletteData[i * 4 + 1];
		pcolor[i].blue = PaletteData[i * 4 + 2];
		png_alpha[i] = PaletteData[i * 4 + 4];
	}
	png_set_PLTE(png_ptr, info_ptr, pcolor, nplt);
	png_set_tRNS(png_ptr, info_ptr, (png_bytep)png_alpha, nplt, (png_color_16p)0);
	free(pcolor);
	free(png_alpha);
	png_write_info(png_ptr, info_ptr);
	if (bpp == 8)
		for (i = 0; i < Height; i++)
			png_write_row(png_ptr, data + i*Width);
	else if (bpp == 4)
		for (i = 0; i < Height; i++)
			png_write_row(png_ptr, data + i*Width / 2);
	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);
}

void WritePngFile(unit8* PaletteData)
{
	FILE *src = NULL, *dst = NULL;
	unit32 filesize = 0, Width = 0, Height = 0, i = 0;
	unit8 *data = NULL, *tempdata = NULL, dstname[260];
	Width = 15 * UF_Header.width;
	Height = UF_Header.count / 15 * UF_Header.height;
	src = fopen("UF.bin", "rb");
	fseek(src, 0, SEEK_END);
	filesize = ftell(src);
	fseek(src, 0, SEEK_SET);
	tempdata = malloc(filesize);
	fread(tempdata, filesize, 1, src);
	fclose(src);
	data = malloc(filesize);
	memset(data, 0, filesize);
	tilec(UF_Header.width, UF_Header.height, tempdata, data, Width, Height, 4);
	free(tempdata);
	sprintf(dstname, "UF_png");
	_mkdir(dstname);
	_chdir(dstname);
	for (i = 1; i <= 16; i++)
	{
		sprintf(dstname, "UF%02d.png", i);
		dst = fopen(dstname, "wb");
		WritePng(dst, Width, Height, PaletteData + (i - 1) * 0x40, data, 4);
		//fwrite(data, filesize, 1, dst);
		fclose(dst);
	}
	free(data);
}

void Decompress(char *fname)
{
	FILE *src = NULL, *dst = NULL;
	unit32 i = 0, count = 0;
	unit8 buff = 0, flag = 0, out = 0, *PaletteData = NULL;
	bool ishigh = true;
	src = fopen(fname, "rb");
	ReadIndex(src);
	Dumpsjis(src);
	dst = fopen("UF.bin", "wb");
	fseek(src, sizeof(UF_Header), SEEK_SET);
	PaletteData = malloc(0x400);
	fread(PaletteData, 0x400, 1, src);//256ɫ��16�飬ÿ��16ɫ������ֻ���õ�ǰ8ɫ
	for (i = 0; i < UF_Header.count; i++)
	{
		//printf("No.%d size:0x%X width:%d height:%d src_offset:0x%X dst_offset:0x%X\n", i, Index[i].size, Index[i].width, Index[i].height, Index[i].offset, ftell(dst));
		fseek(src, Index[i].offset, SEEK_SET);
		ishigh = true;
		buff = 0;
		while (Index[i].size)
		{
			fread(&buff, 1, 1, src);
			--Index[i].size;
			//�����λ
			flag = buff >> 4;
			if (flag >= 0x08)
			{
				count = 0;
				fread(&count, 1, 1, src);
				--Index[i].size;
				count += (buff & 0x0F) << 8;
				while (count--)
				{
					if (ishigh)
					{
						//���ֿ�ɫ��8�ף�0x08-0x0F��ѹ����־�������ж��ٸ�0x00-0x07��
						//����flag��0x08�Ǿʹ���ѹ���˶��ٸ�(0x08-8)��flag��0x0C�Ǿʹ���ѹ���˶��ٸ�(0x0C-8)
						//������flag>=0x08ʱ�����Ҫ��8���������ֱ�����
						out = (flag - 8) << 4;
						ishigh = false;
					}
					else
					{
						out += (flag - 8) & 0x0F;
						ishigh = true;
						fwrite(&out, 1, 1, dst);
					}
				}
				//����������ö�ȡ2�ֽڣ�������������ĵ�λ������룬���Խ����¸�ѭ��
				continue;
			}
			else
			{
				if (ishigh)
				{
					out = flag << 4;
					ishigh = false;
				}
				else
				{
					out += flag & 0x0F;
					ishigh = true;
					fwrite(&out, 1, 1, dst);
				}
			}
			//�����λ
			flag = buff & 0x0F;
			if (flag >= 0x08)
			{
				//flag�ڵ�λʱ����ȡÿ�λ�ȡѹ��������ҪԤ���ȡ2�ֽ�ȡǰ12λ��
				//Ȼ���ֻ�����flag�ڵ�λ�������������whileһֱ�ж�flag
				while (flag >= 0x08)
				{
					count = 0;
					fread(&count, 1, 1, src);
					--Index[i].size;
					count <<= 4;
					fread(&buff, 1, 1, src);
					--Index[i].size;
					count += (buff & 0xF0) >> 4;
					while (count--)
					{
						if (ishigh)
						{
							out = (flag - 8) << 4;
							ishigh = false;
						}
						else
						{
							out += (flag - 8) & 0x0F;
							ishigh = true;
							fwrite(&out, 1, 1, dst);
						}
					}
					flag = buff & 0x0F;
				}
				if (ishigh)
				{
					out = flag << 4;
					ishigh = false;
				}
				else
				{
					out += flag & 0x0F;
					ishigh = true;
					fwrite(&out, 1, 1, dst);
				}
			}
			else
			{
				if (ishigh)
				{
					out = flag << 4;
					ishigh = false;
				}
				else
				{
					out += flag;
					ishigh = true;
					fwrite(&out, 1, 1, dst);
				}
			}
		}
	}
	fclose(src);
	fclose(dst);
	WritePngFile(PaletteData);
	free(PaletteData);
}

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "chs");
	printf("project��Helheim-ʮ��֮��\n���ڽ�ѹ�ļ�ͷΪUF���ֿ��ļ���\n���ֿ��ļ��ϵ������ϡ�\nby Darkness-TX 2019.05.23\n\n");
	Decompress(argv[1]);
	printf("����ɣ�����ģ��%d\n", UF_Header.count);
	system("pause");
	return 0;
}