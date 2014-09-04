/*=========================================================================
 
 Program:   Data Reading Tracking Class for OpenIGTLink Simulator
 Language:  C++
 
 Copyright (c) Brigham and Women's Hospital. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <sstream>
#include <cmath>
#include "qDataReadingTracker.h"

//#include "ui_OpenIGTLinkSimulatorWindow.h
//-----------------------------------------------------------------------------
qDataReadingTracker::qDataReadingTracker(): qDataGeneratorBase()
{
    
    //------------------------------------------------------------
    // Allocate TrackingData Message Class
    //
    // NOTE: TrackingDataElement class instances are allocated
    //       before the loop starts to avoid reallocation
    //       in each image transfer.
    
    //setupUI(this);
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
void qDataReadingTracker::GenerateData(igtl::MessageBase::Pointer& data)
{
  if(!this->fTracking && FileName!="")
    {
    this->count++;
    float temp;
    //    float matrix[4][4];
    igtl::Matrix4x4 matrix;
    const char * ccpFileName = FileName.c_str();
    FILE *myfile = std::fopen(ccpFileName, "r");
    //int counter=0;
    igtl::TrackingDataElement::Pointer ptr;                         //!
    // std::cerr << NumberOfChannels << std::endl;
    for(int k = 0; k < this->NumberOfChannels; k ++)
    	{
	  this->TrackingMsg->GetTrackingDataElement(k, ptr);
	  int counter=0;
	  while(counter<count*NumberOfChannels){
	     //if(counter == count-1)break;
	    for(int i=0; i<=3; i++){
	      for(int j=0; j<=3; j++){
	  	std::fscanf(myfile, "%f ", &temp);
	      }
	      std::fscanf(myfile, "\n");
	    }
	    std::fscanf(myfile,"\n");
	    counter++;
	   }
	  for(int i=0;i<=3;i++){
	    for(int j=0;j<=3;j++){
	      std::fscanf(myfile, "%f ", &temp);
	      matrix[i][j]=temp;
	    }
	    std::fscanf(myfile,"\n\n");
	  }
	/*
	  int counter=0;
	  while(1){
	    for(int i=0;i<=3;i++){
	      for(int j=0;j<=3;j++){
		std::fscanf(myfile, "%f ", &temp);
		matrix[i][j]=temp;
	      }
	    }
	    if(counter == count) break;
	    counter++;
	  }
	*/
	  //  std::cout << "counter="<<counter << std::endl;
	  //std::cout << "count="<<count << std::endl;
	  igtl::PrintMatrix(matrix);
	  ptr->SetMatrix(matrix);
	  // fclose(myfile);
	}
    fclose(myfile);
    this->TrackingMsg->Pack();
    data = this->TrackingMsg;
  }


  else
    {
      std::cerr << "Insufficient file name." << std::endl;
    } 
}

//----------------------------------------------------------------------------
void qDataReadingTracker::ReadData(igtl::MessageBase::Pointer& data, std::string filename)
{
    
    if (this->fTracking)
    {
        igtl::Matrix4x4 matrix;
        igtl::TrackingDataElement::Pointer ptr;
        
        for (int i = 0; i < this->NumberOfChannels; i ++)
        {
            this->TrackingMsg->GetTrackingDataElement(i, ptr);
            GetFileMatrix(matrix,filename);
            ptr->SetMatrix(matrix);
            //this->Phi[i] += 0.1*(float)(i+1);
            //this->Theta[i] += 0.05*(float)(i+1);
        }
        
        this->TrackingMsg->Pack();
        data = this->TrackingMsg;
        //   }
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
void qDataReadingTracker::GetFileMatrix(igtl::Matrix4x4& matrix, std::string filename)
{
  /*    float temp;
    const char * ccpfilename = filename.c_str();
    FILE *myfile = fopen(ccpfilename, "r");
    for(int i=0;i<=4;i++){
        for(int j=0;j<=4;j++){

            std::fscanf(myfile, "%f ", &temp);
                matrix[i][j]=temp;
                }
            }
        igtl::PrintMatrix(matrix);
        fclose(myfile);
  */
}
// //    }



    /*
    float position[3];
    float orientation[4];
    
    // random position
    position[0] = 50.0 * cos(phi);
    position[1] = 50.0 * sin(phi);
    position[2] = 50.0 * cos(phi);
    phi = phi + 0.2;
    
    // random orientation
    orientation[0]=0.0;
    orientation[1]=0.6666666666*cos(theta);
    orientation[2]=0.577350269189626;
    orientation[3]=0.6666666666*sin(theta);
    theta = theta + 0.1;
    
    //igtl::Matrix4x4 matrix;
    igtl::QuaternionToMatrix(orientation, matrix);
    
    matrix[0][3] = position[0];
    matrix[1][3] = position[1];
    matrix[2][3] = position[2];
    
    igtl::PrintMatrix(matrix);
    //    teIncomingData->setText( matrix );
     */



//------------------------------------------------------------
void qDataReadingTracker::ChannelChanged(int i)
{
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
