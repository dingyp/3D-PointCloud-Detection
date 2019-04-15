#include "stdafx.h"
#include "nrutil.h"
#include "PointCloud_RigidRegistration.h"

using namespace DataProcess;

#define CGOLD 0.3819660
#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);
#define GOLD 1.618034
#define GLIMIT 100.0
#define TINY 1.0e-20
#define ZEPS 1.0e-10
#define TOL  2.0e-4

int PointCloud_RigidRegistration::m_itmax = 20;//m_itmax最大迭代次数
int PointCloud_RigidRegistration::m_ncom = 0;
double* PointCloud_RigidRegistration::m_pcom = NULL;
double* PointCloud_RigidRegistration::m_pxicom = NULL;
double* PointCloud_RigidRegistration::m_pxt = NULL;
void * PointCloud_RigidRegistration::m_pRegis = NULL;

typedef double(*pfv)(void *pRegis, double[]);
pfv PointCloud_RigidRegistration::m_pnrfunc = NULL;

PointCloud_RigidRegistration::PointCloud_RigidRegistration(void)
{
	m_itmax = 20;//m_itmax最大迭代次数
	m_ncom = 0;
	m_pcom = NULL;
	m_pxicom = NULL;
	m_pxt = NULL;
}

PointCloud_RigidRegistration::~PointCloud_RigidRegistration(void)
{
	delete m_pcom;
	delete m_pxicom;
	delete m_pxt;
	m_pcom = NULL;
	m_pxicom = NULL;
	m_pxt = NULL;
}


void PointCloud_RigidRegistration::mnbrak(double *ax, double *bx, double *cx, double *fa, double *fb, double *fc,
	double(*func)(double))
{
	double ulim, u, r, q, fu, dum;
	*fa = (*func)(*ax);
	*fb = (*func)(*bx);
	if (*fb > *fa)
	{
		SHFT(dum, *ax, *bx, dum);
		SHFT(dum, *fb, *fa, dum);
	}
	*cx = (*bx) + GOLD * (*bx - *ax);
	*fc = (*func)(*cx);

	while (*fb > *fc)
	{
		r = (*bx - *ax)*(*fb - *fc);
		q = (*bx - *cx)*(*fb - *fa);
		u = (*bx) - ((*bx - *cx)*q - (*bx - *ax)*r) / (2.0*SIGN(FMAX(fabs(q - r), TINY), q - r));///////////////////////////////////
		ulim = (*bx) + GLIMIT * (*cx - *bx);
		if ((*bx - u)*(u - *cx) > 0.0)
		{
			fu = (*func)(u);
			if (fu < *fc)
			{
				*ax = (*bx);
				*bx = u;
				*fa = (*fb);
				*fb = fu;
				return;
			}
			else if (fu > *fb)
			{
				*cx = u;
				*fc = fu;
				return;
			}
			u = (*cx) + GOLD * (*cx - *bx);
			fu = (*func)(u);
		}
		else if ((*cx - u)*(u - ulim) > 0.0)
		{
			fu = (*func)(u);
			if (fu < *fc)
			{
				SHFT(*bx, *cx, u, *cx + GOLD * (*cx - *bx))
					SHFT(*fb, *fc, fu, (*func)(u))
			}
		}
		else if ((u - ulim)*(ulim - *cx) >= 0.0)
		{
			u = ulim;
			fu = (*func)(u);
		}
		else
		{
			u = (*cx) + GOLD * (*cx - *bx);
			fu = (*func)(u);
		}
		SHFT(*ax, *bx, *cx, u)
			SHFT(*fa, *fb, *fc, fu)
	}
}

double PointCloud_RigidRegistration::f1dim(double x)
{
	int j;
	double f;
	for (j = 0; j<m_ncom; j++)
		m_pxt[j] = m_pcom[j] + x * m_pxicom[j];

	f = (*m_pnrfunc)(m_pRegis, m_pxt);

	return f;
}

//brent方法计算一维的最优化参数
double PointCloud_RigidRegistration::brent(double ax, double bx, double cx, double(*f)(double), double tol,
	double *xmin)
{
	int iter = 0;
	double a = 0, b = 0, d = 0, etemp = 0, fu = 0, fv = 0, fw = 0, fx = 0, p = 0, q = 0, r = 0;
	double tol1 = 0, tol2 = 0, u = 0, v = 0, w = 0, x = 0, xm = 0;
	double e = 0.0;

	a = (ax < cx ? ax : cx);
	b = (ax > cx ? ax : cx);
	x = w = v = bx;
	fw = fv = fx = (*f)(x);

	for (iter = 1; iter <= m_itmax; iter++)//m_itmax最大迭代次数
	{
		xm = 0.5*(a + b);
		tol1 = tol * fabs(x);
		tol2 = 2.0*(tol1 + ZEPS);
		if (fabs(x - xm) <= (tol2 - 0.5*(b - a)))
		{
			*xmin = x;
			return fx;
		}
		if (fabs(e) > tol1)
		{
			r = (x - w)*(fx - fv);
			q = (x - v)*(fx - fw);
			p = (x - v)*q - (x - w)*r;
			q = 2.0*(q - r);
			if (q > 0.0) p = -p;
			q = fabs(q);
			etemp = e;
			e = d;
			if (fabs(p) >= fabs(0.5*q*etemp) || p <= q * (a - x) || p >= q * (b - x))
			{
				e = (x >= xm ? a - x : b - x);
				d = CGOLD * e;
			}
			else
			{
				d = p / q;
				u = x + d;
				if (u - a < tol2 || b - u < tol2)
				{
					d = SIGN(tol1, xm - x);/////////////////////////////////////////////////////////////////////
				}
					
			}
		}
		else
		{
			e = (x >= xm ? a - x : b - x);
			d = CGOLD * e;
		}

		u = (fabs(d) >= tol1 ? x + d : x + SIGN(tol1, d));//////////////////////////////////////////////////////////
		fu = (*f)(u);
		if (fu <= fx)
		{
			if (u >= x)
				a = x;
			else
				b = x;
			SHFT(v, w, x, u)
				SHFT(fv, fw, fx, fu)
		}
		else
		{
			if (u < x)
				a = u;
			else
				b = u;
			if (fu <= fw || w == x)
			{
				v = w;
				w = u;
				fv = fw;
				fw = fu;
			}
			else if (fu <= fv || v == x || v == w)
			{
				v = u;
				fv = fu;
			}
		}
	}
	TRACE("Too many iterations in brent\n");
	*xmin = x;
	return fx;
}

//计算某一个方向上的最小值
void PointCloud_RigidRegistration::linmin(double p[], double xi[], int n, double *fret)
{
	int j;
	double xx, xmin, fx, fb, fa, bx, ax;

	for (j = 0; j<n; j++)
	{
		m_pcom[j] = p[j];
		m_pxicom[j] = xi[j];
	}
	ax = 0.0;
	xx = 1.0;
	mnbrak(&ax, &xx, &bx, &fa, &fx, &fb, f1dim);
	*fret = brent(ax, xx, bx, f1dim, TOL, &xmin);//brent方法计算一维的最优化参数

	for (j = 0; j<n; j++)
	{
		xi[j] *= xmin;
		p[j] += xi[j];
	}
	return;
}

//通过powell方法，计算最优化参数
//pRegis:指针，是回调函数中的参数
//p:配准结果p[0]、p[1]、p[2]、p[3]、p[4]、p[5]
//xi:向量
//n:6，是配准结果向量的长度
//ftol:1.0e-7
//iter:感觉像迭代次数，达到最大迭代次数便返回
//fret：互信息计算结果
//func:回调函数
void PointCloud_RigidRegistration::powell(void *pRegis, double p[], double **xi, int n, double ftol, int *iter, double *fret,
	double(*func)(void *pRegis, double[]))
{
	int i, ibig, j;
	double del, fp, fptt, t, *pt, *ptt, *xit;

	if (m_pcom != NULL) delete m_pcom;
	if (m_pxicom != NULL) delete m_pxicom;
	if (m_pxt != NULL) delete m_pxt;

	m_ncom = n;
	m_pcom = new double[n];
	m_pxicom = new double[n];
	m_pxt = new double[n];

	pt = new double[n];
	ptt = new double[n];
	xit = new double[n];

	m_pRegis = pRegis;
	m_pnrfunc = func;

	*fret = (*func)(m_pRegis, p);
	for (j = 0; j < n; j++)
	{
		pt[j] = p[j];
	}

	for (*iter = 1;; ++(*iter))
	{
		fp = (*fret);
		ibig = 0; del = 0.0;
		for (i = 0; i<n; i++)
		{
			for (j = 0; j < n; j++)
			{
				xit[j] = xi[j][i];
			}

			fptt = (*fret);
			linmin(p, xit, n, fret);//计算某一个方向上的最小值
			if (fptt - (*fret) > del)
			{
				del = fptt - (*fret);
				ibig = i;
			}
		}

		if (2.0*(fp - (*fret)) <= ftol * (fabs(fp) + fabs(*fret)) + TINY)
		{
			delete[]xit;
			delete[]ptt;
			delete[]pt;
			return;
		}

		if (*iter >= m_itmax)//m_itmax最大迭代次数
		{
			TRACE("powell exceeding maximum iterations.\n");
			delete[]xit;
			delete[]ptt;
			delete[]pt;
			return;
		}

		for (j = 0; j<n; j++)
		{
			ptt[j] = 2.0*p[j] - pt[j];
			xit[j] = p[j] - pt[j];
			pt[j] = p[j];
		}

		fptt = (*func)(m_pRegis, ptt);

		if (fptt < fp)
		{
			t = 2.0*(fp - 2.0*(*fret) + fptt)*SQR(fp - (*fret) - del) - del * SQR(fp - fptt);/////////////////////////////////////////
			if (t < 0.0)
			{
				linmin(p, xit, n, fret);
				for (j = 0; j<n; j++)
				{
					xi[j][ibig] = xi[j][n - 1];
					xi[j][n - 1] = xit[j];
				}
			}
		}
	}

	delete[]xit;
	delete[]ptt;
	delete[]pt;
	return;
}