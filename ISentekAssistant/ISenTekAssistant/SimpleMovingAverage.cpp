#include "StdAfx.h"
#include "SimpleMovingAverage.h"


CSimpleMovingAverage::CSimpleMovingAverage(unsigned int ulQueueSize) : pArray(NULL)
	, fAverage(0.0)
	, indexAdd(0)
	, indexRm(0)
	, nArraySize(ulQueueSize)
	, Sum(0.0)
	, bRmFlag(false)
	, BufferCurrentSize(0)
	, tmpIndex(0)
{
	if(nArraySize!=NULL) {
		pArray = new int[nArraySize];
		if(pArray==NULL) {
			printf("Memory not allocated");
		} else {
			for(int i=0; i<nArraySize; i++)
				pArray[i]=0;
		}
	}
}


CSimpleMovingAverage::~CSimpleMovingAverage(void)
{
	if(pArray != NULL) {
		delete [] pArray; // The memory location being freed.
		pArray = NULL;	//make pointer to point to null
		nArraySize = 0;
	}
}

float CSimpleMovingAverage::AddData(int Data)
{
//	static bool bRmFlag = false;
//	static int BufferCurrentSize = 0;
//	static int tmpIndex = 0;

	if(pArray != NULL) {

		indexAdd = tmpIndex;

		if(indexAdd >= nArraySize)
			indexAdd %= nArraySize;
		if(indexRm >= nArraySize)
			indexRm %= nArraySize;
		
		if(BufferCurrentSize<nArraySize-1) {
			BufferCurrentSize++;
		} else {
			if(bRmFlag)
				Sum -= pArray[indexRm++];
			else
				BufferCurrentSize++;
			
			bRmFlag = true;
		}

		pArray[indexAdd] = Data;
		Sum += Data;
		tmpIndex++;

		fAverage = Sum / BufferCurrentSize;
	}
	return fAverage;
}

int CSimpleMovingAverage::GetData(unsigned int index)  const
{	
	if(pArray != NULL) {
		return pArray[index];	
	} else
		return 0;
}