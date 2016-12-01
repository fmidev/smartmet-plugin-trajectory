// ======================================================================
/*!
 * \brief Configuration file API
 */
// ======================================================================

#pragma once

#include <libconfig.h++>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/utility.hpp>
#include <string>

namespace SmartMet
{
namespace Plugin
{
namespace Trajectory
{
class Config : private boost::noncopyable
{
 public:
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
  Config();

  libconfig::Config itsConfig;
  std::string itsDefaultTemplateDirectory;  // format templates
  std::string itsDefaultUrl;
  unsigned int itsDefaultTimeStep;          // in minutes
  unsigned int itsDefaultSimulationLength;  // simulation length in hours
  std::string itsDefaultFormat;             // kml, kmz, gpx etc
  std::string itsDefaultModel;              // hirlam_eurooppa_mallipinta, ...

  double itsDefaultPlumeDisturbance;     // disturbance percentage
  double itsDefaultPressure;             // hPa
  double itsDefaultPlumePressureRange;   // perturbation in hPa
  double itsDefaultPlumeRadius;          // perturbation in km
  unsigned int itsDefaultPlumeInterval;  // perturbation in minutes

  bool itsDefaultIsentropic;

  bool itsDefaultKmlTessellate;  // KML specific display options
  bool itsDefaultKmlExtrude;

};  // class Config

}  // namespace Trajectory
}  // namespace Plugin
}  // namespace SmartMet

// ======================================================================
