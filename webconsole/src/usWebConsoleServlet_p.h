/*=============================================================================

  Library: CppMicroServices

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#ifndef USWEBCONSOLESERVLET_H
#define USWEBCONSOLESERVLET_H

#include "usHttpServlet.h"

#include <usServiceTracker.h>

US_BEGIN_NAMESPACE

class HttpServletRequest;
class HttpServletResponse;
class AbstractWebConsolePlugin;

class WebConsolePluginTracker : public ServiceTracker<HttpServlet>
{

public:

  WebConsolePluginTracker();

  void Open(ServletContext* servlet);

  AbstractWebConsolePlugin* GetPlugin(const std::string& label) const;

  std::string GetLabelMapJSON() const;

private:

  typedef ServiceTracker<HttpServlet> Superclass;

  void Open();

  HttpServlet* AddingService(const ServiceReferenceType& reference);

  void AddPlugin(const std::string& label, AbstractWebConsolePlugin* plugin);

  std::string GetProperty(const ServiceReferenceType& reference, const std::string& property) const;

  typedef std::map<std::string, AbstractWebConsolePlugin*> PluginMapType;
  PluginMapType m_Plugins;
  Any m_LabelMapAny;
  std::vector<Any>* m_Labels;

  ServletContext* m_ServletContext;
};

class WebConsoleServlet : public HttpServlet
{

public:
  WebConsoleServlet();

  void Init(const ServletConfig &config);

private:

  void Service(HttpServletRequest& request, HttpServletResponse& response);

  AbstractWebConsolePlugin* GetConsolePlugin(const std::string& label) const;

  WebConsolePluginTracker m_PluginTracker;
};

US_END_NAMESPACE

#endif // USWEBCONSOLESERVLET_H
