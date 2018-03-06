#define _USE_MATH_DEFINES
#include <tbb/task_scheduler_init.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/blocked_range2d.h>
#include <cstdlib>
#include <math.h>
#include <random>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include <FreeImagePlus.h>


using namespace std;
using namespace std::chrono;
using namespace tbb;

void SequentialGaussianBlurGreyscale() {
	fipImage inputImage;
	inputImage.load("../Images/trump.png");

	unsigned int width = inputImage.getWidth();
	unsigned int height = inputImage.getHeight();
	//unsigned int width = 480;
	//unsigned int height = 360;

	fipImage outputImage;
	outputImage = fipImage(FIT_BITMAP, width, height, 24);

	// Stores gaussian 
	float *gaussianMask = new float[(width * 2 + 1) * (height * 2 + 1)];

	float sigma = 100.0f;
	int radius = 10;

	double meanSpeedup = 0.0f;
	uint64_t numTests = 3;

	for (uint64_t testIndex = 0; testIndex < numTests; ++testIndex) 
	{
		auto st1 = high_resolution_clock::now();
		// First loop calculate gaussian of neighbours and sum up
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				float sum = 0.0f;

				for (int i = -1 - radius; i < 2 + radius; i++)
				{
					for (int j = -1 - radius; j < 2 + radius; j++)
					{
						// skip rows out of range.
						if ((i + x) < 0 || (i + x >= width)) continue;
						// skip columns out of range.
						if ((j + y) < 0 || (j + y >= height)) continue;

						int xx = i + x;
						int yy = j + y;

						float weight = exp(-((xx - j) * (xx - j) + (yy - i) * (yy - i)) / (2 * sigma * sigma)) / (M_PI * 2 * sigma * sigma);
						sum += weight;
					}
				}

				float weight = exp(-((x) * (x) + (y) * (y)) / (2 * sigma * sigma)) / (M_PI * 2 * sigma * sigma);

				gaussianMask[y * width + x] = weight / sum;
			}
		}
		auto st2 = high_resolution_clock::now();
		auto st_dur = duration_cast<microseconds>(st2 - st1);
		std::cout << "sequential blur operation took = " << st_dur.count() << "\n";
	}

	vector<vector<RGBQUAD>> rgbValues;
	rgbValues.resize(height, vector<RGBQUAD>(width));

	RGBQUAD rgb;  //FreeImage structure to hold RGB values of a single pixel

	// Get colour and make it grayscale, also sum up all its neighbours multiplied by their weights
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			float val = 0.0f;
			for (int i = -1 - radius; i < 2 + radius; i++)
			{
				for (int j = -1 - radius; j < 2 + radius; j++)
				{
					// skip rows out of range.
					if ((i + x) < 0 || (i + x >= width)) continue;
					// skip columns out of range.
					if ((j + y) < 0 || (j + y >= height)) continue;

					int xx = i + x;
					int yy = j + y;

					inputImage.getPixelColor(xx, yy, &rgb); //Extract pixel(x,y) colour data and place it in rgb

					// Convert to gray
					float gray = (rgb.rgbRed + rgb.rgbGreen + rgb.rgbBlue) / 3;
					gray = gray * gaussianMask[yy * width + xx];	// Multiply by gaussian value

					val += gray;
				}
			}

			// Set all its pixel
			rgbValues[y][x].rgbRed = val;
			rgbValues[y][x].rgbGreen = val;
			rgbValues[y][x].rgbBlue = val;
		}
	}

	// Set the output image pixel
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			outputImage.setPixelColor(x, y, &rgbValues[y][x]);
		}
	}

	outputImage.save("grey_blurred.png");
}

void ParallelGaussianBlurGreyscale() {
	fipImage inputImage;
	inputImage.load("../Images/trump.png");

	unsigned int width = inputImage.getWidth();
	unsigned int height = inputImage.getHeight();
	//unsigned int width = 480;
	//unsigned int height = 360;

	fipImage outputImage;
	outputImage = fipImage(FIT_BITMAP, width, height, 24);

	// Stores gaussian 
	float *gaussianMask = new float[(width * 2 + 1) * (height * 2 + 1)];

	float sigma = 100.0f;
	
	int radius = 10;

	double meanSpeedup = 0.0f;
	uint64_t numTests = 3;

	for (uint64_t testIndex = 0; testIndex < numTests; ++testIndex)
	{
		auto pt1 = high_resolution_clock::now();
		parallel_for(blocked_range2d<int, int>(0, height, 0, width), [=](const blocked_range2d<int, int>& r)
		{
			for (auto y = r.rows().begin(); y < r.rows().end(); ++y)
			{
				for (auto x = r.cols().begin(); x < r.cols().end(); ++x)
				{
					float sum = 0.0f;
					for (int i = -1 - radius; i < 2 + radius; i++)
					{
						for (int j = -1 - radius; j < 2 + radius; j++)
						{
							// skip rows out of range.
							if ((i + x) < 0 || (i + x >= width)) continue;
							// skip columns out of range.
							if ((j + y) < 0 || (j + y >= height)) continue;

							int xx = i + x;
							int yy = j + y;


							float weight = exp(-((xx - j) * (xx - j) + (yy - i) * (yy - i)) / (2 * sigma * sigma)) / (M_PI * 2 * sigma * sigma);
							sum += weight;
						}
					}
					float weight = exp(-((x) * (x) + (y) * (y)) / (2 * sigma * sigma)) / (M_PI * 2 * sigma * sigma);

					gaussianMask[y * width + x] = weight / sum;
				}
			}
		});
		auto pt2 = high_resolution_clock::now();
		auto pt_dur = duration_cast<microseconds>(pt2 - pt1);
		std::cout << "parallel blur operation took = " << pt_dur.count() << "\n";
	}


	vector<vector<RGBQUAD>> rgbValues;
	rgbValues.resize(height, vector<RGBQUAD>(width));

	RGBQUAD rgb;  //FreeImage structure to hold RGB values of a single pixel

	// Get colour and make it grayscale, also sum up all its neighbours multiplied by their weights
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			float val = 0.0f;
			for (int i = -1 - radius; i < 2 + radius; i++)
			{
				for (int j = -1 - radius; j < 2 + radius; j++)
				{
					// skip rows out of range.
					if ((i + x) < 0 || (i + x >= width)) continue;
					// skip columns out of range.
					if ((j + y) < 0 || (j + y >= height)) continue;

					int xx = i + x;
					int yy = j + y;

					inputImage.getPixelColor(xx, yy, &rgb); //Extract pixel(x,y) colour data and place it in rgb

					// Convert to gray
					float gray = (rgb.rgbRed + rgb.rgbGreen + rgb.rgbBlue) / 3;
					gray = gray * gaussianMask[yy * width + xx];	// Multiply by gaussian value

					val += gray;
				}
			}

			// Set all its pixel
			rgbValues[y][x].rgbRed = val;
			rgbValues[y][x].rgbGreen = val;
			rgbValues[y][x].rgbBlue = val;
		}
	}

	// Set the output image pixel
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			outputImage.setPixelColor(x, y, &rgbValues[y][x]);
		}
	}
	outputImage.save("grey_blurred.png");
}

void RGB_Processed() 
{

	// Setup and load input image dataset
	fipImage inputImage1;
	inputImage1.load("../Images/render_1.png");
	//inputImage1.convertToFloat();

	unsigned int width = inputImage1.getWidth();
	unsigned int height = inputImage1.getHeight();
	//const float* const inputBuffer1 = (float*)inputImage1.accessPixels();

	fipImage inputImage2;
	inputImage2.load("../Images/render_2.png");
	//inputImage2.convertToFloat();

	//unsigned int width = inputImage2.getWidth();
	//unsigned int height = inputImage2.getHeight();
	//const float* const inputBuffer2 = (float*)inputImage2.accessPixels();

	// Setup output image array
	fipImage outputImage;
	outputImage = fipImage(FIT_BITMAP, width, height, 24);
	//outputImage = fipImage(FIT_FLOAT, width, height, 32);
	//float *outputBuffer = (float*)outputImage.accessPixels();

	//2D Vector to hold the RGB colour data of an image
	vector<vector<RGBQUAD>> rgbValues;
	rgbValues.resize(height, vector<RGBQUAD>(width));

	RGBQUAD rgb1;  //FreeImage structure to hold RGB values of a single pixel
	RGBQUAD rgb2;  //FreeImage structure to hold RGB values of a single pixel

	//Extract colour data from image and store it as individual RGBQUAD elements for every pixel
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			inputImage1.getPixelColor(x, y, &rgb1); //Extract pixel(x,y) colour data and place it in rgb
			inputImage2.getPixelColor(x, y, &rgb2); //Extract pixel(x,y) colour data and place it in rgb

			int threshold = 10;
			float gr1 = (rgb1.rgbRed + rgb1.rgbGreen + rgb1.rgbBlue) / 3;
			float gr2 = (rgb2.rgbRed + rgb2.rgbGreen + rgb2.rgbBlue) / 3;
			float val = abs(gr1 - gr2);

			if (val > threshold)
			{
				val = 255;
			}
			else
			{
				val = 0;
			}

			rgbValues[y][x].rgbRed = val;		// 0 - 255 
			rgbValues[y][x].rgbGreen = val;
			rgbValues[y][x].rgbBlue = val;
		}
	}

	//Place the pixel colour values into output image
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			outputImage.setPixelColor(x, y, &rgbValues[y][x]);
		}
	}

	outputImage.save("RGB_processed.png");

}


int main()
{
    int nt = task_scheduler_init::default_num_threads();
    task_scheduler_init T(nt);

    //Part 1 (Greyscale Gaussian blur): -----------DO NOT REMOVE THIS COMMENT----------------------------//
	//SequentialGaussianBlurGreyscale();
	//ParallelGaussianBlurGreyscale();


    //Part 2 (Colour image processing): -----------DO NOT REMOVE THIS COMMENT----------------------------//
	RGB_Processed();

    return 0;
}