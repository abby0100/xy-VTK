#include "vtkAutoInit.h" 
VTK_MODULE_INIT(vtkRenderingOpenGL2); // VTK was built with vtkRenderingOpenGL2
VTK_MODULE_INIT(vtkInteractionStyle);

#include "vtkConeSource.h"        
#include "vtkPolyDataMapper.h"            
#include "vtkRenderWindow.h"              
#include "vtkCamera.h"                    
#include "vtkActor.h"                     
#include "vtkRenderer.h"  
#include "vtkProperty.h"  

#include "MyLog.h"
#include <stdio.h>  
#include "windows.h"  

const char* LOG_TAG = " [vtkTest] ";

void testVTK2()
{
	//vtkRenderer *ren = vtkRenderer::New();
	//vtkRenderWindow *renWin = vtkRenderWindow::New();
	//renWin->AddRenderer(ren);

	//vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
	//iren->SetRenderWindow(renWin);

	//vtkSmartPointer line = vtkSmartPointer::New();
	//line->SetPoint1(10, 10, 10);
	//line->SetPoint2(10, 10, 50);

	//vtkSmartPointer line_mapper = vtkSmartPointer::New();
	//line_mapper->SetInput(line->GetOutput());

	//vtkSmartPointer line_actor = vtkSmartPointer::New();
	//line_actor->SetMapper(line_mapper);
	//line_actor->GetProperty()->SetColor(1.0, 0, 0);
	//ren->AddActor(line_actor);
	//ren->SetBackground(1, 1, 1);
	//renWin->SetSize(450, 450);

	//vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
	//iren->SetInteractorStyle(style);

	//iren->Initialize();
	//renWin->Render();

	//iren->Start();
}

void testVTK1()
{
	int k = 0;
	vtkConeSource *cone = vtkConeSource::New();
	cone->SetHeight(3.0);
	cone->SetRadius(1.0);
	cone->SetResolution(10);
	vtkPolyDataMapper *coneMapper = vtkPolyDataMapper::New();
	coneMapper->SetInputConnection(cone->GetOutputPort());
	vtkActor *coneActor = vtkActor::New();
	coneActor->SetMapper(coneMapper);
	vtkRenderer *ren1 = vtkRenderer::New();
	ren1->AddActor(coneActor);
	ren1->SetBackground(0.1, 0.2, 0.4);
	vtkRenderWindow *renWin = vtkRenderWindow::New();
	renWin->AddRenderer(ren1);
	renWin->SetSize(300, 300);
	int i = 0;
	for (; i<360; ++i)
	{
		renWin->Render();
		ren1->GetActiveCamera()->Azimuth(1);
		Sleep(10);
	}
	scanf("%d", &k);
	cone->Delete();
	//coneMapper->Delete();
	coneActor->Delete();
	ren1->Delete();
	renWin->Delete();
}

void testVTK3()
{
	MyLog::Debug(LOG_TAG, __LINE__, "testVTK3");

	vtkConeSource *cone = vtkConeSource::New();
	cone->SetHeight(3.0);
	cone->SetRadius(1.0);
	cone->SetResolution(10);

	vtkPolyDataMapper *coneMapper = vtkPolyDataMapper::New();
	coneMapper->SetInputData(cone->GetOutput());

	vtkActor *coneActor = vtkActor::New();
	coneActor->SetMapper(coneMapper);

	vtkRenderer *ren1 = vtkRenderer::New();
	ren1->AddActor(coneActor);
	ren1->SetBackground(0.1, 0.1, 0.1);

	vtkRenderWindow *renWin = vtkRenderWindow::New();
	renWin->AddRenderer(ren1);
	renWin->SetSize(800, 600);


	for (int i = 0; i < 360; ++i)
	{
		renWin->Render();
		ren1->GetActiveCamera()->Azimuth(1);
	}

	MyLog::Debug(LOG_TAG, __LINE__, "testVTK3 Delete");

	cone->Delete();
	coneMapper->Delete();
	coneActor->Delete();
	ren1->Delete();
	renWin->Delete();
}


int main()
{
	MyLog::Debug(LOG_TAG, __LINE__, "main");
	testVTK3();
	return 0;
}