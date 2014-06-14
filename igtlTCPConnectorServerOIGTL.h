/*=========================================================================

  Program:   OpenIGTLink Connector Class for OpenIGTLink Simulator
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __TCPConnectorServerOIGTL_H
#define __TCPConnectorServerOIGTL_H

#include "igtlWin32Header.h"
#include "igtlMultiThreader.h"
#include "igtlServerSocket.h"
#include "igtlMessageHeader.h"

#include "igtlTCPConnectorServer.h"

namespace igtl
{

class IGTLCommon_EXPORT TCPConnectorServerOIGTL : public TCPConnectorServer
{
public:
  
  typedef TCPConnectorServerOIGTL   Self;
  typedef Object  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::TCPConnectorServerOIGTL, igtl::TCPConnectorServer)
  igtlNewMacro(igtl::TCPConnectorServerOIGTL);

public:

  virtual const char * GetClassName() { return "OpenIGTLink"; };
  int SetMessageHandler();
  virtual int    PushMessage(igtl::MessageBase * message);

protected:

  TCPConnectorServerOIGTL();
  ~TCPConnectorServerOIGTL();

  void PrintSelf(std::ostream& os) const;

  virtual int Initialize();
  virtual int ReceiveMessage();
  virtual int Finalize();

  int ReceiveTransform(igtl::MessageHeader * header);
  
private:
  
  igtl::MessageHeader::Pointer HeaderMsg;

};

}

#endif //__THREAD_H
