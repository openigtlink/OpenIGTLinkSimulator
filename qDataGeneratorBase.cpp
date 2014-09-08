/*=========================================================================
  
  Program:   Data Generator Base Class for OpenIGTLink Simulator
  Language:  C++
  
  Copyright (c) Brigham and Women's Hospital. All rights reserved.
  
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.
  
  =========================================================================*/

#include <string.h>
#include "qDataGeneratorBase.h"
#include <QTimer>

//-----------------------------------------------------------------------------
qDataGeneratorBase::qDataGeneratorBase()
{
  this->TimerInterval = 200; // default is 100 ms
  this->Timer = NULL;
}

//-----------------------------------------------------------------------------
qDataGeneratorBase::~qDataGeneratorBase()
{
}


//-----------------------------------------------------------------------------
void qDataGeneratorBase::SetConnector(igtl::TCPConnectorServerOIGTL * connector)
{
  this->Connector = connector;
  this->RegisterHandlers(connector);
}


//-----------------------------------------------------------------------------
void qDataGeneratorBase::Start()
{
  
  this->Timer = new QTimer(this);
  connect(this->Timer, SIGNAL(timeout()), this, SLOT(ProcessTimer()));
  this->Timer->start(this->TimerInterval); 
  
}

//-----------------------------------------------------------------------------
void qDataGeneratorBase::Stop()
{

  if (this->Timer != NULL)
    {
      if (this->Timer->isActive() == true)
	{
	  this->Timer->stop();
	}
      
      delete this->Timer;
      this->Timer = NULL;
    }

}


//-----------------------------------------------------------------------------
void qDataGeneratorBase::ProcessTimer()
{
  
  if (this->Connector.IsNotNull() && 
      this->Connector->GetStatus() == igtl::TCPConnectorBase::STATUS_CONNECTED)
    {
      igtl::MessageBase::Pointer message;
      this->GenerateData(message);
      this->ReadData(message);
      if (message.IsNotNull())
	{
	  this->Connector->PushMessage(message);
	}
    }
  
}
