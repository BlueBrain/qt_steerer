/*----------------------------------------------------------------------------
    ConfigForm class header file for QT steerer GUI. 

    (C)Copyright 2002 The University of Manchester, United Kingdom,
    all rights reserved.

    This software is produced by the Supercomputing, Visualization &
    e-Science Group, Manchester Computing, the Victoria University of
    Manchester as part of the RealityGrid project.

    This software has been tested with care but is not guaranteed for
    any particular purpose. Neither the authors, nor the University of
    Manchester offer any warranties or representations, nor do they
    accept any liabilities with respect to this software.

    This program must not be used for commmercial gain without the
    written permission of the authors.
    
    Supercomputing, Visualization & e-Science Group
    Manchester Computing
    University of Manchester
    Manchester M13 9PL

    email:  csar-advice@cfs.ac.uk.
    Tel:    +44 161 275 6824/5997
    Fax:    +44 161 275 6040    
    
    Date          Version    Updates                            Author
    ----          -------    -------                            ------
    23.10.2002      0.1                                         S Ramsden

---------------------------------------------------------------------------*/
 
#include "configform.h"
#include "utility.h"
#include "types.h"
#include "debug.h"

#include <qvalidator.h>
#include <qhbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qvbox.h>

#include <math.h>

ConfigForm::ConfigForm(int aCurrentIntervalValue, QWidget *parent, const char *name,
		       bool modal, WFlags f)
  : QDialog( parent, name, modal, f ), 
    mApplyButton(kNULL), mCancelButton(kNULL)
{

  DBGCON("ConfigForm");

  // note aCurrentIntervalValue is in milliseconds - convert to seconds for GUI entry

  this->setCaption( "Configure Poling" );
  resize( 150, 150 );
    
  // create the layouts for the form
  QVBoxLayout *lFormLayout = new QVBoxLayout(this, 10, 10, "configformlayout");
  QHBoxLayout *lButtonLayout = new QHBoxLayout(6, "configbuttonlayout");

  //SMR XXX replace with constants for range throughout -to do
  lFormLayout->addWidget(new QLabel("Enter interval value (seconds) \nValid range: 0.1 - 10 (1d.p)", this));

  mLineEdit = new QLineEdit( this );

  double lVal = (double) aCurrentIntervalValue/1000;

  mLineEdit->setText(QString::number(lVal, 'g', 3));
  mLineEdit->setValidator( new QDoubleValidator(mLineEdit, "dbleavlidator" ) );

  lFormLayout->addWidget( mLineEdit);

  mApplyButton = new QPushButton("Apply", this, "Applybutton"); \
  mApplyButton->setMinimumSize(mApplyButton->sizeHint());
  mApplyButton->setMaximumSize(mApplyButton->sizeHint());
  mApplyButton->setAutoDefault(FALSE);
  QToolTip::add(mApplyButton, "Apply to steerer");
  connect(mApplyButton, SIGNAL(clicked()), this, SLOT(applySlot()));
  
    
  mCancelButton = new QPushButton("Cancel", this, "cancelbutton");
  mCancelButton->setMinimumSize(mCancelButton->sizeHint());
  mCancelButton->setMaximumSize(mCancelButton->sizeHint());
  mCancelButton->setAutoDefault(FALSE);
  connect(mCancelButton,  SIGNAL(clicked()), this, SLOT( reject()));
    
  lButtonLayout->addWidget(mApplyButton);
  lButtonLayout->addWidget(mCancelButton);
  
  lFormLayout->addLayout(lButtonLayout);

}

ConfigForm::~ConfigForm()
{
  DBGDST("ConfigForm");
}

int
ConfigForm::getIntervalValue(void) const
{
  return mIntervalValue;
}


void
ConfigForm::applySlot()
{
  bool lOk = false;
  double lValue;
  if (!mLineEdit->text().isEmpty())
  {
    lValue = mLineEdit->text().toDouble(&lOk);
    if (lOk)
    {
      if (lValue >= 0.1 && lValue <= 10)
      {
	// keep only one d.p; add 0.001 to for .9999 representation problem
	lValue += 0.001;
	lValue=floor(lValue*10);

	//convert to int and miiliseconds
	mIntervalValue = (int) (lValue*100);
	QDialog::accept();
      }
      else
	lOk=false;
    }

    if (!lOk)
    { 
      //value is out of range
      QMessageBox::information(0, "Invalid entry", "Please enter a value between 0.1 and 10",
			       QMessageBox::Ok,
			       QMessageBox::NoButton, 
			       QMessageBox::NoButton);
    }

  }
  else
  {
    // no item in the list has been selected
    QMessageBox::information(0, "No value", "Please enter a value",
			     QMessageBox::Ok,
			     QMessageBox::NoButton, 
			     QMessageBox::NoButton);
    

  }
  
    

}

