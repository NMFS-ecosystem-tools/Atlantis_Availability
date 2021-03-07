#include "nmfAvailabilityModelAPI.h"
#include "nmfConstants.h"

using namespace boost::numeric::ublas;

nmfAvailabilityModelAPI::nmfAvailabilityModelAPI(
        nmfLogger*  logger,
        std::string projectDir)
{
    m_ProjectDir = projectDir;
    m_Logger     = logger;
    m_FunctionMap["Standard Holling Type I"]   = &nmfAvailabilityModelAPI::calculateStandardHollingTypeI;
    m_FunctionMap["Standard Holling Type II"]  = &nmfAvailabilityModelAPI::calculateStandardHollingTypeII;
    m_FunctionMap["Standard Holling Type II½"] = &nmfAvailabilityModelAPI::calculateStandardHollingTypeIIhalf;
    m_FunctionMap["Standard Holling Type III"] = &nmfAvailabilityModelAPI::calculateStandardHollingTypeIII;
    m_FunctionMap["Modified Holling Type II"]  = &nmfAvailabilityModelAPI::calculateModifiedHollingTypeII;
    m_FunctionMap["Modified Holling Type II½"] = &nmfAvailabilityModelAPI::calculateModifiedHollingTypeIIhalf;
    m_FunctionMap["Modified Holling Type III"] = &nmfAvailabilityModelAPI::calculateModifiedHollingTypeIII;
}

void
nmfAvailabilityModelAPI::createPredatorMap(
    std::map<QString,PredatorStruct>& PredatorMap)
{
    int counter=0;
    QString predator;

    QString path = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsAvailability::InputDataDir));
    QString speciesFile = QDir(path).filePath(QString::fromStdString(nmfConstantsAvailability::PredatorFile));
    QFile file(speciesFile);
    QStringList parts;
    if ( file.open(QIODevice::ReadOnly|QIODevice::Text) )
    {
        QTextStream stream(&file);
        QString line = stream.readLine();
        while (! line.isNull())
        {
            if (counter++ > 1) { // skip first 2 lines
                parts = line.split(",");
                PredatorStruct predStruct;
                predStruct.growthRate         = parts[1];
                predStruct.initialBiomass     = parts[2];
                predStruct.efficiency         = parts[3];
                predStruct.clearanceRate      = parts[4];
                predStruct.exponent           = parts[5];
                predStruct.ageStructure       = parts[6];
                predStruct.functionalResponse = parts[7];
                PredatorMap[parts[0]]         = predStruct;
            }
            line = stream.readLine();
        }
        file.close();
    }
}

void
nmfAvailabilityModelAPI::createPreyOnlyMap(
    std::map<QString,PreyOnlyStruct>& PreyOnlyMap)
{
    int counter=0;
    QString prey;

    QString path = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsAvailability::InputDataDir));
    QString speciesFile = QDir(path).filePath(QString::fromStdString(nmfConstantsAvailability::PreyOnlyFile));
    QFile file(speciesFile);
    QStringList parts;
    if ( file.open(QIODevice::ReadOnly|QIODevice::Text) )
    {
        QTextStream stream(&file);
        QString line = stream.readLine();
        while (! line.isNull())
        {
            if (counter++ > 1) { // skip first 2 lines
                parts = line.split(",");
                PreyOnlyStruct preyOnlyStruct;
                preyOnlyStruct.initialBiomass = parts[1];
                PreyOnlyMap[parts[0]]         = preyOnlyStruct;
            }
            line = stream.readLine();
        }
        file.close();
    }
}

void
nmfAvailabilityModelAPI::getTotalPreyBiomass(
        const std::map<QString,PredatorStruct>& PredatorMap,
        const std::map<QString,PreyOnlyStruct>& PreyOnlyMap,
        double& totalPreyBiomass,
        double& totalPreyBiomassSquared,
        double& exponent,
        double& totalPreyBiomassExponent)
{
    double preyBiomass = 0;

    auto itPred = PredatorMap.begin();
    while (itPred != PredatorMap.end()) {
        preyBiomass = itPred->second.initialBiomass.toDouble();
        totalPreyBiomass         += preyBiomass;
        totalPreyBiomassSquared  += preyBiomass*preyBiomass;
        totalPreyBiomassExponent += std::pow(preyBiomass,exponent);
        ++itPred;
    }

    auto itPrey = PreyOnlyMap.begin();
    while (itPrey != PreyOnlyMap.end()) {
        preyBiomass = itPrey->second.initialBiomass.toDouble();
        totalPreyBiomass         += preyBiomass;
        totalPreyBiomassSquared  += preyBiomass*preyBiomass;
        totalPreyBiomassExponent += std::pow(preyBiomass,exponent);
        ++itPrey;
    }
}

void
nmfAvailabilityModelAPI::createBiomassMap(
        const std::map<QString,PredatorStruct>& PredatorMap,
        const std::map<QString,PreyOnlyStruct>& PreyOnlyMap,
        std::map<QString,double>&               BiomassMap)
{
    double biomass;
    QString species;

    auto itPred = PredatorMap.begin();
    while (itPred != PredatorMap.end()) {
        species = itPred->first;
        biomass = itPred->second.initialBiomass.toDouble();
        BiomassMap[species] = biomass;
        ++itPred;
    }

    auto itPrey = PreyOnlyMap.begin();
    while (itPrey != PreyOnlyMap.end()) {
        species = itPrey->first;
        biomass = itPrey->second.initialBiomass.toDouble();
        BiomassMap[species] = biomass;
        ++itPrey;
    }
}

void
nmfAvailabilityModelAPI::getSpeciesColumnMap(std::map<QString,int>& SpeciesColumnMap)
{
    QString path = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsAvailability::InputDataDir));
    QString allSpeciesFile = QDir(path).filePath(QString::fromStdString(nmfConstantsAvailability::AllSpeciesFile));
    QFile file(allSpeciesFile);
    QStringList parts;
    QString line;
    QString species;
    int col = 0;

    if ( file.open(QIODevice::ReadOnly|QIODevice::Text) )
    {
        QTextStream stream(&file);
        line = stream.readLine();
        species = line.split(",")[0];
        while (! line.isNull()) {
            SpeciesColumnMap[species] = col++;
            line = stream.readLine();
            species = line.split(",")[0];
        }
    }
}

void
nmfAvailabilityModelAPI::calculateAvailabilityMatrix(
        boost::numeric::ublas::matrix<double>& AvailabilityMatrix)
{
    int predatorNum=0;
    QString path = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsAvailability::InputDataDir));
    QString dietCompositionFile = QDir(path).filePath(QString::fromStdString(nmfConstantsAvailability::DietCompositionFile));
    QFile file(dietCompositionFile);
    QStringList parts;
    QStringList headerParts;
    QString predatorName;
    QString preyName;
    QString line;
    double exponent;
    QString functionalResponse;
    std::map<QString,PredatorStruct> PredatorMap;

    createPredatorMap(PredatorMap);

    // Iterate through list of DietComposition species and find availability factors for each predator species
    if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream stream(&file);
        line = stream.readLine();
        headerParts = line.split(",");
        line = stream.readLine(); // skip header
        while (! line.isNull()) {
            parts = line.split(",");
            predatorName = parts[0];
            exponent           = PredatorMap[predatorName].exponent.toDouble();
            functionalResponse = PredatorMap[predatorName].functionalResponse;
            if (m_FunctionMap.find(functionalResponse) != m_FunctionMap.end()) {
                (this->*m_FunctionMap[functionalResponse])(headerParts,parts,exponent,
                                                           predatorName,predatorNum,
                                                           PredatorMap,AvailabilityMatrix);
            }
            line = stream.readLine();
            ++predatorNum;
        }
        file.close();
    }
}

void
nmfAvailabilityModelAPI::calculateStandardHollingTypeI(
        QStringList&                           HeaderParts,
        QStringList&                           FileParts,
        double&                                Exponent,
        QString&                               PredatorName,
        int&                                   PredatorNum,
        std::map<QString,PredatorStruct>&      PredatorMap,
        boost::numeric::ublas::matrix<double>& AvailabilityMatrix)
{
    m_Logger->logMsg(nmfConstants::Normal,"Calling species ["+PredatorName.toStdString()+"] functional response: calculateStandardHollingTypeI");
    int numPrey = FileParts.size()-3;
    int preyAvailabilityCol;
    double result = 0;
    double dcPrey = 0;
    double clPred = 0;
    double bmPred = 0;
    double bmPrey = 0;
    double totalPreyBiomass         = 0;
    double totalPreyBiomassSquared  = 0;
    double totalPreyBiomassExponent = 0;
    QString preyName;
    std::map<QString,PreyOnlyStruct> PreyOnlyMap;
    std::map<QString,double>         BiomassMap;
    std::map<QString,int>            SpeciesColumnMap;

    createPreyOnlyMap(PreyOnlyMap);
    createBiomassMap(PredatorMap,PreyOnlyMap,BiomassMap);
    getTotalPreyBiomass(PredatorMap,PreyOnlyMap,totalPreyBiomass,totalPreyBiomassSquared,
                        Exponent,totalPreyBiomassExponent);
    getSpeciesColumnMap(SpeciesColumnMap);

    clPred = PredatorMap[PredatorName].clearanceRate.toDouble();

    for (int preyCol=3; preyCol<numPrey+3; ++preyCol) {
        preyName = HeaderParts[preyCol];
        dcPrey = FileParts[preyCol].toDouble();
        bmPred = BiomassMap[PredatorName];
        bmPrey = BiomassMap[preyName];
        result = -1.0;
        if ((bmPred != 0) && (bmPrey != 0) && (clPred != 0)) {
            result = (dcPrey/(bmPred*bmPrey*clPred)) * totalPreyBiomass;
        }
        preyAvailabilityCol = SpeciesColumnMap[preyName];
        AvailabilityMatrix(PredatorNum,preyAvailabilityCol) = result;
    }
}

void
nmfAvailabilityModelAPI::calculateStandardHollingTypeII(
        QStringList&                           HeaderParts,
        QStringList&                           FileParts,
        double&                                Exponent,
        QString&                               PredatorName,
        int&                                   PredatorNum,
        std::map<QString,PredatorStruct>&      PredatorMap,
        boost::numeric::ublas::matrix<double>& AvailabilityMatrix)
{
    m_Logger->logMsg(nmfConstants::Normal,"Calling species ["+PredatorName.toStdString()+"] functional response: calculateStandardHollingTypeII");
    int numPrey = FileParts.size()-3;
    int preyAvailabilityCol;
    double result = 0;
    double dcPrey = 0;
    double efPred = 0;
    double grPred = 0;
    double clPred = 0;
    double bmPred = 0;
    double bmPrey = 0;
    double totalPreyBiomass         = 0;
    double totalPreyBiomassSquared  = 0;
    double totalPreyBiomassExponent = 0;
    QString preyName;
    std::map<QString,PreyOnlyStruct> PreyOnlyMap;
    std::map<QString,double>         BiomassMap;
    std::map<QString,int>            SpeciesColumnMap;

    createPreyOnlyMap(PreyOnlyMap);
    createBiomassMap(PredatorMap,PreyOnlyMap,BiomassMap);
    getTotalPreyBiomass(PredatorMap,PreyOnlyMap,totalPreyBiomass,totalPreyBiomassSquared,
                        Exponent,totalPreyBiomassExponent);
    getSpeciesColumnMap(SpeciesColumnMap);

    clPred = PredatorMap[PredatorName].clearanceRate.toDouble();
    efPred = PredatorMap[PredatorName].efficiency.toDouble();
    grPred = PredatorMap[PredatorName].growthRate.toDouble();

    for (int preyCol=3; preyCol<numPrey+3; ++preyCol) {
        preyName = HeaderParts[preyCol];
        dcPrey = FileParts[preyCol].toDouble();
        bmPred = BiomassMap[PredatorName];
        bmPrey = BiomassMap[preyName];
        result = -1.0;
        if ((bmPred != 0) && (bmPrey != 0) && (clPred != 0) && (grPred != 0)) {
            result = (dcPrey/(bmPred*bmPrey*clPred)) * (1.0 + clPred*(efPred/grPred)*totalPreyBiomass);
        }
        preyAvailabilityCol = SpeciesColumnMap[preyName];
        AvailabilityMatrix(PredatorNum,preyAvailabilityCol) = result;
    }
}

void
nmfAvailabilityModelAPI::calculateStandardHollingTypeIIhalf(
        QStringList&                           HeaderParts,
        QStringList&                           FileParts,
        double&                                Exponent,
        QString&                               PredatorName,
        int&                                   PredatorNum,
        std::map<QString,PredatorStruct>&      PredatorMap,
        boost::numeric::ublas::matrix<double>& AvailabilityMatrix)
{
    m_Logger->logMsg(nmfConstants::Normal,"Calling species ["+PredatorName.toStdString()+"] functional response: calculateStandardHollingTypeIIhalf with exponent: "+std::to_string(Exponent));
    int numPrey = FileParts.size()-3;
    int preyAvailabilityCol;
    double result = 0;
    double dcPrey = 0;
    double efPred = 0;
    double grPred = 0;
    double clPred = 0;
    double bmPred = 0;
    double bmPrey = 0;
    double totalPreyBiomass         = 0;
    double totalPreyBiomassSquared  = 0;
    double totalPreyBiomassExponent = 0;
    QString preyName;
    std::map<QString,PreyOnlyStruct> PreyOnlyMap;
    std::map<QString,double>         BiomassMap;
    std::map<QString,int>            SpeciesColumnMap;

    createPreyOnlyMap(PreyOnlyMap);
    createBiomassMap(PredatorMap,PreyOnlyMap,BiomassMap);
    getTotalPreyBiomass(PredatorMap,PreyOnlyMap,totalPreyBiomass,totalPreyBiomassSquared,
                        Exponent,totalPreyBiomassExponent);
    getSpeciesColumnMap(SpeciesColumnMap);

    clPred = PredatorMap[PredatorName].clearanceRate.toDouble();
    efPred = PredatorMap[PredatorName].efficiency.toDouble();
    grPred = PredatorMap[PredatorName].growthRate.toDouble();

    for (int preyCol=3; preyCol<numPrey+3; ++preyCol) {
        preyName = HeaderParts[preyCol];
        dcPrey = FileParts[preyCol].toDouble();
        bmPred = BiomassMap[PredatorName];
        bmPrey = BiomassMap[preyName];
        result = -1.0;
        if ((bmPred != 0) && (bmPrey != 0) && (clPred != 0) && (grPred != 0) && (Exponent != 0)) {
            result = std::pow((dcPrey/(bmPred*clPred)) * (1.0 + clPred*(efPred/grPred)*totalPreyBiomassExponent),(1.0/Exponent)) /bmPrey;
        }
        preyAvailabilityCol = SpeciesColumnMap[preyName];
        AvailabilityMatrix(PredatorNum,preyAvailabilityCol) = result;
    }
}

void
nmfAvailabilityModelAPI::calculateStandardHollingTypeIII(
        QStringList&                           HeaderParts,
        QStringList&                           FileParts,
        double&                                Exponent,
        QString&                               PredatorName,
        int&                                   PredatorNum,
        std::map<QString,PredatorStruct>&      PredatorMap,
        boost::numeric::ublas::matrix<double>& AvailabilityMatrix)
{
    m_Logger->logMsg(nmfConstants::Normal,"Calling species ["+PredatorName.toStdString()+"] functional response: calculateStandardHollingTypeIII");
    int numPrey = FileParts.size()-3;
    int preyAvailabilityCol;
    double result = 0;
    double dcPrey = 0;
    double efPred = 0;
    double grPred = 0;
    double clPred = 0;
    double bmPred = 0;
    double bmPrey = 0;
    double totalPreyBiomass         = 0;
    double totalPreyBiomassSquared  = 0;
    double totalPreyBiomassExponent = 0;
    QString preyName;
    std::map<QString,PreyOnlyStruct> PreyOnlyMap;
    std::map<QString,double>         BiomassMap;
    std::map<QString,int>            SpeciesColumnMap;

    createPreyOnlyMap(PreyOnlyMap);
    createBiomassMap(PredatorMap,PreyOnlyMap,BiomassMap);
    getTotalPreyBiomass(PredatorMap,PreyOnlyMap,totalPreyBiomass,totalPreyBiomassSquared,
                        Exponent,totalPreyBiomassExponent);
    getSpeciesColumnMap(SpeciesColumnMap);

    clPred = PredatorMap[PredatorName].clearanceRate.toDouble();
    efPred = PredatorMap[PredatorName].efficiency.toDouble();
    grPred = PredatorMap[PredatorName].growthRate.toDouble();

    for (int preyCol=3; preyCol<numPrey+3; ++preyCol) {
        preyName = HeaderParts[preyCol];
        dcPrey = FileParts[preyCol].toDouble();
        bmPred = BiomassMap[PredatorName];
        bmPrey = BiomassMap[preyName];
        result = -1.0;
        if ((bmPred != 0) && (bmPrey != 0) && (clPred != 0) && (grPred != 0)) {
            result = sqrt((dcPrey/(bmPred*clPred)) * (1.0 + clPred*(efPred/grPred)*totalPreyBiomassSquared)) /bmPrey;
        }
        preyAvailabilityCol = SpeciesColumnMap[preyName];
        AvailabilityMatrix(PredatorNum,preyAvailabilityCol) = result;
    }
}

void
nmfAvailabilityModelAPI::calculateModifiedHollingTypeII(
        QStringList&                           HeaderParts,
        QStringList&                           FileParts,
        double&                                Exponent,
        QString&                               PredatorName,
        int&                                   PredatorNum,
        std::map<QString,PredatorStruct>&      PredatorMap,
        boost::numeric::ublas::matrix<double>& AvailabilityMatrix)
{
    m_Logger->logMsg(nmfConstants::Normal,"Calling species ["+PredatorName.toStdString()+"] functional response: calculateModifiedHollingTypeII");
    int numPrey = FileParts.size()-3;
    int preyAvailabilityCol;
    int preyNum;
    int preyCol;
    double dcPrey = 0;
    double efPred = 0;
    double grPred = 0;
    double clPred = 0;
    double bmPred = 0;
    double bmPrey = 0;
    double totalPreyBiomass         = 0;
    double totalPreyBiomassSquared  = 0;
    double totalPreyBiomassExponent = 0;
    double X = 0;
    double Y = 0;
    double kPrey = 0;
    double den;
    QString preyName;
    std::map<QString,PreyOnlyStruct> PreyOnlyMap;
    std::map<QString,double>         BiomassMap;
    std::map<QString,int>            SpeciesColumnMap;
    boost::numeric::ublas::matrix<double> AvailabilityVector;
    boost::numeric::ublas::matrix<double> matrix;
    boost::numeric::ublas::matrix<double> inverseMatrix;
    boost::numeric::ublas::matrix<double> kMatrix;
    nmfUtils::initialize(matrix,numPrey,numPrey);
    nmfUtils::initialize(kMatrix,numPrey,1);
    nmfUtils::initialize(AvailabilityVector,numPrey,1);

    createPreyOnlyMap(PreyOnlyMap);
    createBiomassMap(PredatorMap,PreyOnlyMap,BiomassMap);
    getTotalPreyBiomass(PredatorMap,PreyOnlyMap,totalPreyBiomass,totalPreyBiomassSquared,
                        Exponent,totalPreyBiomassExponent);
    getSpeciesColumnMap(SpeciesColumnMap);

    clPred = PredatorMap[PredatorName].clearanceRate.toDouble();
    efPred = PredatorMap[PredatorName].efficiency.toDouble();
    grPred = PredatorMap[PredatorName].growthRate.toDouble();

    /*

    p = predator species
    i,j,k = prey species

    dc(pi) = B(p)C(p)a(pi)B(i) / [1 + (C(p)E(p)/g(p)) x ∑(a(prey)B(prey)) ]

    Ex. For 3 prey species:

    dc(i)  = B(p)B(i)a(pi)C(p) / [1 + (C(p)E(p)/g(p))(a(pi)B(i)+a(pj)B(j)+a(pk)B(k))]
    dc(j)  = B(p)B(j)a(pj)C(p) / [1 + (C(p)E(p)/g(p))(a(pi)B(i)+a(pj)B(j)+a(pk)B(k))]
    dc(k)  = B(p)B(k)a(pk)C(p) / [1 + (C(p)E(p)/g(p))(a(pi)B(i)+a(pj)B(j)+a(pk)B(k))]

    Need to solve for a(pi), a(pj), and a(pk)

    Let X(p) = B(p)C(p) and Y(p) = C(p)E(p)/g(p) we get:

    dc(i)  = X(p)B(i)a(pi) / [1 + Y(p)(a(pi)B(i)+a(pj)B(j)+a(pk)B(k))]
    dc(j)  = X(p)B(j)a(pj) / [1 + Y(p)(a(pi)B(i)+a(pj)B(j)+a(pk)B(k))]
    dc(k)  = X(p)B(k)a(pk) / [1 + Y(p)(a(pi)B(i)+a(pj)B(j)+a(pk)B(k))]

    Cross multiplying and with some algebraic manipulation...

                 a(pi) - Y(p)k(i)B(j)a(pj) - Y(p)k(i)B(k)a(pk) = k(i)
    -Y(p)k(j)B(i)a(pi) +             a(pj) - Y(p)k(j)B(k)a(pk) = k(j)
    -Y(p)k(k)B(i)a(pi) - Y(p)k(k)B(j)a(pj) +             a(pk) = k(k)

    where k(i) = dc(i) / [B(i)((X(p) - dc(i)Y(p))] and similarly for k(j) and k(k).

    Putting in matrix form...

    ⎛      1       -Y(p)k(i)B(j) -Y(p)k(i)B(k) ⎞ ⎛a(pi)⎞     ⎛k(i)⎞
    ⎜-Y(p)k(j)B(i)       1       -Y(p)k(j)B(k) ⎟ ⎜a(pj)⎟  =  ⎜k(j)⎟
    ⎝-Y(p)k(k)B(i) -Y(p)k(k)B(j)       1       ⎠ ⎝a(pk)⎠     ⎝k(k)⎠


    It follows then by taking the inverse of the 3x3 matrix on the left, we can find the values
    for the availability vector (a), which was the original goal.

    */


    //  1. Create temporary variables to keep simplify equations
    bmPred = BiomassMap[PredatorName];
    X = bmPred * clPred;
    Y = (clPred*efPred)/grPred;

    //  2. Build numPrey x numPrey matrix for current predator
    for (int row=0; row<numPrey; ++row) {
        preyNum = row+3;
        dcPrey = FileParts[preyNum].toDouble();
        preyName = HeaderParts[preyNum];
        bmPrey = BiomassMap[preyName];
        den = bmPrey * (X - Y * dcPrey);
        if (den == 0) {
            std::cout << "Error (calculateModifiedHollingTypeII): Found denominator=0" << std::endl;
            return;
        }
        kPrey = dcPrey / den;
        kMatrix(row,0) = kPrey;
        for (int col=0; col<numPrey; ++col) {
            preyCol = col+3;
            preyName = HeaderParts[preyCol];
            bmPrey = BiomassMap[preyName];
            if (col == row) {
                matrix(row,col) = 1.0;
            } else {
                matrix(row,col) = -Y * kPrey * bmPrey;
            }
        }
    }

    //  3. Find inverse of that matrix
    if (! nmfUtils::invertMatrix(matrix,inverseMatrix)) {
        std::cout << "Error: matrix not invertible" << std::endl;
        return;
    }

    //  4. Multiply inverse by kMatrix constants to get the final Availability vector
    AvailabilityVector = boost::numeric::ublas::prod(inverseMatrix,kMatrix);

    //  5. The values in the vector correspond to the Availability matrix values
    for (int preyCol=0; preyCol<numPrey; ++preyCol) {
        preyName = HeaderParts[preyCol+3];
        preyAvailabilityCol = SpeciesColumnMap[preyName];
        AvailabilityMatrix(PredatorNum,preyAvailabilityCol) = AvailabilityVector(preyCol);
    }
}

void
nmfAvailabilityModelAPI::calculateModifiedHollingTypeIIhalf(
        QStringList&                           HeaderParts,
        QStringList&                           FileParts,
        double&                                Exponent,
        QString&                               PredatorName,
        int&                                   PredatorNum,
        std::map<QString,PredatorStruct>&      PredatorMap,
        boost::numeric::ublas::matrix<double>& AvailabilityMatrix)
{
    m_Logger->logMsg(nmfConstants::Normal,"Calling species ["+PredatorName.toStdString()+"] functional response: calculateModifiedHollingTypeII½");
    int numPrey = FileParts.size()-3;
    int preyAvailabilityCol;
    int preyNum;
    int preyCol;
    double dcPrey = 0;
    double efPred = 0;
    double grPred = 0;
    double clPred = 0;
    double bmPred = 0;
    double bmPrey = 0;
    double totalPreyBiomass         = 0;
    double totalPreyBiomassSquared  = 0;
    double totalPreyBiomassExponent = 0;
    double X = 0;
    double Y = 0;
    double kPrey = 0;
    double den;
    QString preyName;
    std::map<QString,PreyOnlyStruct> PreyOnlyMap;
    std::map<QString,double>         BiomassMap;
    std::map<QString,int>            SpeciesColumnMap;
    boost::numeric::ublas::matrix<double> AvailabilityVector;
    boost::numeric::ublas::matrix<double> matrix;
    boost::numeric::ublas::matrix<double> inverseMatrix;
    boost::numeric::ublas::matrix<double> kMatrix;
    nmfUtils::initialize(matrix,numPrey,numPrey);
    nmfUtils::initialize(kMatrix,numPrey,1);
    nmfUtils::initialize(AvailabilityVector,numPrey,1);

    createPreyOnlyMap(PreyOnlyMap);
    createBiomassMap(PredatorMap,PreyOnlyMap,BiomassMap);
    getTotalPreyBiomass(PredatorMap,PreyOnlyMap,totalPreyBiomass,totalPreyBiomassSquared,
                        Exponent,totalPreyBiomassExponent);
    getSpeciesColumnMap(SpeciesColumnMap);

    clPred = PredatorMap[PredatorName].clearanceRate.toDouble();
    efPred = PredatorMap[PredatorName].efficiency.toDouble();
    grPred = PredatorMap[PredatorName].growthRate.toDouble();

    /*

    p = predator species
    i,j,k = prey species

    dc(pi) = B(p)C(p)(a(pi)B(i))ⁿ / [1 + (C(p)E(p)/g(p)) x ∑(a(prey)B(prey))ⁿ ]

    Ex. For 3 prey species:

    dc(i)  = B(p)Bⁿ(i)aⁿ(pi)C(p) / [1 + (C(p)E(p)/g(p))(aⁿ(pi)Bⁿ(i)+aⁿ(pj)ⁿB(j)+aⁿ(pk)Bⁿ(k))]
    dc(j)  = B(p)Bⁿ(j)aⁿ(pj)C(p) / [1 + (C(p)E(p)/g(p))(aⁿ(pi)Bⁿ(i)+aⁿ(pj)ⁿB(j)+aⁿ(pk)Bⁿ(k))]
    dc(k)  = B(p)Bⁿ(k)aⁿ(pk)C(p) / [1 + (C(p)E(p)/g(p))(aⁿ(pi)Bⁿ(i)+aⁿ(pj)ⁿB(j)+aⁿ(pk)Bⁿ(k))]

    Need to solve for a(pi), a(pj), and a(pk).

    Let X(p) = B(p)C(p) and Y(p) = C(p)E(p)/g(p) we get:

    dc(i)  = X(p)Bⁿ(i)aⁿ(pi) / [1 + Y(p)(aⁿ(pi)Bⁿ(i)+aⁿ(pj)Bⁿ(j)+aⁿ(pk)Bⁿ(k))]
    dc(j)  = X(p)Bⁿ(j)aⁿ(pj) / [1 + Y(p)(aⁿ(pi)Bⁿ(i)+aⁿ(pj)Bⁿ(j)+aⁿ(pk)Bⁿ(k))]
    dc(k)  = X(p)Bⁿ(k)aⁿ(pk) / [1 + Y(p)(aⁿ(pi)Bⁿ(i)+aⁿ(pj)Bⁿ(j)+aⁿ(pk)Bⁿ(k))]

    Cross multiplying and with some algebraic manipulation...

                  aⁿ(pi) - Y(p)k(i)Bⁿ(j)aⁿ(pj) - Y(p)k(i)Bⁿ(k)aⁿ(pk) = k(i)
    -Y(p)k(j)Bⁿ(i)aⁿ(pi) +              aⁿ(pj) - Y(p)k(j)Bⁿ(k)aⁿ(pk) = k(j)
    -Y(p)k(k)Bⁿ(i)aⁿ(pi) - Y(p)k(k)Bⁿ(j)aⁿ(pj) +              aⁿ(pk) = k(k)

    where k(i) = dc(i) / [Bⁿ(i)((X(p) - dc(i)Y(p))] and similarly for k(j) and k(k).

    Putting in matrix form...

    ⎛      1        -Y(p)k(i)Bⁿ(j) -Y(p)k(i)Bⁿ(k) ⎞ ⎛aⁿ(pi)⎞     ⎛k(i)⎞
    ⎜-Y(p)k(j)Bⁿ(i)       1        -Y(p)k(j)Bⁿ(k) ⎟ ⎜aⁿ(pj)⎟  =  ⎜k(j)⎟
    ⎝-Y(p)k(k)Bⁿ(i) -Y(p)k(k)Bⁿ(j)       1        ⎠ ⎝aⁿ(pk)⎠     ⎝k(k)⎠


    It follows then by taking the inverse of the 3x3 matrix on the left, we can find the values
    for the availability vector (a) (and after taking some nth roots), which was the original goal.

    N.B. In the code below, n is defined as Exponent.

    */


    //  1. Create temporary variables to keep simplify equations
    bmPred = BiomassMap[PredatorName];
    X = bmPred * clPred;
    Y = (clPred*efPred)/grPred;

    //  2. Build numPrey x numPrey matrix for current predator
    for (int row=0; row<numPrey; ++row) {
        preyNum  = row+3;
        dcPrey   = FileParts[preyNum].toDouble();
        preyName = HeaderParts[preyNum];
        bmPrey   = BiomassMap[preyName];
        den      = std::pow(bmPrey,Exponent) * (X - Y * dcPrey);
        if (den == 0) {
            std::cout << "Error (calculateModifiedHollingTypeII½): Found denominator=0" << std::endl;
            return;
        }
        kPrey = dcPrey / den;
        kMatrix(row,0) = kPrey;
        for (int col=0; col<numPrey; ++col) {
            preyCol = col+3;
            preyName = HeaderParts[preyCol];
            bmPrey = BiomassMap[preyName];
            if (col == row) {
                matrix(row,col) = 1.0;
            } else {
                matrix(row,col) = -Y * kPrey * std::pow(bmPrey,Exponent);
            }
        }
    }

    //  3. Find inverse of that matrix
    if (! nmfUtils::invertMatrix(matrix,inverseMatrix)) {
        std::cout << "Error: matrix not invertible" << std::endl;
        return;
    }

    //  4. Multiply inverse by kMatrix constants to get the final Availability vector
    AvailabilityVector = boost::numeric::ublas::prod(inverseMatrix,kMatrix);

    //  5. The values in the vector correspond to the Availability matrix values
    for (int preyCol=0; preyCol<numPrey; ++preyCol) {
        preyName = HeaderParts[preyCol+3];
        preyAvailabilityCol = SpeciesColumnMap[preyName];
        AvailabilityMatrix(PredatorNum,preyAvailabilityCol) = std::pow(AvailabilityVector(preyCol),1.0/Exponent);
    }
}

void
nmfAvailabilityModelAPI::calculateModifiedHollingTypeIII(
        QStringList&                           HeaderParts,
        QStringList&                           FileParts,
        double&                                Exponent,
        QString&                               PredatorName,
        int&                                   PredatorNum,
        std::map<QString,PredatorStruct>&      PredatorMap,
        boost::numeric::ublas::matrix<double>& AvailabilityMatrix)
{
    m_Logger->logMsg(nmfConstants::Normal,"Calling species ["+PredatorName.toStdString()+"] functional response: calculateModifiedHollingTypeIII");
    int numPrey = FileParts.size()-3;
    int preyAvailabilityCol;
    int preyNum;
    int preyCol;
    double dcPrey = 0;
    double efPred = 0;
    double grPred = 0;
    double clPred = 0;
    double bmPred = 0;
    double bmPrey = 0;
    double totalPreyBiomass         = 0;
    double totalPreyBiomassSquared  = 0;
    double totalPreyBiomassExponent = 0;
    double X = 0;
    double Y = 0;
    double kPrey = 0;
    double den;
    QString preyName;
    std::map<QString,PreyOnlyStruct> PreyOnlyMap;
    std::map<QString,double>         BiomassMap;
    std::map<QString,int>            SpeciesColumnMap;
    boost::numeric::ublas::matrix<double> AvailabilityVector;
    boost::numeric::ublas::matrix<double> matrix;
    boost::numeric::ublas::matrix<double> inverseMatrix;
    boost::numeric::ublas::matrix<double> kMatrix;
    nmfUtils::initialize(matrix,numPrey,numPrey);
    nmfUtils::initialize(kMatrix,numPrey,1);
    nmfUtils::initialize(AvailabilityVector,numPrey,1);

    createPreyOnlyMap(PreyOnlyMap);
    createBiomassMap(PredatorMap,PreyOnlyMap,BiomassMap);
    getTotalPreyBiomass(PredatorMap,PreyOnlyMap,totalPreyBiomass,totalPreyBiomassSquared,
                        Exponent,totalPreyBiomassExponent);
    getSpeciesColumnMap(SpeciesColumnMap);

    clPred = PredatorMap[PredatorName].clearanceRate.toDouble();
    efPred = PredatorMap[PredatorName].efficiency.toDouble();
    grPred = PredatorMap[PredatorName].growthRate.toDouble();

    /*

    p = predator species
    i,j,k = prey species

    dc(pi) = B(p)C(p)(a(pi)B(i))² / [1 + (C(p)E(p)/g(p)) x ∑(a(prey)B(prey))² ]

    Ex. For 3 prey species:

    dc(i)  = B(p)B²(i)a²(pi)C(p) / [1 + (C(p)E(p)/g(p))(a²(pi)B²(i)+a²(pj)²B(j)+a²(pk)B²(k))]
    dc(j)  = B(p)B²(j)a²(pj)C(p) / [1 + (C(p)E(p)/g(p))(a²(pi)B²(i)+a²(pj)²B(j)+a²(pk)B²(k))]
    dc(k)  = B(p)B²(k)a²(pk)C(p) / [1 + (C(p)E(p)/g(p))(a²(pi)B²(i)+a²(pj)²B(j)+a²(pk)B²(k))]

    Need to solve for a(pi), a(pj), and a(pk).

    Let X(p) = B(p)C(p) and Y(p) = C(p)E(p)/g(p) we get:

    dc(i)  = X(p)B²(i)a²(pi) / [1 + Y(p)(a²(pi)B²(i)+a²(pj)B²(j)+a²(pk)B²(k))]
    dc(j)  = X(p)B²(j)a²(pj) / [1 + Y(p)(a²(pi)B²(i)+a²(pj)B²(j)+a²(pk)B²(k))]
    dc(k)  = X(p)B²(k)a²(pk) / [1 + Y(p)(a²(pi)B²(i)+a²(pj)B²(j)+a²(pk)B²(k))]

    Cross multiplying and with some algebraic manipulation...

                  a²(pi) - Y(p)k(i)B²(j)a²(pj) - Y(p)k(i)B²(k)a²(pk) = k(i)
    -Y(p)k(j)B²(i)a²(pi) +              a²(pj) - Y(p)k(j)B²(k)a²(pk) = k(j)
    -Y(p)k(k)B²(i)a²(pi) - Y(p)k(k)B²(j)a²(pj) +              a²(pk) = k(k)

    where k(i) = dc(i) / [B²(i)((X(p) - dc(i)Y(p))] and similarly for k(j) and k(k).

    Putting in matrix form...

    ⎛      1        -Y(p)k(i)B²(j) -Y(p)k(i)B²(k) ⎞ ⎛a²(pi)⎞     ⎛k(i)⎞
    ⎜-Y(p)k(j)B²(i)       1        -Y(p)k(j)B²(k) ⎟ ⎜a²(pj)⎟  =  ⎜k(j)⎟
    ⎝-Y(p)k(k)B²(i) -Y(p)k(k)B²(j)       1        ⎠ ⎝a²(pk)⎠     ⎝k(k)⎠


    It follows then by taking the inverse of the 3x3 matrix on the left, we can find the values
    for the availability vector (a) (and after taking some square roots), which was the original goal.

    */


    //  1. Create temporary variables to keep simplify equations
    bmPred = BiomassMap[PredatorName];
    X = bmPred * clPred;
    Y = (clPred*efPred)/grPred;

    //  2. Build numPrey x numPrey matrix for current predator
    for (int row=0; row<numPrey; ++row) {
        preyNum = row+3;
        dcPrey = FileParts[preyNum].toDouble();
        preyName = HeaderParts[preyNum];
        bmPrey = BiomassMap[preyName];
        den = (bmPrey*bmPrey) * (X - Y * dcPrey);
        if (den == 0) {
            std::cout << "Error (calculateModifiedHollingTypeIII): Found denominator=0" << std::endl;
            return;
        }
        kPrey = dcPrey / den;
        kMatrix(row,0) = kPrey;
        for (int col=0; col<numPrey; ++col) {
            preyCol = col+3;
            preyName = HeaderParts[preyCol];
            bmPrey = BiomassMap[preyName];
            if (col == row) {
                matrix(row,col) = 1.0;
            } else {
                matrix(row,col) = -Y * kPrey * (bmPrey*bmPrey);
            }
        }
    }

    //  3. Find inverse of that matrix
    if (! nmfUtils::invertMatrix(matrix,inverseMatrix)) {
        std::cout << "Error: matrix not invertible" << std::endl;
        return;
    }

    //  4. Multiply inverse by kMatrix constants to get the final Availability vector
    AvailabilityVector = boost::numeric::ublas::prod(inverseMatrix,kMatrix);

    //  5. The values in the vector correspond to the Availability matrix values
    for (int preyCol=0; preyCol<numPrey; ++preyCol) {
        preyName = HeaderParts[preyCol+3];
        preyAvailabilityCol = SpeciesColumnMap[preyName];
        AvailabilityMatrix(PredatorNum,preyAvailabilityCol) = sqrt(AvailabilityVector(preyCol));
    }
}
