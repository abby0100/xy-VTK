#include "MyLog.h"
#include "CTReconstruction.h"

using namespace MyLog;
using namespace CTReconstruction;

int main()
{
	MyLog::Debug(LOG_TAG, __LINE__, "main");

	const char* testPath = "C:\\Users\\yuanxion\\Pictures\\20170905-VR-SDK\\Resources\\dcm-113\\";
	CTReconstruction::reconstruction(testPath);

	return 0;
}
