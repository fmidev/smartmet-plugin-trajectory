// ======================================================================
/*!
 * \brief Implementation of Config
 */
// ======================================================================

#include "Config.h"
#include <filesystem>
#include <macgyver/Exception.h>
#include <spine/ConfigTools.h>
#include <stdexcept>

using namespace std;

namespace SmartMet
{
namespace Plugin
{
namespace Trajectory
{
// ----------------------------------------------------------------------
/*!
 * \brief Constructor
 */
// ----------------------------------------------------------------------

Config::Config(const string& configfile)
{
  try
  {
    if (!configfile.empty())
    {
      // Enable sensible relative include paths
      std::filesystem::path p = configfile;
      p.remove_filename();
      itsConfig.setIncludeDir(p.c_str());

      itsConfig.readFile(configfile.c_str());
      Spine::expandVariables(itsConfig);

      itsConfig.lookupValue("templates", itsDefaultTemplateDirectory);
      itsConfig.lookupValue("url", itsDefaultUrl);
      itsConfig.lookupValue("timestep", itsDefaultTimeStep);
      itsConfig.lookupValue("length", itsDefaultSimulationLength);
      itsConfig.lookupValue("format", itsDefaultFormat);
      itsConfig.lookupValue("model", itsDefaultModel);
      itsConfig.lookupValue("plume.disturbance", itsDefaultPlumeDisturbance);
      itsConfig.lookupValue("pressure", itsDefaultPressure);
      itsConfig.lookupValue("plume.range", itsDefaultPlumePressureRange);
      itsConfig.lookupValue("plume.radius", itsDefaultPlumeRadius);
      itsConfig.lookupValue("plume.interval", itsDefaultPlumeInterval);
      itsConfig.lookupValue("isentropic", itsDefaultIsentropic);
      itsConfig.lookupValue("kml.tessellate", itsDefaultKmlTessellate);
      itsConfig.lookupValue("kml.extrude", itsDefaultKmlExtrude);
    }
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
/*
 * Accessors
 */
// ----------------------------------------------------------------------

const std::string& Config::defaultTemplateDirectory() const
{
  return itsDefaultTemplateDirectory;
}
const std::string& Config::defaultUrl() const
{
  return itsDefaultUrl;
}
const std::string& Config::defaultFormat() const
{
  return itsDefaultFormat;
}
const std::string& Config::defaultModel() const
{
  return itsDefaultModel;
}
unsigned int Config::defaultTimeStep() const
{
  return itsDefaultTimeStep;
}
unsigned int Config::defaultSimulationLength() const
{
  return itsDefaultSimulationLength;
}
double Config::defaultPlumeDisturbance() const
{
  return itsDefaultPlumeDisturbance;
}
double Config::defaultPressure() const
{
  return itsDefaultPressure;
}
double Config::defaultPlumeRadius() const
{
  return itsDefaultPlumeRadius;
}
double Config::defaultPlumePressureRange() const
{
  return itsDefaultPlumePressureRange;
}
unsigned int Config::defaultPlumeInterval() const
{
  return itsDefaultPlumeInterval;
}
bool Config::defaultIsentropic() const
{
  return itsDefaultIsentropic;
}
bool Config::defaultKmlTessellate() const
{
  return itsDefaultKmlTessellate;
}
bool Config::defaultKmlExtrude() const
{
  return itsDefaultKmlExtrude;
}

}  // namespace Trajectory
}  // namespace Plugin
}  // namespace SmartMet

// ======================================================================
