#ifndef HISTOGRAMMATCHING_H
#define HISTOGRAMMATCHING_H

#include <QQuickItem>
#include <QPointF>
#include "../../../image.h"
#include "../../controls/Curve.h"

class HistogramMatching : public Curve
{
    Q_OBJECT
public:
    explicit HistogramMatching(QQuickItem *parent = 0)
		: Curve(parent) {}

	void process(const cv::Mat &src, cv::Mat &dst, std::vector<double> &curve) {
		polka::adjustLuminace(src, dst,
			[&](const cv::Mat &src, cv::Mat &dst) {
			cv::Mat histogram;
			int hisSize = 256;
			float range[] = { 0, 255 };
			const float* histRange = { range };
			calcHist(&src, 1, 0, cv::Mat(), histogram, 1, &hisSize, &histRange, true, false);

			std::vector<double> accumulation1(hisSize);
			double sum1 = 0;
			cv::MatConstIterator_<float> it = histogram.begin<float>(), it_end = histogram.end<float>();
			for (size_t i = 0; it != it_end; ++it, i++) {
				sum1 += *it;
				accumulation1[i] = sum1;
			}

			std::vector<double> accumulation2(hisSize);
			double sum2 = 0;
			double ratio = static_cast<double>(curve.size() - 1) / static_cast<double>(hisSize - 1);
			for (size_t i = 0; i < hisSize; i++) {
				sum2 += curve[static_cast<size_t>(static_cast<double>(i) * ratio)];
				accumulation2[i] = sum2;
			}

			for (size_t i = 0; i < hisSize; i++) {
				accumulation1[i] /= sum1;
				accumulation2[i] /= sum2;
			}

			std::vector<double> newCurve(hisSize);
			size_t j = 0;
			for (size_t i = 0; i < hisSize; i++) {
				for (; j < hisSize; j++) {
					if (accumulation2[j] - accumulation1[i] >= 0) {
						newCurve[i] = j;
						break;
					}
				}
			}
			CurveFunc::process(src, dst, newCurve);
		});
	}
};

#endif // HISTOGRAMMATCHING_H
