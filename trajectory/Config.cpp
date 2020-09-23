// ======================================================================
/*!
 * \brief Implementation of Config
 */
// ======================================================================

#include "Config.h"
#include <macgyver/Exception.h>
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
    : itsConfig(),
      itsDefaultTemplateDirectory("/usr/share/smartmet/trajectories"),
      itsDefaultUrl("/trajectory"),
      itsDefaultTimeStep(10),
      itsDefaultSimulationLength(24),
      itsDefaultFormat("kml"),
      itsDefaultModel("hirlam_eurooppa_mallipinta"),
      itsDefaultPlumeDisturbance(25),
      itsDefaultPressure(850),
      itsDefaultPlumePressureRange(0),
      itsDefaultPlumeRadius(0),
      itsDefaultPlumeInterval(0),
      itsDefaultIsentropic(false),
      itsDefaultKmlTessellate(false),
      itsDefaultKmlExtrude(false)
{
  try
  {
    if (!configfile.empty())
    {
      itsConfig.readFile(configfile.c_str());

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
    throw Fmi::Exception(BCP, "Operation failed!", NULL);
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
