/*=========================================================================

  Program:   Data Generator Tracking Class for OpenIGTLink Simulator
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include <cstring>
#include <sstream>
#include <cmath>
#include "qDataGeneratorTracking.h"


//-----------------------------------------------------------------------------
qDataGeneratorTracking::qDataGeneratorTracking(): qDataGeneratorBase()
{

  //------------------------------------------------------------
  // Allocate TrackingData Message Class
  //
  // NOTE: TrackingDataElement class instances are allocated
  //       before the loop starts to avoid reallocation
  //       in each image transfer.

  this->TrackingMsg = igtl::TrackingDataMessage::New();
  this->TrackingMsg->SetDeviceName("Tracker");

  this->NumberOfChannels = 3;
  
  this->TrackingElement.resize(this->NumberOfChannels);
  this->Phi.resize(this->NumberOfChannels);
  this->Theta.resize(this->NumberOfChannels);

  this->fTracking = 0;

  for (int i = 0; i <this->NumberOfChannels; i ++)
    {
    std::stringstream ss;
    ss << "Channel " << i;
    this->TrackingElement[i] = igtl::TrackingDataElement::New();
    this->TrackingElement[i]->SetName(ss.str().c_str());
    this->TrackingElement[i]->SetType(igtl::TrackingDataElement::TYPE_TRACKER);
    this->TrackingMsg->AddTrackingDataElement(this->TrackingElement[i]);
    this->Phi[i] = 0.0;
    this->Theta[i] = 0.0;
    }
}


//-----------------------------------------------------------------------------
qDataGeneratorTracking::~qDataGeneratorTracking()
{
}


//-----------------------------------------------------------------------------
void qDataGeneratorTracking::RegisterHandlers(igtl::TCPConnectorServerOIGTL * connector)  
{
  connector->RegisterMessageHandler("STT_TDATA", this);
  connector->RegisterMessageHandler("STP_TDATA", this);
}


//-----------------------------------------------------------------------------
void qDataGeneratorTracking::GenerateData(igtl::MessageBase::Pointer& data)
{

  if (this->fTracking)
    {
      igtl::Matrix4x4 matrix;
      igtl::TrackingDataElement::Pointer ptr;
      for (int i = 0; i < this->NumberOfChannels; i ++)
	{
	  std::cerr << "Data for Channel:" << i << std::endl;
	  this->TrackingMsg->GetTrackingDataElement(i, ptr);
	  GetRandomTestMatrix(matrix, this->Phi[i], this->Theta[i]);
	  ptr->SetMatrix(matrix);
	  
	  this->Phi[i] += 0.1*(float)(i+1);
	  this->Theta[i] += 0.05*(float)(i+1);
	}
      
      this->TrackingMsg->Pack();
      data = this->TrackingMsg;
    }
}


//------------------------------------------------------------
int qDataGeneratorTracking::HandleReceivedMessage(igtl::Socket *socket, igtl::MessageHeader * header)
{

  std::cerr << "qDataGeneratorTracking::HandleReceivedMessage() : " << header->GetDeviceType() << std::endl;

  if (strcmp(header->GetDeviceType(), "STT_TDATA") == 0)
    {
    igtl::StartTrackingDataMessage::Pointer sttMsg;
    sttMsg = igtl::StartTrackingDataMessage::New();
    sttMsg->SetMessageHeader(header);
    sttMsg->AllocatePack();
    
    if (socket && socket->GetConnected())
      {
      int r = socket->Receive(sttMsg->GetPackBodyPointer(), sttMsg->GetPackBodySize());
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

    int c = sttMsg->Unpack(1);
    
    if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
      {
      int interval = sttMsg->GetResolution();
      if (interval > 100)
        this->SetInterval(interval);
      this->fTracking = 1;
      return 1;
      }
    }
  else if (strcmp(header->GetDeviceType(), "STP_TDATA") == 0)
    {
    igtl::StopTrackingDataMessage::Pointer stpMsg;
    stpMsg = igtl::StopTrackingDataMessage::New();
    stpMsg->SetMessageHeader(header);
    stpMsg->AllocatePack();
    //if (socket && socket->GetConnected())
    //  {
    //  //int r = socket->Receive(stpMsg->GetPackBodyPointer(), stpMsg->GetPackBodySize());
    //  //if (r == 0)
    //  //  {
    //  //  // Connection closed.
    //  //  return 0;
    //  //  }
    //  }
    //else
    //  {
    //  return 0;
    //  }
    //
    //int c = stpMsg->Unpack(1);
    //if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    //  {
    this->fTracking = 0;
    return 1;
    //}
    }
  return 1;

}


//------------------------------------------------------------
void qDataGeneratorTracking::GetRandomTestMatrix(igtl::Matrix4x4& matrix, float phi, float theta)
{

  float position[3];
  float orientation[4];

  // random position
  position[0] = 50.0 * cos(phi) + 50.0 * sin(phi);
  position[1] = 50.0 * sin(phi*phi);
  position[2] = 30.0 * cos(phi);
  phi = phi + 0.2;

  // random orientation
  orientation[0]=0.0;
  orientation[1]=cos(theta);
  orientation[2]=0.577350269189626;
  orientation[3]=0.6666666666*sin(theta);
  theta = theta + 0.1;

  //igtl::Matrix4x4 matrix;
  igtl::QuaternionToMatrix(orientation, matrix);

  matrix[0][3] = position[0];
  matrix[1][3] = position[1];
  matrix[2][3] = position[2];
  
  igtl::PrintMatrix(matrix);
}
//-----------------------------------------------------------------------------

void qDataGeneratorTracking::ChannelChanged(int i)
{
  this->TrackingMsg = igtl::TrackingDataMessage::New();
  this->TrackingMsg->SetDeviceName("Tracker");
  this->NumberOfChannels = i;
  
  this->TrackingElement.resize(this->NumberOfChannels);
  this->Phi.resize(this->NumberOfChannels);
  this->Theta.resize(this->NumberOfChannels);
  
  this->fTracking = 0;
  
  for (int i = 0; i < this->NumberOfChannels; i ++)
    {
      std::stringstream ss;
      ss << "Channel " << i;
      this->TrackingElement[i] = igtl::TrackingDataElement::New();
      this->TrackingElement[i]->SetName(ss.str().c_str());
      this->TrackingElement[i]->SetType(igtl::TrackingDataElement::TYPE_TRACKER);
      this->TrackingMsg->AddTrackingDataElement(this->TrackingElement[i]);
      this->Phi[i] = 0.0;
      this->Theta[i] = 0.0;
    }
}

