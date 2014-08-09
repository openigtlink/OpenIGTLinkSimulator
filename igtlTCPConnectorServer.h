/*=========================================================================

  Program:   Connector Server Class for OpenIGTLink Simulator
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __TCPConnectorServer_H
#define __TCPConnectorServer_H

#include "igtlWin32Header.h"
#include "igtlMultiThreader.h"
#include "igtlServerSocket.h"

#include "igtlTCPConnectorBase.h"

namespace igtl
{

class IGTLCommon_EXPORT TCPConnectorServer : public TCPConnectorBase
{
public:
  
  typedef TCPConnectorServer        Self;
  typedef Object  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::TCPConnectorServer, igtl::TCPConnectorBase)
  igtlNewMacro(igtl::TCPConnectorServer);

public:

  virtual const char * GetClassName() { return "Server"; };

  igtl::Socket::Pointer& GetSocket() { return this->Socket; };
  
protected:
  TCPConnectorServer();
  ~TCPConnectorServer();

  void PrintSelf(std::ostream& os) const;

  virtual int Initialize();
  virtual int WaitForConnection();
  virtual int ReceiveMessage() { return 0; };
  virtual int CloseConnection();
  virtual int Finalize();

  
protected:
  igtl::ServerSocket::Pointer ServerSocket;
  igtl::Socket::Pointer       Socket;  

};

}

#endif //__THREAD_H
