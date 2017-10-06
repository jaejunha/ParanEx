#include <jni.h>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

extern "C" {
JNIEXPORT void JNICALL
Java_kr_ac_ajou_paran_util_Recognizer_rectangle(JNIEnv *env, jobject instance, jlong matAddrInput, jlong matAddrPass, jlong matAddrResult) {

    Mat &matInput = *(Mat *) matAddrInput;
    Mat &matPass = *(Mat *) matAddrPass;
    Mat &matResult = *(Mat *) matAddrResult;
    
    Mat gray;
    cvtColor(matInput, gray, CV_RGBA2GRAY);

    Mat bw;
    adaptiveThreshold(~gray, bw, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, -2);

    matResult = bw;
    Mat horizontal = bw.clone();
    Mat vertical = bw.clone();

    int scale = 15; // play with this variable in order to increase/decrease the amount of lines to be detected

    // Specify size on horizontal axis
    int horizontalsize = horizontal.cols / scale;

    // Create structure element for extracting horizontal lines through morphology operations
    Mat horizontalStructure = getStructuringElement(MORPH_RECT, Size(horizontalsize, 1));

    // Apply morphology operations
    erode(horizontal, horizontal, horizontalStructure, Point(-1, -1));
    dilate(horizontal, horizontal, horizontalStructure, Point(-1, -1));
    //    dilate(horizontal, horizontal, horizontalStructure, Point(-1, -1)); // expand horizontal lines

    // Specify size on vertical axis
    int verticalsize = vertical.rows / scale;

    // Create structure element for extracting vertical lines through morphology operations
    Mat verticalStructure = getStructuringElement(MORPH_RECT, Size(1, verticalsize));

    // Apply morphology operations
    erode(vertical, vertical, verticalStructure, Point(-1, -1));
    dilate(vertical, vertical, verticalStructure, Point(-1, -1));

    Mat mask = horizontal + vertical;

    // find the joints between the lines of the tables, we will use this information in order to descriminate tables from pictures (tables will contain more than 4 joints while a picture only 4 (i.e. at the corners))
    Mat joints;
    bitwise_and(horizontal, vertical, joints);

    // Find external contours from the mask, which most probably will belong to tables or to images
    vector<Vec4i> hierarchy;
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    vector<vector<Point> > contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());
    vector<Mat> rois;

    for (size_t i = 0; i < contours.size(); i++)
    {
        // find the area of each contour
        double area = contourArea(contours[i]);

        //        // filter individual lines of blobs that might exist and they do not represent a table
        if (area < 100) // value is randomly chosen, you will need to find that by yourself with trial and error procedure
            continue;

        approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
        boundRect[i] = boundingRect(Mat(contours_poly[i]));

        // find the number of joints that each table has
        Mat roi = joints(boundRect[i]);

        vector<vector<Point> > joints_contours;
        findContours(roi, joints_contours, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

        // if the number is not more than 5 then most likely it not a table
        if (joints_contours.size() <= 4)
            continue;

        rois.push_back(matInput(boundRect[i]).clone());
        rectangle(matInput, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 3, 8, 0);
    }

    matResult = matInput;

    if (rois.size() == 1) {
        matPass = rois[0];
    }
}
}

extern "C" {
JNIEXPORT int JNICALL
Java_kr_ac_ajou_paran_util_Recognizer_getVerticalCoord(JNIEnv *env, jobject instance, jlong matAddrInput) {

    Mat &table = *(Mat *) matAddrInput;
    Mat vertical;
    int width = table.cols, height = table.rows;
    int delta = width*0.02f;
    vector<short> lines;

    cvtColor(table, vertical, CV_RGBA2GRAY);
    adaptiveThreshold(~vertical, vertical, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, -2);

    int size = vertical.rows / 15;
    Mat verticalStructure = getStructuringElement(MORPH_RECT, Size(1, size));

    erode(vertical, vertical, verticalStructure, Point(-1, -1));
    dilate(vertical, vertical, verticalStructure, Point(-1, -1));

    for (int i = delta; i < height && lines.size() == 0; i++) {
        for (int j = delta; j < width; j++) {
            if (vertical.at<uchar>(i, j) != 0) {
                lines.push_back(j);
                j += delta;
            }
        }
    }
 /*   if (lines.size() < 5) {
        printf("error), There are few colums!\n");
        waitKey();
        exit(1);
    }*/
    return lines.size();
}
}