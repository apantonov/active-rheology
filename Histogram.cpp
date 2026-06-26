#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "Histogram.h"

Histogram::Histogram(real_t _x_min, real_t _x_max, int _n_bins) {
    x_min = _x_min;
    x_max = _x_max;
    n_bins = _n_bins;
    
    if (x_min > x_max) {
        std::cerr << "Histogram error: x_min must be larger than x_max!" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (n_bins <= 0) {
        std::cerr << "Histogram error: n_bins must be a positive integer!" << std::endl;
        exit(EXIT_FAILURE);
    }
    deltaX = x_max - x_min;
    bin_size = deltaX / n_bins;
    counts = new unsigned long int[n_bins];
    reset();
}

void Histogram::addSample(real_t x) {
    int binIdx = floor((x - x_min) / bin_size);
    if (binIdx < 0) {
        counts_below += 1;
    } else if(binIdx >= n_bins) {
        counts_above += 1;
    }
    counts[binIdx] += 1;
    n_total += 1;
}

void Histogram::writeToFile(std::string filename) {
    real_t x = 0.0;
    FILE *histogramFile = fopen(filename.c_str(), "w");
    if (histogramFile == NULL) {
        std::cerr << "Histogram error: Unable to open histogram file " << filename << std::endl;
        exit(EXIT_FAILURE);
    }
    fprintf(histogramFile, "-Inf, %f, %lu\n", x_min, counts_below);
    for (int i = 0; i < n_bins; ++i) {
        x = x_min + i*bin_size;
        fprintf(histogramFile, "%f, %f, %lu\n", x, x+bin_size, counts[i]);
    }
    fprintf(histogramFile, "%f, +Inf, %lu\n", x_max, counts_above);
    fclose(histogramFile);
}

void Histogram::printCounts() {
    for (int i = 0; i < n_bins; ++i) {
        printf("%d: %lu\n", i, counts[i]);
    }
    printf("Below: %lu, Above: %lu\n", counts_below, counts_above);
}

void Histogram::reset() {
    for (int i = 0; i < n_bins; ++i) {
        counts[i] = 0;
    }
    n_total = 0;
    counts_below = 0;
    counts_above = 0;
}


/**********************************************************************************************/
/*                                     Histogram Integer                                      */
/**********************************************************************************************/

HistogramInteger::HistogramInteger(int _i_min, int _i_max) {
    i_min = _i_min;
    i_max = _i_max;
    n_bins = i_max - i_min;
    
    if (i_min > i_max) {
        std::cerr << "HistogramInteger error: i_min must be larger than i_max!" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (n_bins <= 0) {
        std::cerr << "HistogramInteger error: n_bins must be a positive integer!" << std::endl;
        exit(EXIT_FAILURE);
    }
    counts = new unsigned long int[n_bins];
    reset();
}

void HistogramInteger::addSample(int i) {
    int binIdx = i - i_min;
    if (binIdx < 0) {
        counts_below += 1;
    } else if(binIdx >= n_bins) {
        counts_above += 1;
    }
    counts[binIdx] += 1;
    n_total += 1;
}

void HistogramInteger::writeToFile(std::string filename) {
    FILE *histogramFile = fopen(filename.c_str(), "w");
    if (histogramFile == NULL) {
        std::cerr << "HistogramInteger error: Unable to open histogram file " << filename << std::endl;
        exit(EXIT_FAILURE);
    }
    fprintf(histogramFile, "-Inf, %lu\n", counts_below);
    for (int i = 0; i < n_bins; ++i) {
        fprintf(histogramFile, "%d, %lu\n", i+i_min, counts[i]);
    }
    fprintf(histogramFile, "+Inf, %lu\n", counts_above);
    fclose(histogramFile);
}

void HistogramInteger::printCounts() {
    for (int i = 0; i < n_bins; ++i) {
        printf("%d: %lu\n", i, counts[i]);
    }
    printf("Below: %lu, Above: %lu\n", counts_below, counts_above);
}

void HistogramInteger::reset() {
    for (int i = 0; i < n_bins; ++i) {
        counts[i] = 0;
    }
    n_total = 0;
    counts_below = 0;
    counts_above = 0;
}

/**********************************************************************************************/
/*                                        Histogram 2D                                        */
/**********************************************************************************************/

Histogram2D::Histogram2D(real_t _x_min, real_t _x_max, int _x_n_bins, 
                         real_t _y_min, real_t _y_max, int _y_n_bins) 
{
    x_min = _x_min;
    x_max = _x_max;
    x_n_bins = _x_n_bins;
    
    y_min = _y_min;
    y_max = _y_max;
    y_n_bins = _y_n_bins;
    
    if (x_min > x_max) {
        std::cerr << "Histogram2D error: x_min must be larger than x_max!" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (x_n_bins <= 0) {
        std::cerr << "Histogram2D error: x_n_bins must be a positive integer!" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    if (y_min > y_max) {
        std::cerr << "Histogram2D error: y_min must be larger than y_max!" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (y_n_bins <= 0) {
        std::cerr << "Histogram2D error: y_n_bins must be a positive integer!" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    deltaX = x_max - x_min;
    x_bin_size = deltaX / x_n_bins;
    deltaY = y_max - y_min;
    y_bin_size = deltaY / y_n_bins;
    counts = new unsigned long int[x_n_bins*y_n_bins];
    reset();
}

void Histogram2D::addSample(real_t x, real_t y) 
{
    int xbinIdx = floor((x - x_min) / x_bin_size);
    int ybinIdx = floor((y - y_min) / y_bin_size);
    if (xbinIdx < 0 || ybinIdx < 0) {
        counts_below += 1;
    } else if(xbinIdx >= x_n_bins || ybinIdx >= y_n_bins) {
        counts_above += 1;
    } else {
        counts[xbinIdx + x_n_bins*ybinIdx] += 1;
    }
    n_total += 1;
}

void Histogram2D::writeToFile(std::string filename) 
{
    //real_t x = 0.0, y = 0.0;
    FILE *histogramFile = fopen(filename.c_str(), "w");
    if (histogramFile == NULL) {
        std::cerr << "Histogram error: Unable to open histogram file " << filename << std::endl;
        exit(EXIT_FAILURE);
    }
    //fprintf(histogramFile, "-Inf, %f, %lu\n", x_min, counts_below);
    for (int i = 0; i < x_n_bins; ++i) {
        //x = x_min + i*x_bin_size;
        for (int j = 0; j < y_n_bins-1; ++j) {
            //y = y_min + i*y_bin_size;
            //fprintf(histogramFile, "%f, %f, %f, %f, %lu", x, x+x_bin_size, y, y+y_bin_size, counts[i + j*x_n_bins]);
            fprintf(histogramFile, "%lu, ", counts[i + j*x_n_bins]);
        }
        fprintf(histogramFile, "%lu\n", counts[i + (y_n_bins-1)*x_n_bins]);
    }
    //fprintf(histogramFile, "%f, +Inf, %lu\n", x_max, counts_above);
    fclose(histogramFile);
}

void Histogram2D::printCounts() 
{
    for (int i = 0; i < x_n_bins; ++i) {
        for (int j = 0; j < y_n_bins-1; ++j) {
            printf("%lu, ", counts[i + j*x_n_bins]);
        }
        printf("%lu\n", counts[i + (y_n_bins-1)*x_n_bins]);
    }
    printf("Below: %lu, Above: %lu\n", counts_below, counts_above);
}

void Histogram2D::reset() 
{
    for (int i = 0; i < x_n_bins*y_n_bins; ++i) {
        counts[i] = 0;
    }
    n_total = 0;
    counts_below = 0;
    counts_above = 0;
}

/**********************************************************************************************/
/*                                        HistogramValue                                      */
/**********************************************************************************************/

HistogramValue::HistogramValue(real_t _x_min, real_t _x_max, int _n_bins) {
    x_min = _x_min;
    x_max = _x_max;
    n_bins = _n_bins;
    
    if (x_min > x_max) {
        std::cerr << "Histogram error: x_min must be larger than x_max!" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (n_bins <= 0) {
        std::cerr << "Histogram error: n_bins must be a positive integer!" << std::endl;
        exit(EXIT_FAILURE);
    }
    deltaX = x_max - x_min;
    bin_size = deltaX / n_bins;
    samplesInBin = new unsigned long int[n_bins];
    values = new real_t[n_bins];
    reset();
}

void HistogramValue::addSample(real_t x, real_t value) {
    int binIdx = floor((x - x_min) / bin_size);
    if (binIdx < 0) {
        counts_below += 1;
    } else if(binIdx >= n_bins) {
        counts_above += 1;
    }
    unsigned long int N = samplesInBin[binIdx];
    values[binIdx] = (N*values[binIdx] + value) / (N+1);
    samplesInBin[binIdx] += 1;
    n_total += 1;
}

void HistogramValue::writeToFile(std::string filename) {
    real_t x = 0.0;
    FILE *histogramFile = fopen(filename.c_str(), "w");
    if (histogramFile == NULL) {
        std::cerr << "Histogram error: Unable to open histogram file " << filename << std::endl;
        exit(EXIT_FAILURE);
    }
    fprintf(histogramFile, "-Inf, %f, %lu\n", x_min, counts_below);
    for (int i = 0; i < n_bins; ++i) {
        x = x_min + i*bin_size;
        fprintf(histogramFile, "%f, %f, %f\n", x, x+bin_size,values[i]);
    }
    fprintf(histogramFile, "%f, +Inf, %lu\n", x_max, counts_above);
    fclose(histogramFile);
}

void HistogramValue::printValues() {
    for (int i = 0; i < n_bins; ++i) {
        printf("%d: %f\n", i, values[i]);
    }
}

void HistogramValue::reset() {
    for (int i = 0; i < n_bins; ++i) {
        samplesInBin[i] = 0;
        values[i] = 0.0;
    }
    n_total = 0;
    counts_below = 0;
    counts_above = 0;
}
