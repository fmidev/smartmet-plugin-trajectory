// ======================================================================
/*!
 * \brief Configuration file API
 */
// ======================================================================

#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>
#include <libconfig.h++>
#include <string>

namespace SmartMet
{
namespace Plugin
{
namespace Trajectory
{
class Config
{
 public:
  Config() = delete;
  Config(const Config& other) = delete;
  Config& operator=(const Config& other) = delete;
  Config(const std::string& configfile);

  const std::string& defaultTemplateDirectory() const;
  const std::string& defaultUrl() const;
  const std::string& defaultFormat() const;
  const std::string& defaultModel() const;
  unsigned int defaultTimeStep() const;
  unsigned int defaultSimulationLength() const;
  double defaultPlumeDisturbance() const;
  double defaultPressure() const;
  double defaultPlumeRadius() const;
  double defaultPlumePressureRange() const;
  unsigned int defaultPlumeInterval() const;
  bool defaultIsentropic() const;
  bool defaultKmlTessellate() const;
  bool defaultKmlExtrude() const;

 private:
  libconfig::Config itsConfig;
  std::string itsDefaultTemplateDirectory = "/usr/share/smartmet/trajectories";  // format templates
  std::string itsDefaultUrl = "/trajectory";
  unsigned int itsDefaultTimeStep = 10;          // in minutes
  unsigned int itsDefaultSimulationLength = 24;  // simulation length in hours
  std::string itsDefaultFormat = "kml";          // kml, kmz, gpx etc
  std::string itsDefaultModel = "ecmwf_eurooppa_mallipinta";

  double itsDefaultPlumeDisturbance = 25;    // disturbance percentage
  double itsDefaultPressure = 850;           // hPa
  double itsDefaultPlumePressureRange = 0;   // perturbation in hPa
  double itsDefaultPlumeRadius = 0;          // perturbation in km
  unsigned int itsDefaultPlumeInterval = 0;  // perturbation in minutes

  bool itsDefaultIsentropic = false;

  bool itsDefaultKmlTessellate = false;  // KML specific display options
  bool itsDefaultKmlExtrude = false;

};  // class Config

}  // namespace Trajectory
}  // namespace Plugin
}  // namespace SmartMet

// ======================================================================
