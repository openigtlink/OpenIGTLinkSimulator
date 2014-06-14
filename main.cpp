/*=========================================================================

  Program:   OpenIGTLink Simulator
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <QApplication>
 
#include "OpenIGTLinkSimulatorApp.h"
 
int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  OpenIGTLinkSimulatorApp *window = new OpenIGTLinkSimulatorApp;
  
  window->show();
  return app.exec();
}


