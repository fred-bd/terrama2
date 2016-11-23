/*
  Copyright (C) 2007 National Institute For Space Research (INPE) - Brazil.

  This file is part of TerraMA2 - a free and open source computational
  platform for analysis, monitoring, and alert of geo-environmental extremes.

  TerraMA2 is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License,
  or (at your option) any later version.

  TerraMA2 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with TerraMA2. See LICENSE. If not, write to
  TerraMA2 Team at <terrama2-team@dpi.inpe.br>.
*/

/*!
  \file terrama2/services/analysis/core/grid/zonal/forecast/Operator.cpp

  \brief Contains grid zonal analysis operators.

  \author Jano Simas
*/


#include "Operator.hpp"
#include "../Operator.hpp"
#include "../../../ContextManager.hpp"
#include "../../../../../../core/utility/Logger.hpp"
#include "../../../../../../core/utility/TimeUtils.hpp"
#include "../../../utility/Verify.hpp"
#include "../Utils.hpp"

//TerraLib
#include <terralib/dataaccess/utils/Utils.h>

double terrama2::services::analysis::core::grid::zonal::forecast::operatorImpl( terrama2::services::analysis::core::StatisticOperation statisticOperation,
                                                                                const std::string& dataSeriesName,
                                                                                const std::string& dateDiscardBefore,
                                                                                const std::string& dateDiscardAfter,
                                                                                const size_t var,
                                                                                terrama2::services::analysis::core::Buffer buffer)
{
  OperatorCache cache;
  terrama2::services::analysis::core::python::readInfoFromDict(cache);
  // After the operator lock is released it's not allowed to return any value because it doesn' have the interpreter lock.
  // In case an exception is thrown, we need to set this boolean. Once the code left the lock is acquired we should return NAN.
  bool exceptionOccurred = false;

  auto& contextManager = ContextManager::getInstance();
  auto analysis = cache.analysisPtr;

  try
  {
    terrama2::core::verify::analysisMonitoredObject(analysis);
  }
  catch (const terrama2::core::VerifyException&)
  {
    contextManager.addError(cache.analysisHashCode, QObject::tr("Use of invalid operator for analysis %1.").arg(analysis->id).toStdString());
    return NAN;
  }

  terrama2::services::analysis::core::MonitoredObjectContextPtr context;
  try
  {
    context = ContextManager::getInstance().getMonitoredObjectContext(cache.analysisHashCode);
  }
  catch(const terrama2::Exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
    return NAN;
  }


  try
  {
    // In case an error has already occurred, there is nothing to be done
    if(!context->getErrors().empty())
    {
      return NAN;
    }

    bool hasData = false;

    auto dataManagerPtr = context->getDataManager().lock();
    if(!dataManagerPtr)
    {
      QString errMsg(QObject::tr("Invalid data manager."));
      throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
    }

    std::shared_ptr<ContextDataSeries> moDsContext = context->getMonitoredObjectContextDataSeries(dataManagerPtr);
    if(!moDsContext)
    {
      QString errMsg(QObject::tr("Could not recover monitored object data series."));
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }

    if(moDsContext->series.syncDataSet->size() == 0)
    {
      QString errMsg(QObject::tr("Could not recover monitored object data series."));
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }

    auto moGeom = moDsContext->series.syncDataSet->getGeometry(cache.index, moDsContext->geometryPos);
    if(!moGeom.get())
    {
      QString errMsg(QObject::tr("Could not recover monitored object geometry."));
      throw InvalidDataSetException() << terrama2::ErrorDescription(errMsg);
    }
    auto geomResult = createBuffer(buffer, moGeom);

    auto dataSeries = context->findDataSeries(dataSeriesName);
    if(!dataSeries)
    {
      QString errMsg(QObject::tr("Could not find a data series with the given name: %1"));
      errMsg = errMsg.arg(QString::fromStdString(dataSeriesName));
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }

    terrama2::core::Filter filter;
    filter.discardAfter = context->getStartTime();
    filter.lastValue = true;

    auto seriesMap = context->getSeriesMap(dataSeries->id, filter);

    auto datasets = dataSeries->datasetList;
    for(const auto& dataset : datasets)
    {
      auto syncDataset = seriesMap.at(dataset).syncDataSet;
      if(syncDataset->size() > 1)
      {
        QString errMsg(QObject::tr("Invalid list of raster for dataset: %1").arg(dataset->id));
        throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
      }

      auto rasterPos = te::da::GetFirstPropertyPos(syncDataset->dataset().get(), te::dt::RASTER_TYPE);
      auto raster = syncDataset->getRaster(0, rasterPos);

      auto datePos = te::da::GetFirstPropertyPos(syncDataset->dataset().get(), te::dt::DATETIME_TYPE);
      auto rasterDate = syncDataset->getDateTime(0, datePos);
      auto rasterTimestamp = std::dynamic_pointer_cast<te::dt::TimeInstantTZ>(rasterDate)->getTimeInstantTZ();

      auto currentTimestamp = context->getStartTime()->getTimeInstantTZ();

      std::vector<double> values;

      {
        geomResult->transform(raster->getSRID());
        //no intersection between the raster and the object geometry
        if(!raster->getExtent()->intersects(*geomResult->getMBR()))
          continue;

        double interval = 1;//getBandTimeInterval(dataset);
        double secondsToBefore = terrama2::core::TimeUtils::convertTimeString(dateDiscardBefore, "SECOND", "h");
        double secondsToAfter = terrama2::core::TimeUtils::convertTimeString(dateDiscardAfter, "SECOND", "h");

        auto timePassed = currentTimestamp.utc_time() - rasterTimestamp.utc_time();
        auto secondsPassed = timePassed.total_seconds();

        int bandBegin = 1 + std::ceil(secondsPassed + secondsToBefore)/interval;
        int bandEnd = 1 + std::ceil(secondsPassed + secondsToAfter)/interval;

        std::map<std::pair<int, int>, double> tempValuesMap;
        for(size_t band = bandBegin; band <= bandEnd; ++ band)
        {
          utils::getRasterValues<double>(geomResult.get(), raster, band, tempValuesMap);
          transform(tempValuesMap.cbegin(), tempValuesMap.cend(), back_inserter(values), [](const std::pair<std::pair<int, int>, double>& val){ return val.second;} );
        }
      }

      if(!values.empty())
      {
        terrama2::services::analysis::core::calculateStatistics(values, cache);
        hasData = true;
      }

      if(hasData)
        break;
    }

    if(exceptionOccurred)
      return NAN;

    if(!hasData && statisticOperation != StatisticOperation::COUNT)
    {
      return NAN;
    }

    return terrama2::services::analysis::core::getOperationResult(cache, statisticOperation);
  }
  catch(const terrama2::Exception& e)
  {
    context->addError(boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    return NAN;
  }
  catch(const std::exception& e)
  {
    context->addError(e.what());
    return NAN;
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addError(errMsg.toStdString());
    return NAN;
  }
}

double terrama2::services::analysis::core::grid::zonal::forecast::min(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const size_t var, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::forecast::operatorImpl(StatisticOperation::MIN, dataSeriesName, dateDiscardBefore, "", var, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::max(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const size_t var, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::forecast::operatorImpl(StatisticOperation::MAX, dataSeriesName, dateDiscardBefore, "", var, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::mean(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const size_t var, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::forecast::operatorImpl(StatisticOperation::MEAN, dataSeriesName, dateDiscardBefore, "", var, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::median(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const size_t var, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::forecast::operatorImpl(StatisticOperation::MEDIAN, dataSeriesName, dateDiscardBefore, "", var, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::standardDeviation(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const size_t var, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::forecast::operatorImpl(StatisticOperation::STANDARD_DEVIATION, dataSeriesName, dateDiscardBefore, "", var, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::variance(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const size_t var, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::forecast::operatorImpl(StatisticOperation::VARIANCE, dataSeriesName, dateDiscardBefore, "", var, buffer);
}

double terrama2::services::analysis::core::grid::zonal::forecast::sum(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const size_t var, terrama2::services::analysis::core::Buffer buffer)
{
  return terrama2::services::analysis::core::grid::zonal::forecast::operatorImpl(StatisticOperation::SUM, dataSeriesName, dateDiscardBefore, "", var, buffer);
}
