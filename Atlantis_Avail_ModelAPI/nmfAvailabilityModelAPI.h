/**
 * @file nmfAvailabilityModelAPI.h
 * @brief Contains all of the business (i.e., non-GUI) logic for the Availability value calculations.
 *
 * This file contains the logic to calculate Atlantis food availability values from the
 * required input data. A matrix is passed by reference to the calculate method and is
 * likewise returned, updated with the newly calculated values.
 *
 * @copyright
 * Public Domain Notice\n
 *
 * National Oceanic And Atmospheric Administration\n\n
 *
 * This software is a "United States Government Work" under the terms of the
 * United States Copyright Act.  It was written as part of the author's official
 * duties as a United States Government employee/contractor and thus cannot be copyrighted.
 * This software is freely available to the public for use. The National Oceanic
 * And Atmospheric Administration and the U.S. Government have not placed any
 * restriction on its use or reproduction.  Although all reasonable efforts have
 * been taken to ensure the accuracy and reliability of the software and data,
 * the National Oceanic And Atmospheric Administration and the U.S. Government
 * do not and cannot warrant the performance or results that may be obtained
 * by using this software or data. The National Oceanic And Atmospheric
 * Administration and the U.S. Government disclaim all warranties, express
 * or implied, including warranties of performance, merchantability or fitness
 * for any particular purpose.\n\n
 *
 * Please cite the author(s) in any work or product based on this material.
 */

#ifndef ATLANTIS_AVAIL_MODELAPI_H
#define ATLANTIS_AVAIL_MODELAPI_H

#include "nmfConstantsAvailability.h"
#include "nmfLogger.h"
#include "nmfUtils.h"

#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/operation.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

#include <map>

#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QString>
#include <QTextStream>

typedef nmfConstantsAvailability::PredatorStruct PredatorStruct;
typedef nmfConstantsAvailability::PreyOnlyStruct PreyOnlyStruct;

/**
 * @brief This class contains the logic to calculate Atlantis food availability values from the
 * required input data. A matrix is passed by reference to the calculate method and is
 * likewise returned, updated with the newly calculated values.
 */
class nmfAvailabilityModelAPI
{
private:
    nmfLogger*  m_Logger;
    std::string m_ProjectDir;
    std::map<QString, void(nmfAvailabilityModelAPI::*)(
            QStringList&                           HeaderParts,
            QStringList&                           FileParts,
            double&                                Exponent,
            QString&                               PredatorName,
            int&                                   PredatorNum,
            std::map<QString,PredatorStruct>&      PredatorMap,
            boost::numeric::ublas::matrix<double>& AvailabilityMatrix)> m_FunctionMap;

    void createBiomassMap(
            const std::map<QString,PredatorStruct>& PredatorMap,
            const std::map<QString,PreyOnlyStruct>& PreyOnlyMap,
            std::map<QString,double>&               BiomassMap);
    void createPredatorMap(
            std::map<QString,PredatorStruct>& PredatorMap);
    void createPreyOnlyMap(
            std::map<QString,PreyOnlyStruct>& PreyOnlyMap);
    void calculateStandardHollingTypeI(
            QStringList&                           HeaderParts,
            QStringList&                           FileParts,
            double&                                Exponent,
            QString&                               PredatorName,
            int&                                   PredatorNum,
            std::map<QString,PredatorStruct>&      PredatorMap,
            boost::numeric::ublas::matrix<double>& AvailabilityMatrix);
    void calculateStandardHollingTypeII(
            QStringList&                           HeaderParts,
            QStringList&                           FileParts,
            double&                                Exponent,
            QString&                               PredatorName,
            int&                                   PredatorNum,
            std::map<QString,PredatorStruct>&      PredatorMap,
            boost::numeric::ublas::matrix<double>& AvailabilityMatrix);
    void calculateStandardHollingTypeIIhalf(
            QStringList&                           HeaderParts,
            QStringList&                           FileParts,
            double&                                Exponent,
            QString&                               PredatorName,
            int&                                   PredatorNum,
            std::map<QString,PredatorStruct>&      PredatorMap,
            boost::numeric::ublas::matrix<double>& AvailabilityMatrix);
    void calculateStandardHollingTypeIII(
            QStringList&                           HeaderParts,
            QStringList&                           FileParts,
            double&                                Exponent,
            QString&                               PredatorName,
            int&                                   PredatorNum,
            std::map<QString,PredatorStruct>&      PredatorMap,
            boost::numeric::ublas::matrix<double>& AvailabilityMatrix);
    void calculateModifiedHollingTypeII(
            QStringList&                           HeaderParts,
            QStringList&                           FileParts,
            double&                                Exponent,
            QString&                               PredatorName,
            int&                                   PredatorNum,
            std::map<QString,PredatorStruct>&      PredatorMap,
            boost::numeric::ublas::matrix<double>& AvailabilityMatrix);
    void calculateModifiedHollingTypeIIhalf(
            QStringList&                           HeaderParts,
            QStringList&                           FileParts,
            double&                                Exponent,
            QString&                               PredatorName,
            int&                                   PredatorNum,
            std::map<QString,PredatorStruct>&      PredatorMap,
            boost::numeric::ublas::matrix<double>& AvailabilityMatrix);
    void calculateModifiedHollingTypeIII(
            QStringList&                           HeaderParts,
            QStringList&                           FileParts,
            double&                                Exponent,
            QString&                               PredatorName,
            int&                                   PredatorNum,
            std::map<QString,PredatorStruct>&      PredatorMap,
            boost::numeric::ublas::matrix<double>& AvailabilityMatrix);
    void getSpeciesColumnMap(
            std::map<QString,int>& SpeciesColumnMap);
    void getTotalPreyBiomass(
            const std::map<QString,PredatorStruct>& PredatorMap,
            const std::map<QString,PreyOnlyStruct>& PreyOnlyMap,
            double& totalPreyBiomass,
            double& totalPreyBiomassSquared,
            double& exponent,
            double& totalPreyBiomassExponent);

public:
    /**
     * @brief Class constructor for the Availability calculation operations
     * @param Logger : application logger pointer
     * @param ProjectDir : name of project directory
     */
    nmfAvailabilityModelAPI(
            nmfLogger*  Logger,
            std::string ProjectDir);
   ~nmfAvailabilityModelAPI() {}

    /**
     * @brief This method calculates the Atlantis food availability values and passed them back
     * to the calling routine via the passed by reference matrix
     * @param AvailabilityMatrix : matrix that will ultimately contain the newly calculated Availability values
     */
    void calculateAvailabilityMatrix(
            boost::numeric::ublas::matrix<double>& AvailabilityMatrix);
};

#endif
