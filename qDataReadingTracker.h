/*=========================================================================
 
 Program:   Data Reading Tracking Class for OpenIGTLink Simulator
 Language:  C++
 
 Copyright (c) Brigham and Women's Hospital. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#ifndef __qDataReadingTracking_H
#define __qDataReadingTracking_H

#include <string>

#include "qDataGeneratorBase.h"
#include "igtlTrackingDataMessage.h"

class IGTLCommon_EXPORT qDataReadingTracker : public qDataGeneratorBase
{
    Q_OBJECT
    
public:
    
    qDataReadingTracker();
    virtual ~qDataReadingTracker();
    
    virtual const char * GetClassName() { return "qDataReadingTracking"; };
    
    void    SetFileName(std::string name) { this->FileName = name; };
    
protected:
    
    virtual void RegisterHandlers(igtl::TCPConnectorServerOIGTL * connector);
    virtual void GenerateData(igtl::MessageBase::Pointer& data);

    virtual void ReadData(igtl::MessageBase::Pointer& data, std::string);
    
    virtual int  HandleReceivedMessage(igtl::Socket *socket, igtl::MessageHeader * header);
    
    // Function to Read matrix.
    void    GetFileMatrix(igtl::Matrix4x4& matrix, std::string);
    
    
protected:
    
    igtl::TrackingDataMessage::Pointer TrackingMsg;
    std::vector<igtl::TrackingDataElement::Pointer> TrackingElement;
    
    std::vector< float > Phi;
    std::vector< float > Theta;
    
    int NumberOfChannels;
    
    int fTracking;

    std::string FileName;
    int count;
    
};

#endif



