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


//-----------------------------------------------------------------------------
qDataGeneratorTracking::~qDataGeneratorTracking()
{
}


//-----------------------------------------------------------------------------
void qDataGeneratorTracking::GenerateData(igtl::MessageBase::Pointer& data)
{

  std::cerr << "void qDataGeneratorTracking::GenerateData(igtl::MessageBase::Pointer& data) is called " <<std::endl;

  igtl::Matrix4x4 matrix;
  igtl::TrackingDataElement::Pointer ptr;

  for (int i = 0; i < this->NumberOfChannels; i ++)
    {
    this->TrackingMsg->GetTrackingDataElement(i, ptr);
    GetRandomTestMatrix(matrix, this->Phi[i], this->Theta[i]);
    ptr->SetMatrix(matrix);
    this->Phi[i] += 0.1*(float)(i+1);
    this->Theta[i] += 0.05*(float)(i+1);
    }

  this->TrackingMsg->Pack();
  data = this->TrackingMsg;
}


//------------------------------------------------------------
void qDataGeneratorTracking::GetRandomTestMatrix(igtl::Matrix4x4& matrix, float phi, float theta)
{

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
  
  //igtl::PrintMatrix(matrix);
}







