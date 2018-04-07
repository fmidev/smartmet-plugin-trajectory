// ======================================================================
/*!
 * \brief SmartMet Trajectory plugin implementation
 */
// ======================================================================

#include "Plugin.h"

#include <engines/geonames/Engine.h>
#include <engines/querydata/Engine.h>
#include <spine/Convenience.h>
#include <spine/Exception.h>
#include <spine/SmartMet.h>

#include <trajectory/NFmiTrajectory.h>
#include <trajectory/NFmiTrajectorySystem.h>

#include <newbase/NFmiFastQueryInfo.h>
#include <newbase/NFmiQueryData.h>

#include <macgyver/AnsiEscapeCodes.h>
#include <macgyver/TemplateFormatterMT.h>
#include <macgyver/TimeFormatter.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/foreach.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>

#include <stdexcept>

namespace SmartMet
{
namespace Plugin
{
namespace Trajectory
{
// ----------------------------------------------------------------------
/*!
 * \brief Parse the start time of the simulation
 *
 * TODO: Almost duplicate from qdtrajectory.cpp
 */
// ----------------------------------------------------------------------

boost::posix_time::ptime parse_starttime(const std::string &theStr, unsigned long theTimeStep)
{
  try
  {
    boost::posix_time::ptime p;

    if (theStr == "now")
      p = Fmi::TimeParser::parse("0");
    else if (theStr != "origintime")
      p = Fmi::TimeParser::parse(theStr);

    // Round up to nearest timestep
    if (!p.is_not_a_date_time() && theTimeStep > 1)
    {
      unsigned long offset =
          boost::numeric_cast<unsigned long>(p.time_of_day().total_seconds()) % (60 * theTimeStep);
      if (offset > 0)
        p += boost::posix_time::seconds(boost::numeric_cast<long>(60 * theTimeStep - offset));
    }

    return p;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Hash a single trajector
 */
// ----------------------------------------------------------------------

void hash_trajector(CTPP::CDT &hash,
                    uint32_t index,
                    const NFmiSingleTrajector &trajector,
                    unsigned int theTimeStep,
                    bool backwards)
{
  try
  {
    static Fmi::TimeFormatter *isoformatter = Fmi::TimeFormatter::create("iso");
    static Fmi::TimeFormatter *epochformatter = Fmi::TimeFormatter::create("epoch");
    static Fmi::TimeFormatter *stampformatter = Fmi::TimeFormatter::create("timestamp");
    static Fmi::TimeFormatter *sqlformatter = Fmi::TimeFormatter::create("sql");
    static Fmi::TimeFormatter *xmlformatter = Fmi::TimeFormatter::create("xml");

    // Data being printed

    const auto &points = trajector.Points();
    const auto &pressures = trajector.Pressures();
    const auto &heights = trajector.HeightValues();

    // For iteration
    long timestep = (backwards ? -theTimeStep : theTimeStep);

    // Common information

    hash["index"] = index;

    if (index == 0)
      hash["name"] = "Main trajectory";
    else
      hash["name"] = "Plume " + boost::lexical_cast<std::string>(index);

    // Start time

    NFmiMetTime t(trajector.StartTime());
    t.SetTimeStep(1);

    for (uint32_t i = 0; i < points.size(); i++)
    {
      CTPP::CDT &group = hash["points"][i];

      boost::posix_time::ptime pt = t;

      // Track start and end times
      if (i == 0)
        hash["starttime"] = xmlformatter->format(pt);
      else if (i == points.size() - 1)
        hash["endtime"] = xmlformatter->format(pt);

      group["timestamp"] = stampformatter->format(pt);
      group["epoch"] = epochformatter->format(pt);
      group["isotime"] = isoformatter->format(pt);
      group["sqltime"] = sqlformatter->format(pt);
      group["xmltime"] = xmlformatter->format(pt);

      group["longitude"] = round(10000 * points[i].X()) / 10000;
      group["latitude"] = round(10000 * points[i].Y()) / 10000;
      group["pressure"] = round(10 * pressures[i]) / 10;
      group["height"] = round(10 * heights[i]) / 10;

      t.ChangeByMinutes(timestep);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Easier representation for direction
 */
// ----------------------------------------------------------------------

int pretty_direction(FmiDirection dir)
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch-enum"

  try
  {
    // This newbase enum is hideous
    switch (dir)
    {
      case kForward:
        return 1;
      case kBackward:
        return -1;
      default:
        throw SmartMet::Spine::Exception(BCP, "Invalid trajectory direction encountered");
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
#pragma clang diagnostic pop
}

// ----------------------------------------------------------------------
/*!
 * \brief Build CTTP hash of the results
 *
 * We assume the input hash is clear.
 *
 * General structure:
 *
 *  direction
 *  producer
 *  trajectory
 *  list<plumes>
 *
 * Trajectory contains points only:
 *
 *  time
 *  longitude
 *  latitude
 *  pressure
 *  height
 */
// ----------------------------------------------------------------------

void build_hash(CTPP::CDT &hash,
                const NFmiTrajectory &trajectory,
                bool tessellate,
                bool extrude,
                unsigned int plumecount,
                unsigned int timestep,
                bool backwards)

{
  try
  {
    hash["direction"] = pretty_direction(trajectory.Direction());
    hash["producer"] = trajectory.Producer().GetName().CharPtr();
    hash["pressure"] = trajectory.PressureLevel();
    hash["longitude"] = trajectory.LatLon().X();
    hash["latitude"] = trajectory.LatLon().Y();

    hash["kml"]["tessellate"] = tessellate ? 1 : 0;
    hash["kml"]["extrude"] = extrude ? 1 : 0;

    if (plumecount > 0)
    {
      hash["plume"]["radius"] = trajectory.StartLocationRangeInKM();
      hash["plume"]["disturbance"] = trajectory.PlumeProbFactor();
      hash["plume"]["interval"] = trajectory.StartTimeRangeInMinutes();
      hash["plume"]["range"] = trajectory.StartPressureLevelRange();
    }

    CTPP::CDT &tgroup = hash["trajectories"];

    uint32_t index = 0;
    hash_trajector(tgroup[index], index, trajectory.MainTrajector(), timestep, backwards);

    if (trajectory.PlumesUsed())
    {
      const auto &plumes = trajectory.PlumeTrajectories();
      BOOST_FOREACH (const auto &traj, plumes)
      {
        ++index;
        hash_trajector(tgroup[index], index, *traj, timestep, backwards);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Establish template file
 */
// ----------------------------------------------------------------------

std::string template_filename(const std::string &theFormat, const Config &theConfig)
{
  try
  {
    return (theConfig.defaultTemplateDirectory() + "/" + theFormat + ".c2t");
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Format the result
 */
// ----------------------------------------------------------------------

std::string format_result(boost::shared_ptr<NFmiTrajectory> trajectory,
                          const Config &theConfig,
                          const std::string &theFormat,
                          bool tessellate,
                          bool extrude,
                          unsigned int plumecount,
                          unsigned int timestep,
                          bool backwards)
{
  try
  {
    if (theFormat == "debug")
      return trajectory->ToXMLStr();

    // Get the output template

    std::string tmpl = template_filename(theFormat, theConfig);

    if (!boost::filesystem::exists(tmpl))
      throw SmartMet::Spine::Exception(BCP, "Unknown format '" + theFormat + "'");

    // Build the hash

    CTPP::CDT hash;
    build_hash(hash, *trajectory, tessellate, extrude, plumecount, timestep, backwards);

    // Format data using the template

    std::ostringstream output;
    std::ostringstream log;
    try
    {
      Fmi::TemplateFormatterMT formatter(tmpl);
      formatter.get()->process(hash, output, log);
    }
    catch (...)
    {
      throw SmartMet::Spine::Exception(
          BCP, "Failed to process the trajectories for output: " + log.str());
    }

    return output.str();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Mime-type for the requested data format
 */
// ----------------------------------------------------------------------

std::string mime_type(const Config &theConfig, const SmartMet::Spine::HTTP::Request &theRequest)
{
  try
  {
    std::string format = SmartMet::Spine::optional_string(theRequest.getParameter("format"),
                                                          theConfig.defaultFormat());

    if (format == "kml" || format == "kmlx")
      return "application/vnd.google-earth.kml+xml";
    if (format == "kmz" || format == "kmzx")
      return "application/vnd.google-earth.kmz";
    if (format == "gpx")
      return "application/gpx+xml";
    if (format == "xml")
      return "application/xml";
    if (format == "debug")
      return "application/xml";

    // Unknown format - return best guess

    return "application/xml";
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Is the format compressed?
 */
// ----------------------------------------------------------------------

bool is_compressed_format(const std::string &theFormat)
{
  try
  {
    return (theFormat == "kmz" || theFormat == "kmzx");
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Find maximum value at a location
 */
// ----------------------------------------------------------------------

float maximum_value_vertically(NFmiFastQueryInfo &theQ,
                               const NFmiPoint &thePoint,
                               const NFmiMetTime &theTime)
{
  try
  {
    float pmax = kFloatMissing;
    for (theQ.ResetLevel(); theQ.NextLevel();)
    {
      float value = theQ.InterpolatedValue(thePoint, theTime);
      if (pmax == kFloatMissing)
        pmax = value;
      else if (value != kFloatMissing)
        pmax = std::max(pmax, value);
    }
    return pmax;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Perform a Trajectory query
 */
// ----------------------------------------------------------------------

std::string Trajectory::Plugin::query(SmartMet::Spine::Reactor & /* theReactor */,
                                      const SmartMet::Spine::HTTP::Request &theRequest,
                                      SmartMet::Spine::HTTP::Response & /* theResponse */)
{
  try
  {
    // Parse generic location options

    SmartMet::Engine::Geonames::LocationOptions loptions(itsGeoEngine->parseLocations(theRequest));

    if (loptions.locations().empty())
      throw SmartMet::Spine::Exception(BCP, "No location selected for the simulation");

    if (loptions.locations().size() > 1)
      throw SmartMet::Spine::Exception(BCP, "Currently only one simulation location is supported");

    // Plugin specific settings

    auto producer = SmartMet::Spine::optional_string(theRequest.getParameter("model"),
                                                     itsConfig.defaultModel());

    std::string format = SmartMet::Spine::optional_string(theRequest.getParameter("format"),
                                                          itsConfig.defaultFormat());

    unsigned long timestep = SmartMet::Spine::optional_unsigned_long(
        theRequest.getParameter("timestep"), itsConfig.defaultTimeStep());

    unsigned long length = SmartMet::Spine::optional_unsigned_long(
        theRequest.getParameter("length"), itsConfig.defaultSimulationLength());

    unsigned long plumecount =
        SmartMet::Spine::optional_unsigned_long(theRequest.getParameter("plumes"), 0);

    double plumedisturbance = SmartMet::Spine::optional_double(
        theRequest.getParameter("disturbance"), itsConfig.defaultPlumeDisturbance());

    double pressure = SmartMet::Spine::optional_double(theRequest.getParameter("pressure"),
                                                       itsConfig.defaultPressure());

    double plumeradius = SmartMet::Spine::optional_double(theRequest.getParameter("radius"),
                                                          itsConfig.defaultPlumeRadius());

    double plumerange = SmartMet::Spine::optional_double(theRequest.getParameter("range"),
                                                         itsConfig.defaultPlumePressureRange());

    unsigned int plumeinterval = SmartMet::Spine::optional_unsigned_long(
        theRequest.getParameter("interval"), itsConfig.defaultPlumeInterval());
    bool isentropic = SmartMet::Spine::optional_bool(theRequest.getParameter("isentropic"),
                                                     itsConfig.defaultIsentropic());

    bool tessellate = SmartMet::Spine::optional_bool(theRequest.getParameter("tessellate"),
                                                     itsConfig.defaultKmlTessellate());

    bool extrude = SmartMet::Spine::optional_bool(theRequest.getParameter("extrude"),
                                                  itsConfig.defaultKmlExtrude());

    bool backwards = SmartMet::Spine::optional_bool(theRequest.getParameter("backwards"), false);

    // Start time parsing is a bit special

    boost::posix_time::ptime starttime = parse_starttime(
        SmartMet::Spine::optional_string(theRequest.getParameter("starttime"), "now"), timestep);

    // Get the data to run the model with

    auto q = itsQEngine->get(producer);

    // Verify level type

    auto leveltype = q->levelType();
    if (leveltype != kFmiHybridLevel && leveltype != kFmiPressureLevel)
      throw SmartMet::Spine::Exception(
          BCP, "Selected model does not contain hybrid or pressure level data");

    // Calculate the trajectories

    auto trajectory = boost::make_shared<NFmiTrajectory>();

    // Copy producer information
    trajectory->Producer(*q->info()->Producer());

    // Time settings

    trajectory->TimeStepInMinutes(boost::numeric_cast<int>(timestep));
    trajectory->TimeLengthInHours(boost::numeric_cast<int>(length));

    // Plume initial dispersion radius, start time interval, and pressure disribution
    trajectory->StartLocationRangeInKM(plumeradius);
    trajectory->StartTimeRangeInMinutes(boost::numeric_cast<int>(plumeinterval));
    trajectory->StartPressureLevelRange(plumerange);

    // Follow potential temperature
    trajectory->Isentropic(isentropic);

    // Forward or backward in time
    trajectory->Direction(backwards ? kBackward : kForward);

    // Point of interest
    const auto &loc = *loptions.locations().front().loc;
    trajectory->LatLon(NFmiPoint(loc.longitude, loc.latitude));

    // Start time is given or is the origin time
    if (starttime.is_not_a_date_time())
      trajectory->Time(q->originTime());
    else
      trajectory->Time(starttime);

    // Plume mode, disturbance factor as a percentage, plume size
    trajectory->PlumesUsed(plumecount > 0);
    trajectory->PlumeParticleCount(boost::numeric_cast<int>(plumecount));
    trajectory->PlumeProbFactor(plumedisturbance);
    trajectory->PressureLevel(pressure);

    // Handle height input

    double height = SmartMet::Spine::optional_double(theRequest.getParameter("height"), -1);

    if (height > 0)
    {
      double heightrange =
          SmartMet::Spine::optional_double(theRequest.getParameter("heightrange"), -1);

      auto info = q->info();
      if (!info->Param(kFmiPressure))
        throw SmartMet::Spine::Exception(BCP, "Forecast data missing parameter Pressure");

      // maximum pressure = lowest level value, but we do not know the order of the levels
      float pmax = maximum_value_vertically(*info, trajectory->LatLon(), trajectory->Time());

      if (heightrange <= 0)
      {
        double p = info->HeightValue(height, trajectory->LatLon(), trajectory->Time());
        if (p == kFloatMissing)
          trajectory->PressureLevel(pmax);
        else
          trajectory->PressureLevel(p);
      }
      else
      {
        float dz = heightrange / 2;
        float p1 = info->HeightValue(height - dz, trajectory->LatLon(), trajectory->Time());
        float p2 = info->HeightValue(height + dz, trajectory->LatLon(), trajectory->Time());

        if (p2 == kFloatMissing)
          p2 = pmax;
        if (p1 == kFloatMissing)
          p1 = pmax;

        trajectory->PressureLevel((p1 + p2) / 2);
        trajectory->StartPressureLevelRange((p1 - p2) / 2);
      }
    }

    // Calculate the trajectories
    auto qi = q->info();
    NFmiTrajectorySystem::CalculateTrajectory(trajectory, qi);

    // Convert the results to a string

    std::string result = format_result(
        trajectory, itsConfig, format, tessellate, extrude, plumecount, timestep, backwards);

    // Compress if so requested

    if (!is_compressed_format(format))
      return result;

    boost::iostreams::filtering_stream<boost::iostreams::output> filter;
    std::ostringstream out;
    filter.push(boost::iostreams::gzip_compressor());
    filter.push(out);
    filter << result;
    filter.flush();
    return out.str();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Main content handler
 */
// ----------------------------------------------------------------------

void Plugin::requestHandler(SmartMet::Spine::Reactor &theReactor,
                            const SmartMet::Spine::HTTP::Request &theRequest,
                            SmartMet::Spine::HTTP::Response &theResponse)
{
  try
  {
    using boost::posix_time::ptime;

    bool isdebug =
        ("debug" == SmartMet::Spine::optional_string(theRequest.getParameter("format"), ""));

    // Default expiration time

    const int expires_seconds = 60;

    // Now

    ptime t_now = boost::posix_time::second_clock::universal_time();

    try
    {
      std::string response = query(theReactor, theRequest, theResponse);
      theResponse.setStatus(SmartMet::Spine::HTTP::Status::ok);
      theResponse.setContent(response);

      // Build cache expiration time info

      ptime t_expires = t_now + boost::posix_time::seconds(expires_seconds);

      // The headers themselves

      boost::shared_ptr<Fmi::TimeFormatter> tformat(Fmi::TimeFormatter::create("http"));

      std::string cachecontrol =
          "public, max-age=" + boost::lexical_cast<std::string>(expires_seconds);
      std::string expiration = tformat->format(t_expires);
      std::string modification = tformat->format(t_now);

      theResponse.setHeader("Cache-Control", cachecontrol);
      theResponse.setHeader("Expires", expiration);
      theResponse.setHeader("Last-Modified", modification);

      theResponse.setHeader("Content-type", mime_type(itsConfig, theRequest) + "; charset=UTF-8");

      if (response.size() == 0)
      {
        std::cerr << "Warning: Empty input for request " << theRequest.getQueryString() << " from "
                  << theRequest.getClientIP() << std::endl;
      }

#ifdef MYDEBUG
      std::cout << "Output:" << std::endl << response << std::endl;
#endif
    }
    catch (std::exception &e)
    {
      std::cerr << boost::posix_time::second_clock::local_time() << " error: " << e.what()
                << std::endl
                << "Query: " << theRequest.getURI() << std::endl;

      if (isdebug)
      {
        std::string msg = std::string("Error: ") + e.what();
        theResponse.setContent(msg);
        theResponse.setStatus(SmartMet::Spine::HTTP::Status::ok);
      }
      else
      {
        theResponse.setStatus(SmartMet::Spine::HTTP::Status::service_unavailable);
      }
      // Remove newlines, make sure length is reasonable
      std::string msg = e.what();
      boost::algorithm::replace_all(msg, "\n", " ");
      msg = msg.substr(0, 100);
      theResponse.setHeader("X-Trajectory-Error", msg.c_str());
    }
    catch (...)
    {
      std::cerr << boost::posix_time::second_clock::local_time() << " error: "
                << "Unknown exception" << std::endl
                << "Query: " << theRequest.getURI() << std::endl;

      theResponse.setHeader("X-Trajectory-Error", "Unknown exception");
      if (isdebug)
      {
        std::string msg = "Error: Unknown exception";
        theResponse.setContent(msg);
        theResponse.setStatus(SmartMet::Spine::HTTP::Status::no_content);
      }
      else
      {
        theResponse.setStatus(SmartMet::Spine::HTTP::Status::service_unavailable);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Plugin constructor
 */
// ----------------------------------------------------------------------

Plugin::Plugin(SmartMet::Spine::Reactor *theReactor, const char *theConfig)
    : SmartMetPlugin(), itsModuleName("Trajectory"), itsConfig(theConfig), itsReactor(theReactor)
{
  try
  {
    if (theReactor->getRequiredAPIVersion() != SMARTMET_API_VERSION)
    {
      std::cerr << ANSI_BOLD_ON << ANSI_FG_RED
                << "*** Trajectory Plugin and Server SmartMet API version mismatch ***"
                << ANSI_FG_DEFAULT << ANSI_BOLD_OFF << std::endl;
      return;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Initialize in a separate thread due to PostGIS slowness
 */
// ----------------------------------------------------------------------

void Plugin::init()
{
  try
  {
    /* QEngine */

    auto engine = itsReactor->getSingleton("Querydata", NULL);
    if (!engine)
      throw SmartMet::Spine::Exception(BCP, "Querydata engine unavailable");

    itsQEngine = reinterpret_cast<SmartMet::Engine::Querydata::Engine *>(engine);

    /* GeoEngine */

    engine = itsReactor->getSingleton("Geonames", NULL);
    if (!engine)
      throw SmartMet::Spine::Exception(BCP, "Geonames engine unavailable");

    itsGeoEngine = reinterpret_cast<SmartMet::Engine::Geonames::Engine *>(engine);

    /* Register handler */

    if (!itsReactor->addContentHandler(this,
                                       itsConfig.defaultUrl(),
                                       boost::bind(&Plugin::callRequestHandler, this, _1, _2, _3)))
      throw SmartMet::Spine::Exception(BCP, "Failed to register trajectory content handler");
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Shutdown the plugin
 */
// ----------------------------------------------------------------------

void Plugin::shutdown()
{
  std::cout << "  -- Shutdown requested (trajectory)\n";
}
// ----------------------------------------------------------------------
/*!
 * \brief Destructor
 */
// ----------------------------------------------------------------------

Plugin::~Plugin() {}
// ----------------------------------------------------------------------
/*!
 * \brief Return the plugin name
 */
// ----------------------------------------------------------------------

const std::string &Plugin::getPluginName() const
{
  return itsModuleName;
}
// ----------------------------------------------------------------------
/*!
 * \brief Return the required version
 */
// ----------------------------------------------------------------------

int Plugin::getRequiredAPIVersion() const
{
  return SMARTMET_API_VERSION;
}

}  // namespace Trajectory
}  // namespace Plugin
}  // namespace SmartMet

/*
 * Server knows us through the 'SmartMetPlugin' virtual interface, which
 * the 'Plugin' class implements.
 */

extern "C" SmartMetPlugin *create(SmartMet::Spine::Reactor *them, const char *config)
{
  return new SmartMet::Plugin::Trajectory::Plugin(them, config);
}

extern "C" void destroy(SmartMetPlugin *us)
{
  // This will call 'Plugin::~Plugin()' since the destructor is virtual
  delete us;
}

// ======================================================================
