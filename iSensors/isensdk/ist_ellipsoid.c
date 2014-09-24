#include "ist_milieu.h"

#ifdef IST_ELLIPSOID

typedef ISTBOOL (*ist_FitModel)(Ellipsoid *, ISTFLOAT *, ISTFLOAT *, ISTFLOAT *, ISTINT);

struct ellipsoid_data {
	ISTFLOAT c[6*6];
	ISTFLOAT s[10*10];
	ISTFLOAT s11[6*6];
	ISTFLOAT s12[6*4];
	ISTFLOAT s12t[4*6];
	ISTFLOAT s22[4*4];
	ISTFLOAT s22_1[4*4];
	ISTFLOAT s22a[4*6];
	ISTFLOAT s22b[6*6];
	ISTFLOAT ss[6*6];
	ISTFLOAT sss[6*6];
	ISTFLOAT ssss[3*3];
	ISTFLOAT e[6*6];
	ISTFLOAT eigen_real[6];
	ISTFLOAT eigen_real3[3];
	ISTFLOAT eigen_imag[6];
	ISTFLOAT eigen_imag3[3];
	ISTFLOAT v1[6];
	ISTFLOAT v2[4];
	ISTFLOAT v[10];
	ISTFLOAT q[3*3];
	ISTFLOAT q_1[3*3];
	ISTFLOAT u[3];
	ISTFLOAT b[3];
	ISTFLOAT qb[3];
	ISTFLOAT dz[3*3];
	ISTFLOAT vdz[3*3];
	ISTFLOAT sq[3*3];
	ISTFLOAT a_1[3*3];
};

struct ellipsoid_model {
	ISTUINT max;
	ISTFLOAT *datas;
	ISTFLOAT mat[3][3];
	ISTFLOAT bias[3];
	ISTFLOAT radius;
	ISTFLOAT covar;
};

struct ellipsoid {
	Ellipsoid pub;
	ist_FitModel FitModel;
	struct ellipsoid_model model;
	ISTBOOL is_valid;
	ISTBOOL is_enable;
};

STATIC Ellipsoid *New(ISTUINT max);
STATIC ISTVOID Delete(Ellipsoid *self);
STATIC ISTBOOL Process(Ellipsoid *self, ISTFLOAT *x, ISTFLOAT *y, ISTFLOAT *z, ISTUINT ndata);
STATIC ISTVOID SetRadius(Ellipsoid *self, ISTFLOAT radius);
STATIC ISTVOID Enable(Ellipsoid *self);
STATIC ISTVOID Disable(Ellipsoid *self);
STATIC ISTBOOL FitModel(Ellipsoid *self, ISTFLOAT *x, ISTFLOAT *y, ISTFLOAT *z, ISTINT ndata);
STATIC ISTBOOL Reset(Ellipsoid *self, ISTUINT max);
STATIC ISTVOID MatDotMulMatT(ISTFLOAT *C, ISTFLOAT *A, ISTINT nrows, ISTINT ncols);
STATIC ISTVOID GetSubMat(ISTFLOAT *S, ISTINT mrows, ISTINT mcols, ISTFLOAT *A, ISTINT ncols, ISTINT row, ISTINT col);
STATIC ISTINT GetCholeskiLUDecomp(ISTFLOAT *A, ISTINT n);
STATIC ISTINT GetLowerTriangularInverse(ISTFLOAT *L, ISTINT n);
STATIC ISTINT GetCholeskiLUInverse(ISTFLOAT *LU, ISTINT n);
STATIC ISTVOID MatDotMulMats(ISTFLOAT *C, ISTFLOAT *A, ISTINT nrows, ISTINT ncols, ISTFLOAT *B, ISTINT mcols);
STATIC ISTVOID ExchangeRows(ISTFLOAT *A, ISTINT row1, ISTINT row2, ISTINT ncols);
STATIC ISTVOID ExchangeCols(ISTFLOAT *A, ISTINT col1, ISTINT col2, ISTINT nrows, ISTINT ncols);
STATIC ISTVOID CopyVector(ISTFLOAT *d, ISTFLOAT *s, ISTINT n);
STATIC ISTVOID GetIdentMat(ISTFLOAT *A, ISTINT n);
STATIC ISTVOID HessenbergElemTrans(ISTFLOAT *H, ISTFLOAT* S, ISTINT perm[], ISTINT n);
STATIC ISTINT GetHessenbergFormElem3(ISTFLOAT *A, ISTFLOAT* S);
STATIC ISTINT GetHessenbergFormElem6(ISTFLOAT *A, ISTFLOAT* S);
STATIC ISTVOID OneRealEigenValue(ISTFLOAT Hrow[], ISTFLOAT eigen_real[], ISTFLOAT eigen_imag[], ISTINT row, ISTFLOAT shift);
STATIC ISTVOID UpdateRow(ISTFLOAT *Hrow, ISTFLOAT cos, ISTFLOAT sin, ISTINT n, ISTINT row);
STATIC ISTVOID UpdateCol(ISTFLOAT* H, ISTFLOAT cos, ISTFLOAT sin, ISTINT n, ISTINT col);
STATIC ISTVOID UpdateTrans(ISTFLOAT *S, ISTFLOAT cos, ISTFLOAT sin, ISTINT n, ISTINT k);
STATIC ISTVOID TwoEigenValues(ISTFLOAT *H, ISTFLOAT* S, ISTFLOAT eigen_real[], ISTFLOAT eigen_imag[], ISTINT n, ISTINT row, ISTFLOAT shift);
STATIC ISTVOID DotMulAndSumOfShifts(ISTFLOAT *H, ISTINT n, ISTINT max_row, ISTFLOAT* shift, ISTFLOAT *trace, ISTFLOAT *det, ISTINT iteration);
STATIC ISTINT TwoConscutiveSmallSubDiags(ISTFLOAT* H, ISTINT min_row, ISTINT max_row, ISTINT n, ISTFLOAT trace, ISTFLOAT det);
STATIC ISTVOID DoubleQRStep(ISTFLOAT *H, ISTINT min_row, ISTINT max_row, ISTINT min_col, ISTFLOAT trace, ISTFLOAT det, ISTFLOAT *S, ISTINT n);
STATIC ISTVOID DoubleQRIter(ISTFLOAT *H, ISTFLOAT *S, ISTINT min_row, ISTINT max_row, ISTINT n, ISTFLOAT* shift, ISTINT iteration);
STATIC ISTVOID BackSubstituteRealVector(ISTFLOAT *H, ISTFLOAT eigen_real[], ISTFLOAT eigen_imag[], ISTINT row, ISTFLOAT zero_tolerance, ISTINT n);
STATIC ISTVOID ComplexDiv(ISTFLOAT x, ISTFLOAT y, ISTFLOAT u, ISTFLOAT v, ISTFLOAT* a, ISTFLOAT* b);
STATIC ISTVOID BackSubstituteComplexVector(ISTFLOAT *H, ISTFLOAT eigen_real[], ISTFLOAT eigen_imag[], ISTINT row, ISTFLOAT zero_tolerance, ISTINT n);
STATIC ISTVOID BackSubstitute(ISTFLOAT *H, ISTFLOAT eigen_real[], ISTFLOAT eigen_imag[], ISTINT n);
STATIC ISTVOID GetEigenVectors(ISTFLOAT *H, ISTFLOAT *S, ISTFLOAT eigen_imag[], ISTINT n);
STATIC ISTINT GetQRHessenbergMat(ISTFLOAT *H, ISTFLOAT *S, ISTFLOAT eigen_real[], ISTFLOAT eigen_imag[], ISTINT n, ISTINT max_iteration_count);
STATIC ISTVOID TransSquareMat(ISTFLOAT *A, ISTINT n);
STATIC ISTFLOAT CalcCovar(ISTINT ndata, ISTFLOAT *x, ISTFLOAT *y, ISTFLOAT *z, ISTFLOAT W[3][3], ISTFLOAT V[3], ISTFLOAT R);
STATIC ISTFLOAT RecalcRadius(ISTINT ndata, ISTFLOAT x[], ISTFLOAT y[], ISTFLOAT z[], ISTFLOAT W[3][3], ISTFLOAT V[3]);

STATIC Ellipsoid ThisClass = {0};
STATIC Ellipsoid *This = (Ellipsoid *)NULL;

ISTBOOL Reset(Ellipsoid *self, ISTUINT max)
{
	struct ellipsoid *s;
	struct ellipsoid_model *model;

	if (!self) {
		goto EXIT;
	}
	s = (struct ellipsoid *)self;
	model = &s->model;
	model->max = max;
	model->datas = (ISTFLOAT *)ist_realloc(model->datas, 10*max*sizeof(ISTFLOAT));
	if (!model->datas) {
		goto EXIT;
	}
	ist_memset(model->datas, 0, 10*max*sizeof(ISTFLOAT));
	model->radius = _float(IST_ELLIPSOID_RADIUS);
	s->is_valid = ISTFALSE;
	s->is_enable = ISTFALSE;
	return ISTTRUE;

EXIT:
	return ISTFALSE;
}

Ellipsoid *New(ISTUINT max)
{
	struct ellipsoid *s;

	if (max < 10) {
		goto EXIT;
	}
	s = (struct ellipsoid *)ist_calloc(1, sizeof(struct ellipsoid));
	if (!s) {
		goto EXIT;
	}
	ist_memset(s, 0, sizeof(*s));
	s->pub = ThisClass;
	s->pub.IsObject = ISTTRUE;
	s->FitModel = FitModel;
	if (!Reset((Ellipsoid *)s, max)) {
		goto EXIT_DELETE_ELLIPSOID;
	}
	return (Ellipsoid *)s;

EXIT_DELETE_ELLIPSOID:
	Delete((Ellipsoid *)s);

EXIT:
	return (Ellipsoid *)NULL;
}

ISTVOID Delete(Ellipsoid *self)
{
	struct ellipsoid *s;
	struct ellipsoid_model *model;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct ellipsoid *)self;
	model = &s->model;
	if (model->datas) {
		ist_free(model->datas);
		model->datas = NULL;
	}
	ist_free(s);
	s = NULL;
	return;

EXIT:
	return;
}

ISTVOID MatDotMulMatT(ISTFLOAT *C, ISTFLOAT *A, ISTINT nrows, ISTINT ncols)
{
	ISTINT i, j, k;
	ISTFLOAT *pAi0 = A;
	ISTFLOAT *pAj0;
	ISTFLOAT *pCi0 = C;
	ISTFLOAT *pCji;

	for (i = 0; i < nrows; pCi0 += nrows, pAi0 += ncols, i++) {
		pCji = pCi0 + i;
		pAj0 = pAi0; 
		for (j = i; j < nrows; pCji += nrows, j++) {
			*(pCi0 + j) = 0; 
			for (k = 0; k < ncols; k++)
				*(pCi0 + j) = _add(*(pCi0 + j), (_mul(*(pAi0 + k), *pAj0++)));
			*pCji = *(pCi0 + j);
		}
	}
}

ISTVOID GetSubMat(ISTFLOAT *S, ISTINT mrows, ISTINT mcols, ISTFLOAT *A, ISTINT ncols, ISTINT row, ISTINT col)
{
	ISTINT nbytes = sizeof(ISTFLOAT) * mcols;

	for (A += row * ncols + col; mrows > 0; A += ncols, S+= mcols, mrows--)
		ist_memcpy(S, A, nbytes);
}

ISTINT GetCholeskiLUDecomp(ISTFLOAT *A, ISTINT n)
{
	ISTINT i, k, p;
	ISTFLOAT *pLk0;
	ISTFLOAT *pLkp;
	ISTFLOAT *pLkk;
	ISTFLOAT *pLi0;
	ISTFLOAT recip;

	for (k = 0, pLk0 = A; k < n; pLk0 += n, k++) {
		pLkk = pLk0 + k;
		for (p = 0, pLkp = pLk0; p < k; pLkp += 1, p++)
			*pLkk = _sub(*pLkk, _mul(*pLkp, *pLkp));
		if (_le(*pLkk, 0))
			return -1;
		*pLkk = _sqrt(*pLkk);
		recip = _div(_float(1), *pLkk);
		pLi0 = pLk0 + n;
		for (i = k + 1; i < n; pLi0 += n, i++) {
			for (p = 0; p < k; p++)
				*(pLi0 + k) = _sub(*(pLi0 + k), _mul(*(pLi0 + p), *(pLk0 + p)));
			*(pLi0 + k) = _mul(*(pLi0 + k), recip);
			*(pLk0 + i) = *(pLi0 + k);
		}  
	}
	return 0;
}

ISTINT GetLowerTriangularInverse(ISTFLOAT *L, ISTINT n)
{
	ISTINT i, j, k;
	ISTFLOAT *pi, *pj, *pk;
	ISTFLOAT sum;

	for (k = 0, pk = L; k < n; pk += (n + 1), k++) {
		if (*pk == 0)
			return -1;
		else
			*pk = _div(_float(1), *pk);
	}
	for (i = 1, pi = L + n; i < n; i++, pi += n) {
		for (j = 0, pj = L; j < i; pj += n, j++) {
			sum = 0;
			for (k = j, pk = pj; k < i; k++, pk += n)
				sum = _add(sum, _mul(*(pi + k), *(pk + j)));
			*(pi + j) = _mul(_neg(*(pi + i)), sum);
		}
	}
	return 0;
}

ISTINT GetCholeskiLUInverse(ISTFLOAT *LU, ISTINT n)
{
	ISTINT i, j, k;
	ISTFLOAT *p_i, *p_j, *p_k;
	ISTFLOAT sum;

	if (GetLowerTriangularInverse(LU, n) < 0)
		return -1;
	for (i = 0, p_i = LU; i < n; i++, p_i += n) {
		for (j = 0, p_j = LU; j <= i; j++, p_j += n) {
			sum = 0;
			for (k = i, p_k = p_i; k < n; k++, p_k += n)
				sum = _add(sum, _mul(*(p_k + i), *(p_k + j)));
			*(p_i + j) = sum;
			*(p_j + i) = sum;
		}
	}
	return 0;
}

ISTVOID MatDotMulMats(ISTFLOAT *C, ISTFLOAT *A, ISTINT nrows, ISTINT ncols, ISTFLOAT *B, ISTINT mcols) 
{
	ISTFLOAT *pB;
	ISTFLOAT *pB1;
	ISTINT i, j, k;

	for (i = 0; i < nrows; A += ncols, i++) {
		for (pB1 = B, j = 0; j < mcols; C++, pB1++, j++) {
			pB = pB1;
			*C = 0; 
			for (k = 0; k < ncols; pB += mcols, k++)
				*C = _add(*C, _mul(*(A+k), *pB));
		}
	}
}

ISTVOID ExchangeRows(ISTFLOAT *A, ISTINT row1, ISTINT row2, ISTINT ncols)
{
	ISTINT i;
	ISTFLOAT *pA1, *pA2;
	ISTFLOAT temp;

	pA1 = A + row1 * ncols;
	pA2 = A + row2 * ncols;
	for (i = 0; i < ncols; i++) {
		temp = *pA1;
		*pA1++ = *pA2;
		*pA2++ = temp;
	}
}

ISTVOID ExchangeCols(ISTFLOAT *A, ISTINT col1, ISTINT col2, ISTINT nrows, ISTINT ncols)
{
	ISTINT i;
	ISTFLOAT *pA1, *pA2;
	ISTFLOAT temp;

	pA1 = A + col1;
	pA2 = A + col2;
	for (i = 0; i < nrows; pA1 += ncols, pA2 += ncols, i++) {
		temp = *pA1;
		*pA1 = *pA2;
		*pA2 = temp;
	}
}

ISTVOID CopyVector(ISTFLOAT *d, ISTFLOAT *s, ISTINT n)
{
	ist_memcpy(d, s, sizeof(ISTFLOAT) * n);
}

ISTVOID GetIdentMat(ISTFLOAT *A, ISTINT n)
{
	ISTINT i, j;
	ISTFLOAT a1 = _float(1);

	for (i = 0; i < n - 1; i++) {
		*A++ = a1;
		for (j = 0; j < n; j++)
			*A++ = 0;
	} 
	*A = a1;
}

ISTVOID HessenbergElemTrans(ISTFLOAT *H, ISTFLOAT* S, ISTINT perm[], ISTINT n)
{
	ISTINT i, j;
	ISTFLOAT *pS, *pH;

	GetIdentMat(S, n);
	for (i = n - 2; i >= 1; i--) {
		pH = H + n * (i + 1);
		pS = S + n * (i + 1);
		for (j = i + 1; j < n; pH += n, pS += n, j++) {
			*(pS + i) = *(pH + i - 1);
			*(pH + i - 1) = 0;
		}
		if (perm[i] != i) {
			pS = S + n * i;
			pH = S + n * perm[i];
			for (j = i; j < n; j++) {
				*(pS + j) = *(pH + j);
				*(pH + j) = 0;
			}
			*(pH + i) = _float(1);
		}
	}
}

ISTINT GetHessenbergFormElem3(ISTFLOAT *A, ISTFLOAT* S)
{
	ISTINT i, j, col, row;
	ISTINT* perm;
	ISTFLOAT *pRow, *pSRow;
	ISTFLOAT max;
	ISTFLOAT s;
	ISTFLOAT *pA, *pB, *pC, *pS;
	ISTINT n = 3;
	ISTINT temp[3] = {0};

	perm = &temp[0];
	pRow = A + n;
	pSRow = S + n;
	for (col = 0; col < (n - 2); pRow += n, pSRow += n, col++) {
		row = col + 1;
		perm[row] = row;
		for (pA = pRow + col, max = 0, i = row; i < n; pA += n, i++)
			if (_gt(_abs(*pA), max)) { 
				perm[row] = i; 
				max = _abs(*pA); 
			}
			if (perm[row] != row) {
				ExchangeRows(A, row, perm[row], n);
				ExchangeCols(A, row, perm[row], n, n);
			}
			pA = pRow + n;
			pS = pSRow + n;
			for (i = col + 2; i < n; pA += n, pS += n, i++) {
				s = _div(*(pA + col), *(pRow + col));
				for (j = 0; j < n; j++)
					*(pA + j) = _sub(*(pA + j), _mul(*(pRow + j), s));
				*(pS + col) = s;
				for (j = 0, pB = A + col + 1, pC = A + i; j < n; pB +=n, pC += n, j++)
					*pB = _add(*pB, _mul(s, *pC));
			}
	}
	pA = A + n + n;
	pS = S + n + n;
	for (i = 2; i < n; pA += n, pS += n, i++)
		CopyVector(pA, pS, i - 1);
	HessenbergElemTrans(A, S, perm, n);
	return 0;
}

ISTINT GetHessenbergFormElem6(ISTFLOAT *A, ISTFLOAT* S)
{
	ISTINT i, j, col, row;
	ISTINT* perm;
	ISTFLOAT *pRow, *pSRow;
	ISTFLOAT max;
	ISTFLOAT s;
	ISTFLOAT *pA, *pB, *pC, *pS;
	ISTINT n = 6;
	ISTINT temp[6] = {0};

	perm = &temp[0];
	pRow = A + n;
	pSRow = S + n;
	for (col = 0; col < (n - 2); pRow += n, pSRow += n, col++) {
		row = col + 1;
		perm[row] = row;
		for (pA = pRow + col, max = 0, i = row; i < n; pA += n, i++)
			if (_gt(_abs(*pA), max)) { 
				perm[row] = i; 
				max = _abs(*pA); 
			}
			if (perm[row] != row) {
				ExchangeRows(A, row, perm[row], n);
				ExchangeCols(A, row, perm[row], n, n);
			}
			pA = pRow + n;
			pS = pSRow + n;
			for (i = col + 2; i < n; pA += n, pS += n, i++) {
				s = _div(*(pA + col), *(pRow + col));
				for (j = 0; j < n; j++)
					*(pA + j) = _sub(*(pA + j), _mul(*(pRow + j), s));
				*(pS + col) = s;
				for (j = 0, pB = A + col + 1, pC = A + i; j < n; pB +=n, pC += n, j++)
					*pB = _add(*pB, _mul(s, *pC));
			}
	}
	pA = A + n + n;
	pS = S + n + n;
	for (i = 2; i < n; pA += n, pS += n, i++)
		CopyVector(pA, pS, i - 1);
	HessenbergElemTrans(A, S, perm, n);
	return 0;
}

ISTVOID OneRealEigenValue(ISTFLOAT Hrow[], ISTFLOAT eigen_real[], ISTFLOAT eigen_imag[], ISTINT row, ISTFLOAT shift)
{
	Hrow[row] = _add(Hrow[row], shift);      
	eigen_real[row] = Hrow[row];
	eigen_imag[row] = 0;
}

ISTVOID UpdateRow(ISTFLOAT *Hrow, ISTFLOAT cos, ISTFLOAT sin, ISTINT n, ISTINT row)
{
	ISTFLOAT x;
	ISTFLOAT *Hnextrow = Hrow + n;
	ISTINT i;

	for (i = row; i < n; i++) {
		x = Hrow[i];
		Hrow[i] = _add(_mul(cos, x), _mul(sin, Hnextrow[i]));
		Hnextrow[i] = _sub(_mul(cos, Hnextrow[i]), _mul(sin, x));
	}
}

ISTVOID UpdateCol(ISTFLOAT* H, ISTFLOAT cos, ISTFLOAT sin, ISTINT n, ISTINT col)
{
	ISTFLOAT x;
	ISTINT i;
	ISTINT next_col = col + 1;

	for (i = 0; i <= next_col; i++, H += n) {
		x = H[col];
		H[col] = _add(_mul(cos, x), _mul(sin, H[next_col]));
		H[next_col] = _sub(_mul(cos, H[next_col]), _mul(sin, x));
	}
}

ISTVOID UpdateTrans(ISTFLOAT *S, ISTFLOAT cos, ISTFLOAT sin, ISTINT n, ISTINT k)
{
	ISTFLOAT x;
	ISTINT i;
	ISTINT k1 = k + 1;

	for (i = 0; i < n; i++, S += n) {
		x = S[k];
		S[k] = _add(_mul(cos, x), _mul(sin, S[k1]));
		S[k1] = _sub(_mul(cos, S[k1]), _mul(sin, x));
	}
}

ISTVOID TwoEigenValues(ISTFLOAT *H, ISTFLOAT* S, ISTFLOAT eigen_real[], ISTFLOAT eigen_imag[], ISTINT n, ISTINT row, ISTFLOAT shift)
{
	ISTFLOAT p, q, x, discriminant, r;
	ISTFLOAT cos, sin;
	ISTFLOAT *Hrow = H + n * row;
	ISTFLOAT *Hnextrow = Hrow + n;
	ISTINT nextrow = row + 1;

	p = _mul(_frac(1, 2), _sub(Hrow[row], Hnextrow[nextrow]));
	x = _mul(Hrow[nextrow], Hnextrow[row]);
	discriminant = _add(_mul(p, p), x);
	Hrow[row] = _add(Hrow[row], shift);
	Hnextrow[nextrow] = _add(Hnextrow[nextrow], shift);
	if (_gt(discriminant, 0)) {
		q = _sqrt(discriminant);
		if (_lt(p, 0))
			q = _sub(p, q);
		else
			q = _add(q, p);
		eigen_real[row] = _add(Hnextrow[nextrow], q);
		eigen_real[nextrow] = _sub(Hnextrow[nextrow], _div(x, q));
		eigen_imag[row] = 0;
		eigen_imag[nextrow] = 0;
		r = _sqrt(_add(_pow2(Hnextrow[row]), _pow2(q)));
		sin = _div(Hnextrow[row], r);
		cos = _div(q, r);
		UpdateRow(Hrow, cos, sin, n, row);
		UpdateCol(H, cos, sin, n, row);
		UpdateTrans(S, cos, sin, n, row);
	} else {
		eigen_real[nextrow] = eigen_real[row] = _add(Hnextrow[nextrow], p);
		eigen_imag[row] = _sqrt(_abs(discriminant));
		eigen_imag[nextrow] = _neg(eigen_imag[row]);
	}
}

ISTVOID DotMulAndSumOfShifts(ISTFLOAT *H, ISTINT n, ISTINT max_row, ISTFLOAT* shift, ISTFLOAT *trace, ISTFLOAT *det, ISTINT iter) 
{
	ISTFLOAT *pH = H + max_row * n;
	ISTFLOAT *p_aux;
	ISTINT i;
	ISTINT min_col = max_row - 1;

	if ((iter%10) == 0) {
		*shift = _add(*shift, pH[max_row]);
		for (i = 0, p_aux = H; i <= max_row; p_aux += n, i++)
			p_aux[i] = _sub(p_aux[i], pH[max_row]);
		p_aux = pH - n;
		*trace = _add(_abs(pH[min_col]), _abs(p_aux[min_col - 1]));
		*det = _mul(*trace, *trace);
		*trace = _mul(*trace, _frac(3, 2));
	} else {
		p_aux = pH - n;
		*trace = _add(p_aux[min_col], pH[max_row]);
		*det = _sub(_mul(p_aux[min_col], pH[max_row]), _mul(p_aux[max_row], pH[min_col]));
	}
};

ISTINT TwoConscutiveSmallSubDiags(ISTFLOAT* H, ISTINT min_row, ISTINT max_row, ISTINT n, ISTFLOAT trace, ISTFLOAT det)
{
	ISTFLOAT x, y ,z, s;
	ISTFLOAT* pH;
	ISTINT i, k;
	ISTFLOAT tor;

	for (k = max_row - 2, pH = H + k * n; k >= min_row; pH -= n, k--) {
		x = _add(_div(_add(_mul(pH[k], _sub(pH[k], trace)), det), pH[n+k]), pH[k+1]);
		y = _sub(_add(pH[k], pH[n+k+1]), trace);
		z = pH[n + n + k + 1];
		s = _add3(_abs(x), _abs(y), _abs(z));
		x = _div(x, s);
		y = _div(y, s);
		z = _div(z, s);
		if (k == min_row)
			break;
		tor = _mul3(_eps, _abs(x), _add3(_abs(pH[k-1-n]), _abs(pH[k]), _abs(pH[n + k + 1])));
		if (_le(_mul(_abs(pH[k-1]), _add(_abs(y), _abs(z))), tor))
			break;
	}
	for (i = k+2, pH = H + i * n; i <= max_row; pH += n, i++)
		pH[i-2] = 0;
	for (i = k+3, pH = H + i * n; i <= max_row; pH += n, i++)
		pH[i-3] = 0;
	return k;
};

ISTVOID DoubleQRStep(ISTFLOAT *H, ISTINT min_row, ISTINT max_row, ISTINT min_col, ISTFLOAT trace, ISTFLOAT det, ISTFLOAT *S, ISTINT n)
{
	ISTFLOAT s, x, y, z;
	ISTFLOAT a, b, c;
	ISTFLOAT *pH;
	ISTFLOAT *tH;
	ISTFLOAT *pS;
	ISTINT i,j,k;
	ISTINT last_test_row_col = max_row - 1;

	k = min_col;
	pH = H + min_col * n;
	a = _add(_div(_add(_mul(pH[k], _sub(pH[k], trace)), det), pH[n+k]), pH[k+1]);
	b = _sub(_add(pH[k], pH[n+k+1]), trace);
	c = pH[n + n + k + 1];
	s = _add3(_abs(a), _abs(b), _abs(c));
	a = _div(a, s);
	b = _div(b, s);
	c = _div(c, s);
	for (; k <= last_test_row_col; k++, pH += n) {
		if (k > min_col) {
			c = (k == last_test_row_col) ? 0 : pH[n + n + k - 1];
			x = _add3(_abs(pH[k-1]), _abs(pH[n + k - 1]), _abs(c));
			if (_eq(x, 0))
				continue;
			a = _div(pH[k - 1], x);
			b = _div(pH[n + k - 1], x);
			c = _div(c, x);
		}
		s = _sqrt(_add3(_pow2(a), _pow2(b), _pow2(c)));
		if (_lt(a, 0))
			s = _neg(s);
		if (k > min_col)
			pH[k-1] = _mul(_neg(s), x);
		else if (min_row != min_col)
			pH[k-1] = _neg(pH[k-1]);
		a = _add(a, s);
		x = _div(a, s);
		y = _div(b, s);
		z = _div(c, s);
		b = _div(b, a);
		c = _div(c, a);
		for (j = k; j < n; j++) {
			a = _add(pH[j], _mul(b, pH[n+j]));
			if (k != last_test_row_col) {
				a = _add(a, _mul(c, pH[n + n + j]));
				pH[n + n + j] = _sub(pH[n + n + j], _mul(a, z));
			}
			pH[n + j] = _sub(pH[n + j], _mul(a, y));
			pH[j] = _sub(pH[j], _mul(a, x));
		}
		j = k + 3;
		if (j > max_row)
			j = max_row;
		for (i = 0, tH = H; i <= j; i++, tH += n) {
			a = _add(_mul(x, tH[k]), _mul(y, tH[k+1]));
			if (k != last_test_row_col) {
				a = _add(a, _mul(z, tH[k+2]));
				tH[k+2] = _sub(tH[k+2], _mul(a, c));
			}
			tH[k+1] = _sub(tH[k+1], _mul(a, b));
			tH[k] = _sub(tH[k], a);           
		}
		for (i = 0, pS = S; i < n; pS += n, i++) {
			a = _add(_mul(x, pS[k]), _mul(y, pS[k+1]));
			if ( k != last_test_row_col ) {
				a = _add(a, _mul(z, pS[k+2]));
				pS[k+2] = _sub(pS[k+2], _mul(a, c));
			}
			pS[k+1] = _sub(pS[k+1], _mul(a, b));
			pS[k] = _sub(pS[k], a);
		}
	}; 
}

ISTVOID DoubleQRIter(ISTFLOAT *H, ISTFLOAT *S, ISTINT min_row, ISTINT max_row, ISTINT n, ISTFLOAT* shift, ISTINT iter) 
{
	ISTINT k;
	ISTFLOAT trace, det;

	DotMulAndSumOfShifts(H, n, max_row, shift, &trace, &det, iter);
	k = TwoConscutiveSmallSubDiags(H, min_row, max_row, n, trace, det);
	DoubleQRStep(H, min_row, max_row, k, trace, det, S, n);
}

ISTVOID BackSubstituteRealVector(ISTFLOAT *H, ISTFLOAT eigen_real[], ISTFLOAT eigen_imag[], ISTINT row, ISTFLOAT zero_tor, ISTINT n)
{
	ISTFLOAT *pH;
	ISTFLOAT *pV;
	ISTFLOAT x;
	ISTFLOAT u[4];
	ISTFLOAT v[2];
	ISTINT i,j,k;

	k = row;
	pH = H + row * n;
	pH[row] = _float(1);
	for (i = row - 1, pH -= n; i >= 0; i--, pH -= n) {
		u[0] = _sub(pH[i], eigen_real[row]);
		v[0] = pH[row];
		pV = H + n * k;
		for (j = k; j < row; j++, pV += n)
			v[0] = _add(v[0], _mul(pH[j], pV[row]));
		if (_lt(eigen_imag[i], 0)) {
			u[3] = u[0];
			v[1] = v[0];
		} else {
			k = i;
			if (_eq(eigen_imag[i], 0)) {
				if (_neq(u[0], 0))
					pH[row] = _div(_neg(v[0]), u[0]);
				else
					pH[row] = _div(_neg(v[0]), zero_tor);
			} else {
				u[1] = pH[i+1];
				u[2] = pH[n+i];
				x = _sub(eigen_real[i], eigen_real[row]);
				x = _mul(x, x);
				x = _add(x, _mul(eigen_imag[i], eigen_imag[i]));
				pH[row] = _div(_sub(_mul(u[1], v[1]), _mul(u[3], v[0])), x); 
				if (_gt(_abs(u[1]), _abs(u[3])))
					pH[n+row] = _div(_neg(_add(v[0], _mul(u[0], pH[row]))), u[1]);
				else
					pH[n+row] = _div(_neg(_add(v[1], _mul(u[2], pH[row]))), u[3]);
			}
		}
	}    
}

ISTVOID ComplexDiv(ISTFLOAT x, ISTFLOAT y, ISTFLOAT u, ISTFLOAT v, ISTFLOAT* a, ISTFLOAT* b)
{
	ISTFLOAT q = _add(_mul(u, u), _mul(v, v));

	*a = _div(_add(_mul(x, u), _mul(y, v)), q);
	*b = _div(_sub(_mul(y, u), _mul(x, v)), q);
}

ISTVOID BackSubstituteComplexVector(ISTFLOAT *H, ISTFLOAT eigen_real[], ISTFLOAT eigen_imag[], ISTINT row, ISTFLOAT zero_tor, ISTINT n)
{
	ISTFLOAT *pH;
	ISTFLOAT *pV;
	ISTFLOAT x,y;
	ISTFLOAT u[4];
	ISTFLOAT v[2];
	ISTFLOAT w[2];
	ISTINT i,j,k;

	k = row - 1;
	pH = H + n * row;
	if (_gt(_abs(pH[k]), _abs(pH[row-n]))) {
		pH[k-n] = _div(_neg(_sub(pH[row], eigen_real[row])), pH[k]);
		pH[row-n] = _div(_neg(eigen_imag[row]), pH[k]);
	} else
		ComplexDiv(_neg(pH[row-n]), 0, _sub(pH[k-n], eigen_real[row]), eigen_imag[row], &pH[k-n], &pH[row-n]);
	pH[k] = _float(1);
	pH[row] = 0;
	for (i = row - 2, pH = H + n * i; i >= 0; pH -= n, i--) {
		u[0] = _sub(pH[i], eigen_real[row]);
		w[0] = pH[row];
		w[1] = 0;
		pV = H + k * n;
		for (j = k; j < row; j++, pV+=n) {
			w[0] = _add(w[0], _mul(pH[j], pV[row - 1]));
			w[1] = _add(w[1], _mul(pH[j], pV[row]));
		}
		if (_lt(eigen_imag[i], 0)) {
			u[3] = u[0];
			v[0] = w[0];
			v[1] = w[1];
		} else {
			k = i;
			if (_eq(eigen_imag[i], 0))
				ComplexDiv(_neg(w[0]), _neg(w[1]), u[0], eigen_imag[row], &pH[row-1], &pH[row]);
			else {
				u[1] = pH[i+1];
				u[2] = pH[n + i];
				x = _sub(eigen_real[i], eigen_real[row]);
				y = _mul3(_float(2), x, eigen_imag[row]);
				x = _sub(_add(_mul(x, x), _mul(eigen_imag[i], eigen_imag[i])), _mul(eigen_imag[row], eigen_imag[row]));
				if (_eq(x, 0) && _eq(y, 0))
					x = _mul(zero_tor, _add5(_abs(u[0]), _abs(u[1]), _abs(u[2]), _abs(u[3]), _abs(eigen_imag[row])));
				ComplexDiv(_add(_sub(_mul(u[1], v[0]), _mul(u[3], w[0])), _mul(w[1], eigen_imag[row])),
					_sub3(_mul(u[1], v[1]), _mul(u[3], w[1]), _mul(w[0], eigen_imag[row])),
					x, y, &pH[row-1], &pH[row]);
				if (_gt(_abs(u[1]), _add(_abs(u[3]), _abs(eigen_imag[row])))) {
					pH[n+row-1] = _add(_sub(_neg(w[0]), _mul(u[0], pH[row-1])), _div(_mul(eigen_imag[row], pH[row]), u[1]));
					pH[n+row] = _sub3(_neg(w[1]), _mul(u[0], pH[row]), _div(_mul(eigen_imag[row], pH[row-1]), u[1]));
				} else {
					ComplexDiv(_sub(_neg(v[0]), _mul(u[2], pH[row-1])), _sub(_neg(v[1]), _mul(u[2], pH[row])),
						u[3], eigen_imag[row], &pH[n+row-1], &pH[n+row]);
				}
			} 
		}
	}
}

ISTVOID BackSubstitute(ISTFLOAT *H, ISTFLOAT eigen_real[], ISTFLOAT eigen_imag[], ISTINT n)
{
	ISTFLOAT zero_tolerance;
	ISTFLOAT *pH;
	ISTINT i, j, row;

	pH = H;
	zero_tolerance = _abs(pH[0]);
	for (pH += n, i = 1; i < n; pH += n, i++) {
		for (j = i-1; j < n; j++)
			zero_tolerance = _add(zero_tolerance, _abs(pH[j]));
	}
	zero_tolerance = _mul(zero_tolerance, _eps);
	for (row = n-1; row >= 0; row--) {
		if (_eq(eigen_imag[row], 0))
			BackSubstituteRealVector(H, eigen_real, eigen_imag, row, zero_tolerance, n);
		else if (_lt(eigen_imag[row], 0))
			BackSubstituteComplexVector(H, eigen_real, eigen_imag, row, zero_tolerance, n);
	} 
}

ISTVOID GetEigenVectors(ISTFLOAT *H, ISTFLOAT *S, ISTFLOAT eigen_imag[], ISTINT n)
{
	ISTFLOAT* pH;
	ISTFLOAT* pS;
	ISTFLOAT x,y;
	ISTINT i,j,k;

	for (k = n-1; k >= 0; k--) {
		if (_lt(eigen_imag[k], 0)) {
			for (i = 0, pS = S; i < n; pS += n, i++) {
				x = 0;
				y = 0;
				for (j = 0, pH = H; j <= k; pH += n, j++) {
					x = _add(x, _mul(pS[j], pH[k-1]));
					y = _add(y, _mul(pS[j], pH[k]));
				}
				pS[k-1] = x;
				pS[k] = y;
			}
		} else if (_eq(eigen_imag[k], 0)) { 
			for (i = 0, pS = S; i < n; i++, pS += n) {
				x = 0;
				for (j = 0, pH = H; j <= k; j++, pH += n)
					x = _add(x, _mul(pS[j], pH[k]));
				pS[k] = x;
			}
		}
	}
}

ISTINT GetQRHessenbergMat(ISTFLOAT *H, ISTFLOAT *S, ISTFLOAT eigen_real[], ISTFLOAT eigen_imag[], ISTINT n, ISTINT iter_max)
{
	ISTINT i;
	ISTINT row;
	ISTINT iter;
	ISTINT found;
	ISTFLOAT shift = 0;
	ISTFLOAT* pH;
	ISTFLOAT tor;

	for (row = n - 1; row >= 0; row--) {
		found = 0;
		for (iter = 1; iter <= iter_max; iter++) {
			for (i = row, pH = H + row * n; i > 0; i--, pH -= n) {
				tor = _mul(_eps, _add(_abs(*(pH - n + i - 1)), _abs(*(pH + i))));
				if (_le(_abs(*(pH + i - 1 )), tor))
					break;
			}
			switch(row - i) {
			case 0: // One real eigenvalue
				OneRealEigenValue(pH, eigen_real, eigen_imag, i, shift);
				found = 1;
				break;
			case 1: // Either two real eigenvalues or a complex pair
				row--;
				TwoEigenValues(H, S, eigen_real, eigen_imag, n, row, shift);
				found = 1;
				break;    
			default:
				DoubleQRIter(H, S, i, row, n, &shift, iter);
			}
			if (found)
				break;
		}
		if (iter > iter_max)
			return -1;
	}

	BackSubstitute(H, eigen_real, eigen_imag, n);
	GetEigenVectors(H, S, eigen_imag, n);
	return 0;
}

ISTVOID TransSquareMat(ISTFLOAT *A, ISTINT n) 
{
	ISTFLOAT *pA, *pAt;
	ISTFLOAT temp;
	ISTINT i,j;

	for (i = 0; i < n; A += n + 1, i++) {
		pA = A + 1;
		pAt = A + n;
		for (j = i+1 ; j < n; pA++, pAt += n, j++) {
			temp = *pAt;
			*pAt = *pA;
			*pA = temp;
		} 
	}
}

ISTFLOAT CalcCovar(ISTINT ndata, ISTFLOAT *x, ISTFLOAT *y, ISTFLOAT *z, ISTFLOAT W[3][3], ISTFLOAT V[3], ISTFLOAT R)
{
	ISTINT i;
	ISTFLOAT xv[3];
	ISTFLOAT m[3];
	ISTFLOAT temp, chisq = 0;

	if (ndata <= 0)
		goto EXIT;
	for (i = 0; i < ndata; ++i) {
		// xv = xyz-V
		xv[0] = _sub(x[i], V[0]);
		xv[1] = _sub(y[i], V[1]);
		xv[2] = _sub(z[i], V[2]);
		// Wxv
		m[0] = _add3(_mul(W[0][0], xv[0]), _mul(W[0][1], xv[1]), _mul(W[0][2], xv[2]));
		m[1] = _add3(_mul(W[1][0], xv[0]), _mul(W[1][1], xv[1]), _mul(W[1][2], xv[2]));
		m[2] = _add3(_mul(W[2][0], xv[0]), _mul(W[2][1], xv[1]), _mul(W[2][2], xv[2]));
		// Wxv^T*Wxv - R^2
		temp = _abs(_sub(_add3(_mul(m[0], m[0]), _mul(m[1], m[1]), _mul(m[2], m[2])), _mul(R, R)));
		chisq = _add(chisq, _div(temp, _mul(R, R)));
	}
	return chisq;

EXIT:
	return 0;
}

ISTFLOAT RecalcRadius(ISTINT ndata, ISTFLOAT x[], ISTFLOAT y[], ISTFLOAT z[], ISTFLOAT W[3][3], ISTFLOAT V[3])
{
	ISTINT i, j;
	ISTFLOAT xv[3];
	ISTFLOAT m[3];
	ISTFLOAT R = 0;
	ISTFLOAT w[3][3] = {{0}};

	if (ndata <= 0) {
		goto EXIT;
	}
	for (i = 0; i < 3; ++i) {
		for (j = 0; j < 3; ++j)
			w[i][j] = W[i][j];
	}
	for (i = 0; i < ndata; ++i) {
		// xv = xyz-V
		xv[0] = _sub(x[i], V[0]);
		xv[1] = _sub(y[i], V[1]);
		xv[2] = _sub(z[i], V[2]);
		// Wxv
		m[0] = _add3(_mul(w[0][0], xv[0]), _mul(w[0][1], xv[1]), _mul(w[0][2], xv[2]));
		m[1] = _add3(_mul(w[1][0], xv[0]), _mul(w[1][1], xv[1]), _mul(w[1][2], xv[2]));
		m[2] = _add3(_mul(w[2][0], xv[0]), _mul(w[2][1], xv[1]), _mul(w[2][2], xv[2]));
		R = _add(R, _div(_sqrt(_add3(_pow2(m[0]), _pow2(m[1]), _pow2(m[2]))), _float(ndata)));
	}
	return R;

EXIT:
	return 0;
}

ISTBOOL FitModel(Ellipsoid *self, ISTFLOAT *x, ISTFLOAT *y, ISTFLOAT *z, ISTINT ndata)
{
	struct ellipsoid *s;
	struct ellipsoid_model *model;
	struct ellipsoid_data f = {{0}};
	ISTFLOAT *datas;
	ISTFLOAT maxval;
	ISTFLOAT norm;
	ISTFLOAT norm1;
	ISTFLOAT norm2;
	ISTFLOAT norm3;
	ISTFLOAT btqb;
	ISTFLOAT J;
	ISTFLOAT hmb;
	ISTFLOAT hm;
	ISTINT i;
	ISTINT j;
	ISTINT index;
	ISTFLOAT a1 = _float(1);
	ISTFLOAT a2 = _float(2);
	ISTFLOAT a0_5 = _frac(1,2);
	ISTFLOAT _a0_25 = _neg(_frac(1,4));
	ISTFLOAT c_matrix[6*6] = {
		0, a0_5, a0_5, 0, 0, 0,
		a0_5, 0, a0_5, 0, 0, 0,
		a0_5, a0_5, 0, 0, 0, 0,
		0, 0, 0, _a0_25, 0, 0,
		0, 0, 0, 0, _a0_25, 0,
		0, 0, 0, 0, 0, _a0_25,
	};

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct ellipsoid *)self;
	model = &s->model;
	datas = model->datas;	// fitting matrix [10 * ndata]
	if ((!datas) || (ndata < 10) || (!x) || (!y) || (!z)) {
		goto EXIT;
	}
	for (i = 0; i < ndata; ++i) {
		datas[i] = _mul(x[i], x[i]);
		datas[ndata + i] = _mul(y[i], y[i]);
		datas[ndata*2 + i] = _mul(z[i], z[i]);
		datas[ndata*3 + i] = _mul3(a2, y[i], z[i]);
		datas[ndata*4 + i] = _mul3(a2, x[i], z[i]);
		datas[ndata*5 + i] = _mul3(a2, x[i], y[i]);
		datas[ndata*6 + i] = _mul(a2, x[i]);
		datas[ndata*7 + i] = _mul(a2, y[i]);
		datas[ndata*8 + i] = _mul(a2, z[i]);
		datas[ndata*9 + i] = a1;
	}
	ist_memcpy(f.c, c_matrix, sizeof(c_matrix));
	MatDotMulMatT(f.s, datas, 10, ndata);
	GetSubMat(f.s11, 6, 6, f.s, 10, 0, 0);
	GetSubMat(f.s12, 6, 4, f.s, 10, 0, 6);
	GetSubMat(f.s12t, 4, 6, f.s, 10, 6, 0);
	GetSubMat(f.s22, 4, 4, f.s, 10, 6, 6);
	for (i = 0; i < 16; ++i)
		f.s22_1[i] = f.s22[i];
	GetCholeskiLUDecomp(f.s22_1, 4);
	GetCholeskiLUInverse(f.s22_1, 4);
	// Calculate S22a = S22_1 * S12t   4*6 = 4x4 * 4x6   C = AB
	MatDotMulMats(f.s22a, f.s22_1, 4, 4, f.s12t, 6);
	// Then calculate S22b = S12 * S22a      ( 6x6 = 6x4 * 4x6)
	MatDotMulMats(f.s22b, f.s12, 6, 4, f.s22a, 6);
	// Calculate SS = S11 - S22b
	for (i = 0; i < 36; i++)
		f.ss[i] = _sub(f.s11[i], f.s22b[i]);
	MatDotMulMats(f.e, f.c, 6, 6, f.ss, 6);
	GetHessenbergFormElem6(f.e, f.sss);
	GetQRHessenbergMat(f.e, f.sss, f.eigen_real, f.eigen_imag, 6, 100);
	index = 0;
	maxval = f.eigen_real[0];
	for (i = 1; i < 6; i++) {
		if (_gt(f.eigen_real[i], maxval)) {
			maxval = f.eigen_real[i];
			index = i;
		}
	}
	f.v1[0] = f.sss[index]; 
	f.v1[1] = f.sss[index + 6];
	f.v1[2] = f.sss[index + 12];
	f.v1[3] = f.sss[index + 18];
	f.v1[4] = f.sss[index + 24];
	f.v1[5] = f.sss[index + 30];
	// normalize v1
	norm = _sqrt(_add6(_pow2(f.v1[0]), _pow2(f.v1[1]), _pow2(f.v1[2]), 
		_pow2(f.v1[3]), _pow2(f.v1[4]), _pow2(f.v1[5])));
	f.v1[0] = _div(f.v1[0], norm);
	f.v1[1] = _div(f.v1[1], norm);
	f.v1[2] = _div(f.v1[2], norm);
	f.v1[3] = _div(f.v1[3], norm);
	f.v1[4] = _div(f.v1[4], norm);
	f.v1[5] = _div(f.v1[5], norm);
	if (_lt(f.v1[0], 0)) {
		f.v1[0] = _neg(f.v1[0]);
		f.v1[1] = _neg(f.v1[1]);
		f.v1[2] = _neg(f.v1[2]);
		f.v1[3] = _neg(f.v1[3]);
		f.v1[4] = _neg(f.v1[4]);
		f.v1[5] = _neg(f.v1[5]);
	}
	// Calculate v2 = S22a * v1      ( 4x1 = 4x6 * 6x1)
	MatDotMulMats(f.v2, f.s22a, 4, 6, f.v1, 1);
	f.v[0] = f.v1[0];
	f.v[1] = f.v1[1];
	f.v[2] = f.v1[2];
	f.v[3] = f.v1[3];
	f.v[4] = f.v1[4];
	f.v[5] = f.v1[5];
	f.v[6] = _neg(f.v2[0]);
	f.v[7] = _neg(f.v2[1]);
	f.v[8] = _neg(f.v2[2]);
	f.v[9] = _neg(f.v2[3]);
	f.q[0] = f.v[0];
	f.q[1] = f.v[5];
	f.q[2] = f.v[4];
	f.q[3] = f.v[5];
	f.q[4] = f.v[1];
	f.q[5] = f.v[3];
	f.q[6] = f.v[4];
	f.q[7] = f.v[3];
	f.q[8] = f.v[2];
	f.u[0] = f.v[6];
	f.u[1] = f.v[7];
	f.u[2] = f.v[8];
	for(i = 0; i < 9; i++) {
		f.q_1[i] = f.q[i];
	}
	GetCholeskiLUDecomp(f.q_1, 3);
	GetCholeskiLUInverse(f.q_1, 3);
	// Calculate B = Q-1 * U   ( 3x1 = 3x3 * 3x1)
	MatDotMulMats(f.b, f.q_1, 3, 3, f.u, 1);
	f.b[0] = _neg(f.b[0]);     // x-axis combined bias
	f.b[1] = _neg(f.b[1]);     // y-axis combined bias
	f.b[2] = _neg(f.b[2]);     // z-axis combined bias
	// First calculate QB = Q * B   ( 3x1 = 3x3 * 3x1)
	MatDotMulMats(f.qb, f.q, 3, 3, f.b, 1);
	// Then calculate btqb = BT * QB    ( 1x1 = 1x3 * 3x1)
	MatDotMulMats(&btqb, f.b, 1, 3, f.qb, 1);
	// Calculate hmb = sqrt(btqb - J).
	J = f.v[9];
	if (_lt(_sub(btqb, J), 0))
		goto EXIT;
	hmb = _sqrt(_sub(btqb, J));
	// Calculate SQ, the square root of matrix Q
	GetHessenbergFormElem3(f.q, f.ssss);
	GetQRHessenbergMat(f.q, f.ssss, f.eigen_real3, f.eigen_imag3, 3, 100);
	// normalize eigenvectors
	norm1 = _sqrt(_add3(_pow2(f.ssss[0]), _pow2(f.ssss[3]), _pow2(f.ssss[6])));
	f.ssss[0] = _div(f.ssss[0], norm1);
	f.ssss[3] = _div(f.ssss[3], norm1);
	f.ssss[6] = _div(f.ssss[6], norm1);
	norm2 = _sqrt(_add3(_pow2(f.ssss[1]), _pow2(f.ssss[4]), _pow2(f.ssss[7])));
	f.ssss[1] = _div(f.ssss[1], norm2);
	f.ssss[4] = _div(f.ssss[4], norm2);
	f.ssss[7] = _div(f.ssss[7], norm2);
	norm3 = _sqrt(_add3(_pow2(f.ssss[2]), _pow2(f.ssss[5]), _pow2(f.ssss[8])));
	f.ssss[2] = _div(f.ssss[2], norm3);
	f.ssss[5] = _div(f.ssss[5], norm3);
	f.ssss[8] = _div(f.ssss[8], norm3);
	for (i = 0; i < 9; i++)
		f.dz[i] = 0;
	if (_lt(f.eigen_real3[0], 0))
		goto EXIT;
	f.dz[0] = _sqrt(f.eigen_real3[0]);
	if (_lt(f.eigen_real3[1], 0))
		goto EXIT;
	f.dz[4] = _sqrt(f.eigen_real3[1]);
	if (_lt(f.eigen_real3[2], 0))
		goto EXIT;
	f.dz[8] = _sqrt(f.eigen_real3[2]);
	MatDotMulMats(f.vdz, f.ssss, 3, 3, f.dz, 3);
	TransSquareMat(f.ssss, 3);
	MatDotMulMats(f.sq, f.vdz, 3, 3, f.ssss, 3);
	hm = model->radius;
	for (i = 0; i < 9; i++)
		f.a_1[i] = _div(_mul(f.sq[i], hm), hmb);
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			model->mat[i][j] = f.a_1[i * 3 + j];
		}
	}
	for (j = 0; j < 3; ++j) {
		model->bias[j] = f.b[j];
	}
	model->radius = RecalcRadius(ndata, x, y, z, model->mat, model->bias);
	if (_eq(model->radius, 0)) {
		goto EXIT;
	}
	model->covar = CalcCovar(ndata, x, y, z, model->mat, model->bias, model->radius);
	if (_eq(model->covar, 0)) {
		goto EXIT;
	}
	datas = (ISTFLOAT *)0;
	return ISTTRUE;

EXIT:
	return ISTFALSE;
}

ISTBOOL Process(Ellipsoid *self, ISTFLOAT *x, ISTFLOAT *y, ISTFLOAT *z, ISTUINT num)
{
	struct ellipsoid *s;
	struct ellipsoid_model *model;
	ISTBOOL res;
	ISTSHORT i, j;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct ellipsoid *)self;
	model = &s->model;
	if (!x || !y || !z) {
		goto EXIT;
	}
	if (num > model->max) {
		num = model->max;
	}
	if (_le(model->radius, 0)) {
		model->radius = _float(IST_ELLIPSOID_RADIUS);
	}
	if (s->is_enable) {
		res = s->FitModel(self, x, y, z, (ISTINT)num);
		if (res) {
			self->Radius = model->radius;
			self->Covariant = model->covar;
			for (i = 0; i < 3; ++i) {
				self->Bias[i] = model->bias[i];
				for (j = 0; j < 3; ++j) {
					self->Matrix[i][j] = model->mat[i][j];
				}
			}
			if (!s->is_valid) {
				s->is_valid = ISTTRUE;
			}
		}
	}
	return s->is_valid;

EXIT:
	return ISTFALSE;
}

ISTVOID SetRadius(Ellipsoid *self, ISTFLOAT radius)
{
	struct ellipsoid *s;
	struct ellipsoid_model *model;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct ellipsoid *)self;
	model = &s->model;
	model->radius = radius;
	return;

EXIT:
	return;
}

ISTVOID Enable(Ellipsoid *self)
{
	struct ellipsoid *s;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct ellipsoid *)self;
	s->is_enable = ISTTRUE;
	return;

EXIT:
	return;
}

ISTVOID Disable(Ellipsoid *self)
{
	struct ellipsoid *s;
	struct ellipsoid_model *model;

	if (!self || self->IsObject == ISTFALSE) {
		goto EXIT;
	}
	s = (struct ellipsoid *)self;
	model = &s->model;
	Reset(self, model->max);
	return;

EXIT:
	return;
}

Ellipsoid *IST_Ellipsoid(ISTVOID)
{
	if (!This) {
		ThisClass.IsObject = ISTFALSE;
		ThisClass.New = New;
		ThisClass.Delete = Delete;
		ThisClass.Process = Process;
		ThisClass.SetRadius = SetRadius;
		ThisClass.Enable = Enable;
		ThisClass.Disable = Disable;
		This = &ThisClass;
	}
	return This;
}

#endif // IST_ELLIPSOID