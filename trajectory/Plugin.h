// ======================================================================
/*!
 * \brief SmartMet Trajectory plugin interface
 */
// ======================================================================

#pragma once

#include "Config.h"

#include <macgyver/TemplateFactory.h>
#include <spine/HTTP.h>
#include <spine/Reactor.h>
#include <spine/SmartMetPlugin.h>

namespace SmartMet
{
namespace Engine
{
namespace Querydata
{
class Engine;
}
namespace Geonames
{
class Engine;
}
}  // namespace Engine

namespace Plugin
{
namespace Trajectory
{
class PluginImpl;

class Plugin : public SmartMetPlugin
{
 public:
  Plugin() = delete;
  Plugin(const Plugin& other) = delete;
  Plugin& operator=(const Plugin& other) = delete;
  Plugin(SmartMet::Spine::Reactor* theReactor, const char* theConfig);

  const std::string& getPluginName() const override;
  int getRequiredAPIVersion() const override;

 protected:
  void init() override;
  void shutdown() override;
  void requestHandler(SmartMet::Spine::Reactor& theReactor,
                      const SmartMet::Spine::HTTP::Request& theRequest,
                      SmartMet::Spine::HTTP::Response& theResponse) override;

 private:
  std::string query(SmartMet::Spine::Reactor& theReactor,
                    const SmartMet::Spine::HTTP::Request& theRequest,
                    SmartMet::Spine::HTTP::Response& theResponse);

  const std::string itsModuleName;
  Config itsConfig;
  Fmi::TemplateFactory itsTemplateFactory;

  SmartMet::Spine::Reactor* itsReactor;
  SmartMet::Engine::Querydata::Engine* itsQEngine;
  SmartMet::Engine::Geonames::Engine* itsGeoEngine;

};  // class Plugin

}  // namespace Trajectory
}  // namespace Plugin
}  // namespace SmartMet

// ======================================================================
