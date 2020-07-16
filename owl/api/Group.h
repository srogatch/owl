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

#include "RegisteredObject.h"
#include "Geometry.h"
// #include "ll/DeviceMemory.h"
// #include "ll/Device.h"

namespace owl {

  // using ll::DeviceMemory;
  
  /*! any sort of group */
  struct Group : public RegisteredObject {
    typedef std::shared_ptr<Group> SP;

    virtual int getSBTOffset() const = 0;
    
    /*! any device-specific data, such as optix handles, cuda device
        pointers, etc */
    struct DeviceData : public RegisteredObject::DeviceData {
      typedef std::shared_ptr<DeviceData> SP;

      DeviceData(const DeviceContext::SP &device)
        : RegisteredObject::DeviceData(device)
      {};
      virtual ~DeviceData() {}
      
      template<typename T>
      inline T *as() { return dynamic_cast<T*>(this); }
      
      OptixTraversableHandle traversable = 0;
      DeviceMemory           bvhMemory;
    };

    Group(Context *const context,
          ObjectRegistry &registry)
      : RegisteredObject(context,registry)
    {}
    virtual ~Group() {}
    virtual std::string toString() const { return "Group"; }
    virtual void buildAccel() = 0;
    virtual void refitAccel() = 0;
    
    DeviceData &getDD(const DeviceContext::SP &device) const
    {
      assert(device->ID < deviceData.size());
      return *deviceData[device->ID]->as<DeviceData>();
    }

    /*! creates the device-specific data for this group */
    RegisteredObject::DeviceData::SP createOn(const DeviceContext::SP &device) override
    { return std::make_shared<DeviceData>(device); }
    
    OptixTraversableHandle getTraversable(const DeviceContext::SP &device) const
    { return getDD(device).traversable; }
    
    /*! bounding box for t=0 and t=1, respectively; for motion
        blur. */
    box3f bounds[2];
  };

  /*! a group containing geometries */
  struct GeomGroup : public Group {
    typedef std::shared_ptr<GeomGroup> SP;

    
    
    GeomGroup(Context *const context,
              size_t numChildren);
    virtual ~GeomGroup();
    
    void setChild(int childID, Geom::SP child);
    
    int getSBTOffset() const override { return sbtOffset; }

    virtual std::string toString() const { return "GeomGroup"; }
    std::vector<Geom::SP> geometries;
    const int sbtOffset;
  };

} // ::owl
