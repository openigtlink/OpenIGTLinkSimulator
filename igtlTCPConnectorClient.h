/*=========================================================================

  Program:   Connector Client Class for OpenIGTLink Simulator
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

  =========================================================================*/

#ifndef __TCPConnectorClient_H
#define __TCPConnectorClient_H

#include "igtlWin32Header.h"
#include "igtlMultiThreader.h"
#include "igtlServerSocket.h"

#include "igtlTCPConnectorBase.h"

namespace igtl
{

  class IGTLCommon_EXPORT TCPConnectorClient : public TCPConnectorBase
  {
  public:
  
    typedef TCPConnectorClient        Self;
    typedef Object  Superclass;
    typedef SmartPointer<Self>        Pointer;
    typedef SmartPointer<const Self>  ConstPointer;

    igtlTypeMacro(igtl::TCPConnectorClient, igtl::TCPConnectorBase)
      igtlNewMacro(igtl::TCPConnectorClient);

  public:

    virtual const char * GetClassName() { return "Client"; };

  protected:
    TCPConnectorClient();
    ~TCPConnectorClient();

    void PrintSelf(std::ostream& os) const;

    virtual int Initialize();
    virtual int WaitForConnection();
    virtual int ReceiveMessage();
    virtual int CloseConnection();
    virtual int Finalize();
  
  protected:

    igtl::ClientSocket::Pointer     Socket;

  };

}

#endif //__THREAD_H
