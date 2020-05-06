#ifndef _BASIC_H
#define _BASIC_H


#include "image_add.h"

#include "image_basic/naninf2zero.h"
#include "image_basic/imcontract.h"
#include "image_basic/imexpand.h"
#include "image_basic/imresize.h"
#include "image_basic/imrotate.h"
#include "image_basic/imstretch.h"


void __attribute__((constructor)) libinit_image_basic();






int basic_lmin_im(const char *ID_name, const char *out_name);

int basic_lmax_im(const char *ID_name, const char *out_name);


long basic_diff(const char *ID1_name, const char *ID2_name,
                const char *ID3_name, long off1, long off2);

imageID basic_extract(const char *ID_in_name, const char *ID_out_name, long n1,
                   long n2, long n3, long n4);

int basic_trunc_circ(const char *ID_name, float f1);



imageID basic_zoom2(const char *ID_name, const char *ID_name_out);



long basic_average_column(const char *ID_name, const char *IDout_name);

long basic_padd(const char *ID_name, const char *ID_name_out, int n1, int n2);

long basic_fliph(const char *ID_name);

long basic_flipv(const char *ID_name);

long basic_fliphv(const char *ID_name);

int basic_median(const char *ID_name, const char *options);

long basic_renorm_max(const char *ID_name);



int basic_translate(const char *ID_name, const char *ID_out, float xtransl,
                    float ytransl);

float basic_correlation(const char *ID_name1, const char *ID_name2);

long IMAGE_BASIC_get_assym_component(const char *ID_name,
                                     const char *ID_out_name, float xcenter, float ycenter, const char *options);

long IMAGE_BASIC_get_sym_component(const char *ID_name, const char *ID_out_name,
                                   float xcenter, float ycenter);




int gauss_histo_image(const char *ID_name, const char *ID_out_name, float sigma,
                      float center);

long load_fitsimages(const char *strfilter);

long load_fitsimages_cube(const char *strfilter, const char *ID_out_name);

long basic_cube_center(const char *ID_in_name, const char *ID_out_name);

long cube_average(const char *ID_in_name, const char *ID_out_name, float alpha);

long cube_collapse(const char *ID_in_name, const char *ID_out_name);

long image_basic_indexmap(char *ID_index_name, char *ID_values_name,
                          char *IDout_name);

long basic_addimagesfiles(const char *strfilter, const char *outname);

long basic_pasteimages(const char *prefix, long NBcol, const char *IDout_name);

long basic_aveimagesfiles(const char *strfilter, const char *outname);

long basic_addimages(const char *prefix, const char *ID_out);

long basic_averageimages(const char *prefix, const char *ID_out);





imageID image_basic_3Dto2D(
    const char *IDname
);

imageID image_basic_SwapAxis2D(
    const char *IDin_name,
    const char *IDout_name
);



imageID basic_tableto2Dim(
    const char *fname,
    float       xmin,
    float       xmax,
    float       ymin,
    float       ymax,
    long        xsize,
    long        ysize,
    const char *ID_name,
    float       convsize
);

imageID basic_2Dextrapolate_nearestpixel(
    const char *IDin_name,
    const char *IDmask_name,
    const char *IDout_name
);

double basic_measure_transl(
    const char *ID_name1,
    const char *ID_name2,
    long tmax
);

// Operations on image streams

/** @brief Average an image stream */
imageID IMAGE_BASIC_streamaverage(
    const char *IDname,
    long        NBcoadd,
    const char *IDoutname,
    int         mode,
    int         semindex
);

long IMAGE_BASIC_streamfeed(
    const char *IDname,
    const char *streamname,
    float       frequ
);

imageID IMAGE_BASIC_streamrecord(
    const char *streamname,
    long        NBframes,
    const char *IDname
);
#endif
