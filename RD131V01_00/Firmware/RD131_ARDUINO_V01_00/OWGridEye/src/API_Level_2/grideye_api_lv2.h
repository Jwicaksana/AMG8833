#ifndef	__GRIDEYE_API_LV2_H
#define	__GRIDEYE_API_LV2_H

/*******************************************************************************
	include file
*******************************************************************************/
#include <stdint.h>
#include	"grideye_api_common.h"
#include	"API_Level_1/grideye_api_lv1.h"


#ifdef __cplusplus
extern "C"{
/*******************************************************************************
	public method definition
*******************************************************************************/
int16_t	shAMG_PUB_CMN_CalcAve( int16_t*, USHORT, UCHAR, UCHAR, BOOL* );
int16_t	shAMG_PUB_CMN_CalcIIR( int16_t, int16_t, int16_t );
int16_t	shAMG_PUB_CMN_CalcIIR_f( int16_t, int16_t, float );

void	vAMG_PUB_IMG_ConvertFlipX    ( UCHAR, UCHAR, int16_t*, int16_t* );
void	vAMG_PUB_IMG_ConvertFlipY    ( UCHAR, UCHAR, int16_t*, int16_t* );
BOOL	bAMG_PUB_IMG_ConvertFlipXY   ( UCHAR, UCHAR, int16_t*, int16_t* );
BOOL	bAMG_PUB_IMG_ConvertRotate90 ( UCHAR, UCHAR, int16_t*, int16_t* );
void	vAMG_PUB_IMG_ConvertRotate180( UCHAR, UCHAR, int16_t*, int16_t* );
BOOL	bAMG_PUB_IMG_ConvertRotate270( UCHAR, UCHAR, int16_t*, int16_t* );

BOOL	bAMG_PUB_IMG_LinearInterpolationSQ15( int16_t*, int16_t* );
BOOL	bAMG_PUB_IMG_LinearInterpolation( UCHAR, UCHAR, int16_t*, int16_t* );

BOOL	bAMG_PUB_IMG_ImageDilation1( UCHAR, UCHAR, UCHAR*, UCHAR* );
BOOL	bAMG_PUB_IMG_ImageDilation2( UCHAR, UCHAR, UCHAR, UCHAR*, UCHAR* );

void	vAMG_PUB_ODT_CalcDiffImage   ( USHORT, int16_t*, int16_t*, int16_t* );
void	vAMG_PUB_ODT_CalcDetectImage1( USHORT, int16_t*, int16_t,  UCHAR, UCHAR* );
void	vAMG_PUB_ODT_CalcDetectImage2( USHORT, int16_t*, int16_t*, UCHAR, UCHAR* );

UCHAR	ucAMG_PUB_ODT_CalcDataLabeling8( UCHAR, UCHAR, UCHAR, USHORT, UCHAR*, USHORT* );

BOOL	bAMG_PUB_FEA_CalcArea        ( USHORT,       UCHAR, UCHAR*,         USHORT* );
BOOL	bAMG_PUB_FEA_CalcRectangle   ( UCHAR, UCHAR, UCHAR, UCHAR*,         UCHAR*  );
BOOL	bAMG_PUB_FEA_CalcMinTemp     ( USHORT,       UCHAR, UCHAR*, int16_t*, int16_t*  );
BOOL	bAMG_PUB_FEA_CalcMaxTemp     ( USHORT,       UCHAR, UCHAR*, int16_t*, int16_t*  );
BOOL	bAMG_PUB_FEA_CalcAveTemp     ( USHORT,       UCHAR, UCHAR*, int16_t*, int16_t*  );
BOOL	bAMG_PUB_FEA_CalcStdDevTemp  ( USHORT,       UCHAR, UCHAR*, int16_t*, USHORT* );
BOOL	bAMG_PUB_FEA_CalcStdDevTemp_f( USHORT,       UCHAR, UCHAR*, int16_t*, float*  );
BOOL	bAMG_PUB_FEA_CalcCenterTemp  ( UCHAR, UCHAR, UCHAR, UCHAR*, int16_t*, int16_t*  );
BOOL	bAMG_PUB_FEA_CalcCenterTemp_f( UCHAR, UCHAR, UCHAR, UCHAR*, int16_t*, float*  );

BOOL	bAMG_PUB_BGT_UpdateBackTemp( USHORT, UCHAR*, int16_t*, int16_t, int16_t* );

BOOL	bAMG_PUB_HDT_JudgeHuman( USHORT, USHORT );

BOOL	bAMG_PUB_OUT_CalcOutImage  ( UCHAR, UCHAR, UCHAR, UCHAR, int16_t*, UCHAR* );
BOOL	bAMG_PUB_OUT_CalcOutImage_f( UCHAR, UCHAR, UCHAR, UCHAR, float*, UCHAR* );
}
#endif  /* extern "C" */

#endif	/* __GRIDEYE_API_LV2_H */

