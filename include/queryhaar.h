//Based on DigiKam fuzzy search functionality
//
#include <iostream>
#include <ostream>
#include <queue>
#include <map>
#include <list>
#include <vector>

#include "CImg.h"

using namespace std;
using namespace cimg_library;


 /** Weights for the Haar coefficients. Straight from the referenced paper
     "Fast Multiresolution Image Querying"
     by Charles E. Jacobs, Adam Finkelstein and David H. Salesin.
     http://www.cs.washington.edu/homes/salesin/abstracts.html
 */
static const float s_haar_weights[2][6][3] =
{
    // For scanned picture (sketch=0):
    //   Y      I      Q         idx  total       occurs
    {{ 5.00, 19.21, 34.37 },   // 0   58.58       1 (`DC' component)
     { 0.83,  1.26,  0.36 },   // 1    2.45       3
     { 1.01,  0.44,  0.45 },   // 2    1.90       5
     { 0.52,  0.53,  0.14 },   // 3    1.19       7
     { 0.47,  0.28,  0.18 },   // 4    0.93       9
     { 0.30,  0.14,  0.27 }},  // 5    0.71       16384-25=16359

    // For handdrawn/painted sketch (sketch=1):
    //   Y      I      Q
    {{ 4.04, 15.14, 22.62 },
     { 0.78,  0.92,  0.40 },
     { 0.46,  0.53,  0.63 },
     { 0.42,  0.26,  0.25 },
     { 0.41,  0.14,  0.15 },
     { 0.32,  0.07,  0.38 }}
};

/** Number of pixels on one side of image; required to be a power of 2.
 */
enum { NumberOfPixels = 128 };

/** Total pixels in a square image.
 */
enum { NumberOfPixelsSquared = NumberOfPixels * NumberOfPixels };

/** Number of Haar coefficients we retain as signature for an image.
 */
enum { NumberOfCoefficients = 40 };

enum SketchType
{
    ScannedSketch   = 0,
    HanddrawnSketch = 1
};

typedef double Unit;

// Keep this definition constant at qint32 (guaranteed binary size!)
typedef int Idx;

/** Signature structure
 */
class valStruct
{
public:
  Unit d;   // [f]abs(a[i])
  int  i;   // index i of a[i]

  bool operator< (const valStruct &right) const
  {
    return d > right.d;
  }
};

typedef std::priority_queue<valStruct> valqueue;

class ImageData
{
public:
  Unit data1[NumberOfPixelsSquared];
  Unit data2[NumberOfPixelsSquared];
  Unit data3[NumberOfPixelsSquared];

  /** Write pixels of a DImg in three arrays (one per color channel, pixels linearly)
  */
  void fillPixelData(const CImg<unsigned char> &image)
  {
    CImg<unsigned char> im(image);
    im.resize(NumberOfPixels,NumberOfPixels);

    int cn = 0;
    cimg_forXY(im,x,y) {
      data1[cn] = im(x,y,0);
      data2[cn] = im(x,y,1);
      data3[cn] = im(x,y,2);
      cn++;
    }
  }
};

class SignatureData
{

public:

    /** Y/I/Q positions with largest magnitude
     */
    Idx sig[3][NumberOfCoefficients];

    /** YIQ for position [0,0]
     */
    double    avg[3];
};

// ---------------------------------------------------------------------------------

/** This class provides very fast lookup if a certain pixel
 *  is set (positive or negative) in the loaded coefficient set.
 */
class SignatureMap
{

public:

    typedef bool MapIndexType;

    SignatureMap()
    {
        indexList = new MapIndexType[2 * NumberOfPixelsSquared];
    }

    ~SignatureMap()
    {
        delete indexList;
    }

    /// Load a set of coefficients
    void fill(Idx *coefs)
    {
        // For maximum performance, we use a flat array.
        // First 16k for negative values, second 16k for positive values.
        // All values or false, only 2*40 are true.
        memset(indexList, 0, sizeof(MapIndexType[2 * NumberOfPixelsSquared]));
        int x;
        for (int i=0; i<NumberOfCoefficients; i++)
        {
            x = coefs[i] + NumberOfPixelsSquared;
            indexList[x] = true;
        }
    }

    /// Query if the given index is set.
    /// Index must be in the range -16383..16383.
    bool operator[](Idx index)
    {
        return indexList[index + NumberOfPixelsSquared];
    }

    MapIndexType *indexList;
};

// ---------------------------------------------------------------------------------

class WeightBin
{
public:
  /** Fixed weight mask for pixel positions (i,j).
      Each entry x = i*NUM_PIXELS + j, gets value max(i,j) saturated at 5.
      To be treated as a constant.
  */
  unsigned char m_bin[16384];

  unsigned char bin(int index)    { return m_bin[index];                               }
  unsigned char binAbs(int index) { return (index > 0) ? m_bin[index] : m_bin[-index]; }

  /** Setup initial fixed Haar weights that each coefficient represents
  */
  WeightBin()
  {
    int i, j;

    /*
    0 1 2 3 4 5 6 i
    0 0 1 2 3 4 5 5
    1 1 1 2 3 4 5 5
    2 2 2 2 3 4 5 5
    3 3 3 3 3 4 5 5
    4 4 4 4 4 4 5 5
    5 5 5 5 5 5 5 5
    5 5 5 5 5 5 5 5
    j
    */

    // Every position has value 5
    memset(m_bin, 5, NumberOfPixelsSquared);

    // Except for the 5 by 5 upper-left quadrant
    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 5; j++)
        {
            m_bin[i*128+j] = max(i, j);
            // NOTE: imgBin[0] == 0
        }
    }
  }

};

// ---------------------------------------------------------------------------------

class Weights
{
public:

    enum SketchType
    {
        ScannedSketch = 0,
        PaintedSketch = 1
    };

    Weights(SketchType type = ScannedSketch)
    : m_type(type)
    {
    }

    float weight(int weight, int channel) { return s_haar_weights[(int)m_type][weight][channel]; }
    float weightForAverage(int channel)   { return s_haar_weights[(int)m_type][0][channel];      }

private:

    SketchType m_type;
};

// ---------------------------------------------------------------------------------

class Calculator
{

public:
  Calculator()  {};
  ~Calculator() {};

  /** Determines a total of NUM_COEFS positions in the image that have the
    largest magnitude (absolute value) in color value. Returns linearized
    coordinates in sig1, sig2, and sig3. avgl are the [0,0] values.
    The order of occurrence of the coordinates in sig doesn't matter.
    Complexity is 3 x NUM_PIXELS^2 x 2log(NUM_COEFS).
  */
  int  calcHaar(ImageData *data, SignatureData *sigData)
  {
    sigData->avg[0]=data->data1[0];
    sigData->avg[1]=data->data2[0];
    sigData->avg[2]=data->data3[0];

    // Color channel 1:
    getmLargests(data->data1, sigData->sig[0]);

    // Color channel 2:
    getmLargests(data->data2, sigData->sig[1]);

    // Color channel 3:
    getmLargests(data->data3, sigData->sig[2]);

    return 1;
  }

  /** Do the Haar tensorial 2d transform itself.
    Here input is RGB data [0..255] in Unit arrays.
    Results are available in a, b, and c.
    Fully inplace calculation; order of result is interleaved though,
    but we don't care about that.
  */
  void transform(ImageData *data)
  {
    // RGB -> YIQ colorspace conversion; Y luminance, I,Q chrominance.
    // If RGB in [0..255] then Y in [0..255] and I,Q in [-127..127].
    Unit* a = data->data1;
    Unit* b = data->data2;
    Unit* c = data->data3;

    for (int i = 0; i < NumberOfPixelsSquared; i++)
    {
        Unit Y, I, Q;

        Y    = 0.299 * a[i] + 0.587 * b[i] + 0.114 * c[i];
        I    = 0.596 * a[i] - 0.275 * b[i] - 0.321 * c[i];
        Q    = 0.212 * a[i] - 0.523 * b[i] + 0.311 * c[i];
        a[i] = Y;
        b[i] = I;
        c[i] = Q;
    }

    haar2D(a);
    haar2D(b);
    haar2D(c);

    // Reintroduce the skipped scaling factors
    a[0] /= 256 * 128;
    b[0] /= 256 * 128;
    c[0] /= 256 * 128;
  }

private:

  /** Do the Haar tensorial 2d transform itself.
    Here input is RGB data [0..255] in Unit arrays
    Computation is (almost) in-situ.
  */
  void haar2D(Unit a[])
  {
    int  i;
    Unit t[NumberOfPixels >> 1];

    // scale by 1/sqrt(128) = 0.08838834764831843:
    /*
    for (i = 0; i < NUM_PIXELS_SQUARED; i++)
        a[i] *= 0.08838834764831843;
    */

    // Decompose rows:
    for (i = 0; i < NumberOfPixelsSquared; i += NumberOfPixels)
    {
        int h, h1;
        Unit C = 1;

        for (h = NumberOfPixels; h > 1; h = h1)
        {
            int j1, j2, k;

            h1 = h >> 1;        // h = 2*h1
            C *= 0.7071;        // 1/sqrt(2)
            for (k = 0, j1 = j2 = i; k < h1; k++, j1++, j2 += 2)
            {
                int j21 = j2+1;
                t[k]    = (a[j2] - a[j21]) * C;
                a[j1]   = (a[j2] + a[j21]);
            }
            // Write back subtraction results:
            memcpy(a+i+h1, t, h1*sizeof(a[0]));
        }
        // Fix first element of each row:
        a[i] *= C;  // C = 1/sqrt(NUM_PIXELS)
    }

    // scale by 1/sqrt(128) = 0.08838834764831843:
    /*
    for (i = 0; i < NUM_PIXELS_SQUARED; i++)
        a[i] *= 0.08838834764831843;
    */

    // Decompose columns:
    for (i = 0; i < NumberOfPixels; i++)
    {
        Unit C=1;
        int  h, h1;

        for (h = NumberOfPixels; h > 1; h = h1)
        {
            int j1, j2, k;

            h1 = h >> 1;
            C *= 0.7071;       // 1/sqrt(2) = 0.7071
            for (k = 0, j1 = j2 = i; k < h1; k++, j1 += NumberOfPixels, j2 += 2*NumberOfPixels)
            {
                int j21 = j2+NumberOfPixels;
                t[k]    = (a[j2] - a[j21]) * C;
                a[j1]   = (a[j2] + a[j21]);
            }

            // Write back subtraction results:
            for (k = 0, j1 = i+h1*NumberOfPixels; k < h1; k++, j1 += NumberOfPixels)
            {
                a[j1]=t[k];
            }
        }
        // Fix first element of each column:
        a[i] *= C;
    }
  }

  /** Find the m=NUM_COEFS largest numbers in cdata[] (in magnitude that is)
    and store their indices in sig[].
    Skips entry 0.
  */
  inline void getmLargests(Unit *cdata, Idx *sig)
  {
    int       cnt, i;
    valStruct val;
    valqueue  vq;     // dynamic priority queue of valStruct's

    // Could skip i=0: goes into separate avgl

    // Fill up the bounded queue. (Assuming NUM_PIXELS_SQUARED > NUM_COEFS)
    for (i = 1; i < NumberOfCoefficients+1; i++)
    {
        val.i = i;
        val.d = fabs(cdata[i]);
        vq.push(val);
    }
    // Queue is full (size is NUM_COEFS)

    for (/*i = NUM_COEFS+1*/; i < NumberOfPixelsSquared; i++)
    {
        val.d = fabs(cdata[i]);

        if (val.d > vq.top().d)
        {
            // Make room by dropping smallest entry:
            vq.pop();
            // Insert val as new entry:
            val.i = i;
            vq.push(val);
        }
        // else discard: do nothing
    }

    // Empty the (non-empty) queue and fill-in sig:
    cnt=0;
    do
    {
        int t;

        val = vq.top();
        t   = (cdata[val.i] <= 0);       // t = 0 if pos else 1
        // i - 0 ^ 0 = i; i - 1 ^ 0b111..1111 = 2-compl(i) = -i
        sig[cnt++] = (val.i - t) ^ -t;   // never 0
        vq.pop();
    }
    while(!vq.empty());

    // Must have cnt==NUM_COEFS here.
  }

};


//==================================================================================

class HaarIface
{
private:
  ImageData *data;
  WeightBin *bin;
  map<int, SignatureData> *sigMap;

public:
  HaarIface() {
    data   = new ImageData();
    bin    = new WeightBin();
    sigMap = new map<int, SignatureData>();
  }
  ~HaarIface() {}

private:
  void calculateScore(double &score, SignatureData &querySig, SignatureData &targetSig,
                           Weights &weights, SignatureMap** queryMaps)
  {
    // this is a reference
    // double &score                  = scores[imageid];
    // SignatureData &targetSig = sigMap[imageid];

    // Step 1: Initialize scores with average intensity values of all three channels
    for (int channel=0; channel<3; channel++)
    {
        score += weights.weightForAverage(channel) * fabs( querySig.avg[channel] - targetSig.avg[channel] );
    }

    // Step 2: Decrease the score if query and target have significant coefficients in common
    for (int channel=0; channel<3; channel++)
    {
        Idx *sig               = targetSig.sig[channel];
        SignatureMap *queryMap = queryMaps[channel];
        int x;
        for (int coef = 0; coef < NumberOfCoefficients; coef++)
        {
            // x is a pixel index, either positive or negative, 0..16384
            x = sig[coef];
            // If x is a significant coefficient with the same sign in the query signature as well,
            // decrease the score (lower is better)
            // Note: both method calls called with x accept positive or negative values
            if ((*queryMap)[x])
                score -= weights.weight(bin->binAbs(x), channel);
        }
    }
  }

  /// This method is the core functionality: It assigns a score to every image in the db
  map<int, double> searchDatabase(SignatureData *querySig, SketchType type)
  {
    //d->createWeightBin();

    // The table of constant weight factors applied to each channel and bin
    Weights weights((Weights::SketchType)type);

    // layout the query signature for fast lookup
    SignatureMap queryMapY, queryMapI, queryMapQ;
    queryMapY.fill(querySig->sig[0]);
    queryMapI.fill(querySig->sig[1]);
    queryMapQ.fill(querySig->sig[2]);
    SignatureMap *queryMaps[3] = { &queryMapY, &queryMapI, &queryMapQ };

    // Map imageid -> score. Lowest score is best.
    // any newly inserted value will be initialized with a score of 0, as required
    map<int, double> scores;
    int           imageid;

    for ( map<int, SignatureData>::iterator it = sigMap->begin(); it != sigMap->end(); ++it )
    {
      const int imageid= it->first;
      double        &score  = scores[imageid];
      SignatureData &qSig   = *querySig;
      SignatureData &tSig   = (*sigMap)[imageid];

      calculateScore(score, qSig, tSig, weights, queryMaps);
    }

    return scores;
  }

  multimap<double, int> bestMatches(SignatureData *querySig, int numberOfResults, SketchType type)
  {
    map<int, double> scores = searchDatabase(querySig, type);

    // Find out the best matches, those with the lowest score
    // We make use of the feature that QMap keys are sorted in ascending order
    // Of course, images can have the same score, so we need a multi map
    multimap<double, int> bestMatches;
    bool initialFill = false;
    double score, worstScore, bestScore;
    int id;
    for (map<int, double>::iterator it = scores.begin(); it != scores.end(); ++it)
    {
        score = it->second; //value();
        id    = it->first; // key();

        if (!initialFill)
        {
            // as long as the maximum number of results is not reached, just fill up the map
            bestMatches.insert(pair<double, int>(score, id));
            initialFill = (bestMatches.size() >= numberOfResults);
        }
        else
        {
            // find the last entry, the one with the highest (=worst) score
            map<double, int>::iterator last = bestMatches.end();
            --last;
            worstScore = last->first; //key();
            // if the new entry has a higher score, put it in the list and remove that last one
            if (score < worstScore)
            {
                bestMatches.erase(last);
                bestMatches.insert(pair<double, int>(score, id));
            }
            else if (score == worstScore)
            {
                bestScore = bestMatches.begin()->first; //.key();
                // if the score is identical for all entries, increase the maximum result number
                if (score == bestScore)
                    bestMatches.insert(pair<double, int>(score, id));
            }
        }
    }

    return bestMatches;
  }

public:

  bool indexImage(int imageid, const CImg<unsigned char>& image)
  {
    if (image.is_empty())
        return false;

    data->fillPixelData(image);

    Calculator haar;
    haar.transform(data);

    SignatureData sig;
    haar.calcHaar(data, &sig);

    sigMap->insert( make_pair(imageid, sig ) );

    return true;
  }

  multimap<double, int> bestMatchesForImageWithScores(const CImg<unsigned char>& image, int numberOfResults, SketchType type)
  {
    data->fillPixelData(image);
    
    Calculator haar;
    haar.transform(data);
    SignatureData sig;
    haar.calcHaar(data, &sig);

    return bestMatches(&sig, numberOfResults, type);    
  }

  vector<int> bestMatchesForImage(const CImg<unsigned char>& image, int numberOfResults, SketchType type)
  {
    multimap<double, int> bestMatches = bestMatchesForImageWithScores(image,numberOfResults,type);
    
    vector<int> res;
    //printf("\nbestMatches: ");
    for (multimap<double, int>::iterator it = bestMatches.begin(); it != bestMatches.end(); ++it) {
      res.push_back(it->second);
      //printf("%d ",(int)it->second);
    }
    return res;
  }

  /*
  vector<int> bestMatchesForImage(int imageid, int numberOfResults, SketchType type)
  {
    SignatureData sig = (*sigMap)[imageid];

    return bestMatches(&sig, numberOfResults, type);
  }
  */
  
};

