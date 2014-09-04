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
#include "qDataGeneratorBase.h"

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
	std::cerr << "Socket is null (PushMessage)." << std::endl;
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
  
  // std::cerr << "TCPConnectorServerOIGTL::ReceiveMessage() : Waiting for messages." << std::endl;

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
  MessageHandlerTypeMapType::iterator iter;
  iter = this->MessageHandlerTypeMap.find(header->GetDeviceType());

  qDataGeneratorBase* handler = NULL;

  if (iter != this->MessageHandlerTypeMap.end())
    {
    MessageHandlerNameMapType& nameMap = iter->second;
    MessageHandlerNameMapType::iterator niter;
    std::string key = std::string("+") + header->GetDeviceName();
    niter = nameMap.find(key);
    if (niter != nameMap.end())
      {
      handler = niter->second;
      }
    else // No handler for this specific message name
      {
      niter = nameMap.find("*");
      if (niter != nameMap.end()) // There is a wild card handler
        {
        handler = niter->second;
        }
      }
    }

  if (handler)
    {
    handler->HandleReceivedMessage(this->Socket, header);
    }

  else // No handler is available for this type of message.
    {
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
void TCPConnectorServerOIGTL::RegisterMessageHandler(const char* type, qDataGeneratorBase* handler)
{
  
  MessageHandlerTypeMapType::iterator iter;
  
  iter = this->MessageHandlerTypeMap.find(type);

  if (iter == this->MessageHandlerTypeMap.end()) // the type has not been registered yet.
    {
    MessageHandlerNameMapType nameMap;
    nameMap["*"] = handler;
    this->MessageHandlerTypeMap[type] = nameMap;
    }
  else
    {
    MessageHandlerNameMapType& nameMap = iter->second;
    nameMap.clear();
    nameMap["*"] = handler;
    }

}


//-----------------------------------------------------------------------------
void TCPConnectorServerOIGTL::RegisterMessageHandler(const char* type, const char* name, qDataGeneratorBase* handler)
{
  
  MessageHandlerTypeMapType::iterator iter;
  
  iter = this->MessageHandlerTypeMap.find(type);

  std::string nkey = std::string("+") + name;
  if (iter == this->MessageHandlerTypeMap.end()) // the type has not been registered yet.
    {
    MessageHandlerNameMapType nameMap;
    nameMap[nkey] = handler;
    this->MessageHandlerTypeMap[type] = nameMap;
    }
  else
    {
    MessageHandlerNameMapType& nameMap = iter->second;
    nameMap[nkey] = handler;
    }
}


//-----------------------------------------------------------------------------
void TCPConnectorServerOIGTL::UnRegisterMessageHandler(const char* type)
{

  MessageHandlerTypeMapType::iterator iter;
  
  iter = this->MessageHandlerTypeMap.find(type);
  
  if (iter != this->MessageHandlerTypeMap.end()) // the type is found in the map
    {
    this->MessageHandlerTypeMap.erase(iter);
    }
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






