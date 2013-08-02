#include "defines.h"

#include <functional>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "cwt_noise_estimator.h"



CWTNoiseEstimator::CWTNoiseEstimator()
{
}

void CWTNoiseEstimator::clean(SubtractionConfiguration &config)
{
	if (config.noise_est_cfg.cwt_areaParams != nullptr) delete config.noise_est_cfg.cwt_areaParams;
	if (config.noise_est_cfg.cwt_signal != nullptr) delete config.noise_est_cfg.cwt_signal;
	if (config.noise_est_cfg.cwt_scales != nullptr) delete config.noise_est_cfg.cwt_scales;
	if (config.noise_est_cfg.cwt_arr != nullptr) delete config.noise_est_cfg.cwt_arr;
	if (config.noise_est_cfg.cwt_copyFromWT != nullptr) delete config.noise_est_cfg.cwt_copyFromWT;
}

void CWTNoiseEstimator::initialize(SubtractionConfiguration &config)
{
	samplingRate = config.samplingRate;
	spectrumSize = config.spectrumSize;
	fftSize = config.fftSize;

	config.noise_est_cfg.cwt_areaParams = new std::vector<NoiseEstimatorDataHolder::areaParams_>(config.spectrumSize, NoiseEstimatorDataHolder::areaParams_());
	config.noise_est_cfg.cwt_signal = new Signal(config.fftSize, NULL, NULL, 1.0, "signal");
	config.noise_est_cfg.cwt_scales = new LinearRangeFunctor(config.noise_est_cfg.cwt_amin, config.noise_est_cfg.cwt_astp, config.noise_est_cfg.cwt_amax);
	config.noise_est_cfg.cwt_arr = new Matrix(config.fftSize + 4, (config.noise_est_cfg.cwt_amax - config.noise_est_cfg.cwt_amin) / config.noise_est_cfg.cwt_astp + 4);
	config.noise_est_cfg.cwt_copyFromWT = new ArrayValueFilter([&](uint i, uint j) { (*config.noise_est_cfg.cwt_arr)[i][j] = config.noise_est_cfg.cwt_wt->mag(j, i) / fftSize; });
}

void CWTNoiseEstimator::writeFiles(SubtractionConfiguration &config, std::string dir, int file_no)
{
	std::ofstream file(std::string(dir + "/" + std::to_string(file_no) + ".dat"));
	for (auto i = 0U; i < config.noise_est_cfg.cwt_wt->rows() + 2; i++)
	{
		for (auto j = 0U; j < config.noise_est_cfg.cwt_wt->cols() + 2; j++)
		{
			std::string s = std::string(" " + std::to_string((*config.noise_est_cfg.cwt_arr)[j][i]));
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


void CWTNoiseEstimator::computeCWT(SubtractionConfiguration &config, double *signal)
{
	ReDataProxy data = config.noise_est_cfg.cwt_signal->reData();

	// remplir data
	for (auto i = 0U; i < fftSize; ++i)
	{
		data[i] = signal[i];
	}

	config.noise_est_cfg.cwt_wt = CWTalgorithm::cwtft(*config.noise_est_cfg.cwt_signal, *config.noise_est_cfg.cwt_scales, ComplexMorlet(), "NOPE");
	config.noise_est_cfg.cwt_arr->setColPadding(2);
	config.noise_est_cfg.cwt_arr->setRowPadding(2);
}

void CWTNoiseEstimator::computeAreas(SubtractionConfiguration &config)
{
	config.noise_est_cfg.cwt_areas.clear();
	for (uint i = config.noise_est_cfg.cwt_arr->getColPadding(); i < config.noise_est_cfg.cwt_wt->cols(); ++i)
		for (uint j = config.noise_est_cfg.cwt_arr->getRowPadding(); j < config.noise_est_cfg.cwt_wt->rows(); ++j)
			if ((*config.noise_est_cfg.cwt_arr)[i][j] > 0)
			{
				if (config.noise_est_cfg.cwt_arr->isMasked(i, j)) // already explored area
				{
					while ((*config.noise_est_cfg.cwt_arr)[i][++j] > 0); // need to find more efficient way, by using Area* and jumping
				}
				else
				{
					Area *a = new Area((*config.noise_est_cfg.cwt_arr));
					a->plotContour((*config.noise_est_cfg.cwt_arr), i, j);
					if (a->getWidth() > 1)
					{
						a->computeParameters((*config.noise_est_cfg.cwt_arr));
						if (a->verticalSize() < 100) // Limiting the frequency span of an area
						{
							config.noise_est_cfg.cwt_areas.push_back(a);
						}
						else
						{
							a->removeArea((*config.noise_est_cfg.cwt_arr));
							delete a;
						}
					}
					else
					{
						a->removeArea((*config.noise_est_cfg.cwt_arr));
						delete a;
					}
				}
			}
}

void CWTNoiseEstimator::estimate(SubtractionConfiguration &config, double *signal_in, double *noise_power, bool computeMax)
{
	//TODO take care
	static double maxi = 0, ceil = 0, upperceil = 1000; // Put them in config
	if (computeMax) maxi = 0;

	// Lambdas initialisation
	ArrayValueFilter lowCeiling([&](uint i, uint j) { (*config.noise_est_cfg.cwt_arr)[i][j] = ((*config.noise_est_cfg.cwt_arr)[i][j] > ceil) ? (*config.noise_est_cfg.cwt_arr)[i][j] : 0; });
	ArrayValueFilter dblCeiling([&](uint i, uint j) { (*config.noise_est_cfg.cwt_arr)[i][j] = ((*config.noise_est_cfg.cwt_arr)[i][j] > ceil) ? (((*config.noise_est_cfg.cwt_arr)[i][j] < upperceil) ? (*config.noise_est_cfg.cwt_arr)[i][j] : 0) : 0; });
	ArrayValueFilter updateMax([&](uint i, uint j) { maxi = std::max(maxi, (*config.noise_est_cfg.cwt_arr)[i][j]); });

	computeCWT(config, signal_in);

	// Copy from wt, and compute maximum by the same occasion
	if (computeMax)
		applyToArr(config, {*config.noise_est_cfg.cwt_copyFromWT, updateMax});
	else
		applyToArr(config, {*config.noise_est_cfg.cwt_copyFromWT});


	// Apply ceiling
	//TODO Get a good ceiling estimation
	if (computeMax) ceil = 0.03; // maxi - 10.0 / fftSize; // 0.03;
	applyToArr(config, {lowCeiling});


	createFilterBinsSeparation();
	computeAreas(config);

#ifdef PLOT_CWT
	static int file_no = 0; // Find a better way
	writeFiles(config, "dataBefore", file_no++);
#endif

	computeAreasParameters(config);
	reestimateNoise(config, noise_power);

	delete config.noise_est_cfg.cwt_wt;
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

void CWTNoiseEstimator::computeAreasParameters(SubtractionConfiguration &config)
{
	// Computation of mean musical tone power for each frequency bin
	config.noise_est_cfg.cwt_areaParams->clear();
	for (Area * area : config.noise_est_cfg.cwt_areas)
	{
		if (area->getWidth() >= 2 && area->getNumPixels() > 4)
		{
			//          int bin = std::min(spectrumSize - 1, std::max((unsigned int) 0, getFFTBin(area->getMedianHeight())));
			int bin = std::min(spectrumSize - 1, std::max((unsigned int) 0, getFFTBin(area->getMax().y - 2)));
			(*config.noise_est_cfg.cwt_areaParams)[bin].numAreas += 1;
			//          (*areaParams)[bin].mean     += area->getSumOfValues() / area->getNumPixels();
			(*config.noise_est_cfg.cwt_areaParams)[bin].mean     += area->getMax().val;
		}
		delete area;
	}
}

void CWTNoiseEstimator::reestimateNoise(SubtractionConfiguration &config, double *noise_power)
{
	// Decrease of the power according to estimation
	for (auto i = 0U; i < spectrumSize; ++i)
	{
		if ((*config.noise_est_cfg.cwt_areaParams)[i].numAreas != 0  && (*config.noise_est_cfg.cwt_areaParams)[i].mean > 0 && (*config.noise_est_cfg.cwt_areaParams)[i].mean < 1000)
		{
			//TODO get a good power estimation
			//          qDebug() << "noise_power[" << i << "] =" << noise_power[i] << "  subtracted= " << (*areaParams)[i].mean / (*areaParams)[i].numAreas;
			noise_power[i] = std::max(0.0, noise_power[i] -  pow((*config.noise_est_cfg.cwt_areaParams)[i].mean, 2.0) / (*config.noise_est_cfg.cwt_areaParams)[i].numAreas);
		}
	}
}

void CWTNoiseEstimator::applyToArr(SubtractionConfiguration &config, std::initializer_list<ArrayValueFilter> funs)
{
	for (uint i = config.noise_est_cfg.cwt_arr->getColPadding(); i < config.noise_est_cfg.cwt_wt->cols(); ++i)
		for (uint j = config.noise_est_cfg.cwt_arr->getRowPadding(); j < config.noise_est_cfg.cwt_wt->rows(); ++j)
			for (auto f : funs)
				f(i, j);
}

void CWTNoiseEstimator::writeSimpleCWT(SubtractionConfiguration &config, double *signal_in)
{
	static int file_no = 0; // find a better way

	computeCWT(config, signal_in);

	// Copy from wt
	applyToArr(config, {*config.noise_est_cfg.cwt_copyFromWT});
	writeFiles(config, "dataAfter", file_no++);

	delete config.noise_est_cfg.cwt_wt;
}

void CWTNoiseEstimator::clearAreaParams(SubtractionConfiguration &config)
{
	std::fill_n(config.noise_est_cfg.cwt_areaParams->begin(), config.noise_est_cfg.cwt_areaParams->size(), NoiseEstimatorDataHolder::areaParams_());
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


