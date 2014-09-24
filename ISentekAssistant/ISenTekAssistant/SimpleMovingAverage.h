#pragma once
class CSimpleMovingAverage
{
public:
	CSimpleMovingAverage(unsigned int ulQueueSize=0);
	~CSimpleMovingAverage(void);

protected:
	float fAverage;
	int *pArray;
	int indexAdd;
	int indexRm;
	unsigned int nArraySize; 	
	float Sum;

	int tmpIndex;
	int BufferCurrentSize;
	bool bRmFlag;

public:	
	float AddData(int Data);
	inline int GetIndex(void)  const		{	return indexAdd;	}
	inline float GetAverage(void) const		{	return fAverage;	}
	inline int GetSum(void)  const		{	return (int)Sum;	}
	int GetData(unsigned int index)  const;
	inline unsigned int GetSize(void)  const		{	return nArraySize;	}
};


/*
inline int CAverageQueue::GetAverage(void)
{
}*/

