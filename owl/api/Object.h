// ======================================================================== //
// Copyright 2019-2020 Ingo Wald                                            //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "owl/owl.h"
#include "owl/ll/common.h"
#include "owl/ll/DeviceGroup.h"
#include "owl/ll/Device.h"
#include "owl/ll/DeviceMemory.h"

namespace owl {

  using ll::Device;
  using ll::DeviceMemory;
  
  std::string typeToString(OWLDataType type);
  size_t      sizeOf(OWLDataType type);

  struct Context;

  /*! common "root" abstraction for every object this library creates */
  struct Object : public std::enable_shared_from_this<Object> {
    typedef std::shared_ptr<Object> SP;

    /*! any device-specific data, such as optix handles, cuda device
        pointers, etc */
    struct DeviceData {
      typedef std::shared_ptr<DeviceData> SP;

      virtual ~DeviceData() {}
      
      template<typename T>
      inline T *as() { return dynamic_cast<T*>(this); }
    };

    Object();

    /*! pretty-printer, for debugging */
    virtual std::string toString() const { return "Object"; }

    /*! creates the device-specific data for this group */
    virtual DeviceData::SP createOn(ll::Device *device)
    { return std::make_shared<DeviceData>(); }

    void createDeviceData(const std::vector<ll::Device *> &devices)
    {
      assert(deviceData.empty());
      for (auto device : devices)
        deviceData.push_back(createOn(device));
    }

    template<typename T>
    inline std::shared_ptr<T> as() 
    { return std::dynamic_pointer_cast<T>(shared_from_this()); }

    /*! a unique ID we assign to each newly created object - this
        allows any caching algorithms to check if a given object was
        replaced with another, even if in some internal array it ends
        up with the same array index as a previous other object */
    const size_t uniqueID;

    static std::atomic<uint64_t> nextAvailableID;

    std::vector<DeviceData::SP> deviceData;
  };

  
  /*! a object that belongs to a context */
  struct ContextObject : public Object {
    typedef std::shared_ptr<ContextObject> SP;
    
    ContextObject(Context *const context)
      : context(context)
    {}
    
    virtual std::string toString() const { return "ContextObject"; }
    
    Context *const context;
  };

} // ::owl

