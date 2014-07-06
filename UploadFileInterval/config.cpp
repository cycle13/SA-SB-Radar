
#include "config.h"
#include "Poco/Util/XMLConfiguration.h"

std::string getLastTimeStr(const std::string & dir)
{
	Poco::AutoPtr<Poco::Util::XMLConfiguration>
		pConf(new Poco::Util::XMLConfiguration("radar-config.xml"));

	return pConf->getString("lasttime." + dir, "0");
}

void setLastTimeStr(const std::string & dir, const std::string & timestr)
{
	Poco::AutoPtr<Poco::Util::XMLConfiguration>
		pConf(new Poco::Util::XMLConfiguration("radar-config.xml"));

	pConf->setString("lasttime." + dir, timestr);
	pConf->save("radar-config.xml");
}

std::vector<std::string> & getDirList(std::vector<std::string> & dirs)
{
	Poco::AutoPtr<Poco::Util::XMLConfiguration>
		pConf(new Poco::Util::XMLConfiguration("radar-config.xml"));

	int count = pConf->getInt("dircount");

	for (int i = 0; i<count; i++)
	{
		//dirs.push_back(pConf->getString("dirname[" + to_string(i) + "]"));
	}
	return dirs;
}

std::string getScanPath(const std::string & dir)
{
	Poco::AutoPtr<Poco::Util::XMLConfiguration>
		pConf(new Poco::Util::XMLConfiguration("radar-config.xml"));

	//"dirtest/*.txt";
	return pConf->getString("path." + dir, "dirtest/*.txt");
}

std::string getUploadUrl()
{
	Poco::AutoPtr<Poco::Util::XMLConfiguration>
		pConf(new Poco::Util::XMLConfiguration("radar-config.xml"));

	//"dirtest/*.txt";
	return pConf->getString("uploadurl", "http://radaronline.sinaapp.com/post.php");
}

std::string getProxy()
{
	Poco::AutoPtr<Poco::Util::XMLConfiguration>
		pConf(new Poco::Util::XMLConfiguration("radar-config.xml"));

	//"dirtest/*.txt";
	return pConf->getString("proxy", "0");
}

std::string getPngOutDir()
{
	Poco::AutoPtr<Poco::Util::XMLConfiguration>
		pConf(new Poco::Util::XMLConfiguration("radar-config.xml"));

	//"dirtest/*.txt";
	return pConf->getString("pngoutdir", "pngout");
	
}