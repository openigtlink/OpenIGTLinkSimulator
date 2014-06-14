/*=========================================================================

  Program:   Connector Base Class for OpenIGTLink Simulator
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include <string.h>
#include "igtlTCPConnectorBase.h"
#include "igtlOSUtil.h"

namespace igtl
{

const char* TCPConnectorBase::StatusString[] = {
  "OFF",
  "WAITING",
  "CONNECTED",
  "ERROR"
};



//-----------------------------------------------------------------------------
TCPConnectorBase::TCPConnectorBase()
{
  this->Hostname = "localhost";
  this->Port     = 0;
  this->Active   = 0;
  this->Status   = STATUS_STOP; 

  this->ConfigurationUpdated = true;
  this->DataReceivedFlag = false;
  this->DataSentFlag = false;
}

//-----------------------------------------------------------------------------
TCPConnectorBase::~TCPConnectorBase()
{
}


//-----------------------------------------------------------------------------
void TCPConnectorBase::PrintSelf(std::ostream& os) const
{
  this->Superclass::PrintSelf(os);
}


//-----------------------------------------------------------------------------
void TCPConnectorBase::MonitorThreadFunction(void * ptr)
{
  igtl::MultiThreader::ThreadInfo* info = 
    static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);

  TCPConnectorBase * con = static_cast<TCPConnectorBase *>(info->UserData);

  con->Active = 0;
  while (con->Active >= 0)
    {
    if (con->ConfigurationUpdated)
      {
      con->ConfigurationUpdated = false;
      if (!con->Initialize())
        {
        return;
        }
      }
    con->Status = STATUS_STOP;
    while (con->Active)
      {
      con->Status = STATUS_WAITING;
      if(con->WaitForConnection())
        {
        con->Status = STATUS_CONNECTED;
        while (con->Status == STATUS_CONNECTED)
          {
          if (con->ReceiveMessage() == 0)
            {
            // Disconnected
            con->CloseConnection();
            con->Status = STATUS_WAITING;
            break;
            }
          con->SetDataReceivedFlag();
          }
        }
      }
    igtl::Sleep(500);
    }  

  con->Finalize();
}

} // End of igtl namespace




