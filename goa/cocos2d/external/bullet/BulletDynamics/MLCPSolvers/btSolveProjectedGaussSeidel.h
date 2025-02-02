/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2013 Erwin Coumans  http://bulletphysics.org

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
///original version written by Erwin Coumans, October 2013

#ifndef BT_SOLVE_PROJECTED_GAUSS_SEIDEL_H
#define BT_SOLVE_PROJECTED_GAUSS_SEIDEL_H


#include "btMLCPSolverInterface.h"

class btSolveProjectedGaussSeidel : public btMLCPSolverInterface
{
public:
	virtual bool solveMLCP(const btMatrixXu & A, const btVectorXu & b, btVectorXu& x, const btVectorXu & lo,const btVectorXu & hi,const btAlignedObjectArray<int>& limitDependency, int numIterations, bool useSparsity = true) override
	{
		//A is a m-n matrix, m rows, n columns
		btAssert(A.rows() == b.rows());

		int i, j, numRows = A.rows();
	
		float delta;

		for (int k = 0; k <numIterations; k++)
		{
			for (i = 0; i <numRows; i++)
			{
				delta = 0.0f;
				if (useSparsity)
				{
					for (int h=0;h<A.m_rowNonZeroElements1[i].size();h++)
					{
						int j = A.m_rowNonZeroElements1[i][h];
						if (j != i)//skip main diagonal
						{
							delta += A(i,j) * x[j];
						}
					}
				} else
				{
					for (j = 0; j <i; j++) 
						delta += A(i,j) * x[j];
					for (j = i+1; j<numRows; j++) 
						delta += A(i,j) * x[j];
				}

				float aDiag = A(i,i);
				x [i] = (b [i] - delta) / A(i,i);
				float s = 1.f;

				if (limitDependency[i]>=0)
				{
					s = x[limitDependency[i]];
					if (s<0)
						s=1;
				}
			
				if (x[i]<lo[i]*s)
					x[i]=lo[i]*s;
				if (x[i]>hi[i]*s)
					x[i]=hi[i]*s;
			}
		}
		return true;
	}

};

#endif //BT_SOLVE_PROJECTED_GAUSS_SEIDEL_H
