#include "defines.h"

#include <functional>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "cwt_noise_estimator.h"

#define AMIN 0                  /* A min  */
#define ASTP 0.05               /* A step */
#define AMAX 64                 /* A max  */

CWTNoiseEstimator::CWTNoiseEstimator():
	areaParams(nullptr),
	s(nullptr),
	scales(nullptr),
	arr(nullptr),
	copyFromWT(nullptr)
{
}

void CWTNoiseEstimator::clean()
{
	if (areaParams != nullptr) delete areaParams;
	if (s != nullptr) delete s;
	if (scales != nullptr) delete scales;
	if (arr != nullptr) delete arr;
	if (copyFromWT != nullptr) delete copyFromWT;
}

void CWTNoiseEstimator::initialize(SubtractionConfiguration &config)
{
	samplingRate = config.samplingRate;
	spectrumSize = config.spectrumSize;
	fftSize = config.fftSize;

	areaParams = new std::vector<CWTNoiseEstimator::areaParams_>(config.spectrumSize, areaParams_());
	s = new Signal(config.fftSize, NULL, NULL, 1.0, "signal");
	scales = new LinearRangeFunctor(AMIN, ASTP, AMAX);
	arr = new Matrix(config.fftSize + 4, (AMAX - AMIN) / ASTP + 4);
	copyFromWT = new ArrayValueFilter([&](uint i, uint j) { (*arr)[i][j] = wt->mag(j, i) / fftSize; });
}

void CWTNoiseEstimator::writeFiles(std::string dir, int file_no)
{
	std::ofstream file(std::string(dir + "/" + std::to_string(file_no) + ".dat"));
	for (auto i = 0U; i < wt->rows() + 2; i++)
	{
		for (auto j = 0U; j < wt->cols() + 2; j++)
		{
			std::string s = std::string(" " + std::to_string((*arr)[j][i]));
			file.write(s.c_str(), s.length());
		}
		file.write(std::string("\n").c_str(), 1);
	}
	file.close();
#ifdef QT_VERSION

	QFile file(QString(dir) + QString("/%1.dat").arg(file_no));
	file.open(QIODevice::WriteOnly);
	for (auto i = 0U; i < wt->rows() + 2; i++)
	{
		for (auto j = 0U; j < wt->cols() + 2; j++)
		{
			file.write(QString(" %1").arg((*arr)[j][i]).toLatin1().data());
			//          file.write(QString(" %1").arg((arr->getMask())[j][i]).toLatin1().data());
		}
		file.write("\n");
	}
	file.close();
#endif
}


void CWTNoiseEstimator::computeCWT(double *signal)
{
	ReDataProxy data = s->reData();

	// remplir data
	for (auto i = 0U; i < fftSize; ++i)
	{
		data[i] = signal[i];
	}

	wt = CWTalgorithm::cwtft(*s, *scales, ComplexMorlet(), "NOPE");
	arr->setFirstCol(2);
	arr->setFirstRow(2);
	arr->setNumCols(wt->cols());
	arr->setNumRows(wt->rows());
}

void CWTNoiseEstimator::computeAreas()
{
	areas.clear();
	for (uint i = arr->getFirstCol(); i < arr->getNumCols(); ++i)
		for (uint j = arr->getFirstRow(); j < arr->getNumRows(); ++j)
			if ((*arr)[i][j] > 0)
			{
				if (arr->isMasked(i, j)) // already explored area
				{
					while ((*arr)[i][++j] > 0); // need to find more efficient way, by using Area* and jumping
				}
				else
				{
					Area *a = new Area((*arr));
					a->plotContour((*arr), i, j);
					if (a->getWidth() > 1)
					{
						a->computeParameters((*arr));
						if (a->verticalSize() < 100) // Limiting the frequency span of an area
						{
							areas.push_back(a);
						}
						else
						{
							a->removeArea((*arr));
							delete a;
						}
					}
					else
					{
						a->removeArea((*arr));
						delete a;
					}
				}
			}
}

void CWTNoiseEstimator::estimate(double *signal_in, double *noise_power, bool computeMax)
{
	static double maxi = 0, ceil = 0, upperceil = 1000; // Put them in config
	if (computeMax) maxi = 0;

	// Lambdas initialisation
	ArrayValueFilter lowCeiling([&](uint i, uint j) { (*arr)[i][j] = ((*arr)[i][j] > ceil) ? (*arr)[i][j] : 0; });
	ArrayValueFilter dblCeiling([&](uint i, uint j) { (*arr)[i][j] = ((*arr)[i][j] > ceil) ? (((*arr)[i][j] < upperceil) ? (*arr)[i][j] : 0) : 0; });
	ArrayValueFilter updateMax([&](uint i, uint j) { maxi = std::max(maxi, (*arr)[i][j]); });

	computeCWT(signal_in);

	// Copy from wt, and compute maximum by the same occasion
	if (computeMax)
		applyToArr({*copyFromWT, updateMax});
	else
		applyToArr({*copyFromWT});


	// Apply ceiling
	//TODO Get a good ceiling estimation
	if (computeMax) ceil = 0.03; // maxi - 10.0 / fftSize; // 0.03;
	applyToArr({lowCeiling});


	createFilterBinsSeparation();
	computeAreas();

#ifdef PLOT_CWT
	static int file_no = 0; // Find a better way
	writeFiles("dataBefore", file_no++);
#endif

	computeAreasParameters();
	reestimateNoise(noise_power);

	delete wt;
}

void CWTNoiseEstimator::createFilterBinsSeparation()
{
	// TODO
	//  for(int fbin = 0; fbin < spectrumSize; ++fbin)
	//  {
	//      int wbin = getWaveletBinFromFFTBin(fbin);
	//      qDebug() << fbin << wbin;
	////            for(int i = 2; i < wt->cols(); ++i)
	////                arr[i][wbin] = 10;
	//  }
}

void CWTNoiseEstimator::computeAreasParameters()
{
	// Computation of mean musical tone power for each frequency bin
	areaParams->clear();
	for (Area * area : areas)
	{
		if (area->getWidth() >= 2 && area->getNumPixels() > 4)
		{
			//          int bin = std::min(spectrumSize - 1, std::max((unsigned int) 0, getFFTBin(area->getMedianHeight())));
			int bin = std::min(spectrumSize - 1, std::max((unsigned int) 0, getFFTBin(area->getMax().y - 2)));
			(*areaParams)[bin].numAreas += 1;
			//          (*areaParams)[bin].mean     += area->getSumOfValues() / area->getNumPixels();
			(*areaParams)[bin].mean     += area->getMax().val;
		}
		delete area;
	}
}

void CWTNoiseEstimator::reestimateNoise(double *noise_power)
{
	// Decrease of the power according to estimation
	for (auto i = 0U; i < spectrumSize; ++i)
	{
		if ((*areaParams)[i].numAreas != 0  && (*areaParams)[i].mean > 0 && (*areaParams)[i].mean < 1000)
		{
			//TODO get a good power estimation
			//          qDebug() << "noise_power[" << i << "] =" << noise_power[i] << "  subtracted= " << (*areaParams)[i].mean / (*areaParams)[i].numAreas;
			noise_power[i] = std::max(0.0, noise_power[i] -  pow((*areaParams)[i].mean, 2.0) / (*areaParams)[i].numAreas);
		}
	}
}

void CWTNoiseEstimator::applyToArr(std::initializer_list<ArrayValueFilter> funs)
{
	for (uint i = arr->getFirstCol(); i < arr->getNumCols(); ++i)
		for (uint j = arr->getFirstRow(); j < arr->getNumRows(); ++j)
			for (auto f : funs)
				f(i, j);
}

void CWTNoiseEstimator::writeSimpleCWT(double *signal_in)
{
	static int file_no = 0; // find a better way

	computeCWT(signal_in);

	// Copy from wt
	applyToArr({*copyFromWT});
	writeFiles("dataAfter", file_no++);

	delete wt;
}

void CWTNoiseEstimator::clearAreaParams()
{
	std::fill_n(areaParams->begin(), areaParams->size(), areaParams_());
}


double CWTNoiseEstimator::getFreq(int pixel)
{
	return 259500.0 / (pixel - 2.0); // two pixel padding
}

unsigned int CWTNoiseEstimator::getFFTBin(int pixel)
{
	// remove static
	static double f_per_bin = (samplingRate / 2.0) / spectrumSize;
	return std::max((unsigned int) 10, std::min((unsigned int) std::round(getFreq(pixel) / f_per_bin), spectrumSize - 1));
	// TODO 10 empirique, cf. Excel
}

CWTNoiseEstimator::areaParams_::areaParams_(): numAreas(0), mean(0) { }
