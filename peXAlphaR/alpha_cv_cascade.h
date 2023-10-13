#ifndef ALPHA_CV_CASCADE_H
#define ALPHA_CV_CASCADE_H

#include "alpha_cv.h"

typedef struct headData {
	int faceLB_x = 0, faceLB_y = 0;
	int faceUB_x = 0, faceUB_y = 0;
	cv::Mat img;
	cv::Point leftEyeHead, rightEyeHead;
	void load(int facelx, int faceux, int facely, int faceuy, cv::InputArray headImg, const cv::Point& leftEye, const cv::Point& rightEye);
	void load(int facelx, int faceux, int facely, int faceuy, const std::string& headImgPath, const cv::Point& leftEye, const cv::Point& rightEye);
} headData;

double pointDistance(const cv::Point& point1, const cv::Point& point2);

void headChange(cv::InputOutputArray img, cv::Rect face_roi, cv::InputArray head_img
	, int faceLB_x, int faceUB_x, int faceLB_y, int faceUB_y);
void headChange(cv::InputOutputArray img, cv::Rect face_roi, cv::InputArray head_img
	, int faceLB_x, int faceUB_x, int faceLB_y, int faceUB_y, const cv::Point& leftEyeHead, const cv::Point& rightEyeHead
	, const cv::Point& leftEyeCentre, const cv::Point& rightEyeCentre);
void headChange(cv::InputOutputArray img, cv::Rect faceROI, const headData& head);
void headChange(cv::InputOutputArray img, cv::Rect faceROI, const headData& head, const cv::Point& leftEyeCentre, const cv::Point& rightEyeCentre);

int detectChangeHeads(cv::InputOutputArray img, const std::string& cascade_face_path, const std::string& cascade_eyes_path,
	const headData& head, bool useEyeCorrection = true, int headNum = 1);
int detectChangeHeads(cv::InputOutputArray img, const std::vector<headData>& headDataset,
	const std::string& cascade_face_path, const std::string& cascade_eyes_path, int* headsIndex, bool useEyeCorrection = true, int headNum = 1);

#endif // !ALPHA_CV_CASCADE_H

