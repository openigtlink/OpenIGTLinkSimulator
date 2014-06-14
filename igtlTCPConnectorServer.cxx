/*=========================================================================

  Program:   Connector Server Class for OpenIGTLink Simulator
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlSocket.h"

#include <string.h>

#include "igtlTCPConnectorServer.h"

namespace igtl
{

//-----------------------------------------------------------------------------
TCPConnectorServer::TCPConnectorServer()
{
  this->ServerSocket = igtl::ServerSocket::New();
  this->ServerSocket->DebugOff();
}

//-----------------------------------------------------------------------------
TCPConnectorServer::~TCPConnectorServer()
{
}


//-----------------------------------------------------------------------------
void TCPConnectorServer::PrintSelf(std::ostream& os) const
{
  this->Superclass::PrintSelf(os);
}


//-----------------------------------------------------------------------------
int TCPConnectorServer::Initialize()
{
  if (this->ServerSocket->CreateServer(this->Port))
    {
    //std::cerr << "TCPConnectorServer::Initialize() : Cannot create a server socket." << std::endl;
    return 0;
    }
  return 1;
}


//-----------------------------------------------------------------------------
int TCPConnectorServer::WaitForConnection()
{
  std::cerr << this->GetClassName() << ": WaitForConnection(): Port number # = " << this->Port << std::endl;
  this->Socket = NULL;
  if (this->ServerSocket.IsNotNull())
    {
    //this->ServerSocket->CreateServer(this->Port);
    this->Socket = this->ServerSocket->WaitForConnection(1000);
    }

  if (this->Socket.IsNotNull() && this->Socket->GetConnected()) // if client connected
    {
    this->Socket->DebugOff();
    return 1;
    }

  return 0;

}


//-----------------------------------------------------------------------------
int TCPConnectorServer::CloseConnection()
{
  if (this->Socket.IsNotNull())
    {
    this->Socket->CloseSocket();
    }

  return 0;
}

//-----------------------------------------------------------------------------
int TCPConnectorServer::Finalize()
{
  if (this->ServerSocket.IsNotNull())
    {
    this->ServerSocket->CloseSocket();
    }

  return 0;
}

} // End of igtl namespace






