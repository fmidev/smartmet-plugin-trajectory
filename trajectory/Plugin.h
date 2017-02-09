// ======================================================================
/*!
 * \brief SmartMet Trajectory plugin interface
 */
// ======================================================================

#pragma once

#include "Config.h"

#include <spine/SmartMetPlugin.h>
#include <spine/HTTP.h>
#include <spine/Reactor.h>

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
}

namespace Plugin
{
namespace Trajectory
{
class PluginImpl;

class Plugin : public SmartMetPlugin, private boost::noncopyable
{
 public:
  Plugin(SmartMet::Spine::Reactor* theReactor, const char* theConfig);
  virtual ~Plugin();

  const std::string& getPluginName() const;
  int getRequiredAPIVersion() const;

 protected:
  void init();
  void shutdown();
  void requestHandler(SmartMet::Spine::Reactor& theReactor,
                      const SmartMet::Spine::HTTP::Request& theRequest,
                      SmartMet::Spine::HTTP::Response& theResponse);

 private:
  Plugin();
  std::string query(SmartMet::Spine::Reactor& theReactor,
                    const SmartMet::Spine::HTTP::Request& req,
                    SmartMet::Spine::HTTP::Response& response);

  const std::string itsModuleName;
  Config itsConfig;

  SmartMet::Spine::Reactor* itsReactor;
  SmartMet::Engine::Querydata::Engine* itsQEngine;
  SmartMet::Engine::Geonames::Engine* itsGeoEngine;

};  // class Plugin

}  // namespace Trajectory
}  // namespace Plugin
}  // namespace SmartMet

// ======================================================================
