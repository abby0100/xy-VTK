#include "MyLog.h"
#include "ExportSphere.h"

using MyLog::Debug;
using Exporter::ExportSphere;

int main()
{
	Debug(LOG_TAG, __LINE__, "main");

	ExportSphere(LOG_TAG);

	return 0;
}