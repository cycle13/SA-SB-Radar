#include <iostream>
#include <fstream>
#include <string>

#include "bzlib.h"

int use_high_level_interface()
{
	FILE*   f;
	BZFILE* b;
	int     nBuf = 0;

	char buf[1000];

	int     bzerror;
	int     nWritten;


	if (fopen_s(&f, "sample1.bz2", "r")) {
		/* handle error */
		std::cout << " fopen error!";
		return 1;
	}

	b = BZ2_bzReadOpen(&bzerror, f, 4, 0, NULL, 0);
	if (bzerror != BZ_OK) {
		BZ2_bzReadClose(&bzerror, b);
		/* handle error */
		std::cout << " BZ2_bzReadOpen error!";
		return 2;
	}

	bzerror = BZ_OK;
	while (bzerror == BZ_OK) {
		nBuf = BZ2_bzRead(&bzerror, b, buf, 100);
		std::cout << nBuf;
		if (bzerror == BZ_OK) {
			/* do something with buf[0 .. nBuf-1] */
			std::cout << "sizeof(buf):" << sizeof(buf) << " nbuf:" << nBuf;
		}
		else
		{
			std::cout << "bzerror" << bzerror;
			std::cout << " BZ2_bzRead error!";
			return 3;
		}
	}
	if (bzerror != BZ_STREAM_END) {
		BZ2_bzReadClose(&bzerror, b);
		/* handle error */
		std::cout << " BZ2_bzReadEND error!";
		return 4;
	}
	else {
		BZ2_bzReadClose(&bzerror, b);
	}

	return 0;
}

int use_low_level_interface()
{
	//���������ַ����ֱ������洢ѹ���ַ��� �� ����ַ���
	std::string instr(200,' '), outstr;
	//instr = std::string(100, 'a') + std::string(100, 'b');
	
	//��ѹ���ļ�����instr ѹ��ǰԭ�ļ�����100��a��100��b
	std::ifstream fin("100ab.txt.bz2", std::ios_base::in | std::ios_base::binary);
	fin.read(&*instr.begin(), instr.size());
	
	instr.resize(fin.gcount());
	std::cout <<"ѹ�����ַ���:"<<instr;

	fin.close();

	outstr.resize(1000);

	//bz_stream ���ݽṹ�����洢ѹ�����ͽ�ѹ�������Ϣ
	bz_stream bsread;
	bsread.bzalloc = NULL;
	bsread.bzfree = NULL;
	bsread.opaque = NULL;

	//��ʼ��bsread ����������������������Ϊ0 ����ѹ��״̬ 
	//�ڶ����������ý�ѹ״̬��� ��Ϊ0������ڲ�״̬
	BZ2_bzDecompressInit(&bsread, 4, 0);
	
	int ret, last_total_out=0;

	//�����������λ�ú��ֽ���
	bsread.avail_in = instr.size();
	bsread.next_in = &*instr.begin();

	bsread.avail_out = 70;//outstr.size()
	bsread.next_out = &*outstr.begin();	
	
	
	do{//ѭ����ѹ ÿ�ν�ѹ���70�ֽڴ���outstr�����

	ret = BZ2_bzDecompress(&bsread);
	
	std::cout << "\n bsread.total_out_hi32:" << bsread.total_out_hi32
		<< "\n bsread.total_out_lo32:" << bsread.total_out_lo32;
	std::cout << " ret:" << ret;

	std::cout << '\n' << outstr.substr(0, bsread.total_out_lo32 - last_total_out);

	last_total_out = bsread.total_out_lo32;

	//BZ2_bzDecompress����ÿ��ִ�к��Զ�����avail_in next_in avail_out next_out
	//�����һ��ִ�е���λ��ʱ avail_in==10 avail_out=0 
	bsread.avail_out = 70;
	bsread.next_out = &(*outstr.begin() );

	} while (ret == BZ_OK);

	BZ2_bzDecompressEnd(&bsread);
	
	return 0;
	
}

int main()
{
	use_low_level_interface();

	return 0;
}