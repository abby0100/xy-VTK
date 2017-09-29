#include "vtkAutoInit.h" 
VTK_MODULE_INIT(vtkRenderingOpenGL2); // VTK was built with vtkRenderingOpenGL2
VTK_MODULE_INIT(vtkInteractionStyle); 
VTK_MODULE_INIT(vtkRenderingFreeType);

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

#include "vtkImageviewer2.h"
#include "vtkTextProperty.h"
#include "vtkTextMapper.h"
#include "vtkActor2D.h"
#include "vtkInteractorStyleImage.h"


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

/**
* From blog: 用VTK实现CT图片的三维重建过程
* http://blog.sina.com.cn/s/blog_7e40ef210100to11.html
*	http://blog.sina.com.cn/s/blog_bda802080101k5pb.html
*	http://blog.csdn.net/chinamming/article/details/16829351
*	
* Error blog: VTK: VTK Error 问题集
* http://blog.csdn.net/fanhenghui/article/details/52785745
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
		//dicomReader->SetFilePrefix(dicom_path);
		//dicomReader->SetFilePattern("*.dcm");
		dicomReader->Update();


		vtkImageShrink3D *shrink = vtkImageShrink3D::New();
		shrink->SetShrinkFactors(4, 4, 1);
		shrink->AveragingOn();
		shrink->SetInputConnection(dicomReader->GetOutputPort());

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

	void reconstruction2(const char* dicom_path)
	{
		MyLog::Debug(LOG_TAG, __LINE__, "reconstruction2 dicom_path:", dicom_path);

		// Read all the DICOM files in the specified directory.  
		vtkSmartPointer<vtkDICOMImageReader> reader =
			vtkSmartPointer<vtkDICOMImageReader>::New();
		reader->SetDirectoryName(dicom_path);
		reader->Update();

		// Visualize  
		vtkSmartPointer<vtkImageViewer2> imageViewer =
			vtkSmartPointer<vtkImageViewer2>::New();
		imageViewer->SetInputConnection(reader->GetOutputPort());

		// slice status message  
		vtkSmartPointer<vtkTextProperty> sliceTextProp = vtkSmartPointer<vtkTextProperty>::New();
		sliceTextProp->SetFontFamilyToCourier();
		sliceTextProp->SetFontSize(20);
		sliceTextProp->SetVerticalJustificationToBottom();
		sliceTextProp->SetJustificationToLeft();

		vtkSmartPointer<vtkTextMapper> sliceTextMapper = vtkSmartPointer<vtkTextMapper>::New();
		//std::string msg = StatusMessage::Format(imageViewer->GetSliceMin(), imageViewer->GetSliceMax());
		//sliceTextMapper->SetInput(msg.c_str());
		sliceTextMapper->SetTextProperty(sliceTextProp);

		vtkSmartPointer<vtkActor2D> sliceTextActor = vtkSmartPointer<vtkActor2D>::New();
		sliceTextActor->SetMapper(sliceTextMapper);
		sliceTextActor->SetPosition(15, 10);

		// usage hint message  
		vtkSmartPointer<vtkTextProperty> usageTextProp = vtkSmartPointer<vtkTextProperty>::New();
		usageTextProp->SetFontFamilyToCourier();
		usageTextProp->SetFontSize(14);
		usageTextProp->SetVerticalJustificationToTop();
		usageTextProp->SetJustificationToLeft();

		vtkSmartPointer<vtkTextMapper> usageTextMapper = vtkSmartPointer<vtkTextMapper>::New();
		usageTextMapper->SetInput("- Slice with mouse wheel\n  or Up/Down-Key\n- Zoom with pressed right\n  mouse button while dragging");
		usageTextMapper->SetTextProperty(usageTextProp);

		vtkSmartPointer<vtkActor2D> usageTextActor = vtkSmartPointer<vtkActor2D>::New();
		usageTextActor->SetMapper(usageTextMapper);
		usageTextActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
		usageTextActor->GetPositionCoordinate()->SetValue(0.05, 0.95);

		vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
			vtkSmartPointer<vtkRenderWindowInteractor>::New();

		vtkSmartPointer<vtkInteractorStyleImage> myInteractorStyle =
			vtkSmartPointer<vtkInteractorStyleImage>::New();

		//myInteractorStyle->SetImageViewer(imageViewer);
		//myInteractorStyle->SetStatusMapper(sliceTextMapper);


		imageViewer->SetupInteractor(renderWindowInteractor);
		renderWindowInteractor->SetInteractorStyle(myInteractorStyle);

		imageViewer->GetRenderer()->AddActor2D(sliceTextActor);
		imageViewer->GetRenderer()->AddActor2D(usageTextActor);

		imageViewer->Render();
		imageViewer->GetRenderer()->ResetCamera();
		imageViewer->Render();
		renderWindowInteractor->Start();
	}

	void reconstruction3(const char* dicom_path)
	{
		MyLog::Debug(LOG_TAG, __LINE__, "reconstruction3 dicom_path:", dicom_path);

		vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
		reader->SetDirectoryName(dicom_path);

		// Visualize  
		vtkSmartPointer<vtkImageViewer2> imageViewer = vtkSmartPointer<vtkImageViewer2>::New();
		imageViewer->SetInputConnection(reader->GetOutputPort());
		imageViewer->Render();

		while (true) {}
	}

	void reconstruction4(const char* dicom_path)
	{
		MyLog::Debug(LOG_TAG, __LINE__, "reconstruction4 dicom_path:", dicom_path);

		vtkRenderer *aRenderer = vtkRenderer::New();
		vtkRenderWindow *renWin = vtkRenderWindow::New();
		renWin->AddRenderer(aRenderer);
		vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
		iren->SetRenderWindow(renWin);


		vtkDICOMImageReader *v16 = vtkDICOMImageReader::New();
		v16->SetDataByteOrderToLittleEndian();
		v16->SetDirectoryName(dicom_path);
		v16->SetDataSpacing(3.2, 3.2, 1.5);
		
		vtkContourFilter *skinExtractor = vtkContourFilter::New();
		skinExtractor->SetInputConnection(v16->GetOutputPort());

		// air?
		//skinExtractor->SetValue(0, -1000);
		// lung
		//skinExtractor->SetValue(0, -500);
		// skin
		skinExtractor->SetValue(0, -100);
		// fat?
		//skinExtractor->SetValue(0, -50);
		// water?
		//skinExtractor->SetValue(0, 0);
		// soft tissue?
		//skinExtractor->SetValue(0, 100);
		// contrast?
		//skinExtractor->SetValue(0, 300);
		// skeleton
		//skinExtractor->SetValue(0, 500);
		//skinExtractor->SetValue(0, 700);
		
		vtkPolyDataNormals *skinNormals = vtkPolyDataNormals::New();
		skinNormals->SetInputConnection(skinExtractor->GetOutputPort());
		skinNormals->SetFeatureAngle(60.0);
		vtkPolyDataMapper *skinMapper = vtkPolyDataMapper::New();
		skinMapper->SetInputConnection(skinNormals->GetOutputPort());

		// draw color
		skinMapper->ScalarVisibilityOn();
		//skinMapper->ScalarVisibilityOff();
		vtkActor *skin = vtkActor::New();
		skin->SetMapper(skinMapper);


		vtkOutlineFilter *outlineData = vtkOutlineFilter::New();
		outlineData->SetInputConnection(v16->GetOutputPort());
		vtkPolyDataMapper *mapOutline = vtkPolyDataMapper::New();
		mapOutline->SetInputConnection(outlineData->GetOutputPort());
		vtkActor *outline = vtkActor::New();
		outline->SetMapper(mapOutline);
		//outline->GetProperty()->SetColor(0, 0, 0);
		outline->GetProperty()->SetColor(1, 1, 1);


		vtkCamera *aCamera = vtkCamera::New();
		aCamera->SetViewUp(0, 0, -1);
		aCamera->SetPosition(0, 1, 0);
		aCamera->SetFocalPoint(0, 0, 0);
		aCamera->ComputeViewPlaneNormal();


		aRenderer->AddActor(outline);
		aRenderer->AddActor(skin);
		aRenderer->SetActiveCamera(aCamera);
		aRenderer->ResetCamera();
		aCamera->Dolly(1.5);


		aRenderer->SetBackground(0.5, 0.5, 0.5);
		renWin->SetSize(640, 480);


		aRenderer->ResetCameraClippingRange();

		// Initialize the event loop and then start it.     
		iren->Initialize();
		iren->Start();

		v16->Delete();
		skinExtractor->Delete();
		skinNormals->Delete();
		skinMapper->Delete();
		skin->Delete();
		outlineData->Delete();
		mapOutline->Delete();
		outline->Delete();
		aCamera->Delete();
		iren->Delete();
		renWin->Delete();
		aRenderer->Delete();
	}


	void reconstruction5(const char* dicom_path)
	{
		MyLog::Debug(LOG_TAG, __LINE__, "reconstruction5 dicom_path:", dicom_path);

		// Create the renderer, the render window, and the interactor. The renderer
		vtkRenderer *aRenderer = vtkRenderer::New();
		vtkRenderWindow *renWin = vtkRenderWindow::New();
		renWin->AddRenderer(aRenderer);
		vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
		iren->SetRenderWindow(renWin);

		// start
		MyLog::Debug(LOG_TAG, __LINE__, "reconstruction5 start");

		vtkDICOMImageReader *dicomReader = vtkDICOMImageReader::New();
		dicomReader->SetDataByteOrderToLittleEndian();
		dicomReader->SetDirectoryName(dicom_path);
		dicomReader->SetDataSpacing(3.2, 3.2, 1.5);

		vtkImageShrink3D *shrink = vtkImageShrink3D::New();
		shrink->SetShrinkFactors(4, 4, 1);
		shrink->AveragingOn();
		shrink->SetInputConnection(dicomReader->GetOutputPort());

		vtkMarchingCubes *skinExtractor = vtkMarchingCubes::New();
		//skinExtractor->SetValue(0, 300);
		skinExtractor->SetValue(0, -100);
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
		// draw color
		skinMapper->ScalarVisibilityOn();
		//skinMapper->ScalarVisibilityOff();
		vtkActor *skin = vtkActor::New();
		skin->SetMapper(skinMapper);


		vtkOutlineFilter *outlineData = vtkOutlineFilter::New();
		outlineData->SetInputConnection(dicomReader->GetOutputPort());
		vtkPolyDataMapper *mapOutline = vtkPolyDataMapper::New();
		mapOutline->SetInputConnection(outlineData->GetOutputPort());
		
		vtkActor *outline = vtkActor::New();
		outline->SetMapper(mapOutline);
		//outline->GetProperty()->SetColor(0, 0, 0);
		outline->GetProperty()->SetColor(1, 1, 1);


		vtkCamera *aCamera = vtkCamera::New();
		aCamera->SetViewUp(0, 0, -1);
		aCamera->SetPosition(0, 1, 0);
		aCamera->SetFocalPoint(0, 0, 0);
		aCamera->ComputeViewPlaneNormal();


		aRenderer->AddActor(outline);
		aRenderer->AddActor(skin);
		aRenderer->SetActiveCamera(aCamera);
		aRenderer->ResetCamera();
		aCamera->Dolly(1.5);


		aRenderer->SetBackground(0.5, 0.5, 0.5);
		renWin->SetSize(640, 480);
		aRenderer->ResetCameraClippingRange();

		// Initialize the event loop and then start it.     
		iren->Initialize();
		iren->Start();

		dicomReader->Delete();
		skinExtractor->Delete();
		skinNormals->Delete();
		skinMapper->Delete();
		skin->Delete();
		outlineData->Delete();
		mapOutline->Delete();
		outline->Delete();
		aCamera->Delete();
		iren->Delete();
		renWin->Delete();
		aRenderer->Delete();
	}
}