/*=============================================================================

  Library: CppMicroServices

  Copyright (c) The CppMicroServices developers. See the COPYRIGHT
  file at the top-level directory of this distribution and at
  https://github.com/saschazelzer/CppMicroServices/COPYRIGHT .

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


#ifndef USSERVICELISTENERS_H
#define USSERVICELISTENERS_H

#include <list>
#include <string>
#include <set>

#include <usGlobalConfig.h>

#include "usServiceListenerEntry_p.h"
#include "usThreads_p.h"

#include <unordered_map>
#include <unordered_set>

namespace us {

class CoreBundleContext;
class BundleContext;

/**
 * Here we handle all listeners that bundles have registered.
 *
 */
class ServiceListeners : private MultiThreaded<>
{

public:

  typedef std::unordered_map<BundleContext*, std::list<std::pair<BundleListener,void*> > > BundleListenerMap;
  struct : public MultiThreaded<> {
    BundleListenerMap value;
  } bundleListenerMap;

  typedef std::unordered_map<std::string, std::list<ServiceListenerEntry> > CacheType;
  typedef std::unordered_set<ServiceListenerEntry> ServiceListenerEntries;

private:

  std::vector<std::string> hashedServiceKeys;
  static const int OBJECTCLASS_IX; // = 0;
  static const int SERVICE_ID_IX; // = 1;

  /* Service listeners with complicated or empty filters */
  std::list<ServiceListenerEntry> complicatedListeners;

  /* Service listeners with "simple" filters are cached. */
  CacheType cache[2];

  ServiceListenerEntries serviceSet;

  CoreBundleContext* coreCtx;

public:

  ServiceListeners(CoreBundleContext* coreCtx);

  /**
   * Add a new service listener. If an old one exists, and it has the
   * same owning bundle, the old listener is removed first.
   *
   * @param mc The bundle context adding this listener.
   * @param listener The service listener to add.
   * @param data Additional data to distinguish ServiceListener objects.
   * @param filter An LDAP filter string to check when a service is modified.
   * @exception org.osgi.framework.InvalidSyntaxException
   * If the filter is not a correct LDAP expression.
   */
  void AddServiceListener(BundleContext* mc, const ServiceListener& listener,
                          void* data, const std::string& filter);

  /**
   * Remove service listener from current framework. Silently ignore
   * if listener doesn't exist. If listener is registered more than
   * once remove all instances.
   *
   * @param mc The bundle context who wants to remove listener.
   * @param listener Object to remove.
   * @param data Additional data to distinguish ServiceListener objects.
   */
  void RemoveServiceListener(BundleContext* mc, const ServiceListener& listener,
                             void* data);

  /**
   * Add a new bundle listener.
   *
   * @param mc The bundle context adding this listener.
   * @param listener The bundle listener to add.
   * @param data Additional data to distinguish BundleListener objects.
   */
  void AddBundleListener(BundleContext* mc, const BundleListener& listener, void* data);

  /**
   * Remove bundle listener from current framework. Silently ignore
   * if listener doesn't exist.
   *
   * @param mc The bundle context who wants to remove listener.
   * @param listener Object to remove.
   * @param data Additional data to distinguish BundleListener objects.
   */
  void RemoveBundleListener(BundleContext* mc, const BundleListener& listener, void* data);

  void BundleChanged(const BundleEvent& evt);

  /**
   * Remove all listener registered by a bundle in the current framework.
   *
   * @param mc Bundle context which listeners we want to remove.
   */
  void RemoveAllListeners(BundleContext* mc);

  /**
   * Notify hooks that a bundle is about to be stopped
   *
   * @param mc Bundle context which listeners are about to be removed.
   */
  void HooksBundleStopped(BundleContext* mc);

  /**
   * Receive notification that a service has had a change occur in its lifecycle.
   *
   * @see org.osgi.framework.ServiceListener#serviceChanged
   */
  void ServiceChanged(ServiceListenerEntries& receivers,
                      const ServiceEvent& evt,
                      ServiceListenerEntries& matchBefore);

  void ServiceChanged(ServiceListenerEntries& receivers,
                      const ServiceEvent& evt);

  /**
   *
   *
   */
  void GetMatchingServiceListeners(const ServiceEvent& evt, ServiceListenerEntries& listeners,
                                   bool lockProps = true);


  std::vector<ServiceListenerHook::ListenerInfo> GetListenerInfoCollection() const;

private:

  void RemoveServiceListener_unlocked(const ServiceListenerEntry& entryToRemove);

  /**
   * Remove all references to a service listener from the service listener
   * cache.
   */
  void RemoveFromCache(const ServiceListenerEntry& sle);

  /**
   * Checks if the specified service listener's filter is simple enough
   * to cache.
   */
  void CheckSimple(const ServiceListenerEntry& sle);

  void AddToSet(ServiceListenerEntries& set, const ServiceListenerEntries& receivers, int cache_ix, const std::string& val);

};

}

#endif // USSERVICELISTENERS_H