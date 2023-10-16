#ifndef ALPHA_CV_H
#define ALPHA_CV_H

#include <opencv2/opencv.hpp>
#include <windows.h>
#include <vector>
#include <string>

namespace alphaCV {

	void		getStringSize(HDC hDC, const char* str, int& w, int& h);

	cv::Size	getTextSize(const char* str,
							int fontSize,
							const char* font,
							int lineWidth = 5,
							bool italic = false,
							bool underline = false);

	cv::Size	putTextZH(cv::Mat& dst,
						const char* str,
						cv::Point org,
						cv::Scalar color,
						int fontSize,
						const char* font = "Arial",
						int lineWidth = 5,
						bool italic = false,
						bool underline = false);

	void		roundedRect(cv::Mat& InputOutputMat,
							cv::Point location,
							int width,
							int height,
							int radius,
							cv::Scalar color);

}

void split(const std::string& str, const char* delim, std::vector<std::string>& result);

typedef enum mergeMode { MERGE_ADD = 1, MERGE_MULTIPLY = 2 };

int mergeImage(cv::InputArray src, cv::InputOutputArray dst, double scale, mergeMode method = MERGE_ADD);
int mergeImage(cv::InputArray src, cv::InputOutputArray dst, int x, int y, double scale, mergeMode method = MERGE_ADD);

#endif // !ALPHA_CV_H
