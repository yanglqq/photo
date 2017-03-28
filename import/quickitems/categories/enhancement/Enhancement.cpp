#include "../../../image.h"
#include "../../imageManager.h"
#include "Enhancement.h"

Enhancement::Enhancement(QQuickItem *parent)
    : QQuickItem(parent), ImageProccessor() {}

QString Enhancement::average(const QString &uri, double mean, double sigma, int count) {
    const cv::Mat *image = ImageManager::get(uri);
    if (image) {
        cv::Mat dst;
		cv::Mat rand;
		int channels = image->channels();
		image->convertTo(dst, CV_32F);
		rand.create(image->size(), CV_MAKE_TYPE(CV_32F, channels));
		cv::Scalar means(mean);
		cv::Scalar sigmas(sigma);
		cv::Mat original = dst.clone();
        for (int i = 0; i < count; i++) {
			cv::randn(rand, means, sigmas);
			rand += original;
            dst += rand;
        }
		std::vector<cv::Mat> planes;
		split(dst, planes);	
		cv::Mat plane(image->size(), CV_MAKE_TYPE(CV_8U, 1));
		dst.create(image->size(), CV_MAKE_TYPE(CV_8U, channels));
		for (int i = 0; i < channels; i++) {
			double minValue;
			double maxValue;
			int from_to[] = { 0,i };
			minMaxLoc(planes[i], &minValue, &maxValue);
			planes[i].convertTo(plane, CV_8U, 255.0 / (maxValue - minValue), -255.0 * minValue / (maxValue - minValue));
			cv::mixChannels(&plane, 1, &dst, 1, from_to, 1);
		}
        return putImage(dst, uri);
    }
    return QString();
}
