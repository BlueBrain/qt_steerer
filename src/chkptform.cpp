/*----------------------------------------------------------------------------
  ChkPtForm class header file for QT steerer GUI. 

  (C) Copyright 2002, 2004, University of Manchester, United Kingdom,
  all rights reserved.

  This software is produced by the Supercomputing, Visualization and
  e-Science Group, Manchester Computing, University of Manchester
  as part of the RealityGrid project (http://www.realitygrid.org),
  funded by the EPSRC under grants GR/R67699/01 and GR/R67699/02.

  LICENCE TERMS

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.

  THIS MATERIAL IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. THE ENTIRE RISK AS TO THE QUALITY
  AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE
  DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR
  CORRECTION.

  Authors........: Mark Riding, Andrew Porter, Sue Ramsden
    
---------------------------------------------------------------------------*/

 
#include "chkptform.h"
#include "chkptvariableform.h"
#include "utility.h"
#include "types.h"
#include "debug.h"

#include <qapplication.h>
#include <qhbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qvbox.h>

ChkPtForm::ChkPtForm(const int aNumEntries, int aSimHandle, int aChkPtHandle,
		     QWidget *parent, const char *name,
		     bool modal, WFlags f)
  : QDialog( parent, name, modal, f ), mNumEntries(aNumEntries), mLibReturnStatus(REG_SUCCESS),
    mIndexSelected(-1), mListBox(kNULL), 
    mFilterLineEdit(kNULL), mRestartButton(kNULL), mCancelButton(kNULL)
{

  DBGCON("ChkPtForm");

  //  set up arrays for ReG lib call
  mLogEntries = new Output_log_struct[mNumEntries];

  // get the log entries from library
  qApp->lock();
  mLibReturnStatus = Get_chk_log_entries(aSimHandle, aChkPtHandle, mNumEntries, mLogEntries); //ReG library
  qApp->unlock();

  // only continue is there is some info to show
  if(mLibReturnStatus == REG_SUCCESS)
  {
    this->setCaption( "CheckPoint Selector" );
    resize( 350, 350 );
    
    // create the layouts for the form
    QHBoxLayout *lFormLayout = new QHBoxLayout(this, 10, 10, "chkptformlayout");
    //    QHBoxLayout *lFilterLayout = new QHBoxLayout(6, "filterlayout");
    QVBoxLayout *lListLayout = new QVBoxLayout(6, "chkptlistlayout");
    QVBoxLayout *lButtonLayout = new QVBoxLayout(6, "chkptbuttonlayout");
     
    // create the list box for the applications on the grid
    lListLayout->addWidget(new TableLabel("CheckPoint Tags", this));
    mListBox = new QListBox(this);
    mListBox->setSelectionMode( QListBox::Single ); 
     
    // populate the list box - each listboxitem holds array index information - this is what 
    // is used by the calling code (via  aSimIndexSelected) to identify the aSimGSH selected
    ChkPtListItem *lListItem;
    for (int i=0; i<mNumEntries; i++)
    {      
      DBGMSG1("ChkTag ", mLogEntries[i].chk_tag);

      // QString(const char *) is deep copy
      lListItem = new ChkPtListItem(i, QString( mLogEntries[i].chk_tag));
      mListBox->insertItem( lListItem );
    }

    
    // filter to do SMR XXX
///    mFilterLineEdit = new QLineEdit(this, "containsfilter");
///    connect(mFilterLineEdit, SIGNAL(returnPressed()), this, SLOT(filterSlot()));
///  
///    lFilterLayout->addWidget(new QLabel("Contains", this));
///    lFilterLayout->addWidget(mFilterLineEdit);
///    
    
    lListLayout->addWidget(mListBox);
    ///    lListLayout->addLayout(lFilterLayout);
    
    // MR: setup a button to deal with the view checkpoint parameters functionality
    mParametersButton = new QPushButton("Parameters", this, "parametersbutton");
    mParametersButton->setAutoDefault(FALSE);
    QToolTip::add(mParametersButton, "View Parameters for selected checkpoint");
    connect(mParametersButton, SIGNAL(clicked()), this, SLOT(viewChkPtParametersSlot()));
    connect(mListBox, SIGNAL(doubleClicked(QListBoxItem*)), this, SLOT(viewChkPtParametersDblClkSlot(QListBoxItem*)));

    mRestartButton = new QPushButton("Restart", this, "restartbutton"); 
    mRestartButton->setAutoDefault(FALSE);
    QToolTip::add(mRestartButton, "Restart using selected checkpoint");
    connect(mRestartButton, SIGNAL(clicked()), this, SLOT(restartSlot()));
    
    mCancelButton = new QPushButton("Cancel", this, "cancelbutton");
    mCancelButton->setAutoDefault(FALSE);
    connect(mCancelButton,  SIGNAL(clicked()), this, SLOT( reject()));

    mParametersButton->setMinimumSize(mParametersButton->sizeHint());
    mParametersButton->setMaximumSize(mParametersButton->sizeHint());
    mRestartButton->setMinimumSize(mParametersButton->sizeHint());
    mRestartButton->setMaximumSize(mRestartButton->sizeHint());
    mCancelButton->setMinimumSize(mParametersButton->sizeHint());
    mCancelButton->setMaximumSize(mCancelButton->sizeHint());

    lButtonLayout->addItem(new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ));
    lButtonLayout->addWidget(mParametersButton);
    lButtonLayout->addWidget(mRestartButton);
    lButtonLayout->addWidget(mCancelButton);
    
    lFormLayout->addLayout(lListLayout);
    lFormLayout->addLayout(lButtonLayout);
  }

}

ChkPtForm::~ChkPtForm()
{
  DBGDST("ChkPtForm");
  cleanUp();
}

void
ChkPtForm::cleanUp()
{

  delete []  mLogEntries;
}


int 
ChkPtForm::getLibReturnStatus() const
{
  return mLibReturnStatus;
  
}

char *
ChkPtForm::getChkTagSelected() const
{
  return mLogEntries[mIndexSelected].chk_tag;

}


void
ChkPtForm::restartSlot()
{

  ChkPtListItem *lTmpPtr;
  int lCurrentItem = mListBox->currentItem();
  bool lSelected = false;

  if (lCurrentItem >= 0)
  {
    if (mListBox->isSelected(lCurrentItem))
    { 
      lSelected = true;
      lTmpPtr = (ChkPtListItem *)mListBox->item(lCurrentItem);
      
      DBGMSG1("selected app index: ",lTmpPtr->getEntryIndex());
      mIndexSelected = lTmpPtr->getEntryIndex();
      DBGMSG1("selected chkpt: ", mLogEntries[mIndexSelected].chk_tag);

      QDialog::accept();
    }
  }

  if (!lSelected)
  {
    // no item in the list has been selected
    QMessageBox::information(0, "Nothing selected", "Please select an item in the list",
			     QMessageBox::Ok,
			     QMessageBox::NoButton, 
			     QMessageBox::NoButton);
    

  }
    

}


void
ChkPtForm::filterSlot()
{
#if 0
  // remove list and start again applying filter
  mListBox->clear();

  int lLen = mFilterLineEdit->text().length();

  ChkPtListItem *lListItem;
  
  if (lLen > 0)
  {
    for (int i=0; i<mNumSims; i++)
    {  
      DBGMSG2("***filter: ", mSimName[i], mFilterLineEdit->text().latin1());
      if (strstr(mSimName[i], mFilterLineEdit->text().latin1()) != kNULL)
      {
	lListItem = new ChkPtListItem(i, QString(mSimName[i]));
	mListBox->insertItem( lListItem );
      }      
    }
  }
  else
  {
    // no filter specified so just show all sim names
    for (int i=0; i<mNumSims; i++)
    {
      lListItem = new ChkPtListItem(i, QString(mSimName[i]));
      mListBox->insertItem( lListItem );
    }
  }
#endif
}

/** MR: When the user selects a checkpoint, show the variables
 */
void ChkPtForm::viewChkPtParametersSlot(){
  // We need to pass the relevant Output_log_struct to the new form,
  // so that it can populate it's own list appropiately
  int selectedIndex = mListBox->currentItem();

  Output_log_struct *tmp = &mLogEntries[selectedIndex];
  if (tmp==NULL){
    printf("Error - can't find any parameter log entries for this checkpoint");
    return;
  }

  // Create a modeless chkptvariableform and show it
  ChkPtVariableForm *lChkPtVariableForm = new ChkPtVariableForm(tmp, this);
  lChkPtVariableForm->show();

}

/** MR: When the user selects a checkpoint, show the variables
 */
void ChkPtForm::viewChkPtParametersDblClkSlot(QListBoxItem *t){
  viewChkPtParametersSlot();
}


ChkPtListItem::ChkPtListItem(int aEntryIndex, const QString &text)
  : QListBoxItem(), mEntryIndex(aEntryIndex)
{
  DBGCON("ChkPtListItem");
  setText( text );
  printf("constucted ChkPtListItem mEntryIndex = %d\n", mEntryIndex);

}

ChkPtListItem::~ChkPtListItem()
{
  DBGDST("ChkPtListItem");
}

int
ChkPtListItem::getEntryIndex() const
{
  return mEntryIndex;
}


void 
ChkPtListItem::paint( QPainter *painter )
{
    QFontMetrics fm = painter->fontMetrics();
    painter->drawText( 3, fm.ascent() +  (fm.leading()+1)/2 + 1, text() );
}

int 
ChkPtListItem::height( const QListBox* lb ) const
{
    int h = lb ? lb->fontMetrics().lineSpacing() + 2 : 0;
    if (h > QApplication::globalStrut().height())
      return h;
    else
      return QApplication::globalStrut().height();
}


int 
ChkPtListItem::width( const QListBox* lb ) const
{
    int w = lb ? lb->fontMetrics().width( text() ) + 6 : 0;
    if (w > QApplication::globalStrut().width())
      return w;
    else
      return QApplication::globalStrut().width();
}


