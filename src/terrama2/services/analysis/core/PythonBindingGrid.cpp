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
  \file terrama2/services/analysis/core/PythonBindindGrid.cpp

  \brief

  \author Jano Simas
*/

#include "PythonBindingGrid.hpp"
#include "grid/Operator.hpp"
#include "grid/history/Operator.hpp"
#include "grid/history/interval/Operator.hpp"
#include "grid/forecast/Operator.hpp"
#include "grid/forecast/interval/Operator.hpp"
#include "grid/zonal/Operator.hpp"
#include "grid/zonal/history/Operator.hpp"

void terrama2::services::analysis::core::python::Grid::registerFunctions()
{
  registerGridFunctions();
  registerGridHistoryFunctions();
  registerGridHistoryIntervalFunctions();
  registerGridForecastFunctions();
  registerGridForecastIntervalFunctions();
  registerGridZonalFunctions();
}

void terrama2::services::analysis::core::python::Grid::registerGridFunctions()
{
  using namespace boost::python;

  // map the grid namespace to a sub-module
  // make "from terrama2.grid import <function>" work
  boost::python::object gridModule(handle<>(borrowed(PyImport_AddModule("terrama2.grid"))));
  // make "from terrama2 import grid" work
  import("terrama2").attr("grid") = gridModule;
  // set the current scope to the new sub-module
  scope gridScope = gridModule;

  // export functions inside grid namespace
  def("sample", terrama2::services::analysis::core::grid::sample);
}

void terrama2::services::analysis::core::python::Grid::registerGridHistoryFunctions()
{
  using namespace boost::python;

  // Register operations for grid.history
  object gridHistoryModule(handle<>(borrowed(PyImport_AddModule("terrama2.grid.history"))));
  // make "from terrama2.grid import history" work
  import("terrama2.grid").attr("history") = gridHistoryModule;
  // set the current scope to the new sub-module
  scope gridHistoryScope = gridHistoryModule;

  def("min", terrama2::services::analysis::core::grid::history::min);
  def("max", terrama2::services::analysis::core::grid::history::max);
  def("mean", terrama2::services::analysis::core::grid::history::mean);
  def("median", terrama2::services::analysis::core::grid::history::median);
  def("standard_deviation", terrama2::services::analysis::core::grid::history::standardDeviation);
  def("variance", terrama2::services::analysis::core::grid::history::variance);
}

void terrama2::services::analysis::core::python::Grid::registerGridHistoryIntervalFunctions()
{
  using namespace boost::python;

  // Register operations for grid.history.interval
  object gridHistoryIntervalModule(handle<>(borrowed(PyImport_AddModule("terrama2.grid.history.interval"))));
  // make "from terrama2.grid.history import interval" work
  import("terrama2.grid.history").attr("interval") = gridHistoryIntervalModule;
  // set the current scope to the new sub-module
  scope gridHistoryIntervalScope = gridHistoryIntervalModule;

  def("min", terrama2::services::analysis::core::grid::history::interval::min);
  def("max", terrama2::services::analysis::core::grid::history::interval::max);
  def("mean", terrama2::services::analysis::core::grid::history::interval::mean);
  def("median", terrama2::services::analysis::core::grid::history::interval::median);
  def("standard_deviation", terrama2::services::analysis::core::grid::history::interval::standardDeviation);
  def("variance", terrama2::services::analysis::core::grid::history::interval::variance);
}

void terrama2::services::analysis::core::python::Grid::registerGridForecastFunctions()
{
  using namespace boost::python;

  // Register operations for grid.forecast
  object gridForecastModule(handle<>(borrowed(PyImport_AddModule("terrama2.grid.forecast"))));
  // make "from terrama2.grid import forecast" work
  import("terrama2.grid").attr("forecast") = gridForecastModule;
  // set the current scope to the new sub-module
  scope gridHistoryIntervalScope = gridForecastModule;

  def("min", terrama2::services::analysis::core::grid::forecast::min);
  def("max", terrama2::services::analysis::core::grid::forecast::max);
  def("mean", terrama2::services::analysis::core::grid::forecast::mean);
  def("median", terrama2::services::analysis::core::grid::forecast::median);
  def("standard_deviation", terrama2::services::analysis::core::grid::forecast::standardDeviation);
  def("variance", terrama2::services::analysis::core::grid::forecast::variance);
}

void terrama2::services::analysis::core::python::Grid::registerGridForecastIntervalFunctions()
{
  using namespace boost::python;

  // Register operations for grid.forecast.interval
  object gridHistoryIntervalModule(handle<>(borrowed(PyImport_AddModule("terrama2.grid.forecast.interval"))));
  // make "from terrama2.grid.forecast import interval" work
  import("terrama2.grid.forecast").attr("interval") = gridHistoryIntervalModule;
  // set the current scope to the new sub-module
  scope gridHistoryIntervalScope = gridHistoryIntervalModule;

  def("min", terrama2::services::analysis::core::grid::forecast::interval::min);
  def("max", terrama2::services::analysis::core::grid::forecast::interval::max);
  def("mean", terrama2::services::analysis::core::grid::forecast::interval::mean);
  def("median", terrama2::services::analysis::core::grid::forecast::interval::median);
  def("standard_deviation", terrama2::services::analysis::core::grid::forecast::interval::standardDeviation);
  def("variance", terrama2::services::analysis::core::grid::forecast::interval::variance);
}

// pragma to silence python macros warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedef"

// // Declaration needed for default parameter restriction
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalCount_overloads, terrama2::services::analysis::core::grid::zonal::count, 1, 2);
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalMin_overloads, terrama2::services::analysis::core::grid::zonal::min, 1, 2);
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalMax_overloads, terrama2::services::analysis::core::grid::zonal::max, 1, 2);
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalMean_overloads, terrama2::services::analysis::core::grid::zonal::mean, 1, 2);
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalMedian_overloads, terrama2::services::analysis::core::grid::zonal::median, 1, 2);
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalStandardDeviation_overloads, terrama2::services::analysis::core::grid::zonal::standardDeviation, 1, 2);
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalVariance_overloads, terrama2::services::analysis::core::grid::zonal::variance, 1, 2);

// closing "-Wunused-local-typedef" pragma
#pragma GCC diagnostic pop

void terrama2::services::analysis::core::python::Grid::registerGridZonalFunctions()
{
  using namespace boost::python;

  // Register operations for grid.zonal
  object gridZonalModule(handle<>(borrowed(PyImport_AddModule("terrama2.grid.zonal"))));
  // make "from terrama2.grid import zonal" work
  import("terrama2.grid").attr("zonal") = gridZonalModule;
  // set the current scope to the new sub-module
  scope gridZonalScope = gridZonalModule;

  def("count", terrama2::services::analysis::core::grid::zonal::count,
      gridZonalCount_overloads(args("dataSeriesName", "buffer"),
                               "Count operator for grid zonal"));
  def("min", terrama2::services::analysis::core::grid::zonal::min,
      gridZonalMin_overloads(args("dataSeriesName", "buffer"),
                             "Min operator for grid zonal"));
  def("max", terrama2::services::analysis::core::grid::zonal::max,
      gridZonalMax_overloads(args("dataSeriesName", "buffer"),
                             "Max operator for grid zonal"));
  def("mean", terrama2::services::analysis::core::grid::zonal::mean,
      gridZonalMean_overloads(args("dataSeriesName", "buffer"),
                              "Mean operator for grid zonal"));
  def("median", terrama2::services::analysis::core::grid::zonal::median,
      gridZonalMedian_overloads(args("dataSeriesName", "buffer"),
                                "Median operator for grid zonal"));
  def("standard_deviation", terrama2::services::analysis::core::grid::zonal::standardDeviation,
      gridZonalStandardDeviation_overloads(args("dataSeriesName", "buffer"),
          "Standard deviation operator for grid zonal"));
  def("variance", terrama2::services::analysis::core::grid::zonal::variance,
      gridZonalVariance_overloads(args("dataSeriesName", "buffer"),
                                  "Variance operator for grid zonal"));
}

// pragma to silence python macros warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedef"

// // Declaration needed for default parameter restriction
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryCount_overloads, terrama2::services::analysis::core::grid::zonal::history::count, 2, 3);
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryMin_overloads, terrama2::services::analysis::core::grid::zonal::history::min, 2, 3);
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryMax_overloads, terrama2::services::analysis::core::grid::zonal::history::max, 2, 3);
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryMean_overloads, terrama2::services::analysis::core::grid::zonal::history::mean, 2, 3);
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryMedian_overloads, terrama2::services::analysis::core::grid::zonal::history::median, 2, 3);
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryStandardDeviation_overloads, terrama2::services::analysis::core::grid::zonal::history::standardDeviation, 2, 3);
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryVariance_overloads, terrama2::services::analysis::core::grid::zonal::history::variance, 2, 3);

// closing "-Wunused-local-typedef" pragma
#pragma GCC diagnostic pop

void terrama2::services::analysis::core::python::Grid::registerGridZonalHistoryFunctions()
{
  using namespace boost::python;

  // Register operations for grid.zonal
  object gridZonalHistoryModule(handle<>(borrowed(PyImport_AddModule("terrama2.grid.zonal.history"))));
  // make "from terrama2.grid import zonal" work
  import("terrama2.grid.zonal").attr("history") = gridZonalHistoryModule;
  // set the current scope to the new sub-module
  scope gridZonalHistoryScope = gridZonalHistoryModule;

  def("count", terrama2::services::analysis::core::grid::zonal::history::count,
      gridZonalHistoryCount_overloads(args("dataSeriesName", "buffer"),
                               "Count operator for grid zonal"));
  def("min", terrama2::services::analysis::core::grid::zonal::history::min,
      gridZonalHistoryMin_overloads(args("dataSeriesName", "buffer"),
                             "Min operator for grid zonal"));
  def("max", terrama2::services::analysis::core::grid::zonal::history::max,
      gridZonalHistoryMax_overloads(args("dataSeriesName", "buffer"),
                             "Max operator for grid zonal"));
  def("mean", terrama2::services::analysis::core::grid::zonal::history::mean,
      gridZonalHistoryMean_overloads(args("dataSeriesName", "buffer"),
                              "Mean operator for grid zonal"));
  def("median", terrama2::services::analysis::core::grid::zonal::history::median,
      gridZonalHistoryMedian_overloads(args("dataSeriesName", "buffer"),
                                "Median operator for grid zonal"));
  def("standard_deviation", terrama2::services::analysis::core::grid::zonal::history::standardDeviation,
      gridZonalHistoryStandardDeviation_overloads(args("dataSeriesName", "buffer"),
          "Standard deviation operator for grid zonal"));
  def("variance", terrama2::services::analysis::core::grid::zonal::history::variance,
      gridZonalHistoryVariance_overloads(args("dataSeriesName", "buffer"),
                                  "Variance operator for grid zonal"));
}
