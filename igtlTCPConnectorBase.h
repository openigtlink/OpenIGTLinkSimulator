/*=========================================================================

  Program:   Connector Base Class for OpenIGTLink Simulator
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __TCPConnectorBase_H
#define __TCPConnectorBase_H

#include "igtlWin32Header.h"
#include "igtlMultiThreader.h"
#include "igtlImageMessage.h"
#include "igtlMutexLock.h"

#include <map>

//----------------------------------------------------------------------
// Description
//
// The TCPConnectorBase class and its sub-classes manage sessions of
// TCP/IP socket connections, and manage dataflows among multiple
// socket interfaces in a single process.
// 
// The dataflows are defined by connecting the subclasses of
// TCPConnectorBase. Suppose there is a proxy server that receives
// images from an external imaging scanner through the network and
// transfer the image data to a remote host. The server has two
// network interfaces, which are instances of the TCPConnectorBase
// subclasses. In such case, the server can be initialized as the
// following code:
//
//   igtl::TCPConnectorServerScanner::Pointer scannerConnector;
//   scannerConnector = igtlTCPConnectorServerScanner::New(); 
//
//   igtl::TCPConnectorServerRemote::Pointer remoteConnector;
//   remoteConnector = igtl::TCPConnectorServerRemote::New();
//
//   scannerConnector->OutputConnector(scannerConnector);
//

class qDataGeneratorBase;

namespace igtl
{

#define NOTUSED(x)

class IGTLCommon_EXPORT TCPConnectorBase : public Object
{
public:
  
  enum {
    STATUS_STOP = 0,
    STATUS_WAITING,
    STATUS_CONNECTED,
    STATUS_ERROR,
    STATUS_NUM
  };

  static const char*  StatusString[];

  typedef TCPConnectorBase          Self;
  typedef Object  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::TCPConnectorBase, igtl::Object)
  igtlNewMacro(igtl::TCPConnectorBase);

public:
  
  virtual const char * GetClassName() { return "Base"; };

  void           SetHostname(const char * str) {this->Hostname = str; this->ConfigurationUpdated = true; };
  const char *   GetHostname() { return this->Hostname.c_str(); };
  void           SetPort(int p) { this->Port = p; this->ConfigurationUpdated = true; };
  int            GetPort() { return this->Port; };

  virtual int    PushMessage(igtl::MessageBase * NOTUSED(message)) { return 0; };

  int            SetOutputConnector(igtl::TCPConnectorBase * con)
  {
    this->OutputConnector = con;
  }
  
  inline int     GetStatus() { return this->Status; };
  inline const char*  GetStatusString() { return this->StatusString[this->Status]; };

  inline void    Activate()    { this->Active = 1; };
  inline void    Deactivate()  { this->Active = 0; };
  inline void    CloseThread() { this->Active = -1; };

  static void    MonitorThreadFunction(void * ptr);

  // Description:
  // DataReceivedFlag and DataSentFlag are used to check if there are any received/
  // sent messages through the connector. Those flags are turned ON when the connector
  // receives/sents messages, and turned OFF when they are checked by using CheckDataReceived()/
  // CheckDataSent() functions. Those flags are used for data receive/sent indicators in a GUI.

  inline void SetDataReceivedFlag() { this->DataReceivedFlag = true; }
  inline void SetDataSentFlag()     { this->DataSentFlag = true; }
  inline bool CheckDataReceived()
  {
    bool r = this->DataReceivedFlag;
    this->DataReceivedFlag = false;
    return r;
  }
  inline bool CheckDataSent()
  {
    bool r = this->DataSentFlag;
    this->DataSentFlag = false;
    return r;
  }


protected:

  TCPConnectorBase();
  ~TCPConnectorBase();

  void           PrintSelf(std::ostream& os) const;

  // Description:
  // Initialization() is called just after starting the 
  virtual int    Initialize() { return 0; };
  virtual int    WaitForConnection() { return 0; };
  virtual int    ReceiveMessage() { return 0; };
  virtual int    CloseConnection() { return 0; };
  virtual int    Finalize() { return 0; };
  
protected:

  int            Active;  // 0: Deactive; 1: Active; -1: exiting
  int            Status;
  std::string    Hostname;
  int            Port;

  bool           ConfigurationUpdated;

  bool           DataReceivedFlag;
  bool           DataSentFlag;

  TCPConnectorBase::Pointer OutputConnector;

};

}

#endif //__THREAD_H

