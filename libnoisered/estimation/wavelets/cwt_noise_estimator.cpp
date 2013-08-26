#include <functional>
#include <algorithm>
#include <iostream>
#include <fstream>

#include "../../subtraction_manager.h"
#include "cwt_noise_estimator.h"



#define PLOT_CWT

CWTNoiseEstimator::CWTNoiseEstimator()
{
}

CWTNoiseEstimator::CWTNoiseEstimator(const CWTNoiseEstimator & other):
	areaParams(other.areaParams),
	s(other.s),
	wt(other.wt->clone()),
	arr(other.arr),
	areas(other.areas),
	copyFromWT(new ArrayValueFilter([&](unsigned int i, unsigned int j) { arr[i][j] = wt->mag(j, i) / fftSize; }))
{
}

const CWTNoiseEstimator &CWTNoiseEstimator::operator=(const CWTNoiseEstimator & other)
{
	s = other.s;
	areaParams = other.areaParams;
	arr = other.arr;
	areas = other.areas;

	wt = other.wt->clone();
	copyFromWT = new ArrayValueFilter([&](unsigned int i, unsigned int j) { arr[i][j] = wt->mag(j, i) / fftSize; });
	return *this;
}

CWTNoiseEstimator::~CWTNoiseEstimator()
{
	delete wt;
	delete copyFromWT;
}

void CWTNoiseEstimator::initialize(SubtractionManager &config)
{
	samplingRate = config.getSamplingRate();
	spectrumSize = config.spectrumSize();
	fftSize = config.FFTSize();

	areaParams = std::vector<CWTNoiseEstimator::areaParams_>(config.spectrumSize(), areaParams_());
	s = Signal(config.FFTSize(), NULL, NULL, 1.0, "signal");

	arr = MaskedMatrix(config.FFTSize() + 4, (AMAX - AMIN) / ASTP + 4);
	copyFromWT = new ArrayValueFilter([&](unsigned int i, unsigned int j) { arr[i][j] = wt->mag(j, i) / fftSize; });
}

void CWTNoiseEstimator::writeFiles(std::string dir, int file_no)
{
	std::ofstream file(std::string(dir + "/" + std::to_string(file_no) + ".dat"));
	for (auto i = 0U; i < wt->rows() + 2; i++)
	{
		for (auto j = 0U; j < wt->cols() + 2; j++)
		{
			std::string s_tmp = std::string(" " + std::to_string(arr[j][i]));
			file.write(s_tmp.c_str(), s_tmp.length());
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
			file.write(QString(" %1").arg(arr[j][i]).toLatin1().data());
			//          file.write(QString(" %1").arg((arr.getMask())[j][i]).toLatin1().data());
		}
		file.write("\n");
	}
	file.close();
#endif
}


void CWTNoiseEstimator::computeCWT(double *signal)
{
	ReDataProxy data = s.reData();

	// remplir data
	for (auto i = 0U; i < fftSize; ++i)
	{
		data[i] = signal[i];
	}

	wt = CWTalgorithm::cwtft(s, scales, ComplexMorlet(), "NOPE");
	arr.setColPadding(2);
	arr.setRowPadding(2);
}

void CWTNoiseEstimator::computeAreas()
{
	areas.clear();
	for (auto i = arr.getColPadding(); i < wt->cols(); ++i)
		for (auto j = arr.getRowPadding(); j < wt->rows(); ++j)
			if (arr[i][j] > 0)
			{
				if (arr.isMasked(i, j)) // already explored area
				{
					while (arr[i][++j] > 0); // need to find more efficient way, by using Area* and jumping
				}
				else
				{
					Area a;// = new Area();
					a.plotContour(arr, i, j);
					if (a.getWidth() > 1)
					{
						a.computeParameters(arr);
						if (a.verticalSize() < 100) // Limiting the frequency span of an area
						{
							areas.push_back(a);
						}
						else
						{
							a.removeArea(arr);
						}
					}
					else
					{
						a.removeArea(arr);
					}
				}
			}
}

void CWTNoiseEstimator::estimate(double *signal_in, double *noise_power, bool computeMax)
{
	static double maxi = 0, ceil = 0, upperceil = 1000; // Put them in config
	if (computeMax) maxi = 0;

	// Lambdas initialisation
	ArrayValueFilter lowCeiling([&](unsigned int i, unsigned int j)
	{ arr[i][j] = (arr[i][j] > ceil) ? arr[i][j] : 0; });
	ArrayValueFilter dblCeiling([&](unsigned int i, unsigned int j)
	{ arr[i][j] = (arr[i][j] > ceil) ? ((arr[i][j] < upperceil) ? arr[i][j] : 0) : 0; });
	ArrayValueFilter updateMax([&](unsigned int i, unsigned int j)
	{ maxi = std::max(maxi, arr[i][j]); });

	computeCWT(signal_in);

	// Copy from wt, and compute maximum by the same occasion
	if (computeMax)
		applyToArr({*copyFromWT, updateMax});
	else
		applyToArr({*copyFromWT});


	// Apply ceiling
	//TODO Get a good ceiling estimation
	//if (computeMax)
	ceil = 0.03; // maxi - 10.0 / fftSize; // 0.03;
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
	areaParams.clear();
	for (Area  area : areas)
	{
		if (area.getWidth() >= 2 && area.getNumPixels() > 4)
		{
			//          int bin = std::min(spectrumSize - 1, std::max((unsigned int) 0, getFFTBin(area.getMedianHeight())));
			MaskedMatrix::size_type bin = std::min(spectrumSize - 1LU, std::max(0LU, getFFTBin(((long long) area.getMax()._y - (long long) 2))));
			areaParams[bin].numAreas += 1;
			//          areaParams[bin].mean     += area.getSumOfValues() / area.getNumPixels();
			areaParams[bin].mean     += area.getMax().val;
		}
	}
}

void CWTNoiseEstimator::reestimateNoise(double *noise_power)
{
	// Decrease of the power according to estimation
	for (auto i = 0U; i < spectrumSize; ++i)
	{
		if (areaParams[i].numAreas != 0  && areaParams[i].mean > 0 && areaParams[i].mean < 1000)
		{
			//TODO get a good power estimation
			//          qDebug() << "noise_power[" << i << "] =" << noise_power[i] << "  subtracted= " << areaParams[i].mean / areaParams[i].numAreas;
			noise_power[i] = std::max(0.0, noise_power[i] -  pow(areaParams[i].mean, 2.0) / areaParams[i].numAreas);
		}
	}
}

void CWTNoiseEstimator::applyToArr(std::initializer_list<ArrayValueFilter> funs)
{
	for (auto i = arr.getColPadding(); i < wt->cols(); ++i)
		for (auto j = arr.getRowPadding(); j < wt->rows(); ++j)
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
	std::fill_n(areaParams.begin(), areaParams.size(), areaParams_());
}


double CWTNoiseEstimator::getFreq(MaskedMatrix::size_type pixel)
{
	return 259500.0 / double(pixel - 2); // two pixel padding
}

long unsigned int CWTNoiseEstimator::getFFTBin(MaskedMatrix::size_type pixel)
{
	// remove static
	static double f_per_bin = (samplingRate / 2.0) / spectrumSize;
	return std::max(10LU, std::min((long unsigned int)(std::round(getFreq(pixel) / f_per_bin)), spectrumSize - 1LU));
	// TODO 10 empirique, cf. Excel
}

CWTNoiseEstimator::areaParams_::areaParams_(): numAreas(0), mean(0) { }
