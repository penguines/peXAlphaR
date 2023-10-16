#include "alpha_cv_cascade.h"


void headChange(cv::InputOutputArray img, cv::Rect face_roi, cv::InputArray head_img
	, int faceLB_x, int faceUB_x, int faceLB_y, int faceUB_y, mergeMode mg_mode) {
	CV_Assert(head_img.channels() == 4 && face_roi.height != 0 && face_roi.width != 0);
	cv::Mat head_tmp;
	double kx, ky;
	int LB_x1, LB_y1;
	kx = face_roi.width / (double)(faceUB_x - faceLB_x);
	ky = face_roi.height / (double)(faceUB_y - faceLB_y);
	LB_x1 = faceLB_x * kx;
	LB_y1 = faceLB_y * ky;
	int placeX, placeY;
	placeX = face_roi.x - LB_x1;
	placeY = face_roi.y - LB_y1;
	cv::resize(head_img, head_tmp, cv::Size(), kx, ky);
	mergeImage(head_tmp, img, placeX, placeY, 1.0, mg_mode);
}

void headChange(cv::InputOutputArray img, cv::Rect face_roi, cv::InputArray head_img
	, int faceLB_x, int faceUB_x, int faceLB_y, int faceUB_y, const cv::Point& leftEyeHead, const cv::Point& rightEyeHead
	, const cv::Point& leftEyeCentre, const cv::Point& rightEyeCentre, mergeMode mg_mode) {
	CV_Assert(head_img.channels() == 4 && face_roi.height != 0 && face_roi.width != 0);
	cv::Mat head_tmp;
	double kx, ky;
	double eyeDistHead = pointDistance(leftEyeHead, rightEyeHead);
	double eyeDistPic = pointDistance(leftEyeCentre, rightEyeCentre);
	double rotateAngle = -atan((double)(rightEyeCentre.y - leftEyeCentre.y) / (double)(rightEyeCentre.x - leftEyeCentre.x));
	double sr = sin(rotateAngle), cr = cos(rotateAngle);

	kx = eyeDistPic / eyeDistHead;
	ky = face_roi.height / (double)(faceUB_y - faceLB_y);
	cv::resize(head_img, head_tmp, cv::Size(), kx, ky);

	int halfWidth = (head_tmp.cols >> 1), halfHeight = (head_tmp.rows >> 1);
	int transX = (head_tmp.cols >> 2), transY = (head_tmp.rows >> 2);
	cv::Mat rotate_mat = (cv::Mat_<double>(3, 3) << cr, sr, -halfWidth * cr - halfHeight * sr + halfWidth
		, -sr, cr, halfWidth * sr - halfHeight * cr + halfHeight
		, 0, 0, 1);
	cv::Mat transfer_mat = (cv::Mat_<double>(3, 3) << 1, 0, transX, 0, 1, transY, 0, 0, 1);
	cv::Mat lefteye_loc = (cv::Mat_<double>(3, 1) << leftEyeHead.x * kx, leftEyeHead.y * ky, 1);
	cv::Mat transform_mat = transfer_mat * rotate_mat;

	cv::Mat rotated_head(1.5 * head_tmp.rows, 1.5 * head_tmp.cols, CV_8UC4);

	cv::warpPerspective(head_tmp, rotated_head, transform_mat, rotated_head.size());
	lefteye_loc = transform_mat * lefteye_loc;

	int LB_x1, LB_y1;
	LB_x1 = round(lefteye_loc.at<double>(0, 0));
	LB_y1 = round(lefteye_loc.at<double>(1, 0));
	int placeX, placeY;
	placeX = leftEyeCentre.x - LB_x1;
	placeY = leftEyeCentre.y - LB_y1;

	mergeImage(rotated_head, img, placeX, placeY, 1.0, mg_mode);
}

void headChange(cv::InputOutputArray img, cv::Rect faceROI, const headData& head) {
	headChange(img, faceROI, head.img
		, head.faceLB_x, head.faceUB_x, head.faceLB_y, head.faceUB_y, head.mode);
}

void headChange(cv::InputOutputArray img, cv::Rect faceROI, const headData& head, const cv::Point& leftEyeCentre, const cv::Point& rightEyeCentre) {
	headChange(img, faceROI, head.img, head.faceLB_x, head.faceUB_x, head.faceLB_y, head.faceUB_y
		, head.leftEyeHead, head.rightEyeHead, leftEyeCentre, rightEyeCentre, head.mode);
}

double pointDistance(const cv::Point& point1, const cv::Point& point2) {
	int dx = point1.x - point2.x, dy = point1.y - point2.y;
	return (double)sqrt(dx * dx + dy * dy);
}

headData::headData(){
	this->faceLB_x = 0;
	this->faceLB_y = 0;
	this->faceUB_x = 0;
	this->faceUB_y = 0;
	this->mode = MERGE_ADD;
}

void headData::load(int facelx, int faceux, int facely, int faceuy, cv::InputArray headImg, const cv::Point& leftEye, const cv::Point& rightEye, mergeMode md) {
	this->faceLB_x = facelx;
	this->faceUB_x = faceux;
	this->faceLB_y = facely;
	this->faceUB_y = faceuy;
	headImg.copyTo(this->img);
	this->leftEyeHead = leftEye;
	this->rightEyeHead = rightEye;
	this->mode = md;
}

void headData::load(int facelx, int faceux, int facely, int faceuy, const std::string& headImgPath, const cv::Point& leftEye, const cv::Point& rightEye, mergeMode md) {
	this->faceLB_x = facelx;
	this->faceUB_x = faceux;
	this->faceLB_y = facely;
	this->faceUB_y = faceuy;
	this->img = cv::imread(headImgPath, cv::IMREAD_UNCHANGED);
	this->leftEyeHead = leftEye;
	this->rightEyeHead = rightEye;
	this->mode = md;
}

//-----------------------------------------------------------------------------------

int detectChangeHeads(cv::InputOutputArray img, const std::vector<headData>& headDataset,
	const std::string& cascade_face_path, const std::string& cascade_eyes_path, int* headsIndex, bool useEyeCorrection, int headNum) {
	cv::CascadeClassifier face, eyes;
	face.load(cascade_face_path);
	cv::Mat gray_img;
	cv::Mat face_tmp;
	cv::cvtColor(img, gray_img, cv::COLOR_BGR2GRAY);
	cv::equalizeHist(gray_img, face_tmp);
	std::vector<cv::Rect> detectedFaces, detectedEyes;
	face.detectMultiScale(face_tmp, detectedFaces, 1.1, 5, 0 | cv::CASCADE_SCALE_IMAGE);
	cv::Mat faceDistrict;
	int faceNum = detectedFaces.size();
	if (faceNum > 1) {
		for (int i = 0; i < faceNum; i++) {
			for (int j = i + 1; j < faceNum; j++) {
				if (detectedFaces[i].area() < detectedFaces[j].area()) std::swap(detectedFaces[i], detectedFaces[j]);
			}
		}
	}
	if (headNum > faceNum)headNum = faceNum;

	if (useEyeCorrection) {
		eyes.load(cascade_eyes_path);
		for (int i = 0; i < headNum; i++) {
			if (headsIndex[i] >= headDataset.size() || headsIndex[i] < 0)continue;
			faceDistrict = face_tmp(detectedFaces[i]);
			eyes.detectMultiScale(faceDistrict, detectedEyes, 1.1, 6, 0 | cv::CASCADE_SCALE_IMAGE);
			int eyeNum = detectedEyes.size();
			int eyeLxMax = 0, eyeRxMin = faceDistrict.cols;
			int indexLeftEye = 0, indexRightEye = 1;
			int cnt = 0, temp;
			if (eyeNum > 2) {
				for (auto eyeIter = detectedEyes.begin(); eyeIter != detectedEyes.end(); eyeIter++) {
					if (eyeIter->x > eyeLxMax) {
						eyeLxMax = eyeIter->x;
						indexRightEye = cnt;
					}
					if ((temp = eyeIter->x + eyeIter->width) < eyeRxMin) {
						eyeRxMin = temp;
						indexLeftEye = cnt;
					}
					cnt++;
				}
			}
			else if (eyeNum == 2) {
				if (detectedEyes[0].x > detectedEyes[1].x) {
					indexRightEye = 0;
					indexLeftEye = 1;
				}
			}
			else {
				headChange(img, detectedFaces[i], headDataset[headsIndex[i]]);
				continue;
			}

			cv::Point leftEyeCentre, rightEyeCentre;
			leftEyeCentre.x = detectedFaces[i].x + detectedEyes[indexLeftEye].x + (detectedEyes[indexLeftEye].width >> 1);
			leftEyeCentre.y = detectedFaces[i].y + detectedEyes[indexLeftEye].y + (detectedEyes[indexLeftEye].height >> 1);
			rightEyeCentre.x = detectedFaces[i].x + detectedEyes[indexRightEye].x + (detectedEyes[indexRightEye].width >> 1);
			rightEyeCentre.y = detectedFaces[i].y + detectedEyes[indexRightEye].y + (detectedEyes[indexRightEye].height >> 1);
			headChange(img, detectedFaces[i], headDataset[headsIndex[i]], leftEyeCentre, rightEyeCentre);
		}
	}
	else {
		for (int i = 0; i < faceNum; i++) {
			headChange(img, detectedFaces[i], headDataset[headsIndex[i]]);
		}
	}
	return faceNum;
}

int detectChangeHeads(cv::InputOutputArray img, const std::string& cascade_face_path, const std::string& cascade_eyes_path,
	const headData& head, bool useEyeCorrection, int headNum) {
	std::vector<headData> heads;
	heads.emplace_back(head);
	int* headsIndex = (int*)malloc(headNum * sizeof(int));
	memset(headsIndex, 0, headNum * sizeof(int));
	int faceNum = detectChangeHeads(img, heads, cascade_face_path, cascade_eyes_path, headsIndex, useEyeCorrection, headNum);
	free(headsIndex);
	return faceNum;
}