#include <iostream>
#include <sstream>  
#include <fstream> 
#include <sys/timeb.h> 
#include <time.h>

const char* logPath = "f:\\logcat/vtktest.txt";

namespace MyLog {

	char*   MyTime(void)
	{
		struct  tm      *ptm;
		struct  timeb   stTimeb;
		static  char    szTime[19];

		ftime(&stTimeb);
		ptm = localtime(&stTimeb.time);
		sprintf(szTime, "%02d-%02d %02d:%02d:%02d.%03d",
			ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, stTimeb.millitm);
		szTime[18] = 0;
		return szTime;
	}

	void Debug(const char* LOG_TAG, int line, char* msg)
	{
		std::ofstream fout(logPath, std::ios::app);

		fout << MyTime() << LOG_TAG << line << "\t" << msg << std::endl;
		fout << std::flush;
		fout.close();
	}

	void Debug(const char* LOG_TAG, int line, char* msg, float param)
	{
		std::ofstream fout(logPath, std::ios::app);

		fout << MyTime() << LOG_TAG << line << "\t" << msg << " " << param << std::endl;
		fout << std::flush;
		fout.close();
	}
}