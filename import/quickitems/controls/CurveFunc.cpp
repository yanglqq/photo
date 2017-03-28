#include <algorithm>
#include <cmath>
#include <cstdint>
#include <memory>
#include <QString>

#include <polka_exception.h>

#include "../../image.h"
#include "../ImageManager.h"
#include "CurveFunc.h"

#define M_PI 3.14159265358979323846

//------------------------------------------------------------------------------
/*  Purpose:
//
//    D3_NP_FS factors and solves a D3 system.
//
//  Discussion:
//
//    The D3 storage format is used for a tridiagonal matrix.
//    The superdiagonal is stored in entries (1,2:N), the diagonal in
//    entries (2,1:N), and the subdiagonal in (3,1:N-1).  Thus, the
//    original matrix is "collapsed" vertically into the array.
//
//    This algorithm requires that each diagonal entry be nonzero.
//    It does not use pivoting, and so can fail on systems that
//    are actually nonsingular.
//
//  Example:
//
//    Here is how a D3 matrix of order 5 would be stored:
//
//       *  A12 A23 A34 A45
//      A11 A22 A33 A44 A55
//      A21 A32 A43 A54  *
//
//  Modified:
//
//    07 January 2005    Shawn Freeman (pure C modifications)
//    15 November 2003    John Burkardt
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the order of the linear system.
//
//    Input/output, double A[3*N].
//    On input, the nonzero diagonals of the linear system.
//    On output, the data in these vectors has been overwritten
//    by factorization information.
//
//    Input, double B[N], the right hand side.
//
//    Output, double D3_NP_FS[N], the solution of the linear system.
//    This is NULL if there was an error because one of the diagonal
//    entries was zero.
*/
double *d3_np_fs(size_t n, double a[], double b[]) {
    int i;
    double *x;
    double xmult;
    //
    //  Check.
    //
    for (i = 0; i < n; i++) {
        if (a[1+i*3] == 0.0E+00) {
            return NULL;
        }
    }
    x = (double *)calloc(n, sizeof(double));
    if (!x)
      throw polka_exception(SOMETHING_FAILED, "calloc failed!");

    for (i = 0; i < n; i++) {
        x[i] = b[i];
    }
    for (i = 1; i < n; i++) {
        xmult = a[2+(i-1)*3] / a[1+(i-1)*3];
        a[1+i*3] = a[1+i*3] - xmult * a[0+i*3];
        x[i] = x[i] - xmult * x[i-1];
    }
    x[n-1] = x[n-1] / a[1+(n-1)*3];
    for (i = static_cast<int>(n)-2; 0 <= i; i--) {
        x[i] = (x[i] - a[0+(i+1)*3] * x[i+1]) / a[1+i*3];
    }
    return x;
}

//------------------------------------------------------------------------------
/*  Purpose:
//
//    SPLINE_CUBIC_SET computes the second derivatives of a piecewise cubic spline.
//
//  Discussion:
//
//    For data interpolation, the user must call SPLINE_SET to determine
//    the second derivative data, passing in the data to be interpolated,
//    and the desired boundary conditions.
//
//    The data to be interpolated, plus the SPLINE_SET output, defines
//    the spline.  The user may then call SPLINE_VAL to evaluate the
//    spline at any point.
//
//    The cubic spline is a piecewise cubic polynomial.  The intervals
//    are determined by the "knots" or abscissas of the data to be
//    interpolated.  The cubic spline has continous first and second
//    derivatives over the entire interval of interpolation.
//
//    For any point T in the interval T(IVAL), T(IVAL+1), the form of
//    the spline is
//
//      SPL(T) = A(IVAL)
//             + B(IVAL) * ( T - T(IVAL) )
//             + C(IVAL) * ( T - T(IVAL) )**2
//             + D(IVAL) * ( T - T(IVAL) )**3
//
//    If we assume that we know the values Y(*) and YPP(*), which represent
//    the values and second derivatives of the spline at each knot, then
//    the coefficients can be computed as:
//
//      A(IVAL) = Y(IVAL)
//      B(IVAL) = ( Y(IVAL+1) - Y(IVAL) ) / ( T(IVAL+1) - T(IVAL) )
//        - ( YPP(IVAL+1) + 2 * YPP(IVAL) ) * ( T(IVAL+1) - T(IVAL) ) / 6
//      C(IVAL) = YPP(IVAL) / 2
//      D(IVAL) = ( YPP(IVAL+1) - YPP(IVAL) ) / ( 6 * ( T(IVAL+1) - T(IVAL) ) )
//
//    Since the first derivative of the spline is
//
//      SPL'(T) =     B(IVAL)
//              + 2 * C(IVAL) * ( T - T(IVAL) )
//              + 3 * D(IVAL) * ( T - T(IVAL) )**2,
//
//    the requirement that the first derivative be continuous at interior
//    knot I results in a total of N-2 equations, of the form:
//
//      B(IVAL-1) + 2 C(IVAL-1) * (T(IVAL)-T(IVAL-1))
//      + 3 * D(IVAL-1) * (T(IVAL) - T(IVAL-1))**2 = B(IVAL)
//
//    or, setting H(IVAL) = T(IVAL+1) - T(IVAL)
//
//      ( Y(IVAL) - Y(IVAL-1) ) / H(IVAL-1)
//      - ( YPP(IVAL) + 2 * YPP(IVAL-1) ) * H(IVAL-1) / 6
//      + YPP(IVAL-1) * H(IVAL-1)
//      + ( YPP(IVAL) - YPP(IVAL-1) ) * H(IVAL-1) / 2
//      =
//      ( Y(IVAL+1) - Y(IVAL) ) / H(IVAL)
//      - ( YPP(IVAL+1) + 2 * YPP(IVAL) ) * H(IVAL) / 6
//
//    or
//
//      YPP(IVAL-1) * H(IVAL-1) + 2 * YPP(IVAL) * ( H(IVAL-1) + H(IVAL) )
//      + YPP(IVAL) * H(IVAL)
//      =
//      6 * ( Y(IVAL+1) - Y(IVAL) ) / H(IVAL)
//      - 6 * ( Y(IVAL) - Y(IVAL-1) ) / H(IVAL-1)
//
//    Boundary conditions must be applied at the first and last knots.
//    The resulting tridiagonal system can be solved for the YPP values.
//
//  Modified:
//
//    07 January 2005    Shawn Freeman (pure C modifications)
//    06 February 2004    John Burkardt
//
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of data points.  N must be at least 2.
//    In the special case where N = 2 and IBCBEG = IBCEND = 0, the
//    spline will actually be linear.
//
//    Input, double T[N], the knot values, that is, the points were data is
//    specified.  The knot values should be distinct, and increasing.
//
//    Input, double Y[N], the data values to be interpolated.
//
//    Input, int IBCBEG, left boundary condition flag:
//      0: the cubic spline should be a quadratic over the first interval;
//      1: the first derivative at the left endpoint should be YBCBEG;
//      2: the second derivative at the left endpoint should be YBCBEG.
//
//    Input, double YBCBEG, the values to be used in the boundary
//    conditions if IBCBEG is equal to 1 or 2.
//
//    Input, int IBCEND, right boundary condition flag:
//      0: the cubic spline should be a quadratic over the last interval;
//      1: the first derivative at the right endpoint should be YBCEND;
//      2: the second derivative at the right endpoint should be YBCEND.
//
//    Input, double YBCEND, the values to be used in the boundary
//    conditions if IBCEND is equal to 1 or 2.
//
//    Output, double SPLINE_CUBIC_SET[N], the second derivatives of the cubic spline.
*/
double *splineCubicSet(size_t n,
                       const std::vector<double> t,
                       const std::vector<double> y,
                       int ibcbeg,
                       double ybcbeg,
                       int ibcend,
                       double ybcend) {
    double *a   = nullptr;
    double *b   = nullptr;
    size_t  i   = 0;
    double *ypp = nullptr;

    for (; i < n - 1; i++) {
        if (t[i+1] <= t[i]) {
            throw polka_exception(SOMETHING_FAILED, "The knots must be strictly increasing, but T(%d) = %f, T(%d) = %f",
                i, t[i], i + 1, t[i + 1]);
        }
    }

    a = (double *)calloc(3 * n, sizeof(double));
    if (!a)
        throw polka_exception(SOMETHING_FAILED, "calloc failed!");
    std::unique_ptr<double> ptr_a(a);
    b = (double *)calloc(n, sizeof(double));
    if (!b)
        throw polka_exception(SOMETHING_FAILED, "calloc failed!");
    std::unique_ptr<double> ptr_b(b);
    //
    //  Set up the first equation.
    //
    if (ibcbeg == 0) {
        b[0] = 0.0E+00;
        a[1+0*3] = 1.0E+00;
        a[0+1*3] = -1.0E+00;
    } else if (ibcbeg == 1) {
        b[0] = (y[1] - y[0]) / (t[1] - t[0]) - ybcbeg;
        a[1+0*3] = (t[1] - t[0]) / 3.0E+00;
        a[0+1*3] = (t[1] - t[0]) / 6.0E+00;
    } else if (ibcbeg == 2) {
        b[0] = ybcbeg;
        a[1+0*3] = 1.0E+00;
        a[0+1*3] = 0.0E+00;
    } else {
        throw polka_exception(SOMETHING_FAILED, "IBCBEG must be 0, 1 or 2, but the input value is %d.", ibcbeg);
    }
    //
    //  Set up the intermediate equations.
    //
    for (i = 1; i < n-1; i++) {
        b[i] = (y[i+1] - y[i]) / (t[i+1] - t[i])
          - (y[i] - y[i-1]) / ( t[i] - t[i-1]);
        a[2+(i-1)*3] = (t[i] - t[i-1]) / 6.0E+00;
        a[1+ i   *3] = (t[i+1] - t[i-1]) / 3.0E+00;
        a[0+(i+1)*3] = (t[i+1] - t[i]) / 6.0E+00;
    }
    //
    //  Set up the last equation.
    //
    if (ibcend == 0) {
        b[n-1] = 0.0E+00;
        a[2+(n-2)*3] = -1.0E+00;
        a[1+(n-1)*3] = 1.0E+00;
    } else if (ibcend == 1) {
        b[n-1] = ybcend - ( y[n-1] - y[n-2] ) / ( t[n-1] - t[n-2] );
        a[2+(n-2)*3] = ( t[n-1] - t[n-2] ) / 6.0E+00;
        a[1+(n-1)*3] = ( t[n-1] - t[n-2] ) / 3.0E+00;
    } else if (ibcend == 2) {
        b[n-1] = ybcend;
        a[2+(n-2)*3] = 0.0E+00;
        a[1+(n-1)*3] = 1.0E+00;
    } else {
        throw polka_exception(SOMETHING_FAILED, "IBCBEG must be 0, 1 or 2, but the input value is %d.", ibcend);
    }
    //
    //  Solve the linear system.
    //
    if (n == 2 && ibcbeg == 0 && ibcend == 0) {
        ypp = (double *)calloc(2, sizeof(double));
        if (!ypp)
            throw polka_exception(SOMETHING_FAILED, "calloc failed!");
        ypp[0] = 0.0E+00;
        ypp[1] = 0.0E+00;
    } else {
        ypp = d3_np_fs(n, a, b);
        if (!ypp)
            throw polka_exception(SOMETHING_FAILED, "The linear system could not be solved.");
    }
    return ypp;
}

//------------------------------------------------------------------------------
/*  Purpose:
//
//    SPLINE_CUBIC_VAL evaluates a piecewise cubic spline at a point.
//
//  Discussion:
//
//    SPLINE_CUBIC_SET must have already been called to define the values of YPP.
//
//    For any point T in the interval T(IVAL), T(IVAL+1), the form of
//    the spline is
//
//      SPL(T) = A
//             + B * ( T - T(IVAL) )
//             + C * ( T - T(IVAL) )**2
//             + D * ( T - T(IVAL) )**3
//
//    Here:
//      A = Y(IVAL)
//      B = ( Y(IVAL+1) - Y(IVAL) ) / ( T(IVAL+1) - T(IVAL) )
//        - ( YPP(IVAL+1) + 2 * YPP(IVAL) ) * ( T(IVAL+1) - T(IVAL) ) / 6
//      C = YPP(IVAL) / 2
//      D = ( YPP(IVAL+1) - YPP(IVAL) ) / ( 6 * ( T(IVAL+1) - T(IVAL) ) )
//
//  Modified:
//
//    07 January 2005    Shawn Freeman (pure C modifications)
//    04 February 1999   John Burkardt
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int n, the number of knots.
//
//    Input, double Y[N], the data values at the knots.
//
//    Input, double T[N], the knot values.
//
//    Input, double TVAL, a point, typically between T[0] and T[N-1], at
//    which the spline is to be evalulated.  If TVAL lies outside
//    this range, extrapolation is used.
//
//    Input, double Y[N], the data values at the knots.
//
//    Input, double YPP[N], the second derivatives of the spline at
//    the knots.
//
//    Output, double *YPVAL, the derivative of the spline at TVAL.
//
//    Output, double *YPPVAL, the second derivative of the spline at TVAL.
//
//    Output, double SPLINE_VAL, the value of the spline at TVAL.
*/
double splineCubicVal(size_t n,
                      const std::vector<double> t,
                      double tval,
                      const std::vector<double> y,
                      double ypp[],
                      double *ypval,
                      double *yppval) {
    double dt;
    double h;
    size_t i;
    size_t ival;
    double yval;
    //
    //  Determine the interval [ T(I), T(I+1) ] that contains TVAL.
    //  Values below T[0] or above T[N-1] use extrapolation.
    //
    ival = n - 2;

    for (i = 0; i < n-1; i++) {
        if (tval < t[i+1]) {
          ival = i;
          break;
        }
    }
    //
    //  In the interval I, the polynomial is in terms of a normalized
    //  coordinate between 0 and 1.
    //
    dt = tval - t[ival];
    h = t[ival+1] - t[ival];

    yval = y[ival]
      + dt * ((y[ival+1] - y[ival]) / h
       - (ypp[ival+1] / 6.0E+00 + ypp[ival] / 3.0E+00) * h
      + dt * (0.5E+00 * ypp[ival]
      + dt * ((ypp[ival+1] - ypp[ival]) / (6.0E+00 * h))));

    *ypval = (y[ival+1] - y[ival]) / h
      - (ypp[ival+1] / 6.0E+00 + ypp[ival] / 3.0E+00) * h
      + dt * (ypp[ival]
      + dt * (0.5E+00 * (ypp[ival+1] - ypp[ival]) / h));

    *yppval = ypp[ival] + dt * (ypp[ival+1] - ypp[ival]) / h;

    return yval;
}

void CurveFunc::calcCosineCurve(std::vector<double> &curve, std::vector<QPointF> &anchors, double xMax, double yMax){
    curve.clear();
	if (anchors.size() == 0)
		return;

    curve.resize(static_cast<size_t>(xMax) + 1);

    for(size_t a = 0; a <= static_cast<size_t>(anchors[0].x() * xMax); a++)
        curve[a] = anchors[0].y() * yMax;
    for(size_t b = 0; b < anchors.size() - 1; b++) {
        double factor = anchors[b + 1].y() - anchors[b].y();
        double scale = M_PI / xMax / (anchors[b + 1].x() - anchors[b].x());
        for(size_t i = 0; i <= static_cast<size_t>(anchors[b + 1].x() * xMax - anchors[b].x() * xMax); i++) {
            curve[i + static_cast<size_t>(anchors[b].x() * xMax)] =
                (((1 - cosf(i * scale)) / 2 ) * factor + anchors[b].y()) * yMax;
        }
    }
    size_t maxFlipperIdx = anchors.size() - 1;
    for (size_t c = static_cast<size_t>(anchors[maxFlipperIdx].x() * xMax); c <= static_cast<size_t>(xMax); c++) {
        curve[c] = anchors[maxFlipperIdx].y() * yMax;
    }
}

void CurveFunc::calcLinearCurve(std::vector<double> &curve, std::vector<QPointF> &anchors, double xMax, double yMax){
	curve.clear();
	if (anchors.size() == 0)
		return;

    curve.resize(static_cast<size_t>(xMax) + 1);

    for(size_t a = 0; a <= static_cast<size_t>(anchors[0].x() * xMax); a++) {
        curve[a] = anchors[0].y() * yMax;
    }
    for (size_t b = 0; b < anchors.size() - 1; b++) {
        double factor = (anchors[b + 1].y() - anchors[b].y()) /
                   (anchors[b + 1].x() - anchors[b].x());
        for(size_t i = 0; i <= static_cast<size_t>(anchors[b + 1].x() * xMax - anchors[b].x() * xMax); i++) {
            curve[i + static_cast<size_t>(anchors[b].x() * xMax)] = anchors[b].y() * yMax + factor * (double)i;
        }
    }
    size_t maxFlipperIdx = anchors.size() - 1;
	for (size_t c = static_cast<size_t>(anchors[maxFlipperIdx].x() * xMax); c <= static_cast<size_t>(xMax); c++) {
        curve[c] = anchors[maxFlipperIdx].y() * yMax;
    }
}

void CurveFunc::calcSplineCurve(std::vector<double> &curve, std::vector<QPointF> &anchors, double xMax, double yMax){
	curve.clear();
	if (anchors.size() == 0)
		return;

    curve.resize(static_cast<size_t>(xMax) + 1);

    std::vector<double> hxs;
    std::vector<double> hys;
    for (int i = 0; i < anchors.size(); ++i) {
        hxs.push_back(anchors[i].x());
        hys.push_back(anchors[i].y());
    }
    double *ypp = splineCubicSet(anchors.size(), hxs, hys, 2, 0.0, 2, 0.0);
    std::unique_ptr<double> ptr_ypp(ypp);

    //first derivative at a point
    double ypval = 0;
    //second derivate at a point
    double yppval = 0;

    //Now build a table
    double firstPointX = hxs[0] * xMax;
    double firstPointY = hys[0] * yMax;
    double lastPointX  = hxs[anchors.size() - 1] * xMax;
    double lastPointY  = hys[anchors.size() - 1] * yMax;

    for (size_t i = 0; i < curve.size(); i++) {
        if (i < firstPointX) {
            curve[i] = firstPointY;
        } else if (i > lastPointX) {
            curve[i] = lastPointY;
        } else {
            double value = splineCubicVal(
                     anchors.size(),
                     hxs,
                     (qreal)i / yMax,
                     hys,
                     ypp,
                     &ypval,
                     &yppval) * yMax + 0.5f;
            curve[i] = std::max<double>(0, std::min<double>(yMax, value));
        }
    }
}

CurveFunc::CurveFunc()
    : ImageProccessor(), m_adjustable(true) {}

void  CurveFunc::process(const cv::Mat &src, cv::Mat &dst, std::vector<double> &curve) {
    polka::curveTransform(src, dst, curve);
}

QString CurveFunc::processImage(const QString &uri) {
	if (m_curve.size() == 0)
		return QString();
    const cv::Mat *image = ImageManager::get(uri);
    if (!image)
        return QString();
    calcCurve(image, m_curve, m_anchors, 255, 255);
    if (m_curve.size() == 0)
        return QString();
	cv::Mat dst;
	process(*image, dst, m_curve);
    return putImage(dst, uri);
}
