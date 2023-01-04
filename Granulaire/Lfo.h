#ifdef __cplusplus
    extern "C" {
    #endif


#ifdef __LFO__
#define LFO_EXTRN
#else
#define LFO_EXTRN	extern
#endif

#define	LFO_SINE			0
#define	LFO_SQUARE			1
#define	LFO_TRI				2
#define	LFO_SAW				3
#define	LFO_INVSAW			4
#define	LFO_SH				5
#define	LFO_SHAPEMAX		5

#ifdef __LFO__
char Tab_LFO_NameShape[LFO_SHAPEMAX+1][20] =
{
		"SINE","SQUARE","TRIANGLE","SAW","INV SAW","RANDOM"
};
#else
LFO_EXTRN	char Tab_LFO_NameShape[LFO_SHAPEMAX+1][20];
#endif

#ifdef __cplusplus
}
#endif



