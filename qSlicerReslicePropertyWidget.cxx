// Qt includes
#include <QButtonGroup>

#include "qSlicerReslicePropertyWidget.h"
#include "ui_qSlicerReslicePropertyWidget.h"

// MRMLWidgets includes
#include <qMRMLSliceWidget.h>
#include <qMRMLSliceControllerWidget.h>
#include <qMRMLThreeDWidget.h>

#include "qMRMLViewControllerBar_p.h"
#include <qMRMLViewControllerBar.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QToolButton>

// MRML includes
#include "vtkSmartPointer.h"
#include "vtkMRMLSliceNode.h"
#include "vtkMRMLViewNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLScalarVolumeNode.h"

// MRMLLogic includes
#include "vtkMRMLLayoutLogic.h"

#include "vtkSlicerVolumeResliceDriverLogic.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkMatrix4x4.h>
#include <vtkImageData.h>

#include "ctkPopupWidget.h"

#include "qMRMLColors.h"


class qSlicerReslicePropertyWidgetPrivate;
class vtkMRMLNode;
class vtkObject;


//------------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_OpenIGTLinkIF
class qSlicerReslicePropertyWidgetPrivate
  : public qMRMLViewControllerBarPrivate, public Ui_qSlicerReslicePropertyWidget
{
  Q_DECLARE_PUBLIC(qSlicerReslicePropertyWidget);
  
public:
  typedef qMRMLViewControllerBarPrivate Superclass;

  qSlicerReslicePropertyWidgetPrivate(qSlicerReslicePropertyWidget& object);
  virtual ~qSlicerReslicePropertyWidgetPrivate() ;

  virtual void init();
  
  QButtonGroup methodButtonGroup;
  QButtonGroup orientationButtonGroup;

  vtkMRMLScene * scene;
  vtkMRMLSliceNode * sliceNode;
  vtkMRMLNode  * driverNode;

protected:
  virtual void setupPopupUi();
};



qSlicerReslicePropertyWidgetPrivate
::qSlicerReslicePropertyWidgetPrivate( qSlicerReslicePropertyWidget& object )
  : Superclass(object)
{
  this->scene = NULL;
  this->sliceNode = NULL;
  this->driverNode = NULL;
}



qSlicerReslicePropertyWidgetPrivate
::~qSlicerReslicePropertyWidgetPrivate()
{
}



void qSlicerReslicePropertyWidgetPrivate
::setupPopupUi()
{
  Q_Q(qSlicerReslicePropertyWidget);

  this->Superclass::setupPopupUi();
  this->PopupWidget->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
  this->Ui_qSlicerReslicePropertyWidget::setupUi(this->PopupWidget);

  this->methodButtonGroup.addButton(this->positionRadioButton, vtkSlicerVolumeResliceDriverLogic::METHOD_POSITION);
  this->methodButtonGroup.addButton(this->orientationRadioButton, vtkSlicerVolumeResliceDriverLogic::METHOD_ORIENTATION);
  this->positionRadioButton->setChecked(true);
  this->orientationButtonGroup.addButton(this->inPlaneRadioButton, vtkSlicerVolumeResliceDriverLogic::ORIENTATION_INPLANE);
  this->orientationButtonGroup.addButton(this->inPlane90RadioButton, vtkSlicerVolumeResliceDriverLogic::ORIENTATION_INPLANE90);
  this->orientationButtonGroup.addButton(this->transverseRadioButton, vtkSlicerVolumeResliceDriverLogic::ORIENTATION_TRANSVERSE);
  this->inPlaneRadioButton->setChecked(true);
  
  QObject::connect(this->driverNodeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)), q, SLOT(setDriverNode(vtkMRMLNode*)));

  QObject::connect(this->positionRadioButton, SIGNAL(clicked()), q, SLOT(onMethodChanged()));
  QObject::connect(this->orientationRadioButton, SIGNAL(clicked()), q, SLOT(onMethodChanged()));
  QObject::connect(this->inPlaneRadioButton, SIGNAL(clicked()), q, SLOT(onOrientationChanged()));
  QObject::connect(this->inPlane90RadioButton, SIGNAL(clicked()), q, SLOT(onOrientationChanged()));
  QObject::connect(this->transverseRadioButton, SIGNAL(clicked()), q, SLOT(onOrientationChanged()));
}



void qSlicerReslicePropertyWidgetPrivate
::init()
{
  this->Superclass::init();
  this->ViewLabel->setText(qSlicerReslicePropertyWidget::tr("1"));
  this->BarLayout->addStretch(1);
  this->setColor(qMRMLColors::threeDViewBlue());
}



qSlicerReslicePropertyWidget
::qSlicerReslicePropertyWidget( vtkSlicerVolumeResliceDriverLogic* logic, QWidget *_parent )
  : Superclass( new qSlicerReslicePropertyWidgetPrivate( *this ), _parent )
{
  Q_D(qSlicerReslicePropertyWidget);
  d->init();
  this->Logic = logic;
}



qSlicerReslicePropertyWidget
::~qSlicerReslicePropertyWidget()
{
  if ( this->Logic != NULL )
  {
    this->Logic = NULL;
  }
}



void qSlicerReslicePropertyWidget
::setSliceViewName( const QString& newSliceViewName )
{
  Q_D(qSlicerReslicePropertyWidget);
  
  d->ViewLabel->setText(newSliceViewName);
}



void qSlicerReslicePropertyWidget
::setSliceViewColor( const QColor& newSliceViewColor )
{
  Q_D(qSlicerReslicePropertyWidget);
  
  d->setColor( newSliceViewColor );
}



void qSlicerReslicePropertyWidget
::setMRMLSliceNode( vtkMRMLSliceNode* newSliceNode )
{
  Q_D(qSlicerReslicePropertyWidget);

  d->sliceNode = newSliceNode;
  if (newSliceNode && newSliceNode->GetScene())
    {
    this->setMRMLScene(newSliceNode->GetScene());
    }
}



void qSlicerReslicePropertyWidget
::setMRMLScene( vtkMRMLScene* newScene )
{
  Q_D(qSlicerReslicePropertyWidget);

  if (d->scene != newScene)
    {
    d->scene = newScene;
    if (d->driverNodeSelector)
      {
      d->driverNodeSelector->setMRMLScene(newScene);
      }
    }
}



void qSlicerReslicePropertyWidget
::setDriverNode( vtkMRMLNode* newNode )
{
  Q_D(qSlicerReslicePropertyWidget);
  
  if ( d->sliceNode == NULL )
  {
    return;
  }
  
  if ( newNode == NULL )
  {
    this->Logic->SetDriverForSlice( "", d->sliceNode );
  }
  else
  {
    this->Logic->SetDriverForSlice( newNode->GetID(), d->sliceNode );
  }
}



void qSlicerReslicePropertyWidget
::onMethodChanged()
{
  Q_D(qSlicerReslicePropertyWidget);
  
  this->Logic->SetMethodForSlice( d->methodButtonGroup.checkedId(), d->sliceNode );
}



void qSlicerReslicePropertyWidget
::onOrientationChanged()
{
  Q_D(qSlicerReslicePropertyWidget);
  
  this->Logic->SetOrientationForSlice( d->orientationButtonGroup.checkedId(), d->sliceNode );
}
