/*=========================================================================
  
  Program:   Data Reading Tracking Class for OpenIGTLink Simulator
  Language:  C++
  
  Copyright (c) Brigham and Women's Hospital. All rights reserved.
  
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.
  
  =========================================================================*/
#include <string>
#include "qDataReadingTracker.h"


//-----------------------------------------------------------------------------
qDataReadingTracker::qDataReadingTracker(): qDataGeneratorBase()
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
qDataReadingTracker::~qDataReadingTracker()
{
}


//-----------------------------------------------------------------------------
void qDataReadingTracker::RegisterHandlers(igtl::TCPConnectorServerOIGTL * connector)
{
  connector->RegisterMessageHandler("STT_TDATA", this);
  connector->RegisterMessageHandler("STP_TDATA", this);
}


//----------------------------------------------------------------------------
void qDataReadingTracker::ReadData(igtl::MessageBase::Pointer& data)
{
  if(this->fTracking && FileName!="")
    {
      float temp;
      igtl::Matrix4x4 matrix;
      this->count++;
 
      const char * ccpFileName = FileName.c_str();
      FILE *myfile = std::fopen(ccpFileName, "r");
      
      igtl::TrackingDataElement::Pointer ptr;
      
      for(int k = 0; k < this->NumberOfChannels; k ++)
	{
	  this->TrackingMsg->GetTrackingDataElement(k, ptr);
	  int counter=0;
	  while(counter<count*NumberOfChannels)
	    {
	    for(int i=0; i<=3; i++)
	      {
		for(int j=0; j<=3; j++)
		  {
		    std::fscanf(myfile, "%f ", &temp);
		  }
		std::fscanf(myfile, "\n");
	      }
	    std::fscanf(myfile,"\n");
	    counter++;
	    }
	  
	  for(int i=0;i<=3;i++)
	    {
	      for(int j=0;j<=3;j++)
		{
		  std::fscanf(myfile, "%f ", &temp);
		  matrix[i][j]=temp;
		}
	      std::fscanf(myfile,"\n\n");
	    }
	  igtl::PrintMatrix(matrix);
	  ptr->SetMatrix(matrix);
	}
      fclose(myfile);
      this->TrackingMsg->Pack();
      data = this->TrackingMsg;
    }
  else
    {
    }

}


//------------------------------------------------------------
int qDataReadingTracker::HandleReceivedMessage(igtl::Socket *socket, igtl::MessageHeader * header)
{
  
  std::cerr << "qDataReadingTracker::HandleReceivedMessage() : " << header->GetDeviceType() << std::endl;
  
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
      //        if (socket && socket->GetConnected())
      //{
      //     int r = socket->Receive(stpMsg->GetPackBodyPointer(), stpMsg->GetPackBodySize());
      //    if (r == 0)
      //    {
      //        // Connection closed.
      //        return 0;
      //    }
      //}
      //else
      //{
      //    return 0;
      //}
      // 
      //int c = stpMsg->Unpack(1);
      //if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
      //{  //up to here
      this->fTracking = 0;
      return 1;
      // }
    }
  return 1;

}


//------------------------------------------------------------
void qDataReadingTracker::ChannelChanged(int i)
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
