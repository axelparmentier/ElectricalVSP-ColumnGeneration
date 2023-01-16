//
//  DctDist.cpp
//  discreteDistributions
//
//  Created by Axel Parmentier on 15/02/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "DctDist.h"
#define EPS 1e-5

namespace ToolsAxel {


//-------------------
// Constructors
//-------------------

// default
DctDist::DctDist(int minimumT0){
    minimumT = minimumT0;
    f = vector<double>(0);
}

// Copy
DctDist::DctDist(const DctDist &a){
    minimumT = a.getMinimumT();
    f = a.getF();
}

DctDist::DctDist(vector<double> f0){
    minimumT= 0;
    f = f0;
}

DctDist::DctDist(vector<double> f0, int minimumT0){
    minimumT = minimumT0;
    f=f0;
}

DctDist::DctDist(string filename){
    ifstream read(filename.c_str());
    if (read.is_open()){
        initialize(read);
    }
    else cout << "Unable to open " << filename << "for DctDist initialization" << endl;
}

DctDist::DctDist(ifstream &read){
    initialize(read);
}

//-----------------
// Intializer
//-----------------
void DctDist::initialize(ifstream &read){
    string dust;
    int n;
    minimumT = 0;
    read >> dust >> n;
    f = vector<double>(n);
    read >> dust;
    for (int i=0; i<n; i++) {
        read >> f[i];
    }
}

//void DctDist::initializeRandomDensity(){
//    for (int i=0; i<f.size(); i++) {
//        f[i] = rand();
//    }
//    normalizeDensityAll();
//}
//
//void DctDist::initializeRandomCumulative(){
//    initializeRandomDensity();
//    integrate();
//}

//------------------
//print and save
//------------------

void DctDist::print() const{
    cout << "n: " << f.size() << endl;
    cout << "f:";
    for (int i = 0; i<f.size(); i++) {
        cout << " " << f[i];
    }
    cout << endl;
}

void DctDist::save(ofstream &logs) const{
    logs << "n: " << f.size() << endl;
    logs << "f:";
    for (int i = 0; i<f.size(); i++) {
        logs << " " << f[i];
    }
    logs << endl;
}

void DctDist::save_gr(ofstream &logs) const{
    logs << " dist nT " << f.size();
    logs << " f ";
    for (int i = 0; i<f.size(); i++) {
        logs << " " << f[i];
    }
}

void DctDist::save(string filename) const{
    ofstream logs(filename.c_str());
    if (logs.is_open()) {
        save(logs);
    }
    else{
        cout << "Unable to open " << filename << " for saving DctDist" << endl;
    }
}

//----------------
// Getters and Setters
//----------------

int DctDist::getMinimumT() const{
    return minimumT;
}

vector<double> DctDist::getF() const {
    return f;
}
    
    unsigned int DctDist::getFsize() const {
        return f.size();
    }

//void DctDist::setN(int n0){
//    n = n0;
//}
    
    void DctDist::setMinimumT(int minimumT0){
        minimumT = minimumT0;
    }

void DctDist::setF(vector<double> f0){
    f = f0;
}

//----------------
// Test equality
//----------------

bool DctDist::isEqual(const DctDist &b) const{
    bool solution = true;
    if (minimumT == b.getMinimumT()) {
        vector<double> f1 = b.getF();
        for (int i = 0; i<f.size(); i++) {
            solution &= (fabs(f[i] - f1[i]) < EPS);
        }
    }
    else solution = false;
    return solution;
}

//-----------------
// Integration and derivation
//-----------------

void DctDist::integrate(){
    for (int i=1; i<f.size(); i++) {
        f[i] += f[i-1];
    }
}

void DctDist::derive(){
    for (int i = f.size()-1; i>0; i--) {
        f[i] -= f[i-1];
    }
}

double DctDist::calcIntegral() const{
    double solution = 0;
    for (int i=0; i<f.size(); i++) {
        solution += fabs(f[i]);
    }
    return solution;
}

DctDist DctDist::densityToCumulative() const{
    DctDist solution(*this);
    solution.integrate();
    return solution;
}

DctDist DctDist::cumulativeToDensity() const{
    DctDist solution(*this);
    solution.derive();
    return solution;
}

//------------------
// Density and renormalization
//------------------

bool DctDist::isDensity() const{
    return (fabs(calcIntegral() - 1) < EPS);
}

bool DctDist::isIncreasing() const{
    bool solution = true;
    for (int i=1; i<f.size(); i++) {
        solution &= f[i] - f[i-1] > - EPS;
    }
    return solution;
}

bool DctDist::isCumulative() const{
    return isIncreasing() && (fabs(f[f.size()-1] - 1) < EPS);
}

double DctDist::errorCumulative() const{
    double solution = 0;
    for (int t = 1; t<f.size(); t++) {
        solution = max(solution,f[t-1]-f[t]);
    }
    solution = max(solution,fabs(f[f.size()-1]-1));
    return solution;
    
}

void DctDist::normalizeDensityAll(){
    double norm = calcIntegral();
    if (norm == INFINITY) cout << "inifinity" << endl;
    for (int i = 0; i<f.size(); i++) {
        f[i] /= norm;
    }
}

bool DctDist::normalizeDensityLast(){
    double norm = calcIntegral();
    if (norm - 1 < f[f.size()-1] - EPS) {
        f[f.size()-1] += 1 - norm;
        return true;
    }
    else return false;
}

bool DctDist::normalizeDensity(){
    if (normalizeDensityLast()) return true;
    else {
        normalizeDensityAll();
        return false;
    }
}

bool DctDist::normalizeCumulative(){
    if (!isIncreasing()) return false;
    else {
        for (int i = 0; i<f.size()-1; i++) {
            f[i] /= f[f.size()-1];
        }
        return true;
    }
}

//-----------------
// Multiply and divide (a density or a cumulative)
//-----------------

void DctDist::multiplyDensity(int factor){
    minimumT *= factor;
    if (f.size() > 1) {
        int newN = factor*(f.size()-1) + 1;
        vector<double>newF(newN);
        newF[0]=f[0];
        for (int i = 1; i<f.size(); i++) {
            for (int j=0; j<factor; j++) {
                newF[1 + factor*(i-1) +j] = f[i] /(double) factor;
            }
        }
        f = newF;
    }
    //if (!isDensity()) throw "aie";
}

//void DctDist::divideDensity(int quotient){
//    if (minimumT ) {
//        <#statements#>
//    }
//    if (n > 1) {
//        int newN = ceil((double)(n-1) /(double) (double)quotient) + 1;
//        vector<double>newF(newN);
//        newF[0]=f[0];
//        for (int i = 1; i<newN; i++) {
//            for (int j=0; j<quotient; j++) {
//                if (1 + quotient*(i-1) +j <n) newF[i] += f[1 + quotient*(i-1) +j];
//            }
//        }
//        n = newN;
//        f = newF;
//    }
//    //if (!isDensity()) throw "aie";
//}

//-----------------
// Expectation density
//-----------------

double DctDist::expectationDensity() const{
    double exp = minimumT;
    for (int t=1; t<f.size(); t++) {
        exp += t*f[t];
    }
    return  exp;
}

double DctDist::expectationCumulative() const{
    double exp = minimumT;
    for (int t=1; t<f.size(); t++) {
        exp += t*(f[t]-f[t-1]);
    }
    return exp;
}


//-----------------
// Slack
//-----------------

//void DctDist::slackDensity(int slack){
//    if (slack) {
//        <#statements#>
//    }
//    
//    if (slack < f.size() - 1) {
//        vector<double> fNew(n-slack,0);
//        for (int i = 0; i < slack + 1; i++) {
//            fNew[0] += f[i];
//        }
//        for (int i = slack + 1; i<n; i++) {
//            fNew[i - slack] = f[i];
//        }
//        n -= slack;
//        f = fNew;
//    }
//    else {
//        n = 1;
//        f = vector<double>(1,1);
//    }
//}
//
//void DctDist::slackCumulative(int slack){
//    if (slack < n-1) {
//        vector<double> fNew(n-slack,0);
//        for (int i = slack; i<n; i++){
//            fNew[i - slack] =  f[i];
//        }
//        n -= slack;
//        f = fNew;
//    }
//    else {
//        n = 1;
//        f = vector<double>(1,1);
//    }
//}

//DctDist DctDist::slackedDensity(int slack) const{
//    DctDist solution(*this);
//    solution.slackDensity(slack);
//    return solution;
//}
//
//DctDist DctDist::slackedCumulative(int slack) const{
//    DctDist solution(*this);
//    solution.slackCumulative(slack);
//    return solution;
//}

//--------------------
// Convolution Product
//--------------------



//double DctDist::convolution(const DctDist &b, int lmax){
//    int nb = b.getN();
//    if (n + nb - 1 <= lmax) {
//        convolution(b);
//        return 0;
//    }
//    else {
//        double solution = 0;
//        int newN = lmax;
//        vector<double> newF(lmax);
//        vector<double> fb = b.getF();
//        for (int t = 0; t< lmax; t++) {
//            for (int i = max(t- (nb-1),0); i < min(t+1,n); i++) {
//                newF[t] += f[i] * fb[t-i];
//            }
//        }
//        for (int t = lmax; t< n + nb -1; t++) {
//            for (int i = max(t- (nb-1),0); i < min(t+1,n); i++) {
//                newF[lmax - 1] += f[i] * fb[t-i];
//                solution += fabs(f[i] * fb[t-i]);
//            }
//        }
//        n = newN;
//        f = newF;
//        return solution;
//    }
//    reduceLengthExactDensity();
//}

void DctDist::convolution(const DctDist &b) {
    /*
    if (useFFT){
        fftConvolution(b);
    }
    else {
        classicConvolution(b);
    }
     */
    minimumT += b.getMinimumT();
    int n = f.size();
    int nb = b.getFsize();
    int m = min(n,nb);
    if (m<20) {
        classicConvolution(b);
    }
    else{
        if (8*m < n) {
            runningFftConvolution(b);
        }
        else if (10 * n < b.getFsize()) {
            DctDist c=b;
            c.runningFftConvolution(*this);
            f = c.getF();
        }
        else fftConvolution(b);
    }
    reduceLengthExactDensity();
}

void DctDist::classicConvolution(const DctDist &b){
    // minimumT is take into account in convolution() and not in this function
    int nb = b.getFsize();
    int n = f.size();
    vector<double> fb = b.getF();
    int newN = n + nb - 1;
    vector<double> newF(newN,0);
    for (int t = 0; t< newN; t++) {
        for (int i = max(t- (nb-1),0); i < min(t+1,n); i++) {
            newF[t] += f[i] * fb[t-i];
        }
    }
//    n = newN;
    f = newF;
}

void DctDist::fftConvolution(const DctDist &b){
    // minimumT is take into account in convolution() and not in this function
    int n = f.size();
    int nb = b.getFsize();
    int nfft = 1;
    while (nfft < n+nb-1) {
        nfft *=2;
    }
    //int nfft = n+nb - 1;
    if (nfft%2 ==1) nfft++;
    vector<double> fb = b.getF();
    
    kiss_fft_scalar* rin_a = new kiss_fft_scalar[nfft];
    kiss_fft_scalar* rin_b = new kiss_fft_scalar[nfft];
    kiss_fft_scalar* rout = new kiss_fft_scalar[nfft];
    /*
     kiss_fft_cpx sout_a[nfft];
     kiss_fft_cpx sout_b[nfft];
     kiss_fft_cpx sout[nfft];
     */
    
    //vector<double> rb(nfft);
    
    kiss_fft_cpx* sout_a = new kiss_fft_cpx[nfft / 2 + 1];
	kiss_fft_cpx* sout_b = new kiss_fft_cpx[nfft / 2 + 1];
	kiss_fft_cpx* sout = new kiss_fft_cpx[nfft / 2 + 1];
    
    for (int i = 0; i<n; i++) {
        rin_a[i] = f[i];
    }
    for (int i=n; i<nfft; i++) {
        rin_a[i] = 0;
    }
    
    
    for (int i = 0; i<nb; i++) {
        rin_b[i] = fb[i];
    }
    for (int i=nb; i<nfft; i++) {
        rin_b[i] = 0;
    }
    //rin[n] = 0;
    //rin[n+1] = 0;
    
    kiss_fftr_cfg kiss_fftr_state = kiss_fftr_alloc(nfft,0,0,0);
    kiss_fftr(kiss_fftr_state,rin_a,sout_a);
    
    free(kiss_fftr_state);
    kiss_fftr_state = kiss_fftr_alloc(nfft,0,0,0);
    kiss_fftr(kiss_fftr_state,rin_b,sout_b);
    
    /*
     vector<double> r(nfft/2+1);
     vector<double> i(nfft/2+1);
     */
    
    for (int t=0; t< nfft/2+1; t++) {
        //for (int t=0; t< nfft; t++) {
        
        sout[t].r = sout_a[t].r * sout_b[t].r - sout_a[t].i * sout_b[t].i;
        sout[t].i = sout_a[t].r * sout_b[t].i + sout_a[t].i * sout_b[t].r;
        /*
         r[t]= sout_a[t].r * sout_b[t].r - sout_a[t].i * sout_b[t].i;
         i[t] =sout_a[t].r * sout_b[t].i + sout_a[t].i * sout_b[t].r;
         r[t] = sout_b[t].r;
         i[t] = sout_b[t].i;
         */
    }
    
    free(kiss_fftr_state);
    kiss_fftr_state = kiss_fftr_alloc(nfft,1,0,0);
    kiss_fftri(kiss_fftr_state, sout, rout);
    free(kiss_fftr_state);
    
    
    n = n+nb-1;
    f = vector<double>(n);
    for (int t = 0; t<n; t++) {
        f[t] = rout[t];
    }
    
    normalizeDensityAll();

	delete[] rin_a;
	delete[] rin_b;
	delete[] rout;

	delete[] sout_a;
	delete[] sout_b;
	delete[] sout;
}

void DctDist::runningFftConvolution(const DctDist &b){
    int n = f.size();
    
    if (b.getFsize() > n) {
        throw "error";
    }
    else {
        int M = b.getFsize();
        int LplusM = 1;
        vector<double> fb = b.getF();
        
        while (LplusM < 4*M ) {
            LplusM *= 2;
        }
        int L = LplusM - M + 1;
        
        //
        kiss_fft_scalar* M_in = new kiss_fft_scalar[LplusM];
        kiss_fft_scalar* L_in = new kiss_fft_scalar[LplusM];
        kiss_fft_scalar* LM_res = new kiss_fft_scalar[LplusM];
        
        kiss_fft_cpx* M_four = new kiss_fft_cpx[LplusM/2 + 1];
        kiss_fft_cpx* L_four = new kiss_fft_cpx[LplusM/2 + 1];
        kiss_fft_cpx* LM_four = new kiss_fft_cpx[LplusM/2 + 1];
        
        vector<double> solution(M+n-1);
        
        // Fourier transform of B
        for (int i = 0; i<M; i++) {
            M_in[i] = fb[i];
        }
        for (int i=M; i<LplusM; i++) {
            M_in[i] = 0;
        }
        
        kiss_fftr_cfg kiss_fftr_state = kiss_fftr_alloc(LplusM,0,0,0);
        kiss_fftr(kiss_fftr_state,M_in,M_four);
        free(kiss_fftr_state);
        
        // Rolling fourrier transform of this
        for (int k = 0; k*L < n; k++) {
            // get the data
            for (int i = 0; i<L; i++) {
                if (k*L+i < n)L_in[i] = f[k*L+i];
                else L_in[i] = 0;
            }
            for (int i=L; i<LplusM; i++) {
                L_in[i] = 0;
            }
            
            // Fourrier
            kiss_fftr_state = kiss_fftr_alloc(LplusM,0,0,0);
            kiss_fftr(kiss_fftr_state,L_in,L_four);
            free(kiss_fftr_state);
            
            // Convolution
            for (int t=0; t<LplusM; t++) {
                LM_four[t].r = L_four[t].r * M_four[t].r - L_four[t].i * M_four[t].i;
                LM_four[t].i = L_four[t].r * M_four[t].i + L_four[t].i * M_four[t].r;
            }
            
            
            // Reverse Fourrier
            kiss_fftr_state = kiss_fftr_alloc(LplusM,1,0,0);
            kiss_fftri(kiss_fftr_state, LM_four, LM_res);
            free(kiss_fftr_state);
            
            // Store Result
            for (int t=0; t<LplusM; t++) {
                if (k*L+t < n + M - 1) {
                    solution[k*L+t] += LM_res[t];
                }
            }
        }
        
        f = solution;
        n = n+M-1;
        normalizeDensityAll();

		delete[] M_in;
		delete[] L_in;
		delete[] LM_res;

		delete[] M_four;
		delete[] L_four;
		delete[] LM_four;
    }
}

DctDist DctDist::convolProduct(const DctDist &b) const{
    DctDist solution(*this);
    solution.convolution(b);
    return solution;
}

//DctDist DctDist::convolProduct(const DctDist &b, int lmax) const{
//    DctDist solution(*this);
//    solution.convolution(b, lmax);
//    return solution;
//}

void DctDist::reduceLengthExactCumulative(){
    int n = f.size();
    if (n>1) {
        // from end of f
        int l = n;
        while ((fabs(1 - f[l-1]) < EPS) && (l>1)) {
            l--;
        }
        l++;
//        n = l;
        f.resize(l);
        
        // from begining of f
        l = 0;
        vector<double>::iterator it = f.begin();
        while (it != f.end() && f[*it] < EPS ) {
            l++;
            ++it;
        }
        minimumT += l;
        f.erase(f.begin(), it);
        
        // normalize
        normalizeCumulative();
        
    }
}

void DctDist::reduceLengthExactDensity(){
    int n = f.size();
    if (n>1) {
        int l = n;
        double val = 0;
        while ((val < EPS) && (l>0)) {
            l--;
            val += f[l];
        }
        l++;
        n = l;
        f.resize(l);
        
        l=0;
        // from begining of f
        l = 0;
        val = 0;
        vector<double>::iterator it = f.begin();
        if (it != f.end()) {
            val += f[0];
            while (it != f.end() && val < EPS ) {
                l++;
                ++it;
                val += f[l];
            }
        }
        minimumT += l;
        f.erase(f.begin(), it);
        
        normalizeDensity();
    }
}



//---------------
// Stochastic order
//---------------
// usual stochastic order

bool DctDist::leqSTcumulative(const DctDist &b) const{
    int n = f.size();
    int nb = b.getFsize();
    vector<double> fb = b.getF();
    int minimumTb = b.getMinimumT();
    
    try{
        if (minimumTb < minimumT) {
            for (int t= 0; t < minimumT - minimumTb; t++) {
                if (fb[t] > EPS) throw false;
            }
        }
        
        int maxT = min(minimumT + n, minimumTb + nb);
        for (int t = max(minimumT, minimumTb); t < maxT; t++) {
            if (fb[t - minimumTb] > f[t - minimumT] + EPS) throw false;
        }
        return true;
    } catch (bool b){
        return false;
    }
}

bool DctDist::lessSTcumulative(const DctDist &b) const{
    int n = f.size();
    int nb = b.getFsize();
    vector<double> fb = b.getF();
    int minimumTb = b.getMinimumT();
    
    try{
        if (minimumTb < minimumT) {
            for (int t= 0; t < minimumT - minimumTb; t++) {
                if (fb[t] > EPS) throw false;
            }
        }
        
        int maxT = min(minimumT + n, minimumTb + nb);
        for (int t = max(minimumT, minimumTb); t < maxT; t++) {
            if (fb[t - minimumTb] > f[t - minimumT] - EPS) throw false;
        }
        return true;
    } catch (bool b){
        return false;
    }
}

bool DctDist::geqSTcumulative(const DctDist &b) const{
    int n = f.size();
    int nb = b.getFsize();
    vector<double> fb = b.getF();
    int minimumTb = b.getMinimumT();
    
    try{
        if (minimumT < minimumTb) {
            for (int t= 0; t < minimumTb - minimumT; t++) {
                if (f[t] > 0) throw false;
            }
        }
        
        int maxT = min(minimumT + n, minimumTb + nb);
        for (int t = max(minimumT,minimumTb); t < maxT; t++) {
            if (fb[t - minimumTb] < f[t - minimumT] - EPS) throw false;
        }
        return true;
    } catch (bool b){
        return false;
    }
}

bool DctDist::greatSTcumulative(const DctDist &b) const{
    int n = f.size();
    int nb = b.getFsize();
    vector<double> fb = b.getF();
    int minimumTb = b.getMinimumT();
    
    try{
        if (minimumT < minimumTb) {
            for (int t= 0; t < minimumTb - minimumT; t++) {
                if (f[t] > 0) throw false;
            }
        }
        
        int maxT = min(minimumT + n, minimumTb + nb);
        for (int t = max(minimumT, minimumTb); t < maxT; t++) {
            if (fb[t - minimumTb] < f[t - minimumT] + EPS) throw false;
        }
        return true;
    } catch (bool b){
        return false;
    }
}

bool DctDist::leqSTdensity(const DctDist &b) const{
   return densityToCumulative().leqSTcumulative(b.densityToCumulative());
}

bool DctDist::lessSTdensity(const DctDist &b) const{
    return densityToCumulative().lessSTcumulative(b.densityToCumulative());
}

bool DctDist::greatSTdensity(const DctDist &b) const {
    return densityToCumulative().greatSTcumulative(b.densityToCumulative());
}

bool DctDist::geqSTdensity(const DctDist &b) const{
    return densityToCumulative().geqSTcumulative(b.densityToCumulative());
}
    
    void DctDist::joinSTcumulative(const ToolsAxel::DctDist &b){
        int nb = b.getFsize();
        int n = f.size();
        vector<double> fb = b.getF();

    
        int minimumTb = b.getMinimumT();
        int newMinimumT = max(minimumT,minimumTb);
        
        int maxT = minimumT + n;
        int maxTb = minimumTb + nb;
        int minMax = min(maxT,maxTb);
        int maxMax = max(maxT,maxTb);
        
        vector<double> newF(maxMax - newMinimumT);
        
        for (int t = newMinimumT; t< minMax; t++) {
            newF[t - newMinimumT] = min(f[t-minimumT],fb[t-minimumTb]);
        }
        if (maxT < maxTb) {
            for (int t=minMax; t<maxMax; t++) {
                newF[t - newMinimumT] = fb[t-minimumTb];
            }
        } else {
            for (int t=minMax; t<maxMax; t++) {
                newF[t - newMinimumT] = f[t-minimumT];
            }
        }
        
        f = newF;
        minimumT = newMinimumT;
    }
    
    void DctDist::meetSTcumulative(const ToolsAxel::DctDist &b){
        int nb = b.getFsize();
        int n = f.size();
        vector<double> fb = b.getF();
        
        
        int minimumTb = b.getMinimumT();
        int newMinimumT = min(minimumT,minimumTb);
        
        int maxMin = max(minimumT, minimumTb);
        int maxT = minimumT + n;
        int maxTb = minimumTb + nb;
        int minMax = min(maxT,maxTb);
        
        vector<double> newF(minMax - newMinimumT);
        
        int bound = min(minMax,maxMin);
        if (minimumT < minimumTb) {
            for (int t = newMinimumT; t< bound; t++) {
                newF[t - newMinimumT] = f[t - minimumT];
            }
        } else {
            for (int t = newMinimumT; t< bound; t++) {
                newF[t - newMinimumT] = fb[t - minimumTb];
            }
        }
        
        if (minMax >= maxMin){
            for (int t = maxMin; t< minMax; t++) {
                newF[t - newMinimumT] = max(f[t-minimumT], fb[t - minimumTb]);
            }
        }
        
        f = newF;
        minimumT = newMinimumT;
    }
    
//
//DctDist DctDist::joinSTcumulative(const DctDist &b) const{
//    int nb = b.getN();
//    vector<double> fb = b.getF();
//    int nNew;
//    vector<double> fNew;
//    if (nb < n) {
//        fNew = f;
//        for (int t=0; t<nb; t++) {
//            fNew[t] = min(f[t],fb[t]);
//        }
//        nNew = n;
//    }
//    else{
//        fNew = fb;
//        for (int t=0; t<n; t++) {
//            fNew[t] = min(f[t],fb[t]);
//        }
//        nNew = nb;
//    }
//    return DctDist(nNew , fNew);
//}
//
//DctDist DctDist::meetSTcumulative(const DctDist &b) const{
//    if (!isCumulative() || !b.isCumulative()) {
//        throw  "aie";
//    }
//    
//    int nb = b.getN();
//    vector<double> fb = b.getF();
//    int nNew;
//    vector<double> fNew;
//    if (nb < n) {
//        fNew = fb;
//        for (int t=0; t<nb; t++) {
//            fNew[t] = max(f[t],fb[t]);
//        }
//        nNew = nb;
//    }
//    else{
//        fNew = f;
//        for (int t=0; t<n; t++) {
//            fNew[t] = max(f[t],fb[t]);
//        }
//        nNew = n;
//    }
//    return DctDist(nNew , fNew);
//}

//---------------
// First Wasserstein Distance
//---------------

double DctDist::firstWassersteinDistance(const DctDist &b) const{
    
    int minimumTb = b.getMinimumT();
    int maxT = minimumT + f.size();
    int maxTb = minimumTb + b.getFsize();
    
    vector<double> fb = b.getF();
    
    int minMin = min(minimumT,minimumTb);
    int maxMin = max(minimumT,minimumTb);
    int minMax = min(maxT,maxTb);
    int maxMax = max(maxT,maxTb);
    
    double solution = 0;
    
    if (minimumT < minimumTb) {
        for (int t = minMin; t < maxMin; t++) {
            solution += f[t- minimumT];
        }
    } else {
        for (int t = minMin; t < maxMin; t++) {
            solution += fb[t- minimumTb];
        }
    }
    
    for (int t = maxMin; t < minMax; t++) {
        solution += fabs(f[t - minimumT] - fb[t - minimumT]);
    }
    
    if (maxT < maxTb) {
        for (int t = minMax; t < maxMax; t++) {
            solution += fabs(1 - fb[t - minimumTb]);
        }
    } else {
        for (int t = minMax; t < maxMax; t++) {
            solution += fabs(1 - f[t - minimumT]);
        }
    }
    
    return solution;
}
    
    //---------------
    // Generate realization
    //---------------
    int DctDist::generateRealization() const{
        if (! isDensity()) throw "must be a density";
        else return generateRealizatonDensity();
    }
    
    vector<double> DctDist::generateNrealizations(int n) const {
        vector<double> result(n,-1);
        for (int i = 0; i < n; i++) {
            result[i] = generateRealization();
        }
        return result;
    }
    
    int DctDist::generateRealizatonDensity() const{
        double value = (rand() % 100000)/(double) 100000;
        double cumulative = 0;
        try {
            for (int i = 0; i<f.size(); i++) {
                cumulative += f[i];
                if (cumulative > value - EPS) {
                    throw i;
                }
            }
        } catch (int sol) {
            return sol + minimumT;
        }
        return minimumT + f.size()-1;
        
    }

    int DctDist::getQuantile(double prop) const{
       if (!isCumulative()){
          throw "must be cumlative in DctDist::getQuantile(double prop)";
       }
       for (int i = 0; i < f.size(); ++i){
          if (f[i] > prop - EPS){
             return i + minimumT;
          }
       }
       return -1;
    }
}
