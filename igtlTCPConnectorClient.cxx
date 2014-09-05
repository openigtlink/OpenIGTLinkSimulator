/*=========================================================================

  Program:   Connector Client Class for OpenIGTLink Simulator
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

  =========================================================================*/

#include "igtlSocket.h"

#include <string.h>

#include "igtlOSUtil.h"
#include "igtlTCPConnectorClient.h"

namespace igtl
{

  //-----------------------------------------------------------------------------
  TCPConnectorClient::TCPConnectorClient()
  {
  }


  //-----------------------------------------------------------------------------
  TCPConnectorClient::~TCPConnectorClient()
  {
  }


  //-----------------------------------------------------------------------------
  void TCPConnectorClient::PrintSelf(std::ostream& os) const
  {
    this->Superclass::PrintSelf(os);
  }


  //-----------------------------------------------------------------------------
  int TCPConnectorClient::Initialize()
  {
    return 1;
  }


  //-----------------------------------------------------------------------------
  int TCPConnectorClient::WaitForConnection()
  {
    //std::cerr << "TCPConnectorClient::WaitForConnection(): Connecting to server" << std::endl;

    this->Socket = NULL;
    this->Socket = igtl::ClientSocket::New();
    this->DebugOff();
    if (this->Hostname.length() == 0)
      {
	return 0;
      }
    //this->Socket->SetConnectTimeout(1000);
    int r = this->Socket->ConnectToServer(this->Hostname.c_str(), this->Port);
    this->Socket->SetReceiveTimeout(0);
    if (r == 0) // if connected to server
      {
	return 1;
      }
    //igtl::Sleep(1000);
    return 0;
  }


  //-----------------------------------------------------------------------------
  int TCPConnectorClient::ReceiveMessage()
  {
    return 0;
  }


  //-----------------------------------------------------------------------------
  int TCPConnectorClient::CloseConnection()
  {
    if (this->Socket.IsNotNull())
      {
	this->Socket->CloseSocket();
      }

    return 0;
  }

  //-----------------------------------------------------------------------------
  int TCPConnectorClient::Finalize()
  {
    return 0;
  }

} // End of igtl namespace






