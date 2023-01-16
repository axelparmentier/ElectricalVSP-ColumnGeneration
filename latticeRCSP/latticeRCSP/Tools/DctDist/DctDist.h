//
//  DctDist.h
//  discreteDistributions
//
//  Created by Axel Parmentier on 15/02/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef discreteDistributions_DctDist_h
#define discreteDistributions_DctDist_h

// #define EPS 1e-5 // Constant used to test inequalities

#include <fstream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>


#include "kissFFT/kiss_fft.h"
#include "kissFFT/kiss_fftr.h"
#include "kissFFT/_kiss_fft_guts.h"

//#include <sys/times.h>
#include <time.h>
//#include <unistd.h>


using namespace std;

namespace ToolsAxel {

class DctDist{
protected:
    int minimumT;
    vector<double> f;
    
    //bool useFFT = true;
    
public:
    //-----------------
    // Constructor
    //-----------------
    DctDist(int minimumT = 0); // default
    DctDist(const DctDist &a); // copy
    DctDist(vector<double> f0);
    DctDist(vector<double> f0, int minimumT);
//    DctDist(int l0, vector<double> f0);
    DctDist(string filename);
    DctDist(ifstream &);
    
    //-----------------
    // Intializer
    //-----------------
    void initialize(ifstream &);
//    void initializeRandomDensity();
//    void initializeRandomCumulative();
    
    //----------------
    // Print and save
    //----------------
    void print() const;
    void save(string filename) const;
    void save(ofstream &logs) const;
    void save_gr(ofstream &logs) const;
    
    //------------------
    // Getters and setters
    //------------------
//    int getN() const;
    unsigned int getFsize() const;
    int getMinimumT() const;
    vector<double> getF() const;
    
//    void setN(int N);
    void setMinimumT(int);
    void setF(vector<double> F0);
    
    //-------------------
    // Test equalities
    //-------------------
    bool isEqual(const DctDist &b) const;
    
    //-------------------
    // Integration and derivation
    //-------------------
    void integrate();
    void derive();
    
    double calcIntegral() const;
    
    DctDist cumulativeToDensity() const;
    DctDist densityToCumulative() const;

    //------------------
    // Density and renormalization
    //------------------
    bool isDensity() const;
    bool isCumulative() const;
    double errorCumulative() const;
    
    void normalizeDensityAll();
    bool normalizeDensityLast();
    bool normalizeDensity(); // returns true if only the last element of f was renormalized
    
    bool isIncreasing() const;
    bool normalizeCumulative(); // returns true if it was an increasing function and could be renormalized
    
    
    //-----------------
    // Multiply and divide (a density or a cumulative)
    //-----------------
    void multiplyDensity(int factor);
//    void divideDensity(int quotient);
    
    //-----------------
    // Expectation
    //-----------------
    double expectationDensity() const;
    double expectationCumulative() const;
    
    //-----------------
    // Slack
    //-----------------
//    void slackDensity(int slack);
//    void slackCumulative(int slack);
//    
//    DctDist slackedDensity(int slack) const;
//    DctDist slackedCumulative(int slack) const;
    
    //----------------
    //convolution exact
    //----------------
//    DctDist convolProduct(const DctDist &b, int lmax) const; // build a new DctDist and give it; maximum size of l is given
    DctDist convolProduct(const DctDist &b) const;
    
//    double convolution(const DctDist &b, int lmax); // store the convolution product in this distribution ;  double is the error due to lmax
    void convolution(const DctDist &b);
    
    void classicConvolution(const DctDist &b);
    void fftConvolution(const DctDist &b);
    void runningFftConvolution(const DctDist &b);
    
    void reduceLengthExactCumulative();
    void reduceLengthExactDensity();
    
    
    
    //---------------
    // Fast Fourier transform and fast convolution
    //---------------
    
    
    //---------------
    // Stochastic order
    //---------------
    // usual stochastic order
    bool lessSTcumulative(const DctDist &b) const;
    bool leqSTcumulative(const DctDist &b) const;
    bool greatSTcumulative(const DctDist &b) const;
    bool geqSTcumulative(const DctDist &b) const;
    
    bool lessSTdensity(const DctDist &b) const;
    bool leqSTdensity(const DctDist &b) const;
    bool greatSTdensity(const DctDist &b) const;
    bool geqSTdensity(const DctDist &b) const;
    
    void meetSTcumulative(const DctDist &b);
    void joinSTcumulative(const DctDist &b);

//    
//    DctDist meetSTcumulative(const DctDist &b) const;
//    DctDist joinSTcumulative(const DctDist &a) const;
    
    //---------------
    // First Wasserstein Distance
    //---------------
    double firstWassersteinDistance(const DctDist & b) const;
    
    //---------------
    // Generate realization
    //---------------
    int generateRealization() const;
    vector<double> generateNrealizations(int n) const;

    //---------------
    // Quantiles
    //---------------
    int getQuantile(double) const;

private:
    int generateRealizatonDensity() const;
    
    
};
}

#endif
