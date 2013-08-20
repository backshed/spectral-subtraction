#pragma once

#include "fftmanager.h"
#include <fftw3.h>

class FFTWManager : public FFTManager
{
	public:
		FFTWManager();
		virtual ~FFTWManager();

		virtual void forward() override;
		virtual void backward() override;

		virtual void updateSize(const unsigned int);

	private:
		fftw_plan plan_fw = nullptr; /**< TODO */
		fftw_plan plan_bw = nullptr; /**< TODO */

		static unsigned int _num_instances;
};

