#pragma once

#ifdef DATAPROCESS_EXPORTS
#define RIGIDREGISTRATION_API __declspec(dllexport)
#else
#define RIGIDREGISTRATION_API __declspec(dllimport)
#endif

namespace DataProcess
{
	class RIGIDREGISTRATION_API PointCloud_RigidRegistration
	{
	public:
		PointCloud_RigidRegistration();										//���캯��
		~PointCloud_RigidRegistration();									//��������

	private:
		static int m_itmax;													//����������
		static int m_ncom;													//
		static double *m_pcom;												//
		static double *m_pxicom;											//
		static double *m_pxt;												//
		static double(*m_pnrfunc)(void *pRegis, double[]);					//
		static void *m_pRegis;												//
	public:
		void powell(void *pRegis, double p[], double **xi, int n, double ftol, int *iter, double *fret,
			double(*func)(void *pRegis, double[]));							//ͨ��powell�������������Ż�����
	private:
		void linmin(double p[], double xi[], int n, double *fret);			//����ĳһ�������ϵ���Сֵ
		double brent(double ax, double bx, double cx, double(*f)(double), double tol,
			double *xmin);													//brent��������һά�����Ż�����
		void mnbrak(double *ax, double *bx, double *cx, double *fa, double *fb,
			double *fc, double(*func)(double));								//

		static double  f1dim(double x);										//
	};
}