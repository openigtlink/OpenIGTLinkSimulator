/*=========================================================================

  Program:   OpenIGTLink Simulator
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <QtGui> 

#include "OpenIGTLinkSimulatorApp.h"
 

#include "igtlOSUtil.h"
#include "igtlImageMessage.h"
#include "igtlServerSocket.h"
#include "igtlMultiThreader.h"
#include "igtlOSUtil.h"
#include "igtlTCPConnectorServerOIGTL.h"
#include "qDataGeneratorTracking.h"

const int OpenIGTLinkSimulatorApp::StatusColorTable[][3] = {
  {100, 100, 100},  // STOP
  {100, 100, 200},  // WAITING
  {100, 200, 100},  // CONNECTED
  {200, 100, 100}   // ERROR
};

const int OpenIGTLinkSimulatorApp::DataIOColorTable[][3] = {
  {50, 50, 50},        // None
  {100, 200, 100},  // Received
  {200, 100, 100}   // ERROR
};

 
OpenIGTLinkSimulatorApp::OpenIGTLinkSimulatorApp(QWidget *NOTUSED(parent))
{
  setupUi(this); // this sets up GUI

  fScannerActive = false;
  fClientActive  = false;

  // Signals and Slots
  connect(pbQuit, SIGNAL( clicked() ), this, SLOT( quit() ));
  connect(pbAbout, SIGNAL( clicked() ), this, SLOT( about() ));
  //connect(pbScannerActivate, SIGNAL( clicked() ), this, SLOT( scannerActivateClicked() ));
  connect(pbClientActivate, SIGNAL( clicked() ), this, SLOT( clientActivateClicked() ));

  //connect(leScannerAddress, SIGNAL( textChanged( const QString &  ) ),
  //        this, SLOT( scannerAddressChanged( const QString &  ) ));
  //connect(leControlPort, SIGNAL( textChanged( const QString & ) ),
  //        this, SLOT( controlPortChanged( const QString & ) ));
  //connect(leImagePort, SIGNAL( textChanged( const QString &  ) ),
  //        this, SLOT( imagePortChanged( const QString & ) ));
  connect(leOpenIGTLinkPort, SIGNAL( textChanged( const QString & ) ),
          this, SLOT( igtlPortChanged( const QString &  ) ));

  // Default values
  QString qs;
  //leScannerAddress->setText(DEFAULT_RMP_ADDR);
  leOpenIGTLinkPort->setText(qs.setNum(DEFAULT_OIGTL_PORT));

  // Time for GUI update (every 200 ms)
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(updateStatus()));
  timer->start(200); 

  // Data Generator
  this->TrackingDataGenerator = new qDataGeneratorTracking();
  
  //// OpenIGTLink Server Socket
  oigtlConnector = igtl::TCPConnectorServerOIGTL::New();
  oigtlConnector->SetPort(18944);
  
  this->TrackingDataGenerator->SetConnector(oigtlConnector);
  this->TrackingDataGenerator->Start();
  this->Threader = igtl::MultiThreader::New();
  this->Threader->SpawnThread((igtl::ThreadFunctionType) &igtl::TCPConnectorServerOIGTL::MonitorThreadFunction,
                              oigtlConnector);

}
 
 
void OpenIGTLinkSimulatorApp::getPath()
{
    QString path;
    
    //path = QFileDialog::getOpenFileName(
    //    this,
    //    "Choose a file to open",
    //    QString::null,
    //    QString::null);
    //
    //lineEdit->setText( path );
}
 
 
void OpenIGTLinkSimulatorApp::about() 
{
  QMessageBox::about(this,
                     "About OpenIGTLinkSimulator",
                     "OpenIGTLink Simulator\n\n"
                     "Written by Junichi Tokuda\n"
                     "Brigham and Women's Hospital.\n\n"
                     "Copyright (C) 2011-2014\n"
                     );
}

void OpenIGTLinkSimulatorApp::scannerActivateClicked()
{
  if (fScannerActive)
    {
    //pbScannerActivate->setText("Activate");
    //fScannerActive = false;
    }
  else
    {
    //pbScannerActivate->setText("Deactivate");
    //fScannerActive = true;
    }
}


void OpenIGTLinkSimulatorApp::clientActivateClicked()
{
  if (fClientActive)
    {
    fClientActive = false;
    if (oigtlConnector.IsNotNull())
      {
      oigtlConnector->Deactivate();
      }
    }
  else
    {
    fClientActive = true;
    if (oigtlConnector.IsNotNull())
      {
      std::cerr << "Activating OpenIGTLink connector with:" << std::endl;
      std::cerr << "    Port: " << igtlPort.toInt() << std::endl;
      oigtlConnector->SetPort(igtlPort.toInt());
      oigtlConnector->Activate();
      }
    }
}


void OpenIGTLinkSimulatorApp::scannerAddressChanged( const QString & text )
{
  scannerAddress = text;
}

void OpenIGTLinkSimulatorApp::imagePortChanged( const QString & text )
{
  imagePort = text;
}

void OpenIGTLinkSimulatorApp::controlPortChanged( const QString & text )
{
  controlPort = text;
}

void OpenIGTLinkSimulatorApp::igtlPortChanged( const QString & text )
{
  igtlPort = text;
}

void OpenIGTLinkSimulatorApp::changeStatusTextColor(QLineEdit* le, int status)
{
  if (status < igtl::TCPConnectorBase::STATUS_NUM && status >= 0)
    {
    QPalette p = le->palette();
    p.setColor( le->backgroundRole(),
                QColor(StatusColorTable[status][0],
                       StatusColorTable[status][1],
                       StatusColorTable[status][2]) );
    le->setPalette(p);
    }
}

void OpenIGTLinkSimulatorApp::changeDataIOTextColor(QLineEdit* le, int status)
{
  if (status < 3 && status >= 0)
    {
    QPalette p = le->palette();
    p.setColor( le->backgroundRole(),
                QColor(DataIOColorTable[status][0],
                       DataIOColorTable[status][1],
                       DataIOColorTable[status][2]) );
    le->setPalette(p);
    }
}

void OpenIGTLinkSimulatorApp::updateStatus() 
{
  bool editClientFlag = true;
  if (oigtlConnector.IsNotNull())
    {
    leStatusClient->setText(oigtlConnector->GetStatusString());
    changeStatusTextColor(leStatusClient, oigtlConnector->GetStatus());
    leOpenIGTLinkPort->setEnabled(oigtlConnector->GetStatus() == igtl::TCPConnectorBase::STATUS_STOP);
    editClientFlag &= oigtlConnector->GetStatus() == igtl::TCPConnectorBase::STATUS_STOP;
    if (oigtlConnector->GetStatus() == igtl::TCPConnectorBase::STATUS_STOP)
      {
      pbClientActivate->setText("Activate");
      }
    else
      {
      pbClientActivate->setText("Deactivate");
      }
    }
  //bool editScannerFlag = true;
  //leScannerAddress->setEnabled(editScannerFlag);
  //leControlPort->setEnabled(editScannerFlag);
  //leImagePort->setEnabled(editClientFlag);
}

void OpenIGTLinkSimulatorApp::quit() 
{

  if (oigtlConnector.IsNotNull())
    {
    oigtlConnector->CloseThread();
    }

  timer->stop(); 
  close();
}


