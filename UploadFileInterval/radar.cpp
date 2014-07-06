#include "radar.h"

#include "bzip2/bzlib.h"
#include "radar/SA-SBDataStruct.h"

#define cimg_use_png
#include "CImg/CImg.h"

using namespace cimg_library;


#include "Poco/File.h"
#include "Poco/Path.h"

#include <iostream>
#include <fstream>
#include <string>


int read_bz2_file(const std::string& filename, SA_SB_Info& radar)
{
	//���������ַ����ֱ������洢ѹ���ַ��� �� ����ַ���
	std::string instr(10 * 1024 * 1024, ' ');

	//��ѹ���ļ�����instr "Z_RADR_I_Z9398_20140704080600_O_DOR_SB_CAP.bin.bz2"
	std::ifstream fin(filename, std::ios_base::in | std::ios_base::binary);
	fin.read(&*instr.begin(), instr.size());

	instr.resize(fin.gcount());
	//std::cout <<"ѹ�����ַ���:"<<instr;

	fin.close();

	//bz_stream ���ݽṹ�����洢ѹ�����ͽ�ѹ�������Ϣ
	bz_stream bsread;
	bsread.bzalloc = NULL;
	bsread.bzfree = NULL;
	bsread.opaque = NULL;

	//��ʼ��bsread ����������������������Ϊ0 ����ѹ��״̬ 
	//�ڶ����������ý�ѹ״̬��� ��Ϊ0������ڲ�״̬
	BZ2_bzDecompressInit(&bsread, 4, 0);

	int ret, last_total_out = 0;

	//�����������λ�ú��ֽ���
	bsread.avail_in = instr.size();
	bsread.next_in = &*instr.begin();


	SA_SB_Info::SB_Base basestruct;

	do{//ѭ����ѹ ÿ�ν�ѹ���70�ֽڴ���outstr�����

		bsread.avail_out = sizeof(basestruct);//outstr.size()
		bsread.next_out = (char*)(&basestruct);

		ret = BZ2_bzDecompress(&bsread);

		radar.alldata.push_back(basestruct);


	} while (ret == BZ_OK);

	BZ2_bzDecompressEnd(&bsread);

	radar.init_elevs();

	std::cout << "sizeof(RadarBaseData):" << sizeof(SA_SB_Info::SB_Base) << std::endl;
	std::cout << "date_begin:" << radar.date_begin << " date_end:" << radar.date_end << std::endl;
	std::cout << "seconds_begin:" << radar.seconds_begin << " seconds_end:" << radar.seconds_begin << std::endl;

	std::ofstream fout("log2.txt");

	radar.out_info(fout);
	fout.close();

	return 0;

}

void read_base_data()
{
	std::string filename = "Z_RADR_I_Z9398_20140704080600_O_DOR_SB_CAP.bin";

	SA_SB_Info radar;

	radar.read_base_data(filename);

	std::cout << "sizeof(RadarBaseData):" << sizeof(SA_SB_Info::SB_Base) << std::endl;
	std::cout << "date_begin:" << radar.date_begin << " date_end:" << radar.date_end << std::endl;
	std::cout << "seconds_begin:" << radar.seconds_begin << " seconds_end:" << radar.seconds_begin << std::endl;

	std::ofstream fout("log1.txt");

	radar.out_info(fout);
	fout.close();
}

//��һ�����ǲ㱣��Ϊ�������ݵ�ͼƬ
bool save_png_data(const RadarElevation &el, const std::string &filename)
{
	CImg<unsigned char> img(360, el.r_gate_count, 1, 1, 0);

	for (std::size_t i = 0; i<el.r.size(); i++)//��λ��
	{
		for (std::size_t j = 0; j<el.r[i].size(); j++)//��j����
		{
			int ref = int(el.r[i][j] + 0.5);
			unsigned char val;

			if (ref < 0) val = 254; //����Ч�۲�����
			else if (ref > 255) val = 255; //��ʾ�о���ģ��
			else val = ref;

			img.atXY(i, j, 0, 0) = val;
		}
	}

	img.save(filename.c_str());

	return true;
}

bool generate_pngdata(Poco::File& cur, UploadInfo& upinfo)
{
	SA_SB_Info radar;
	std::string pathstr = cur.path();

	Poco::Path curpath( pathstr );
	if (curpath.getExtension() == "bz2")
	{
		read_bz2_file(pathstr, radar);
	}
	else
	{
		radar.read_base_data( pathstr );
	}

	upinfo.date_begin = radar.date_begin;
	upinfo.date_end = radar.date_end;
	upinfo.seconds_begin = radar.seconds_begin;
	upinfo.seconds_end = radar.seconds_end;

	for (SA_SB_Info::r_it it = radar.elevations.begin();
		it != radar.elevations.end(); it++)
	{
		if (it->second.r_valid)
		{
			std::string outname = getPngOutDir()+'/' + it->first + "data.png";
			
			save_png_data(it->second, outname);

			upinfo.elevs.push_back(it->first);
			upinfo.files.push_back(outname);

		}
	}

	return true;
}