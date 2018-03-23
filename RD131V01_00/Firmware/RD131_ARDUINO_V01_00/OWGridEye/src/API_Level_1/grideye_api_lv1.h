#ifndef	__GRIDEYE_API_LV1_H
#define	__GRIDEYE_API_LV1_H
	
/*******************************************************************************
	include file
*******************************************************************************/
#include <stdint.h>
#include	"grideye_api_common.h"

#ifdef __cplusplus
extern "C"{
/*******************************************************************************
	public method definition
*******************************************************************************/
BOOL	 bAMG_PUB_I2C_Read( UCHAR, UCHAR, UCHAR, UCHAR* );
int16_t	shAMG_PUB_TMP_ConvThermistor( UCHAR[2] );
int16_t	shAMG_PUB_TMP_ConvTemperature( UCHAR[2] );
void	 vAMG_PUB_TMP_ConvTemperature64( UCHAR*, int16_t* );
int16_t	shAMG_PUB_CMN_ConvFtoS( float );
float	 fAMG_PUB_CMN_ConvStoF( int16_t );
}
#endif  /* extern "C" */

#endif	/* __GRIDEYE_API_LV1_H */

