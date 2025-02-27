/*  OpenVLBI - Open Source Very Long Baseline Interferometry
*   Copyright © 2017-2023  Ilia Platone
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License along
*   with this program; if not, write to the Free Software Foundation, Inc.,
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nodecollection.h>
#include <baselinecollection.h>
#include <modelcollection.h>
#include <base64.h>
#include <thread>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp> // Include for feature detection

using namespace std;
using namespace cv;

extern NodeCollection *vlbi_nodes;

static Mat feature_extract(dsp_stream_p stream)
{
    Mat cv_stream(stream->dims, stream->sizes, CV_64F, static_cast<void*>(stream->buf));
    if (cv_stream.empty()) {
        perr("Error reading stream data!");
        return cv_stream;
    }

    Ptr<Feature2D> extractor;
    //extractor = AKAZE::create();
    //extractor = BRISK::create();
    extractor = ORB::create();

    vector<KeyPoint> keypoints;
    Mat descriptors;

    extractor->detectAndCompute(cv_stream, Mat(), keypoints, descriptors);
    return descriptors;
}

static vector<DMatch> feature_filter(Mat stream, Mat matrix)
{
    int i;
    Ptr<DescriptorMatcher> matcher;
    //matcher = BFMatcher::create();
    matcher = FlannBasedMatcher::create();
    vector<DMatch> matches;
    matcher->match(stream, matrix, matches);

    double min_dist = DBL_MAX;
    for( int i = 0; i < stream.rows; i++ ) {
        double dist = matches[i].distance;
        if( dist < min_dist ) min_dist = dist;
    }
    double good_match_threshold = pow(2.0, 0.5) * min_dist;
    vector<DMatch> good_matches;
    for(i = 0; i < stream.rows; i++ ) {
        if( matches[i].distance < good_match_threshold ) {
            good_matches.push_back( matches[i]);
        }
    }
    return good_matches;
}

double vlbi_stream_compare(dsp_stream_p stream1, dsp_stream_p stream2)
{
    pfunc;
    unsigned int i;
    Mat cv_stream1 = feature_extract(stream1);
    if (cv_stream1.empty()) {
        perr("Error reading stream data!");
        return 0;
    }
    Mat cv_stream2 = feature_extract(stream2);
    if (cv_stream2.empty()) {
        perr("Error reading matrix data!");
        return 0;
    }
    vector<DMatch> matches = feature_filter(cv_stream1, cv_stream2);
    if (matches.size() < 1) {
        perr("Error reading matrix data!");
        return 0;
    }
    double score = 0;
    for(i = 0; i < matches.size(); i++ ) {
        score += matches.size() / matches[i].distance;
    }
    score /= matches.size();
    return score;
}

double vlbi_compare_models(vlbi_context ctx, const char *model1, const char *model2)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    if(!vlbi_has_model(ctx, model1))
        return 0;
    if(!vlbi_has_model(ctx, model2))
        return 0;
    dsp_stream_p stream1 = nodes->getModels()->get(model1);
    dsp_stream_p stream2 = nodes->getModels()->get(model2);
    return vlbi_stream_compare(stream1, stream2);
}

void vlbi_phase_generator(vlbi_context ctx, const char *name, const char *model1, const char *model2)
{
    pfunc;
    int niterations = 10;
    int i;
    double best_score = 0;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    if(!vlbi_has_model(ctx, model1))
        return;
    if(!vlbi_has_model(ctx, model2))
        return;
    dsp_stream_p stream1 = nodes->getModels()->get(model1);
    dsp_stream_p stream2 = nodes->getModels()->get(model2);
    dsp_stream_p stream = dsp_stream_copy(stream1);
    for(i = 0; i < niterations; i ++) {
        double score = vlbi_stream_compare(stream1, stream2);
        if(score == 0) i --;
        else if(score > best_score) best_score = score;
    }
    if(!vlbi_has_model(ctx, name))
        vlbi_add_model(ctx, stream, name);
}
