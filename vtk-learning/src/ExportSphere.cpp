#include "vtkAutoInit.h" 
VTK_MODULE_INIT(vtkRenderingOpenGL2); // VTK was built with vtkRenderingOpenGL2
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType); 

#include "ExportSphere.h"
#include "MyLog.h"

#include "vtkActor.h"
#include "vtkOBJExporter.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"
#include "vtkSphereSource.h"

#pragma comment (lib, "vtkCommonCore-7.1.lib")
#pragma comment (lib, "vtkCommonExecutionModel-7.1.lib")
#pragma comment (lib, "vtkFiltersSources-7.1.lib")
#pragma comment (lib, "vtkRenderingCore-7.1.lib")
#pragma comment (lib, "vtkRenderingOpenGL2-7.1.lib")
#pragma comment (lib, "vtkInteractionStyle-7.1.lib")
#pragma comment (lib, "vtkIOImage-7.1.lib")
#pragma comment (lib, "vtkFiltersCore-7.1.lib")
#pragma comment (lib, "vtkImagingCore-7.1.lib")

#pragma comment (lib, "vtkInteractionImage-7.1.lib")
#pragma comment (lib, "vtkRenderingFreeType-7.1.lib")
#pragma comment (lib, "./vtkFiltersModeling-7.1.lib")
#pragma comment (lib, "./vtkIOExport-7.1.lib")

using MyLog::Debug;

void Exporter::ExportSphere(const char* fileSavePath)
{
	Debug(LOG_TAG, __LINE__, "ExportSphere");

	// Create a sphere  
	vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetCenter(0.0, 0.0, 0.0);
	sphereSource->SetRadius(5.0);
	sphereSource->SetThetaResolution(10);
	sphereSource->SetPhiResolution(10);

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(sphereSource->GetOutputPort());

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor(actor);
	renderer->SetBackground(0.5, 0.5, 0.5);

	vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
	renWin->AddRenderer(renderer);

	vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren->SetRenderWindow(renWin);

	vtkSmartPointer<vtkOBJExporter> exporter = vtkSmartPointer<vtkOBJExporter>::New();
	exporter->SetFilePrefix(fileSavePath);
	exporter->SetInput(renWin);
	exporter->Write();

	renWin->Render();
	iren->Start();
	

}
