#pragma once

namespace MyLog {

	char*   MyTime(void);
	void Debug(const char* LOG_TAG, int line, char* msg);
	void Debug(const char* LOG_TAG, int line, char* msg, float param);
}