/*=========================================================================
  
  Program:   Data Generator Tracking Class for OpenIGTLink Simulator
  Language:  C++
  
  Copyright (c) Brigham and Women's Hospital. All rights reserved.
  
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.
  
  =========================================================================*/

#ifndef __qDataGeneratorTracking_H
#define __qDataGeneratorTracking_H
#include "qDataGeneratorBase.h"
#include "igtlTrackingDataMessage.h"

class IGTLCommon_EXPORT qDataGeneratorTracking : public qDataGeneratorBase
{
  Q_OBJECT
  
 public:
  
  qDataGeneratorTracking();
  virtual ~qDataGeneratorTracking(); 
  virtual const char * GetClassName() { return "qDataGeneratorTracking"; };
  virtual void ChannelChanged(int i);

 protected:
  
  virtual void RegisterHandlers(igtl::TCPConnectorServerOIGTL * connector);  
  virtual void GenerateData(igtl::MessageBase::Pointer& data);
  virtual int  HandleReceivedMessage(igtl::Socket *socket, igtl::MessageHeader * header);

  // Function to generate random matrix.
  void    GetRandomTestMatrix(igtl::Matrix4x4& matrix, float phi, float theta);
  
  
 protected:
  
  igtl::TrackingDataMessage::Pointer TrackingMsg;
  std::vector<igtl::TrackingDataElement::Pointer> TrackingElement;
  
  std::vector< float > Phi;
  std::vector< float > Theta;
  
  int NumberOfChannels;
  
  int fTracking;
  
};

#endif



