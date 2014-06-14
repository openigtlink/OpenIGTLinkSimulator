/*=========================================================================

  Program:   OpenIGTLink Connector Class for OpenIGTLink Simulator
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "igtlSocket.h"

#include <string.h>

#include "igtlTCPConnectorServerOIGTL.h"
#include "igtlOSUtil.h"
#include "igtlTransformMessage.h"

namespace igtl
{

//-----------------------------------------------------------------------------
TCPConnectorServerOIGTL::TCPConnectorServerOIGTL()
{
}


//-----------------------------------------------------------------------------
TCPConnectorServerOIGTL::~TCPConnectorServerOIGTL()
{
}


//-----------------------------------------------------------------------------
void TCPConnectorServerOIGTL::PrintSelf(std::ostream& os) const
{
  this->Superclass::PrintSelf(os);
}


//-----------------------------------------------------------------------------
int TCPConnectorServerOIGTL::PushMessage(igtl::MessageBase * message)
{
  if (this->Status == STATUS_CONNECTED)
    {
    if (this->Socket.IsNull())
      {
      // Do nothing.
      return 1;
      }
    int r = this->Socket->Send(message->GetPackPointer(), message->GetPackSize());
    if (r)
      {
      this->SetDataSentFlag();
      return 1;
      }
    else
      {
      //std::cerr << "TCPConnectorServerOIGTL::PushMessage() : Connection Lost." << std::endl;
      return 0;
      }
    }

  // TODO: Does ReceiveMessage() in MonitorThreadFunction() returns 0 in this case?
  return 0;
}


//-----------------------------------------------------------------------------
int TCPConnectorServerOIGTL::Initialize()
{
  if (TCPConnectorServer::Initialize())
    {
    this->HeaderMsg = igtl::MessageHeader::New();
    return 1;
    }

  this->HeaderMsg = NULL;
  return 0;

}

//-----------------------------------------------------------------------------
int TCPConnectorServerOIGTL::ReceiveMessage()
{

  //std::cerr << "TCPConnectorServerOIGTL::ReceiveMessage() : Waiting for messages." << std::endl;

  // Initialize receive buffer
  //this->HeaderMsg->InitPack();

  igtl::MessageHeader::Pointer header;
  header = igtl::MessageHeader::New();
  header->InitPack();

  if (this->Socket.IsNotNull() && this->Socket->GetConnected())
    {
    int r = this->Socket->Receive(header->GetPackPointer(),
                                  header->GetPackSize());
    if (r != header->GetPackSize())
      {
      if (r == 0)
        {
        // Connection closed
        return 0;
        }
      // Wrong message but continue to communicate
      //std::cerr << "TCPConnectorServerOIGTL::ReceiveMessage() : Illegal message." << std::endl;
      return 1;
      }
    }
  else
    {
    return 0;
    }

  // Deserialize the header
  header->Unpack();

  // Check data type and receive data body
  if (strcmp(header->GetDeviceType(), "TRANSFORM") == 0)
    {
    return ReceiveTransform(header);
    }
  //else if (strcmp(header->GetDeviceType(), "POSITION") == 0)
  //  {
  //  ReceivePosition(socket, header);
  //  }
  //else if (strcmp(header->GetDeviceType(), "IMAGE") == 0)
  //  {
  //  ReceiveImage(socket, header);
  //  }
  //else if (strcmp(header->GetDeviceType(), "STATUS") == 0)
  //  {
  //  ReceiveStatus(socket, header);
  //  }
  //else if (strcmp(header->GetDeviceType(), "POINT") == 0)
  //  {
  //  ReceivePoint(socket, header);
  //  }
  //else if (strcmp(header->GetDeviceType(), "STRING") == 0)
  //  {
  //  ReceiveString(socket, header);
  //  }
  //else if (strcmp(header->GetDeviceType(), "BIND") == 0)
  //  {
  //  ReceiveBind(socket, header);
  //  }
  else
    {
    // if the data type is unknown, skip reading.
    //std::cerr << "Receiving : " << header->GetDeviceType() << std::endl;
    if (this->Socket.IsNotNull() && this->Socket->GetConnected())
      {
      this->Socket->Skip(header->GetBodySizeToRead(), 0);
      }
    }
          
  return 1;

}


//-----------------------------------------------------------------------------
int TCPConnectorServerOIGTL::Finalize()
{
  if (TCPConnectorServer::Finalize())
    {
    return 1;
    }
  return 0;
}


//-----------------------------------------------------------------------------
int TCPConnectorServerOIGTL::ReceiveTransform(igtl::MessageHeader * header)
{

  //std::cerr << "TCPConnectorServerOIGTL::ReceiveTransform() : Receiving TRANSFORM data type." << std::endl;
  
  // Create a message buffer to receive transform data
  igtl::TransformMessage::Pointer transMsg;
  transMsg = igtl::TransformMessage::New();
  transMsg->SetMessageHeader(header);
  transMsg->AllocatePack();
  
  // Receive transform data from the socket
  if (this->Socket.IsNotNull() && this->Socket->GetConnected())
    {
    int r = this->Socket->Receive(transMsg->GetPackBodyPointer(), transMsg->GetPackBodySize());
    if (r == 0)
      {
      // Connection closed.
      return 0;
      }
    }
  else
    {
    return 0;
    }
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = transMsg->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    if (this->OutputConnector.IsNotNull())
      {
      this->OutputConnector->PushMessage(transMsg);
      }
    return 1;
    }

  return 1;
}
    

} // End of igtl namespace






