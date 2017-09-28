#include "MyLog.h"
#include "CTReconstruction.h"

using namespace MyLog;
using namespace CTReconstruction;

int main()
{
	MyLog::Debug(LOG_TAG, __LINE__, "main");

	CTReconstruction::reconstruction();

	return 0;
}
