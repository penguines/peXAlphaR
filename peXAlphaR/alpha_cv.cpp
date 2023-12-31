#include "alpha_cv.h"

void alphaCV::getStringSize(HDC hDC, const char* str, int& w, int& h){
	SIZE size;
	GetTextExtentPoint32A(hDC, str, strlen(str), &size);
	w = size.cx;
	h = size.cy;
}

cv::Size alphaCV::getTextSize(const char* str, int fontSize, const char* font, int lineWidth, bool italic, bool underline) {
	LOGFONTA lf;
	lf.lfHeight = -fontSize;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = lineWidth;
	lf.lfItalic = italic;   //斜体
	lf.lfUnderline = underline; //下划线
	lf.lfStrikeOut = 0;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = 0;
	lf.lfClipPrecision = 0;
	lf.lfQuality = PROOF_QUALITY;
	lf.lfPitchAndFamily = 0;
	strcpy_s(lf.lfFaceName, font);

	HFONT hf = CreateFontIndirectA(&lf);
	HDC hDC = CreateCompatibleDC(0);
	HFONT hOldFont = (HFONT)SelectObject(hDC, hf);

	int str_width = 0, str_height = 0;
	int tmp_width = 0, tmp_height = 0;
	int line_cnt = 0;
	//改用std::vector<std::string>
	std::vector<std::string> str_lines;
	split(str, "\n", str_lines);
	for (auto iter = str_lines.begin(); iter != str_lines.end(); iter++) {
		getStringSize(hDC, iter->c_str(), tmp_width, tmp_height);
		str_width = max(str_width, tmp_width);
		str_height = max(str_height, tmp_height);
		line_cnt++;
	}
	str_height *= line_cnt;

	SelectObject(hDC, hOldFont);
	DeleteObject(hf);
	DeleteObject(hDC);
	return cv::Size(str_width, str_height);
}

cv::Size alphaCV::putTextZH(cv::Mat& dst, const char* str, cv::Point org, cv::Scalar color, int fontSize, const char* font, int lineWidth, bool italic, bool underline){
	CV_Assert(dst.data != 0 && (dst.channels() == 1 || dst.channels() == 3));

	int x, y, r, b;
	if (org.x > dst.cols || org.y > dst.rows) {
		return cv::Size(0, 0);
	}
	x = org.x < 0 ? -org.x : 0;
	y = org.y < 0 ? -org.y : 0;

	LOGFONTA lf;
	lf.lfHeight = -fontSize;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = lineWidth;
	lf.lfItalic = italic;   //斜体
	lf.lfUnderline = underline; //下划线
	lf.lfStrikeOut = 0;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = 0;
	lf.lfClipPrecision = 0;
	lf.lfQuality = PROOF_QUALITY;
	lf.lfPitchAndFamily = 0;
	strcpy_s(lf.lfFaceName, font);

	HFONT hf = CreateFontIndirectA(&lf);
	HDC hDC = CreateCompatibleDC(0);
	HFONT hOldFont = (HFONT)SelectObject(hDC, hf);

	int str_width = 0, str_height = 0;
	int tmp_width = 0, tmp_height = 0;
	int row_height = 0;
	int line_cnt = 0;
	//改用std::vector<std::string>
	std::vector<std::string> str_lines;
	split(str, "\n", str_lines);
	for (auto iter = str_lines.begin(); iter != str_lines.end(); iter++) {
		getStringSize(hDC, iter->c_str(), tmp_width, tmp_height);
		str_width = max(str_width, tmp_width);
		str_height = max(str_height, tmp_height);
		line_cnt++;
	}
	row_height = str_height;
	str_height *= line_cnt;
	
	cv::Size str_size(str_width, str_height);

	if (org.x + str_width < 0 || org.y + str_height < 0){
		SelectObject(hDC, hOldFont);
		DeleteObject(hf);
		DeleteObject(hDC);
		return cv::Size(0, 0);
	}

	r = org.x + str_width > dst.cols ? dst.cols - org.x - 1 : str_width - 1;
	b = org.y + str_height > dst.rows ? dst.rows - org.y - 1 : str_height - 1;
	org.x = org.x < 0 ? 0 : org.x;
	org.y = org.y < 0 ? 0 : org.y;

	BITMAPINFO bmp = { 0 };
	BITMAPINFOHEADER& bih = bmp.bmiHeader;
	int drawline_step = (str_width * 3 % 4 == 0) ? (str_width * 3) : (str_width * 3 + 4 - ((str_width * 3) % 4));

	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = str_width;
	bih.biHeight = str_height;
	bih.biPlanes = 1;
	bih.biBitCount = 24;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = str_height * drawline_step;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;

	void* pDibData = 0;
	HBITMAP hBmp = CreateDIBSection(hDC, &bmp, DIB_RGB_COLORS, &pDibData, 0, 0);

	CV_Assert(pDibData != 0);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hDC, hBmp);

	SetTextColor(hDC, RGB(255, 255, 255));
	SetBkColor(hDC, 0);
	
	int outtext_y = 0;
	for (auto iter = str_lines.begin(); iter != str_lines.end(); iter++) {
		TextOutA(hDC, 0, outtext_y, iter->c_str(), iter->length());
		outtext_y += row_height;
	}
	
	uchar* dst_data = (uchar*)dst.data;
	int dst_step = dst.step / sizeof(dst_data[0]);
	unsigned char* ptr_img = (unsigned char*)dst.data + org.x * dst.channels() + org.y * dst_step;
	unsigned char* ptr_str = (unsigned char*)pDibData + x * 3;
	for (int tty = y; tty <= b; ++tty){
		unsigned char* subImg = ptr_img + (tty - y) * dst_step;
		unsigned char* subStr = ptr_str + (str_height - tty - 1) * drawline_step;
		for (int ttx = x; ttx <= r; ++ttx){
			for (int n = 0; n < dst.channels(); ++n) {
				double vtxt = subStr[n] / 255.0;
				int cvv = vtxt * color.val[n] + (1 - vtxt) * subImg[n];
				subImg[n] = cvv > 255 ? 255 : (cvv < 0 ? 0 : cvv);
			}

			subStr += 3;
			subImg += dst.channels();
		}
	}

	SelectObject(hDC, hOldBmp);
	SelectObject(hDC, hOldFont);
	DeleteObject(hf);
	DeleteObject(hBmp);
	DeleteDC(hDC);
	return str_size;
}

void alphaCV::roundedRect(cv::Mat& InputOutputMat, cv::Point location, int width, int height, int radius, cv::Scalar color) {
	int innerW = width - (radius * 2), innerH = height - (radius * 2);
	CV_Assert(innerW > 0 && innerH > 0);
	cv::Point rectLU(location.x + radius, location.y + radius);
	cv::Point rectRD(rectLU.x + innerW, rectLU.y + innerH);
	cv::rectangle(InputOutputMat, rectLU, rectRD, color, -1, cv::LINE_AA);
	cv::rectangle(InputOutputMat, rectLU, cv::Point(rectLU.x + innerW, rectLU.y - radius), color, -1, cv::LINE_AA);
	cv::rectangle(InputOutputMat, rectLU, cv::Point(rectLU.x - radius, rectLU.y + innerH), color, -1, cv::LINE_AA);
	cv::rectangle(InputOutputMat, rectRD, cv::Point(rectRD.x - innerW, rectRD.y + radius), color, -1, cv::LINE_AA);
	cv::rectangle(InputOutputMat, rectRD, cv::Point(rectRD.x + radius, rectRD.y - innerH), color, -1, cv::LINE_AA);

	cv::circle(InputOutputMat, rectLU, radius, color, -1, cv::LINE_AA);
	cv::circle(InputOutputMat, rectRD, radius, color, -1, cv::LINE_AA);
	cv::circle(InputOutputMat, cv::Point(rectLU.x + innerW, rectLU.y), radius, color, -1, cv::LINE_AA);
	cv::circle(InputOutputMat, cv::Point(rectLU.x, rectLU.y + innerH), radius, color, -1, cv::LINE_AA);
}

void split(const std::string& str, const char* delim, std::vector<std::string>& result) {
	char* tmp_str;
	char* next_tok = nullptr;
	char* tok = nullptr;

	size_t len = str.length();
	tmp_str = new char[len + 1];
	strcpy_s(tmp_str, len + 1, str.c_str());
	tok = strtok_s(tmp_str, delim, &next_tok);
	while (tok != nullptr) {
		result.emplace_back(tok);
		tok = strtok_s(nullptr, delim, &next_tok);
	}
	delete[] tmp_str;
}

int mergeImage(cv::InputArray src, cv::InputOutputArray dst, double scale, mergeMode method) {
	if (dst.channels() != 3 || src.channels() != 4) {
		return 0;
	}
	if (scale < 0.01) {
		return 0;
	}
	std::vector<cv::Mat> src_channels;
	std::vector<cv::Mat> dst_channels;
	cv::split(src, src_channels);
	cv::split(dst, dst_channels);
	CV_Assert(src_channels.size() == 4 && dst_channels.size() == 3);

	if (scale < 1) {
		src_channels[3] *= scale;
		scale = 1;
	}
	cv::Mat alpha_tmp;
	switch (method) {
	case MERGE_ADD:
		for (int i = 0; i < 3; i++) {
			dst_channels[i] = dst_channels[i].mul(255.0 / scale - src_channels[3], scale / 255.0);
			dst_channels[i] += src_channels[i].mul(src_channels[3], scale / 255.0);
		}
		break;
	case MERGE_MULTIPLY:
		src_channels[3].convertTo(alpha_tmp, CV_32FC1, scale / 255.);
		for (int i = 0; i < 3; i++) {
			cv::Mat src_tmp, dst_tmp;
			dst_channels[i].convertTo(dst_tmp, CV_32FC1, 1. / 255.);
			src_channels[i].convertTo(src_tmp, CV_32FC1, 1. / 255.);
			//dst_tmp = dst_tmp.mul(1. - alpha_tmp);
			//src_tmp = src_tmp.mul(alpha_tmp);
			dst_tmp = dst_tmp.mul(src_tmp);
			dst_tmp.convertTo(dst_channels[i], CV_8UC1, 255);
		}
		break;
	}
	merge(dst_channels, dst);
	return true;
}

int mergeImage(cv::InputArray src, cv::InputOutputArray dst, int x, int y, double scale, mergeMode method) {
	cv::Rect src_roi(0, 0, src.cols(), src.rows());
	int srcMax_x = x + src.cols(), srcMax_y = y + src.rows();
	if (x < 0) {
		src_roi.width += x - 1;
		src_roi.x = 1 - x;
		x = 0;
	}
	if (y < 0) {
		src_roi.height += y - 1;
		src_roi.y = 1 - y;
		y = 0;
	}
	if (srcMax_x >= dst.cols()) {
		src_roi.width -= srcMax_x - dst.cols() + 1;
	}
	if (srcMax_y >= dst.rows()) {
		src_roi.height -= srcMax_y - dst.rows() + 1;
	}
	cv::Mat img_src = src.getMat();
	cv::Mat& img_dst = dst.getMatRef();
	cv::Mat img_droi = img_dst(cv::Rect(x, y, src_roi.width, src_roi.height));
	cv::Mat img_sroi = img_src(src_roi);
	return mergeImage(img_sroi, img_droi, scale, method);
}