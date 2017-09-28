#include "vtkAutoInit.h" 
VTK_MODULE_INIT(vtkRenderingOpenGL2); // VTK was built with vtkRenderingOpenGL2
VTK_MODULE_INIT(vtkInteractionStyle); 

#include "MyLog.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include "vtkDICOMImageReader.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkOutlineFilter.h"
#include "vtkCamera.h"
#include "vtkProperty.h"
#include "vtkPolyDataNormals.h"
#include "vtkContourFilter.h"
#include "vtkVolume16Reader.h"
#include "vtkImageCast.h"
#include "vtkImageReader.h"
#include "vtkBMPReader.h"
#include "vtkMarchingCubes.h"
#include "vtkStripper.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkDecimatePro.h"
#include "vtkImageShrink3D.h"

#pragma comment (lib, "vtkCommonCore-7.1.lib")
#pragma comment (lib, "vtkCommonExecutionModel-7.1.lib")
#pragma comment (lib, "vtkFiltersSources-7.1.lib")
#pragma comment (lib, "vtkRenderingCore-7.1.lib")
#pragma comment (lib, "vtkRenderingOpenGL2-7.1.lib")
#pragma comment (lib, "vtkInteractionStyle-7.1.lib")
#pragma comment (lib, "vtkIOImage-7.1.lib")
#pragma comment (lib, "vtkFiltersCore-7.1.lib")
#pragma comment (lib, "vtkImagingCore-7.1.lib")

/**
* From blog: 用VTK实现CT图片的三维重建过程
* http://blog.sina.com.cn/s/blog_7e40ef210100to11.html
*/

namespace CTReconstruction
{
	void init()
	{
		
	}

	void reconstruction(const char* dicom_path)
	{
		MyLog::Debug(LOG_TAG, __LINE__, "reconstruction dicom_path:", dicom_path);

		// Create the renderer, the render window, and the interactor. The renderer
		vtkRenderer *renderer = vtkRenderer::New();
		vtkRenderWindow *renWin = vtkRenderWindow::New();
		renWin->AddRenderer(renderer);
		vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
		iren->SetRenderWindow(renWin);

		// start
		MyLog::Debug(LOG_TAG, __LINE__, "reconstruction start");

		vtkDICOMImageReader *dicomReader = vtkDICOMImageReader::New();
		dicomReader->SetDataByteOrderToLittleEndian();
		dicomReader->SetDirectoryName(dicom_path);
		dicomReader->Update();

		vtkImageShrink3D *shrink = vtkImageShrink3D::New();
		shrink->SetShrinkFactors(4, 4, 1);
		shrink->AveragingOn();
		//shrink->SetInputConnection((vtkDataObject *)dicomReader->GetOutput());

		vtkMarchingCubes *skinExtractor = vtkMarchingCubes::New();
		skinExtractor->SetValue(0, 300);
		skinExtractor->SetInputConnection(shrink->GetOutputPort());

		vtkDecimatePro *deci = vtkDecimatePro::New();
		deci->SetTargetReduction(0.3);
		deci->SetInputConnection(skinExtractor->GetOutputPort());

		vtkSmoothPolyDataFilter *smooth = vtkSmoothPolyDataFilter::New();
		smooth->SetInputConnection(deci->GetOutputPort());
		smooth->SetNumberOfIterations(200);

		vtkPolyDataNormals *skinNormals = vtkPolyDataNormals::New();
		skinNormals->SetInputConnection(smooth->GetOutputPort());
		skinNormals->SetFeatureAngle(60.0);

		vtkStripper *stripper = vtkStripper::New();
		stripper->SetInputConnection(skinNormals->GetOutputPort());

		vtkPolyDataMapper *skinMapper = vtkPolyDataMapper::New();
		skinMapper->SetInputConnection(stripper->GetOutputPort());
		skinMapper->ScalarVisibilityOff();

		// camera
		MyLog::Debug(LOG_TAG, __LINE__, "reconstruction camera");

		vtkCamera *camera = vtkCamera::New();
		camera->SetViewUp(0, 0, -1);
		camera->SetPosition(0, 1, 0);
		camera->SetFocalPoint(0, 0, 0);
		camera->ComputeViewPlaneNormal();

		vtkActor *coneActor = vtkActor::New();
		coneActor->SetMapper(skinMapper);
		coneActor->GetProperty()->SetDiffuse(1);
		coneActor->GetProperty()->SetSpecular(1);

		// show
		MyLog::Debug(LOG_TAG, __LINE__, "reconstruction show");

		renderer->AddActor(coneActor);
		//vtkProp *textActor;
		//vtkProp *textActor = vtkProp::New();
		//renderer->AddActor2D(textActor);

		renderer->SetBackground(0, 0, 0);
		renderer->SetActiveCamera(camera);
		renderer->ResetCamera();

		// destory
		MyLog::Debug(LOG_TAG, __LINE__, "reconstruction destory");

		iren->Delete();
		renWin->Delete();
		renderer->Delete();
	}

}