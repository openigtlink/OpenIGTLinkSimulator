/*=========================================================================
  
  Program:   OpenIGTLink Simulator
  Language:  C++
  
  Copyright (c) Brigham and Women's Hospital. All rights reserved.
  
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.
  
  =========================================================================*/

#ifndef OpenIGTLinkSimulatorApp_h
#define OpenIGTLinkSimulatorApp_h

#include <QtGui> 

#include "ui_OpenIGTLinkSimulatorWindow.h"
#include "igtlMultiThreader.h"
#include "igtlTCPConnectorServerOIGTL.h"
#include "qDataGeneratorTracking.h"
#include "qDataReadingTracker.h"

#define DEFAULT_OIGTL_PORT   18944
#define DEFAULT_SPICE_PORT   2000
#define DEFAULT_RMP_PORT     20001

#define DEFAULT_RMP_ADDR     "192.168.2.1"


class OpenIGTLinkSimulatorApp : public QMainWindow, private Ui::OpenIGTLinkSimulatorWindow
{
  Q_OBJECT
  
 public:
  static const int StatusColorTable[][3];
  static const int DataIOColorTable[][3];
  
 public:
  OpenIGTLinkSimulatorApp(QWidget *parent = 0);
  
  
  public slots:
    void getPath();
    void about();
    void enable();
    void disable();
    void channel(int i);
  
    void scannerActivateClicked();
    void clientActivateClicked();

    void scannerAddressChanged( const QString & text );
    void imagePortChanged( const QString & text );
    void controlPortChanged( const QString & text );
    void igtlPortChanged( const QString & text );
  
    void updateStatus();
    void quit();

 private:

    void changeStatusTextColor(QLineEdit* le, int status);
    void changeDataIOTextColor(QLineEdit* le, int status);

    QTimer *timer;
    igtl::MultiThreader::Pointer Threader;
    igtl::TCPConnectorServerOIGTL::Pointer oigtlConnector;

    qDataGeneratorTracking* TrackingDataGenerator;
    qDataReadingTracker* TrackingDataReader;
    
    bool fScannerActive;
    bool fClientActive;

    QString scannerAddress;
    QString imagePort;
    QString controlPort;
    QString igtlPort;

    std::string PathString;


};
 
 
#endif

