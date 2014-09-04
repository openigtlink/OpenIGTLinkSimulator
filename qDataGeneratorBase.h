/*=========================================================================

  Program:   Data Generator Base Class for OpenIGTLink Simulator
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __qDataGeneratorBase_H
#define __qDataGeneratorBase_H

#include <QObject>
#include <QTimer>

#include "igtlWin32Header.h"
#include "igtlSmartPointer.h"
#include "igtlMessageBase.h"
#include "igtlTCPConnectorServerOIGTL.h"

//----------------------------------------------------------------------
// Description
//

class IGTLCommon_EXPORT qDataGeneratorBase : public QObject
{

  Q_OBJECT

public:

  qDataGeneratorBase();
  virtual ~qDataGeneratorBase();

  
  virtual const char * GetClassName() { return "qDataGeneratorBase"; };

  void    SetInterval(int ms) { this->TimerInterval = ms; };
  int     GetInterval() { return this->TimerInterval; }
  void    SetConnector(igtl::TCPConnectorServerOIGTL * connector);

  virtual int  HandleReceivedMessage(igtl::Socket *socket, igtl::MessageHeader * header) {};

  void    Start();
  void    Stop();

protected slots:

    void    ProcessTimer(std::string filename);
    void    ProcessTimer();

protected:
virtual void RegisterHandlers(igtl::TCPConnectorServerOIGTL * NOTUSED(connector)) {};
 virtual void GenerateData(igtl::MessageBase::Pointer& NOTUSED(data)) {};
    virtual void ReadData(igtl::MessageBase::Pointer& NOTUSED(data), std::string) {};

protected:

  QTimer* Timer;
  int    TimerInterval;
  igtl::TCPConnectorServerOIGTL::Pointer Connector;

};


#endif //__THREAD_H



