#ifndef _MyLog_H_
#define _MyLog_H_

extern const char* LOG_TAG;

namespace MyLog {

	char* MyTime(void);
	void Debug(const char* LOG_TAG, int line, char* msg);
	void Debug(const char* LOG_TAG, int line, char* msg, float param);
}

#endif // !_MyLog_H_