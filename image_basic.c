/**
 * @file    image_basic.c
 * @brief   basic image functions
 *
 * Simple image routines
 *
 */



/* ================================================================== */
/* ================================================================== */
/*            MODULE INFO                                             */
/* ================================================================== */
/* ================================================================== */

// module default short name
// all CLI calls to this module functions will be <shortname>.<funcname>
// if set to "", then calls use <funcname>
#define MODULE_SHORTNAME_DEFAULT "imgbasic"

// Module short description
#define MODULE_DESCRIPTION       "standard image operations"










#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sched.h>

#include <fitsio.h>  /* required by every program that uses CFITSIO  */

#include "CommandLineInterface/CLIcore.h"
#include "COREMOD_tools/COREMOD_tools.h"
#include "COREMOD_memory/COREMOD_memory.h"
#include "COREMOD_iofits/COREMOD_iofits.h"
#include "COREMOD_arith/COREMOD_arith.h"

#include "info/info.h"
#include "fft/fft.h"
#include "image_gen/image_gen.h"
#include "statistic/statistic.h"
#include "linopt_imtools/linopt_imtools.h"
#include "info/info.h"
#include "image_filter/image_filter.h"
#include "kdtree/kdtree.h"


#include "image_basic/image_basic.h"



#define SBUFFERSIZE 1000

#define SWAP(x,y)  temp=(x);x=(y);y=temp;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


char errmsg[SBUFFERSIZE];






/* ================================================================== */
/* ================================================================== */
/*            INITIALIZE LIBRARY                                      */
/* ================================================================== */
/* ================================================================== */

// Module initialization macro in CLIcore.h
// macro argument defines module name for bindings
//
INIT_MODULE_LIB(image_basic)


/* ================================================================== */
/* ================================================================== */
/*            COMMAND LINE INTERFACE (CLI) FUNCTIONS                  */
/* ================================================================== */
/* ================================================================== */



errno_t image_basic_expand_cli()
{
    if(CLI_checkarg(1, 4) + CLI_checkarg(2, 3) + CLI_checkarg(3,
            2) + CLI_checkarg(4, 2) == 0)
    {
        basic_expand(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.string,
                     data.cmdargtoken[3].val.numl, data.cmdargtoken[4].val.numl);
        return CLICMD_SUCCESS;
    }
    else
    {
        return CLICMD_INVALID_ARG;
    }
}

errno_t image_basic_expand3D_cli()
{
    if(CLI_checkarg(1, 4) + CLI_checkarg(2, 3) + CLI_checkarg(3,
            2) + CLI_checkarg(4, 2) + CLI_checkarg(5, 2) == 0)
    {
        basic_expand3D(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.string,
                       data.cmdargtoken[3].val.numl, data.cmdargtoken[4].val.numl,
                       data.cmdargtoken[5].val.numl);
        return CLICMD_SUCCESS;
    }
    else
    {
        return CLICMD_INVALID_ARG;
    }
}


errno_t image_basic_resize_cli()
{
    if(CLI_checkarg(1, 4) + CLI_checkarg(2, 3) + CLI_checkarg(3,
            2) + CLI_checkarg(4, 2) == 0)
    {
        basic_resizeim(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.string,
                       data.cmdargtoken[3].val.numl, data.cmdargtoken[4].val.numl);
        return CLICMD_SUCCESS;
    }
    else
    {
        return CLICMD_INVALID_ARG;
    }
}


errno_t image_basic_rotate_cli()
{
    if(CLI_checkarg(1, 4) + CLI_checkarg(2, 3) + CLI_checkarg(3, 1) == 0)
    {
        basic_rotate(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.string,
                     data.cmdargtoken[3].val.numf);
        return CLICMD_SUCCESS;
    }
    else
    {
        return CLICMD_INVALID_ARG;
    }
}


errno_t image_basic_3Dto2D_cli() // collapse first 2 axis into one
{
    if(CLI_checkarg(1, 4) == 0)
    {
        image_basic_3Dto2D(data.cmdargtoken[1].val.string);
        return CLICMD_SUCCESS;
    }
    else
    {
        return CLICMD_INVALID_ARG;
    }
}

errno_t image_basic_SwapAxis2D_cli() // swap axis of a 2D image
{
    if(CLI_checkarg(1, 4) + CLI_checkarg(2, 3) == 0)
    {
        image_basic_SwapAxis2D(data.cmdargtoken[1].val.string,
                               data.cmdargtoken[2].val.string);
        return CLICMD_SUCCESS;
    }
    else
    {
        return CLICMD_INVALID_ARG;
    }
}

errno_t image_basic_add_cli()
{
    if(CLI_checkarg(1, 4) + CLI_checkarg(2, 4) + CLI_checkarg(3,
            3) + CLI_checkarg(4, 2) + CLI_checkarg(5, 2) == 0)
    {
        basic_add(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.string,
                  data.cmdargtoken[3].val.string, data.cmdargtoken[4].val.numl,
                  data.cmdargtoken[5].val.numl);
        return CLICMD_SUCCESS;
    }
    else
    {
        return CLICMD_INVALID_ARG;
    }
}


errno_t image_basic_add3D_cli()
{
    if(CLI_checkarg(1, 4) + CLI_checkarg(2, 4) + CLI_checkarg(3,
            3) + CLI_checkarg(4, 2) + CLI_checkarg(5, 2) + CLI_checkarg(6, 2) == 0)
    {
        basic_add3D(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.string,
                    data.cmdargtoken[3].val.string, data.cmdargtoken[4].val.numl,
                    data.cmdargtoken[5].val.numl, data.cmdargtoken[6].val.numl);
        return CLICMD_SUCCESS;
    }
    else
    {
        return CLICMD_INVALID_ARG;
    }
}


errno_t image_basic_contract_cli()
{
    if(CLI_checkarg(1, 4) + CLI_checkarg(2, 3) + CLI_checkarg(3,
            2) + CLI_checkarg(4, 2) == 0)
    {
        basic_contract(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.string,
                       data.cmdargtoken[3].val.numl, data.cmdargtoken[4].val.numl);
        return CLICMD_SUCCESS;
    }
    else
    {
        return CLICMD_INVALID_ARG;
    }
}


errno_t image_basic_contract3D_cli()
{
    if(CLI_checkarg(1, 4) + CLI_checkarg(2, 3) + CLI_checkarg(3,
            2) + CLI_checkarg(4, 2) + CLI_checkarg(5, 2) == 0)
    {
        basic_contract3D(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.string,
                         data.cmdargtoken[3].val.numl, data.cmdargtoken[4].val.numl,
                         data.cmdargtoken[5].val.numl);
        return CLICMD_SUCCESS;
    }
    else
    {
        return CLICMD_INVALID_ARG;
    }
}



errno_t IMAGE_BASIC_get_assym_component_cli()
{
    if(CLI_checkarg(1, 4) + CLI_checkarg(2, 3) + CLI_checkarg(3,
            1) + CLI_checkarg(4, 1) == 0)
    {
        IMAGE_BASIC_get_assym_component(data.cmdargtoken[1].val.string,
                                        data.cmdargtoken[2].val.string, data.cmdargtoken[3].val.numf,
                                        data.cmdargtoken[4].val.numf, "");
        return CLICMD_SUCCESS;
    }
    else
    {
        return CLICMD_INVALID_ARG;
    }
}

errno_t IMAGE_BASIC_get_sym_component_cli()
{
    if(CLI_checkarg(1, 4) + CLI_checkarg(2, 3) + CLI_checkarg(3,
            1) + CLI_checkarg(4, 1) == 0)
    {
        IMAGE_BASIC_get_sym_component(data.cmdargtoken[1].val.string,
                                      data.cmdargtoken[2].val.string, data.cmdargtoken[3].val.numf,
                                      data.cmdargtoken[4].val.numf);
        return CLICMD_SUCCESS;
    }
    else
    {
        return CLICMD_INVALID_ARG;
    }
}


errno_t image_basic_load_fitsimages_cube_cli()
{
    if(CLI_checkarg(1, 3) + CLI_checkarg(2, 3) == 0)
    {
        load_fitsimages_cube(data.cmdargtoken[1].val.string,
                             data.cmdargtoken[2].val.string);
        return CLICMD_SUCCESS;
    }
    else
    {
        return CLICMD_INVALID_ARG;
    }
}


errno_t image_basic_cubecollapse_cli()
{
    if(CLI_checkarg(1, 4) + CLI_checkarg(2, 3) == 0)
    {
        cube_collapse(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.string);
        return CLICMD_SUCCESS;
    }
    else
    {
        return CLICMD_INVALID_ARG;
    }
}


errno_t image_basic_indexmap_cli()
{
    if(CLI_checkarg(1, 4) + CLI_checkarg(2, 4) + CLI_checkarg(3, 3) == 0)
    {
        image_basic_indexmap(data.cmdargtoken[1].val.string,
                             data.cmdargtoken[2].val.string, data.cmdargtoken[3].val.string);
        return CLICMD_SUCCESS;
    }
    else
    {
        return CLICMD_INVALID_ARG;
    }
}




errno_t image_basic_streamaverage_cli()
{
    if(CLI_checkarg(1, 4) + CLI_checkarg(2, 2) + CLI_checkarg(3,
            3) + CLI_checkarg(4, 2) + CLI_checkarg(5, 2) == 0)
    {
        IMAGE_BASIC_streamaverage(data.cmdargtoken[1].val.string,
                                  data.cmdargtoken[2].val.numl, data.cmdargtoken[3].val.string,
                                  data.cmdargtoken[4].val.numl, data.cmdargtoken[5].val.numl);
        return CLICMD_SUCCESS;
    }
    else
    {
        return CLICMD_INVALID_ARG;
    }
}


errno_t image_basic_streamfeed_cli()
{
    if(CLI_checkarg(1, 4) + CLI_checkarg(2, 4) + CLI_checkarg(3, 1) == 0)
    {
        IMAGE_BASIC_streamfeed(data.cmdargtoken[1].val.string,
                               data.cmdargtoken[2].val.string, data.cmdargtoken[3].val.numf);
        return CLICMD_SUCCESS;
    }
    else
    {
        return CLICMD_INVALID_ARG;
    }
}

errno_t IMAGE_BASIC_streamrecord_cli()
{
    if(CLI_checkarg(1, 4) + CLI_checkarg(2, 2) + CLI_checkarg(3, 3) == 0)
    {
        IMAGE_BASIC_streamrecord(data.cmdargtoken[1].val.string,
                                 data.cmdargtoken[2].val.numl, data.cmdargtoken[3].val.string);
        return CLICMD_SUCCESS;
    }
    else
    {
        return CLICMD_INVALID_ARG;
    }
}







static errno_t init_module_CLI()
{

    RegisterCLIcommand(
        "imexpand",
        __FILE__,
        image_basic_expand_cli,
        "expand 2D image",
        "<image in> <output image> <x factor> <y factor>",
        "imexpand im1 im2 2 2",
        "long basic_expand(const char *ID_name, const char *ID_name_out, int n1, int n2)");


    RegisterCLIcommand(
        "imexpand3D",
        __FILE__,
        image_basic_expand3D_cli,
        "expand 3D image",
        "<image in> <output image> <x factor> <y factor> <z factor>",
        "imexpand3D im1 im2 2 2 2",
        "long basic_expand3D(const char *ID_name, const char *ID_name_out, int n1, int n2, int n3)");


    RegisterCLIcommand(
        "resizeim",
        __FILE__,
        image_basic_resize_cli,
        "resize 2D image",
        "<image in> <output image> <new x size> <new y size>",
        "resizeim im1 im2 230 200",

        "long basic_resizeim(const char *imname_in, const char *imname_out, long xsizeout, long ysizeout)");


    RegisterCLIcommand(
        "rotateim",
        __FILE__,
        image_basic_rotate_cli,
        "rotate 2D image",
        "<image in> <output image> <angle>",
        "rotateim imin imout 230",
        "long basic_rotate(const char *ID_name, const char *ID_out_name, float angle)");



    RegisterCLIcommand(
        "im3Dto2D",
        __FILE__,
        image_basic_3Dto2D_cli,
        "collapse first 2 axis of 3D image (in place)",
        "<image name>",
        "im3Dto2D im1",
        "long image_basic_3Dto2D(const char *IDname)");


    RegisterCLIcommand(
        "imswapaxis2D",
        __FILE__,
        image_basic_SwapAxis2D_cli,
        "Swap axis of a 2D image",
        "<input image> <output image>",
        "imswapaxis2D im1 im2",
        "long image_basic_SwapAxis2D(const char *IDin_name, const char *IDout_name)");


    RegisterCLIcommand(
        "addim",
        __FILE__,
        image_basic_add_cli,
        "add two 2D images of different size",
        "<im1> <im2> <outim> <offsetx> <offsety>",
        "addim im1 im2 outim 23 201",
        "long basic_add(const char *ID_name1, const char *ID_name2, const char *ID_name_out, long off1, long off2)");


    RegisterCLIcommand(
        "addim3D",
        __FILE__,
        image_basic_add3D_cli,
        "add two 3D images of different size",
        "<im1> <im2> <outim> <offsetx> <offsety> <offsetz>",
        "addim3D im1 im2 outim 23 201 0",
        "long basic_add3D(const char *ID_name1, const char *ID_name2, const char *ID_name_out, long off1, long off2, long off3)");



    RegisterCLIcommand(
        "imcontract3D",
        __FILE__,
        image_basic_contract3D_cli,
        "image binning (3D)",
        "<inim> <outim> <binx> <biny> <binz>",
        "imcontracteD im1 outim 4 4 1",
        "long basic_contract3D(const char *ID_name, const char *ID_name_out, int n1, int n2, int n3)");


    RegisterCLIcommand(
        "imcontract",
        __FILE__,
        image_basic_contract_cli,
        "image binning",
        "<inim> <outim> <binx> <biny>",
        "imcontract im1 outim 4 4",
        "long basic_contract(const char *ID_name, const char *ID_name_out, int n1, int n2)");


    RegisterCLIcommand(
        "imgetcircassym",
        __FILE__,
        IMAGE_BASIC_get_assym_component_cli,
        "extract non-circular symmetric part of image",
        "<inim> <outim> <xcenter> <ycenter>",
        "imcgetcircassym imin imout 256.0 230.5",
        "long IMAGE_BASIC_get_assym_component(const char *ID_name, const char *ID_out_name, float xcenter, float ycenter, const char *options)");


    RegisterCLIcommand(
        "imgetcircsym",
        __FILE__,
        IMAGE_BASIC_get_sym_component_cli,
        "extract circular symmetric part of image",
        "<inim> <outim> <xcenter> <ycenter>",
        "imcgetcircsym imin imout 256.0 230.5",
        "long IMAGE_BASIC_get_sym_component(const char *ID_name, const char *ID_out_name, float xcenter, float ycenter)");



    RegisterCLIcommand(
        "loadfitsimgcube",
        __FILE__,
        image_basic_load_fitsimages_cube_cli,
        "load multiple images into a single cube",
        "loadfitsimgcube <string pattern> <outputcube>",
        "loadfitsimgcube im out",
        "long load_fitsimages_cube(const char *strfilter, const char *ID_out_name)");



    RegisterCLIcommand(
        "cubecollapse",
        __FILE__,
        image_basic_cubecollapse_cli,
        "collapse a cube along z",
        "cubecollapse <inim> <outim>",
        "cubecollapse im1 outim",
        "long cube_collapse(const char *ID_in_name, const char *ID_out_name)");


    RegisterCLIcommand(
        "imindexmap",
        __FILE__,
        image_basic_indexmap_cli,
        "map input values to output image unsing index map",
        "imindexmap <indexmap> <values> <output>",
        "imindexmap imap imval outmap",
        "long image_basic_indexmap(char *ID_index_name, char *ID_values_name, char *IDout_name)");


    RegisterCLIcommand(
        "imgstreamave",
        __FILE__,
        image_basic_streamaverage_cli,
        "average stream of images",
        "imgstreamave <imin> <NBcoadd [long]> <imout> <mode> <semindex>",
        "imgstreamave im 100 imave 0 -1",
        "long IMAGE_BASIC_streamaverage(const char *IDname, long NBcoadd, const char *IDoutname, int mode, int semindex)");


    RegisterCLIcommand(
        "imgstreamfeed",
        __FILE__,
        image_basic_streamfeed_cli,
        "feed stream of images",
        "<input image/cube> <stream> <fequ [Hz]>",
        "imgstreamfeed im imstream 100",
        "long IMAGE_BASIC_streamfeed(const char *IDname, const char *streamname, float frequ)");


    RegisterCLIcommand(
        "imgstreamrec",
        __FILE__,
        IMAGE_BASIC_streamrecord_cli,
        "record stream of images",
        "<stream> <# frames> <output>",
        "imgstreamrec imstream 100 imrec",
        "long IMAGE_BASIC_streamrecord(const char *streamname, long NBframes, const char *IDname)");



    // add atexit functions here

    return RETURN_SUCCESS;

}






/* set all nan and inf pixel values to zero */
int basic_naninf2zero(
    const char *ID_name
)
{
    imageID ID;
    long ii, jj;
    long naxes[2];
    long cnt = 0;

    ID = image_ID(ID_name);
    naxes[0] = data.image[ID].md[0].size[0];
    naxes[1] = data.image[ID].md[0].size[1];

    for(jj = 0; jj < naxes[1]; jj++)
        for(ii = 0; ii < naxes[0]; ii++)
        {
            if(!(fabs(data.image[ID].array.F[jj * naxes[0] + ii]) < HUGE_VAL))
            {
                data.image[ID].array.F[jj * naxes[0] + ii] = 0.0;
                cnt ++;
            }
        }

    printf("%ld values replaced\n", cnt);

    return(0);
}




int basic_mincontract(
    __attribute__((unused)) const char *ID_name,
    __attribute__((unused)) uint8_t     axis,
    __attribute__((unused)) const char *out_name)
{



    return(0);
}



int basic_lmin_im(
    const char *ID_name,
    const char *out_name
)
{
    imageID IDin, IDout;
    long ii, jj;
    long naxes[2];
    float tmp;

    IDin = image_ID(ID_name);
    naxes[0] = data.image[IDin].md[0].size[0];
    naxes[1] = data.image[IDin].md[0].size[1];

    IDout = create_2Dimage_ID(out_name, naxes[0], 1);

    for(ii = 0; ii < naxes[0]; ii++)
    {
        data.image[IDout].array.F[ii] = data.image[IDin].array.F[ii];
    }

    for(jj = 1; jj < naxes[1]; jj++)
        for(ii = 0; ii < naxes[0]; ii++)
        {
            tmp = data.image[IDin].array.F[jj * naxes[0] + ii];
            if(tmp < data.image[IDout].array.F[ii])
            {
                data.image[IDout].array.F[ii] = tmp;
            }
        }

    return(0);
}




int basic_lmax_im(
    const char *ID_name,
    const char *out_name
)
{
    imageID IDin, IDout;
    long ii, jj;
    long naxes[2];
    float tmp;

    IDin = image_ID(ID_name);
    naxes[0] = data.image[IDin].md[0].size[0];
    naxes[1] = data.image[IDin].md[0].size[1];

    IDout = create_2Dimage_ID(out_name, naxes[0], 1);

    for(ii = 0; ii < naxes[0]; ii++)
    {
        data.image[IDout].array.F[ii] = data.image[IDin].array.F[ii];
    }

    for(jj = 1; jj < naxes[1]; jj++)
        for(ii = 0; ii < naxes[0]; ii++)
        {
            tmp = data.image[IDin].array.F[jj * naxes[0] + ii];
            if(tmp > data.image[IDout].array.F[ii])
            {
                data.image[IDout].array.F[ii] = tmp;
            }
        }

    return(0);
}



imageID basic_add(
    const char *ID_name1,
    const char *ID_name2,
    const char *ID_name_out,
    long off1,
    long off2
)
{
    imageID ID1, ID2; /* ID for the 2 images added */
    int ID_out; /* ID for the output image */
    long ii, jj;
    long naxes1[2], naxes2[2], naxes[2];
    long xmin, ymin, xmax, ymax; /* extrema in the ID1 coordinates */
    uint8_t datatype1, datatype2, datatype;
    int datatypeOK;

    ID1 = image_ID(ID_name1);
    ID2 = image_ID(ID_name2);
    naxes1[0] = data.image[ID1].md[0].size[0];
    naxes1[1] = data.image[ID1].md[0].size[1];
    naxes2[0] = data.image[ID2].md[0].size[0];
    naxes2[1] = data.image[ID2].md[0].size[1];

    datatype1 = data.image[ID1].md[0].datatype;
    datatype2 = data.image[ID2].md[0].datatype;

    datatypeOK = 0;

    if((datatype1 == _DATATYPE_FLOAT) && (datatype2 == _DATATYPE_FLOAT))
    {
        datatype = _DATATYPE_FLOAT;
        datatypeOK = 1;
    }
    if((datatype1 == _DATATYPE_DOUBLE) && (datatype2 == _DATATYPE_DOUBLE))
    {
        datatype = _DATATYPE_DOUBLE;
        datatypeOK = 1;
    }

    if(datatypeOK == 0)
    {
        printf("ERROR in basic_add: data type combination not supported\n");
        exit(EXIT_FAILURE);
    }

    /*  if(data.quiet==0)*/
    /* printf("add called with %s ( %ld x %ld ) %s ( %ld x %ld ) and offset ( %ld x %ld )\n",ID_name1,naxes1[0],naxes1[1],ID_name2,naxes2[0],naxes2[1],off1,off2);*/
    xmin = 0;
    if(off1 < 0)
    {
        xmin = off1;
    }
    ymin = 0;
    if(off2 < 0)
    {
        ymin = off2;
    }
    xmax = naxes1[0];
    if((naxes2[0] + off1) > naxes1[0])
    {
        xmax = (naxes2[0] + off1);
    }
    ymax = naxes1[1];
    if((naxes2[1] + off2) > naxes1[1])
    {
        ymax = (naxes2[1] + off2);
    }


    if(datatype == _DATATYPE_FLOAT)
    {
        create_2Dimage_ID(ID_name_out, (xmax - xmin), (ymax - ymin));
        ID_out = image_ID(ID_name_out);
        naxes[0] = data.image[ID_out].md[0].size[0];
        naxes[1] = data.image[ID_out].md[0].size[1];

        for(jj = 0; jj < naxes[1]; jj++)
            for(ii = 0; ii < naxes[0]; ii++)
            {
                {
                    data.image[ID_out].array.F[jj * naxes[0] + ii] = 0;
                    /* if pixel is in ID1 */
                    if(((ii + xmin) >= 0) && ((ii + xmin) < naxes1[0]))
                        if(((jj + ymin) >= 0) && ((jj + ymin) < naxes1[1]))
                        {
                            data.image[ID_out].array.F[jj * naxes[0] + ii] += data.image[ID1].array.F[(jj +
                                    ymin) * naxes1[0] + (ii + xmin)];
                        }
                    /* if pixel is in ID2 */
                    if(((ii + xmin - off1) >= 0) && ((ii + xmin - off1) < naxes2[0]))
                        if(((jj + ymin - off2) >= 0) && ((jj + ymin - off2) < naxes2[1]))
                        {
                            data.image[ID_out].array.F[jj * naxes[0] + ii] += data.image[ID2].array.F[(jj +
                                    ymin - off2) * naxes2[0] + (ii + xmin - off1)];
                        }
                }
            }
    }

    if(datatype == _DATATYPE_DOUBLE)
    {
        create_2Dimage_ID_double(ID_name_out, (xmax - xmin), (ymax - ymin));
        ID_out = image_ID(ID_name_out);
        naxes[0] = data.image[ID_out].md[0].size[0];
        naxes[1] = data.image[ID_out].md[0].size[1];

        for(jj = 0; jj < naxes[1]; jj++)
            for(ii = 0; ii < naxes[0]; ii++)
            {
                {
                    data.image[ID_out].array.D[jj * naxes[0] + ii] = 0;
                    /* if pixel is in ID1 */
                    if(((ii + xmin) >= 0) && ((ii + xmin) < naxes1[0]))
                        if(((jj + ymin) >= 0) && ((jj + ymin) < naxes1[1]))
                        {
                            data.image[ID_out].array.D[jj * naxes[0] + ii] += data.image[ID1].array.D[(jj +
                                    ymin) * naxes1[0] + (ii + xmin)];
                        }
                    /* if pixel is in ID2 */
                    if(((ii + xmin - off1) >= 0) && ((ii + xmin - off1) < naxes2[0]))
                        if(((jj + ymin - off2) >= 0) && ((jj + ymin - off2) < naxes2[1]))
                        {
                            data.image[ID_out].array.D[jj * naxes[0] + ii] += data.image[ID2].array.D[(jj +
                                    ymin - off2) * naxes2[0] + (ii + xmin - off1)];
                        }
                }
            }
    }

    return(ID_out);
}




imageID basic_add3D(
    const char *ID_name1,
    const char *ID_name2,
    const char *ID_name_out,
    long        off1,
    long        off2,
    long        off3
)
{
    imageID ID1, ID2; /* ID for the 2 images added */
    imageID ID_out; /* ID for the output image */
    uint32_t  naxes1[3], naxes2[3], naxes[3];
    long xmin, ymin, zmin, xmax, ymax, zmax; /* extrema in the ID1 coordinates */
    uint8_t datatype1, datatype2, datatype;
    int datatypeOK;

    ID1 = image_ID(ID_name1);
    ID2 = image_ID(ID_name2);
    naxes1[0] = data.image[ID1].md[0].size[0];
    naxes1[1] = data.image[ID1].md[0].size[1];
    naxes1[2] = data.image[ID1].md[0].size[2];

    naxes2[0] = data.image[ID2].md[0].size[0];
    naxes2[1] = data.image[ID2].md[0].size[1];
    naxes2[2] = data.image[ID2].md[0].size[2];

    datatype1 = data.image[ID1].md[0].datatype;
    datatype2 = data.image[ID2].md[0].datatype;

    datatypeOK = 0;

    if((datatype1 == _DATATYPE_FLOAT) && (datatype2 == _DATATYPE_FLOAT))
    {
        datatype = _DATATYPE_FLOAT;
        datatypeOK = 1;
    }
    if((datatype1 == _DATATYPE_DOUBLE) && (datatype2 == _DATATYPE_DOUBLE))
    {
        datatype = _DATATYPE_DOUBLE;
        datatypeOK = 1;
    }

    if(datatypeOK == 0)
    {
        printf("ERROR in basic_add: data type combination not supported\n");
        exit(0);
    }

    /*  if(data.quiet==0)*/
    /* printf("add called with %s ( %ld x %ld ) %s ( %ld x %ld ) and offset ( %ld x %ld )\n",ID_name1,naxes1[0],naxes1[1],ID_name2,naxes2[0],naxes2[1],off1,off2);*/
    xmin = 0;
    if(off1 < 0)
    {
        xmin = off1;
    }

    ymin = 0;
    if(off2 < 0)
    {
        ymin = off2;
    }

    zmin = 0;
    if(off3 < 0)
    {
        zmin = off3;
    }

    xmax = naxes1[0];
    if((naxes2[0] + off1) > naxes1[0])
    {
        xmax = (naxes2[0] + off1);
    }

    ymax = naxes1[1];
    if((naxes2[1] + off2) > naxes1[1])
    {
        ymax = (naxes2[1] + off2);
    }

    zmax = naxes1[2];
    if((naxes2[2] + off3) > naxes1[2])
    {
        zmax = (naxes2[2] + off3);
    }



    if(datatype == _DATATYPE_FLOAT)
    {
        create_3Dimage_ID(ID_name_out, (xmax - xmin), (ymax - ymin), (zmax - zmin));
        ID_out = image_ID(ID_name_out);
        naxes[0] = data.image[ID_out].md[0].size[0];
        naxes[1] = data.image[ID_out].md[0].size[1];
        naxes[2] = data.image[ID_out].md[0].size[2];

        for(uint32_t kk = 0; kk < naxes[2]; kk++)
            for(uint32_t jj = 0; jj < naxes[1]; jj++)
                for(uint32_t ii = 0; ii < naxes[0]; ii++)
                {
                    {
                        data.image[ID_out].array.F[kk * naxes[1]*naxes[0] + jj * naxes[0] + ii] = 0;
                        /* if pixel is in ID1 */

                        if(((ii + xmin) >= 0) && ((ii + xmin) < naxes1[0]))
                            if(((jj + ymin) >= 0) && ((jj + ymin) < naxes1[1]))
                                if(((kk + zmin) >= 0) && ((kk + zmin) < naxes1[2]))
                                {
                                    data.image[ID_out].array.F[kk * naxes[1]*naxes[0] + jj * naxes[0] + ii] +=
                                        data.image[ID1].array.F[(kk + zmin) * naxes1[1] * naxes1[0] +
                                                                (jj + ymin) * naxes1[0] + (ii + xmin)];
                                }
                        /* if pixel is in ID2 */
                        if(((ii + xmin - off1) >= 0) && ((ii + xmin - off1) < naxes2[0]))
                            if(((jj + ymin - off2) >= 0) && ((jj + ymin - off2) < naxes2[1]))
                                if(((kk + zmin - off3) >= 0) && ((kk + zmin - off3) < naxes2[2]))
                                {
                                    data.image[ID_out].array.F[kk * naxes[1]*naxes[0] + jj * naxes[0] + ii] +=
                                        data.image[ID2].array.F[(kk + zmin - off3) * naxes2[1] * naxes2[0] +
                                                                (jj + ymin - off2) * naxes2[0] + (ii + xmin - off1)];
                                }
                    }
                }
    }

    if(datatype == _DATATYPE_DOUBLE)
    {
        create_3Dimage_ID_double(ID_name_out, (xmax - xmin), (ymax - ymin),
                                 (zmax - zmin));
        ID_out = image_ID(ID_name_out);
        naxes[0] = data.image[ID_out].md[0].size[0];
        naxes[1] = data.image[ID_out].md[0].size[1];
        naxes[2] = data.image[ID_out].md[0].size[2];

        for(uint32_t kk = 0; kk < naxes[2]; kk++)
            for(uint32_t jj = 0; jj < naxes[1]; jj++)
                for(uint32_t ii = 0; ii < naxes[0]; ii++)
                {
                    {
                        data.image[ID_out].array.D[kk * naxes[1]*naxes[0] + jj * naxes[0] + ii] = 0;
                        /* if pixel is in ID1 */
                        if(((ii + xmin) >= 0) && ((ii + xmin) < naxes1[0]))
                            if(((jj + ymin) >= 0) && ((jj + ymin) < naxes1[1]))
                                if(((kk + zmin) >= 0) && ((kk + zmin) < naxes1[2]))
                                {
                                    data.image[ID_out].array.D[ kk * naxes[1]*naxes[0] + jj * naxes[0] + ii] +=
                                        data.image[ID1].array.D[(kk + zmin) * naxes1[1] * naxes1[0] +
                                                                (jj + ymin) * naxes1[0] + (ii + xmin) ];
                                }
                        /* if pixel is in ID2 */
                        if(((ii + xmin - off1) >= 0) && ((ii + xmin - off1) < naxes2[0]))
                            if(((jj + ymin - off2) >= 0) && ((jj + ymin - off2) < naxes2[1]))
                                if(((kk + zmin - off3) >= 0) && ((kk + zmin - off3) < naxes2[2]))
                                {
                                    data.image[ID_out].array.D[kk * naxes[1]*naxes[0] + jj * naxes[0] + ii] +=
                                        data.image[ID2].array.D[(kk + zmin - off3) * naxes2[1] * naxes2[0] +
                                                                (jj + ymin - off2) * naxes2[0] + (ii + xmin - off1)];
                                }
                    }
                }
    }

    return(ID_out);
}





long basic_diff(const char *ID_name1, const char *ID_name2,
                const char *ID_name_out, long off1, long off2)
{
    int ID1, ID2; /* ID for the 2 images added */
    int ID_out; /* ID for the output image */
    long ii, jj;
    long naxes1[2], naxes2[2], naxes[2];
    long xmin, ymin, xmax, ymax; /* extrema in the ID1 coordinates */

    ID1 = image_ID(ID_name1);
    ID2 = image_ID(ID_name2);
    naxes1[0] = data.image[ID1].md[0].size[0];
    naxes1[1] = data.image[ID1].md[0].size[1];
    naxes2[0] = data.image[ID2].md[0].size[0];
    naxes2[1] = data.image[ID2].md[0].size[1];

    printf("add called with %s ( %ld x %ld ) %s ( %ld x %ld ) and offset ( %ld x %ld )\n",
           ID_name1, naxes1[0], naxes1[1], ID_name2, naxes2[0], naxes2[1], off1, off2);
    xmin = 0;
    if(off1 < 0)
    {
        xmin = off1;
    }
    ymin = 0;
    if(off2 < 0)
    {
        ymin = off2;
    }
    xmax = naxes1[0];
    if((naxes2[0] + off1) > naxes1[0])
    {
        xmax = (naxes2[0] + off1);
    }
    ymax = naxes1[1];
    if((naxes2[1] + off2) > naxes1[1])
    {
        ymax = (naxes2[1] + off2);
    }

    create_2Dimage_ID(ID_name_out, (xmax - xmin), (ymax - ymin));
    ID_out = image_ID(ID_name_out);
    naxes[0] = data.image[ID_out].md[0].size[0];
    naxes[1] = data.image[ID_out].md[0].size[1];

    for(jj = 0; jj < naxes[1]; jj++)
        for(ii = 0; ii < naxes[0]; ii++)
        {
            {
                data.image[ID_out].array.F[jj * naxes[0] + ii] = 0;
                /* if pixel is in ID1 */
                if(((ii + xmin) >= 0) && ((ii + xmin) < naxes1[0]))
                    if(((jj + ymin) >= 0) && ((jj + ymin) < naxes1[1]))
                    {
                        data.image[ID_out].array.F[jj * naxes[0] + ii] += data.image[ID1].array.F[(jj +
                                ymin) * naxes1[0] + (ii + xmin)];
                    }
                /* if pixel is in ID2 */
                if(((ii + xmin - off1) >= 0) && ((ii + xmin - off1) < naxes2[0]))
                    if(((jj + ymin - off2) >= 0) && ((jj + ymin - off2) < naxes2[1]))
                    {
                        data.image[ID_out].array.F[jj * naxes[0] + ii] -= data.image[ID2].array.F[(jj +
                                ymin - off2) * naxes2[0] + (ii + xmin - off1)];
                    }
            }
        }
    return(ID_out);
}

int basic_add_cst(const char *ID_name, float f1, int sign) /* add a constant */
{
    int ID;
    long ii, jj;
    long naxes[2];

    ID = image_ID(ID_name);
    naxes[0] = data.image[ID].md[0].size[0];
    naxes[1] = data.image[ID].md[0].size[1];

    for(jj = 0; jj < naxes[1]; jj++)
        for(ii = 0; ii < naxes[0]; ii++)
        {
            data.image[ID].array.F[jj * naxes[0] + ii] = data.image[ID].array.F[jj *
                    naxes[0] + ii] + f1 * sign;
        }

    return(0);
}



/* extracts a n1xn2 subwindow of an image at offset n3,n4 */
imageID basic_extract(
    const char *ID_in_name,
    const char *ID_out_name,
    long n1,
    long n2,
    long n3,
    long n4
)
{
    imageID ID_in;
    imageID ID_out;
    long ii, jj;
    char name[SBUFFERSIZE];
    int n;

    /*  printf("starting extraction\n"); */
    ID_in = image_ID(ID_in_name);
    /*  printf("ID_in is %d\n",ID_in);
        printf("%s %ld %ld %ld %ld\n",ID_out_name,n1,n2,n3,n4); */
    n = snprintf(name, SBUFFERSIZE, "%s", ID_out_name);
    if(n >= SBUFFERSIZE)
    {
        PRINT_ERROR("Attempted to write string buffer with too many characters");
    }

    create_2Dimage_ID(name, n1, n2);
    fflush(stdout);
    ID_out = image_ID(ID_out_name);
    for(ii = 0; ii < n1; ii++)
        for(jj = 0; jj < n2; jj++)
        {
            data.image[ID_out].array.F[jj * n1 + ii] = data.image[ID_in].array.F[(jj + n4) *
                    data.image[ID_in].md[0].size[0] + ii + n3];
        }
    /* printf("extraction done\n"); */

    return(ID_out);
}



int basic_trunc_circ(const char *ID_name, float f1)
{
    imageID ID;
    long ii, jj;
    long naxes[2];

    ID = image_ID(ID_name);
    naxes[0] = data.image[ID].md[0].size[0];
    naxes[1] = data.image[ID].md[0].size[1];

    for(jj = 0; jj < naxes[1]; jj++)
        for(ii = 0; ii < naxes[0]; ii++)
        {
            data.image[ID].array.F[jj * naxes[0] + ii] = fmod(data.image[ID].array.F[jj *
                    naxes[0] + ii], f1);
        }

    return(0);
}


/* expand image by factor n1 along x axis and n2 along y axis */
imageID basic_expand(
    const char *ID_name,
    const char *ID_name_out,
    int n1,
    int n2
)
{
    imageID ID;
    imageID ID_out; /* ID for the output image */
    long ii, jj;
    long naxes[2], naxes_out[2];
    int i, j;

    ID = image_ID(ID_name);

    naxes[0] = data.image[ID].md[0].size[0];
    naxes[1] = data.image[ID].md[0].size[1];
    naxes_out[0] = naxes[0] * n1;
    naxes_out[1] = naxes[1] * n2;

    ID_out = create_2Dimage_ID(ID_name_out, naxes_out[0], naxes_out[1]);


    for(jj = 0; jj < naxes[1]; jj++)
        for(ii = 0; ii < naxes[0]; ii++)
            for(i = 0; i < n1; i++)
                for(j = 0; j < n2; j++)
                {
                    data.image[ID_out].array.F[(jj * n2 + j)*naxes_out[0] + ii * n1 + i] =
                        data.image[ID].array.F[jj * naxes[0] + ii];
                }
    return(ID_out);
}


/* expand image by factor n1 along x axis and n2 along y axis */
imageID basic_expand3D(
    const char *ID_name,
    const char *ID_name_out,
    int n1,
    int n2,
    int n3
)
{
    imageID ID;
    imageID ID_out; /* ID for the output image */
    long ii, jj, kk;
    long naxes[3], naxes_out[3];
    int i, j, k;

    ID = image_ID(ID_name);

    naxes[0] = data.image[ID].md[0].size[0];
    if(data.image[ID].md[0].naxis > 1)
    {
        naxes[1] = data.image[ID].md[0].size[1];
    }
    else
    {
        naxes[1] = 1;
    }
    if(data.image[ID].md[0].naxis == 3)
    {
        naxes[2] = data.image[ID].md[0].size[2];
    }
    else
    {
        naxes[2] = 1;
    }
    naxes_out[0] = naxes[0] * n1;
    naxes_out[1] = naxes[1] * n2;
    naxes_out[2] = naxes[2] * n3;

    printf(" %ld %ld %ld -> %ld %ld %ld\n", naxes[0], naxes[1], naxes[2],
           naxes_out[0], naxes_out[1], naxes_out[2]);

    ID_out = create_3Dimage_ID(ID_name_out, naxes_out[0], naxes_out[1],
                               naxes_out[2]);
    list_image_ID();

    for(kk = 0; kk < naxes[2]; kk++)
        for(jj = 0; jj < naxes[1]; jj++)
            for(ii = 0; ii < naxes[0]; ii++)
                for(i = 0; i < n1; i++)
                    for(j = 0; j < n2; j++)
                        for(k = 0; k < n3; k++)
                        {
                            data.image[ID_out].array.F[(kk * n3 + k)*naxes_out[0]*naxes_out[1] +
                                                       (jj * n2 + j)*naxes_out[0] + ii * n1 + i] = data.image[ID].array.F[kk * naxes[0]
                                                               * naxes[1] + jj * naxes[0] + ii];
                        }
    return(ID_out);
}



imageID basic_zoom2(
    const char *ID_name,
    const char *ID_name_out
)
{
    imageID ID;
    imageID ID_out; /* ID for the output image */
    uint32_t naxes[2], naxes_out[2];
    char lstring[SBUFFERSIZE];
    int n;

    n = snprintf(lstring, SBUFFERSIZE, "%s=%s*1", ID_name_out, ID_name);
    if(n >= SBUFFERSIZE)
    {
        PRINT_ERROR("Attempted to write string buffer with too many characters");
    }


    execute_arith(lstring);
    ID = image_ID(ID_name);
    naxes[0] = data.image[ID].md[0].size[0];
    naxes[1] = data.image[ID].md[0].size[1];
    naxes_out[0] = naxes[0];
    naxes_out[1] = naxes[1];
    ID_out = image_ID(ID_name_out);

    for(uint32_t jj = 0; jj < naxes[1] / 2; jj++)
        for(uint32_t ii = 0; ii < naxes[0] / 2; ii++)
        {
            data.image[ID_out].array.F[(2 * jj)*naxes_out[0] + (2 * ii)] =
                data.image[ID].array.F[(jj + naxes[1] / 4) * naxes[0] + (ii + naxes[1] / 4)];
            data.image[ID_out].array.F[(2 * jj + 1)*naxes_out[0] + (2 * ii)] = 0.5 *
                    (data.image[ID].array.F[(jj + naxes[1] / 4) * naxes[0] +
                                            (ii + naxes[1] / 4)] + data.image[ID].array.F[(jj + naxes[1] / 4 + 1) *
                                                    naxes[0] + (ii + naxes[1] / 4)]);
            data.image[ID_out].array.F[(2 * jj)*naxes_out[0] + (2 * ii + 1)] = 0.5 *
                    (data.image[ID].array.F[(jj + naxes[1] / 4) * naxes[0] +
                                            (ii + naxes[1] / 4)] + data.image[ID].array.F[(jj + naxes[1] / 4) * naxes[0] +
                                                    (ii + naxes[1] / 4 + 1)]);
            data.image[ID_out].array.F[(2 * jj + 1)*naxes_out[0] + (2 * ii + 1)] = 0.25 *
                    (data.image[ID].array.F[(jj + naxes[1] / 4) * naxes[0] +
                                            (ii + naxes[1] / 4)] + data.image[ID].array.F[(jj + naxes[1] / 4) * naxes[0] +
                                                    (ii + naxes[1] / 4 + 1)] + data.image[ID].array.F[(jj + naxes[1] / 4 + 1) *
                                                            naxes[0] + (ii + naxes[1] / 4)] + data.image[ID].array.F[(jj + naxes[1] / 4 + 1)
                                                                    * naxes[0] + (ii + naxes[1] / 4 + 1)]);
        }

    return(ID_out);
}



imageID basic_contract(
    const char *ID_name,
    const char *ID_name_out,
    int n1,
    int n2
)
{
    imageID ID;
    imageID ID_out; /* ID for the output image */    
    uint32_t naxes[2], naxes_out[2];
    int i, j;

    ID = image_ID(ID_name);
    naxes[0] = data.image[ID].md[0].size[0];
    naxes[1] = data.image[ID].md[0].size[1];

    naxes_out[0] = naxes[0] / n1;
    naxes_out[1] = naxes[1] / n2;

    //  printf("%ld %ld  ->  %ld %ld\n",naxes[0],naxes[1],naxes_out[0],naxes_out[1]);
    create_2Dimage_ID(ID_name_out, naxes_out[0], naxes_out[1]);
    ID_out = image_ID(ID_name_out);

    for(uint32_t jj = 0; jj < naxes_out[1]; jj++)
        for(uint32_t ii = 0; ii < naxes_out[0]; ii++)
            for(i = 0; i < n1; i++)
                for(j = 0; j < n2; j++)
                {
                    data.image[ID_out].array.F[jj * naxes_out[0] + ii] += data.image[ID].array.F[(jj
                            * n2 + j) * naxes[0] + ii * n1 + i];
                }

    return(ID_out);
}



imageID basic_contract3D(
    const char *ID_name,
    const char *ID_name_out,
    int n1,
    int n2,
    int n3)
{
    imageID ID;
    imageID ID_out; /* ID for the output image */
    uint32_t naxes[3];
    uint32_t *naxes_out;
    uint8_t datatype;

    ID = image_ID(ID_name);
    datatype = data.image[ID].md[0].datatype;
    naxes[0] = data.image[ID].md[0].size[0];
    naxes[1] = data.image[ID].md[0].size[1];
    naxes[2] = data.image[ID].md[0].size[2];

    naxes_out = (uint32_t *) malloc(sizeof(uint32_t) * 3);
    naxes_out[0] = naxes[0] / n1;
    naxes_out[1] = naxes[1] / n2;
    naxes_out[2] = naxes[2] / n3;


    if(naxes_out[2] == 1)
    {
        create_2Dimage_ID(ID_name_out, naxes_out[0], naxes_out[1]);
    }
    else
    {
        printf("(%ld x %ld x %ld)  ->  (%ld x %ld x %ld)\n",
               (long) naxes[0], (long) naxes[1], (long) naxes[2],
               (long) naxes_out[0], (long) naxes_out[1], (long) naxes_out[2]);
        create_image_ID(ID_name_out, 3, naxes_out, datatype, 0, 0);
    }

    ID_out = image_ID(ID_name_out);

    switch(datatype)
    {
        case _DATATYPE_FLOAT :
            for(uint32_t jj = 0; jj < naxes_out[1]; jj++)
                for(uint32_t ii = 0; ii < naxes_out[0]; ii++)
                    for(uint32_t kk = 0; kk < naxes_out[2]; kk++)
                        for(int i = 0; i < n1; i++)
                            for(int j = 0; j < n2; j++)
                                for(int k = 0; k < n3; k++)
                                {
                                    data.image[ID_out].array.F[kk * naxes_out[0]*naxes_out[1] + jj * naxes_out[0] +
                                                               ii] +=
                                                                   data.image[ID].array.F[(kk * n3 + k) * naxes[0] * naxes[1] +
                                                                           (jj * n2 + j) * naxes[0] + ii * n1 + i];
                                }
            break;
        case _DATATYPE_DOUBLE :
            for(uint32_t jj = 0; jj < naxes_out[1]; jj++)
                for(uint32_t ii = 0; ii < naxes_out[0]; ii++)
                    for(uint32_t kk = 0; kk < naxes_out[2]; kk++)
                        for(int i = 0; i < n1; i++)
                            for(int j = 0; j < n2; j++)
                                for(int k = 0; k < n3; k++)
                                {
                                    data.image[ID_out].array.D[kk * naxes_out[0]*naxes_out[1] + jj * naxes_out[0] +
                                                               ii] +=
                                                                   data.image[ID].array.D[(kk * n3 + k) * naxes[0] * naxes[1] +
                                                                           (jj * n2 + j) * naxes[0] + ii * n1 + i];
                                }
            break;
        case _DATATYPE_COMPLEX_FLOAT :
            for(uint32_t jj = 0; jj < naxes_out[1]; jj++)
                for(uint32_t ii = 0; ii < naxes_out[0]; ii++)
                    for(uint32_t kk = 0; kk < naxes_out[2]; kk++)
                        for(int i = 0; i < n1; i++)
                            for(int j = 0; j < n2; j++)
                                for(int k = 0; k < n3; k++)
                                {
                                    data.image[ID_out].array.CF[kk * naxes_out[0]*naxes_out[1] + jj * naxes_out[0] +
                                                                ii].re +=
                                                                    data.image[ID].array.CF[(kk * n3 + k) * naxes[0] * naxes[1] +
                                                                            (jj * n2 + j) * naxes[0] + ii * n1 + i].re;
                                    data.image[ID_out].array.CF[kk * naxes_out[0]*naxes_out[1] + jj * naxes_out[0] +
                                                                ii].im +=
                                                                    data.image[ID].array.CF[(kk * n3 + k) * naxes[0] * naxes[1] +
                                                                            (jj * n2 + j) * naxes[0] + ii * n1 + i].im;
                                }
            break;
        case _DATATYPE_COMPLEX_DOUBLE :
            for(uint32_t jj = 0; jj < naxes_out[1]; jj++)
                for(uint32_t ii = 0; ii < naxes_out[0]; ii++)
                    for(uint32_t kk = 0; kk < naxes_out[2]; kk++)
                        for(int i = 0; i < n1; i++)
                            for(int j = 0; j < n2; j++)
                                for(int k = 0; k < n3; k++)
                                {
                                    data.image[ID_out].array.CD[kk * naxes_out[0]*naxes_out[1] + jj * naxes_out[0] +
                                                                ii].re +=
                                                                    data.image[ID].array.CD[(kk * n3 + k) * naxes[0] * naxes[1] +
                                                                            (jj * n2 + j) * naxes[0] + ii * n1 + i].re;
                                    data.image[ID_out].array.CD[kk * naxes_out[0]*naxes_out[1] + jj * naxes_out[0] +
                                                                ii].im +=
                                                                    data.image[ID].array.CD[(kk * n3 + k) * naxes[0] * naxes[1] +
                                                                            (jj * n2 + j) * naxes[0] + ii * n1 + i].im;
                                }
            break;
    }

    free(naxes_out);

    return(ID_out);
}


long basic_average_column(
    __attribute__((unused)) const char *ID_name,
    __attribute__((unused)) const char *IDout_name
)
{
    long IDout = -1;

    // TO BE WRITTEN

    return(IDout);
}



imageID basic_padd(
    const char *ID_name,
    const char *ID_name_out,
    int n1,
    int n2
)
{
    imageID ID;
    imageID ID_out; /* ID for the output image */
    uint32_t naxes[2], naxes_out[2];

    ID = image_ID(ID_name);
    naxes[0] = data.image[ID].md[0].size[0];
    naxes[1] = data.image[ID].md[0].size[1];

    naxes_out[0] = naxes[0] + 2 * n1;
    naxes_out[1] = naxes[1] + 2 * n2;

    create_2Dimage_ID(ID_name_out, naxes_out[0], naxes_out[1]);
    ID_out = image_ID(ID_name_out);

    for(uint32_t jj = 0; jj < naxes[1]; jj++)
        for(uint32_t ii = 0; ii < naxes[0]; ii++)
        {
            data.image[ID_out].array.F[(jj + n2)*naxes_out[0] + ii + n1] =
                data.image[ID].array.F[jj * naxes[0] + ii];
        }

    return(ID_out);
}


/* flip an image relative to the horizontal axis */
imageID basic_fliph(
    const char *ID_name
)
{
    imageID ID;
    long naxes[2];
    uint32_t tmp_long;
    float temp;

    ID = image_ID(ID_name);
    naxes[0] = data.image[ID].md[0].size[0];
    naxes[1] = data.image[ID].md[0].size[1];

    temp = 0.0;
    tmp_long = (uint32_t) (naxes[1] / 2);
    /* printf("middle point %ld\n",tmp_long); */
    for(uint32_t jj = 0; jj < tmp_long; jj++)
        for(uint32_t ii = 0; ii < naxes[0]; ii++)
        {
            temp = data.image[ID].array.F[jj * naxes[0] + ii];
            data.image[ID].array.F[jj * naxes[0] + ii] = data.image[ID].array.F[(naxes[1] -
                    jj - 1) * naxes[0] + ii];
            data.image[ID].array.F[(naxes[1] - jj - 1)*naxes[0] + ii] = temp;
        }
    return(ID);
}




/* flip an image relative to the vertical axis */
imageID basic_flipv(
    const char *ID_name
)
{
    imageID ID;
    uint32_t naxes[2];
    uint32_t tmp_long;
    float temp;

    ID = image_ID(ID_name);
    naxes[0] = data.image[ID].md[0].size[0];
    naxes[1] = data.image[ID].md[0].size[1];

    temp = 0.0;
    tmp_long = (uint32_t) (naxes[0] / 2);
    /* printf("middle point %ld\n",tmp_long); */
    for(uint32_t jj = 0; jj < naxes[1]; jj++)
        for(uint32_t ii = 0; ii < tmp_long; ii++)
        {
            temp = data.image[ID].array.F[jj * naxes[0] + ii];
            data.image[ID].array.F[jj * naxes[0] + ii] = data.image[ID].array.F[jj *
                    naxes[0] + (naxes[0] - ii - 1)];
            data.image[ID].array.F[jj * naxes[0] + (naxes[0] - ii - 1)] = temp;
        }
    return(ID);
}



/* flip an image horizontally and vertically */
imageID basic_fliphv(
    const char *ID_name
)
{
    imageID ID;
    uint32_t naxes[2];
    uint32_t tmp_long;
    float temp;

    ID = image_ID(ID_name);
    naxes[0] = data.image[ID].md[0].size[0];
    naxes[1] = data.image[ID].md[0].size[1];

    temp = 0.0;
    tmp_long = (uint32_t) (naxes[1] / 2);
    /* printf("middle point %ld\n",tmp_long); */
    for(uint32_t jj = 0; jj < tmp_long; jj++)
        for(uint32_t ii = 0; ii < naxes[0]; ii++)
        {
            temp = data.image[ID].array.F[jj * naxes[0] + ii];
            data.image[ID].array.F[jj * naxes[0] + ii] = data.image[ID].array.F[(naxes[1] -
                    jj - 1) * naxes[0] + (naxes[0] - ii - 1)];
            data.image[ID].array.F[(naxes[1] - jj - 1)*naxes[0] + (naxes[0] - ii - 1)] =
                temp;
        }
    return(ID);
}


/* median of the images specified in options, output is ID_name */
int basic_median(
    const char *ID_name,
    const char *options
)
{
    unsigned int Nb_files;
    imageID ID;
    unsigned int file_nb;
    int str_pos;
    imageID *IDn;
    char file_name[STRINGMAXLEN_FILENAME];
    uint32_t naxes[2];
    int medianpt = 0;

    unsigned long i, j;
    float *array;

    Nb_files = 0;
    i = 0;
    str_pos = 0;
    while((options[i + str_pos] != '\n') && (options[i + str_pos] != '\0'))
    {
        if(options[i + str_pos] == ' ')
        {
            Nb_files += 1;
        }
        i++;
    }

    printf("%d files\n", Nb_files);
    medianpt = (int)(0.5 * (Nb_files - 1));

    IDn = (imageID *) malloc(sizeof(imageID) * Nb_files);
    if(IDn == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    array = (float *) malloc(sizeof(float) * Nb_files);
    if(array == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    i = 1;
    j = 0;
    file_nb = 0;
    while(file_nb < Nb_files)
    {
        if((options[i + str_pos] == ' ') || (options[i + str_pos] == '\0')
                || (options[i + str_pos] == '\n'))
        {
            file_name[j] = '\0';
            IDn[file_nb] = image_ID(file_name);
            printf("%d %s \n", (int) IDn[file_nb], file_name);
            file_nb += 1;
            j = 0;
        }
        else
        {
            file_name[j] = options[i + str_pos];
            j++;
        }
        i++;
    }

    naxes[0] = data.image[IDn[0]].md[0].size[0];
    naxes[1] = data.image[IDn[0]].md[0].size[1];
    create_2Dimage_ID(ID_name, naxes[0], naxes[1]);
    ID = image_ID(ID_name);

    for(uint32_t jj = 0; jj < naxes[1]; jj++)
        for(uint32_t ii = 0; ii < naxes[0]; ii++)
        {
            for(i = 0; i < Nb_files; i++)
            {
                array[i] = data.image[IDn[i]].array.F[jj * naxes[0] + ii];
            }
            quick_sort_float(array, Nb_files);
            if((0.5 * (Nb_files - 1) - medianpt) < 0.1)
            {
                data.image[ID].array.F[jj * naxes[0] + ii] = array[medianpt];
            }
            else
            {
                data.image[ID].array.F[jj * naxes[0] + ii] = 0.5 * array[medianpt] + 0.5 *
                        array[medianpt + 1];
            }
        }

    printf("%d %d \n", Nb_files, medianpt);
    if((0.5 * (Nb_files - 1) - medianpt) > 0.1)
    {
        printf("median of an even number of number: average of the 2 closest \n");
    }

    free(IDn);
    free(array);
    return(0);
}


imageID basic_renorm_max(
    const char *ID_name
)
{
    imageID ID;
    long ii, jj;
    long naxes[2];
    float max;

    ID = image_ID(ID_name);
    naxes[0] = data.image[ID].md[0].size[0];
    naxes[1] = data.image[ID].md[0].size[1];

    max = 0;

    for(jj = 0; jj < naxes[1]; jj++)
        for(ii = 0; ii < naxes[0]; ii++)
            if(data.image[ID].array.F[jj * naxes[0] + ii] > max)
            {
                max = data.image[ID].array.F[jj * naxes[0] + ii];
            }

    if(max != 0)
    {
        for(jj = 0; jj < naxes[1]; jj++)
            for(ii = 0; ii < naxes[0]; ii++)
            {
                data.image[ID].array.F[jj * naxes[0] + ii] /= max;
            }
    }

    return(ID);
}



imageID basic_rotate(
    const char *ID_name,
    const char *IDout_name,
    float angle
)
{
    imageID ID, IDout;
    long ii, jj, iis, jjs;
    long naxes[2];

    ID = image_ID(ID_name);
    naxes[0] = data.image[ID].md[0].size[0];
    naxes[1] = data.image[ID].md[0].size[1];
    create_2Dimage_ID(IDout_name, naxes[0], naxes[1]);
    IDout = image_ID(IDout_name);

    for(jj = 0; jj < naxes[1]; jj++)
        for(ii = 0; ii < naxes[0]; ii++)
        {
            iis = (long)(naxes[0] / 2 + (ii - naxes[0] / 2) * cos(angle) +
                         (jj - naxes[1] / 2) * sin(angle));
            jjs = (long)(naxes[1] / 2 - (ii - naxes[0] / 2) * sin(angle) +
                         (jj - naxes[1] / 2) * cos(angle));
            if((iis > 0) && (jjs > 0) && (iis < naxes[0]) && (jjs < naxes[1]))
            {
                data.image[IDout].array.F[jj * naxes[0] + ii] = data.image[ID].array.F[jjs *
                        naxes[0] + iis];
            }
        }

    return(IDout);
}


int basic_rotate90(
    const char *ID_name,
    const char *ID_out_name
)
{
    int ID, ID_out;
    long ii, jj, iis, jjs;
    long naxes[2];

    ID = image_ID(ID_name);
    naxes[0] = data.image[ID].md[0].size[0];
    naxes[1] = data.image[ID].md[0].size[1];
    create_2Dimage_ID(ID_out_name, naxes[1], naxes[0]);
    ID_out = image_ID(ID_out_name);

    for(jj = 0; jj < naxes[0]; jj++)
        for(ii = 0; ii < naxes[1]; ii++)
        {
            iis = jj;
            jjs = naxes[1] - ii - 1;
            /*	if ((iis>0)&&(jjs>0)&&(iis<naxes[0])&&(jjs<naxes[1])) */
            data.image[ID_out].array.F[jj * naxes[1] + ii] = data.image[ID].array.F[jjs *
                    naxes[0] + iis];
        }

    return(0);
}



int basic_rotate_int(
    const char *ID_name,
    const char *ID_out_name,
    long nbstep
)
{
    float angle;
    int i;
    imageID ID, ID_out;
    long ii, jj, iis, jjs;
    long naxes[2];

    ID = image_ID(ID_name);
    naxes[0] = data.image[ID].md[0].size[0];
    naxes[1] = data.image[ID].md[0].size[1];
    create_2Dimage_ID(ID_out_name, naxes[0], naxes[1]);
    ID_out = image_ID(ID_out_name);

    for(i = 0; i < nbstep; i++)
    {
        angle = M_PI * i / nbstep;
        for(jj = 0; jj < naxes[1]; jj++)
            for(ii = 0; ii < naxes[0]; ii++)
            {
                iis = (long)(naxes[0] / 2 + (ii - naxes[0] / 2) * cos(angle) +
                             (jj - naxes[1] / 2) * sin(angle));
                jjs = (long)(naxes[1] / 2 + (ii - naxes[0] / 2) * sin(angle) -
                             (jj - naxes[1] / 2) * cos(angle));
                if((iis > 0) && (jjs > 0) && (iis < naxes[0]) && (jjs < naxes[1]))
                {
                    data.image[ID_out].array.F[jj * naxes[0] + ii] += data.image[ID].array.F[jjs *
                            naxes[0] + iis];
                }
            }
    }

    return(0);
}



int basic_translate(
    const char *ID_name,
    const char *ID_out,
    float xtransl,
    float ytransl
)
{
    imageID ID;
    long naxes[2];
    long onaxes[2];
    long ii, jj;
    int n0, n1;
    float coeff;

    /*  printf("basic translate\n");*/
    ID = image_ID(ID_name);
    naxes[0] = data.image[ID].md[0].size[0];
    naxes[1] = data.image[ID].md[0].size[1];
    onaxes[0] = naxes[0];
    onaxes[1] = naxes[1];
    n0 = (int)((log10(naxes[0]) / log10(2)) + 0.01);
    n1 = (int)((log10(naxes[0]) / log10(2)) + 0.01);
    /*  printf("(test --- %ld %ld   %d %d   %d %d)\n",naxes[0],naxes[1],n0,n1,(int) pow(2,n0),(int) pow(2,n1));*/

    if((n0 == n1) && (naxes[0] == (int) pow(2, n0))
            && (naxes[1] == (int) pow(2, n1)))
    {
        create_2Dimage_ID("zero_tmp", naxes[0], naxes[1]);
        pupfft(ID_name, "zero_tmp", "out_transl_re_tmp", "out_transl_im_tmp", "-reim");
        delete_image_ID("zero_tmp");
        mk_amph_from_reim("out_transl_re_tmp", "out_transl_im_tmp",
                          "out_transl_ampl_tmp", "out_transl_pha_tmp", 0);
        delete_image_ID("out_transl_re_tmp");
        delete_image_ID("out_transl_im_tmp");

        ID = image_ID("out_transl_pha_tmp");
        for(jj = 1; jj < naxes[1]; jj++)
            for(ii = 1; ii < naxes[0]; ii++)
            {
                data.image[ID].array.F[jj * naxes[0] + ii] -= xtransl * 2.0 * M_PI /
                        naxes[0] * (ii - naxes[0] / 2) + ytransl * 2.0 * M_PI / naxes[1] *
                        (jj - naxes[1] / 2);
            }

        coeff = 1.0 / (naxes[0] * naxes[1]);
        ID = image_ID("out_transl_ampl_tmp");
        for(jj = 0; jj < naxes[1]; jj++)
            for(ii = 0; ii < naxes[0]; ii++)
            {
                data.image[ID].array.F[jj * naxes[0] + ii] *= coeff;
            }


        mk_reim_from_amph("out_transl_ampl_tmp", "out_transl_pha_tmp", "out_re_tmp",
                          "out_im_tmp", 0);
        delete_image_ID("out_transl_ampl_tmp");
        delete_image_ID("out_transl_pha_tmp");
        pupfft("out_re_tmp", "out_im_tmp", ID_out, "tbe_tmp", "-reim -inv");
        delete_image_ID("out_re_tmp");
        delete_image_ID("out_im_tmp");
        delete_image_ID("tbe_tmp");
    }
    else
    {
        basic_add(ID_name, ID_name, "tmp1t", naxes[0], 0);
        basic_add("tmp1t", "tmp1t", "tmp2t", 0, naxes[1]);
        delete_image_ID("tmp1t");
        basic_extract("tmp2t", "tmp3t", pow(2, n0 + 1), pow(2, n1 + 1), 0, 0);
        delete_image_ID("tmp2t");
        ID = image_ID("tmp3t");
        naxes[0] = data.image[ID].md[0].size[0];
        naxes[1] = data.image[ID].md[0].size[1];
        create_2Dimage_ID("zero_tmp", naxes[0], naxes[1]);

        pupfft("tmp3t", "zero_tmp", "out_transl_re_tmp", "out_transl_im_tmp", "-reim");
        delete_image_ID("zero_tmp");
        delete_image_ID("tmp3t");
        mk_amph_from_reim("out_transl_re_tmp", "out_transl_im_tmp",
                          "out_transl_ampl_tmp", "out_transl_pha_tmp", 0);
        delete_image_ID("out_transl_re_tmp");
        delete_image_ID("out_transl_im_tmp");

        ID = image_ID("out_transl_pha_tmp");
        for(jj = 1; jj < naxes[1]; jj++)
            for(ii = 1; ii < naxes[0]; ii++)
            {
                data.image[ID].array.F[jj * naxes[0] + ii] -= xtransl * 2.0 * M_PI /
                        naxes[0] * (ii - naxes[0] / 2) + ytransl * 2.0 * M_PI / naxes[1] *
                        (jj - naxes[1] / 2);
            }
        coeff = 1.0 / (naxes[0] * naxes[1]);
        ID = image_ID("out_transl_ampl_tmp");
        for(jj = 0; jj < naxes[1]; jj++)
            for(ii = 0; ii < naxes[0]; ii++)
            {
                data.image[ID].array.F[jj * naxes[0] + ii] *= coeff;
            }

        mk_reim_from_amph("out_transl_ampl_tmp", "out_transl_pha_tmp", "out_re_tmp",
                          "out_im_tmp", 0);
        delete_image_ID("out_transl_ampl_tmp");
        delete_image_ID("out_transl_pha_tmp");
        pupfft("out_re_tmp", "out_im_tmp", "outtmp", "tbe_tmp", "-reim -inv");
        delete_image_ID("out_re_tmp");
        delete_image_ID("out_im_tmp");
        delete_image_ID("tbe_tmp");

        basic_extract("outtmp", ID_out, onaxes[0], onaxes[1], 0, 0);
        delete_image_ID("outtmp");
    }

    return(0);
}




float basic_correlation(
    const char *ID_name1,
    const char *ID_name2
)
{
    float correl;
    imageID ID1, ID2;
    uint32_t naxes1[2];
    uint32_t naxes2[2];

    ID1 = image_ID(ID_name1);
    naxes1[0] = data.image[ID1].md[0].size[0];
    naxes1[1] = data.image[ID1].md[0].size[1];
    ID2 = image_ID(ID_name2);
    naxes2[0] = data.image[ID2].md[0].size[0];
    naxes2[1] = data.image[ID2].md[0].size[1];

    if((naxes1[0] != naxes2[0]) || (naxes1[1] != naxes2[1]))
    {
        printf("correlation : file size do not match\n");
        exit(1);
    }
    correl = 0;

    for(uint32_t jj = 0; jj < naxes1[1]; jj++)
        for(uint32_t ii = 0; ii < naxes1[0]; ii++)
        {
            correl += (data.image[ID1].array.F[jj * naxes1[0] + ii] -
                       data.image[ID2].array.F[jj * naxes1[0] + ii]) * (data.image[ID1].array.F[jj *
                               naxes1[0] + ii] - data.image[ID2].array.F[jj * naxes1[0] + ii]);
        }

    return(correl);
}





imageID IMAGE_BASIC_get_assym_component(
    const char *ID_name,
    const char *ID_out_name,
    float xcenter,
    float ycenter,
    const char *options
)
{
    float step = 1.0;
    imageID ID;
    uint32_t naxes[2];
    float distance;
    float *dist;
    float *mean;
    float *rms;
    long *counts;
    long i;
    long nb_step;
    imageID IDout;
    char input[50];
    int str_pos;
    float perc;
    float ifloat, x;

    printf("get non-circular symmetric component from image %s\n", ID_name);
    fflush(stdout);

    if(strstr(options, "-perc ") != NULL)
    {
        str_pos = strstr(options, "-perc ") - options;
        str_pos = str_pos + strlen("-perc ");
        i = 0;
        while((options[i + str_pos] != ' ') && (options[i + str_pos] != '\n')
                && (options[i + str_pos] != '\0'))
        {
            input[i] = options[i + str_pos];
            i++;
        }
        input[i] = '\0';
        perc = atof(input);
        printf("percentile is %f\n", perc);
    }

    ID = image_ID(ID_name);
    naxes[0] = data.image[ID].md[0].size[0];
    naxes[1] = data.image[ID].md[0].size[1];
    nb_step = naxes[0] / 2;

    dist = (float *) malloc(sizeof(float) * nb_step);
    if(dist == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    mean = (float *) malloc(sizeof(float) * nb_step);
    if(mean == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    rms = (float *) malloc(sizeof(float) * nb_step);
    if(rms == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    counts = (long *) malloc(sizeof(long) * nb_step);
    if(counts == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    for(i = 0; i < nb_step; i++)
    {
        dist[i] = 0;
        mean[i] = 0;
        rms[i] = 0;
        counts[i] = 0;
    }

    for(uint32_t jj = 0; jj < naxes[1]; jj++)
        for(uint32_t ii = 0; ii < naxes[0]; ii++)
        {
            distance = sqrt((1.0 * ii - xcenter) * (1.0 * ii - xcenter) +
                            (1.0 * jj - ycenter) * (1.0 * jj - ycenter));
            i = (long)(1.0 * distance / step + 0.5);
            if(i < nb_step)
            {
                dist[i] += distance;
                mean[i] += data.image[ID].array.F[jj * naxes[0] + ii];
                rms[i] += data.image[ID].array.F[jj * naxes[0] + ii] * data.image[ID].array.F[jj
                          * naxes[0] + ii];
                counts[i] += 1;
            }
        }

    for(i = 0; i < nb_step; i++)
    {
        dist[i] /= counts[i];
        mean[i] /= counts[i];
        rms[i] = sqrt(rms[i] - 1.0 * counts[i] * mean[i] * mean[i]) / sqrt(counts[i]);
    }

    printf("%u %u\n", naxes[0], naxes[1]);
    create_2Dimage_ID(ID_out_name, naxes[0], naxes[1]);
    IDout = image_ID(ID_out_name);
    for(uint32_t jj = 0; jj < naxes[1]; jj++)
        for(uint32_t ii = 0; ii < naxes[0]; ii++)
        {
            distance = sqrt((1.0 * ii - xcenter) * (1.0 * ii - xcenter) +
                            (1.0 * jj - ycenter) * (1.0 * jj - ycenter));
            i = (long)(1.0 * distance / step);
            ifloat = 1.0 * distance / step;
            x = ifloat - i;

            if((i + 1) < nb_step)
            {
                data.image[IDout].array.F[jj * naxes[0] + ii] = data.image[ID].array.F[jj *
                        naxes[0] + ii] - ((1.0 - x) * mean[i] + x * mean[i + 1]);
            }
        }

    free(counts);
    free(dist);
    free(mean);
    free(rms);

    return(IDout);
}



imageID IMAGE_BASIC_get_sym_component(
    const char *ID_name,
    const char *ID_out_name,
    float       xcenter,
    float       ycenter
)
{
    float step = 1.0;
    imageID ID;
    uint32_t naxes[2];
    float distance;
    float *dist;
    float *mean;
    float *rms;
    long *counts;
    long i;
    long nb_step;
    imageID IDout;
    float ifloat, x;

    ID = image_ID(ID_name);
    naxes[0] = data.image[ID].md[0].size[0];
    naxes[1] = data.image[ID].md[0].size[1];
    nb_step = naxes[0] / 2;

    dist = (float *) malloc(sizeof(float) * nb_step);
    if(dist == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    mean = (float *) malloc(sizeof(float) * nb_step);
    if(mean == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    rms = (float *) malloc(sizeof(float) * nb_step);
    if(rms == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    counts = (long *) malloc(sizeof(long) * nb_step);
    if(counts == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    for(i = 0; i < nb_step; i++)
    {
        dist[i] = 0;
        mean[i] = 0;
        rms[i] = 0;
        counts[i] = 0;
    }


    for(uint32_t jj = 0; jj < naxes[1]; jj++)
        for(uint32_t ii = 0; ii < naxes[0]; ii++)
        {
            distance = sqrt((1.0 * ii - xcenter) * (1.0 * ii - xcenter) +
                            (1.0 * jj - ycenter) * (1.0 * jj - ycenter));
            i = (long)(1.0 * distance / step + 0.5);
            if(i < nb_step)
            {
                dist[i] += distance;
                mean[i] += data.image[ID].array.F[jj * naxes[0] + ii];
                rms[i] += data.image[ID].array.F[jj * naxes[0] + ii] * data.image[ID].array.F[jj
                          * naxes[0] + ii];
                counts[i] += 1;
            }
        }

    for(i = 0; i < nb_step; i++)
    {
        dist[i] /= counts[i];
        mean[i] /= counts[i];
        rms[i] = sqrt(rms[i] - 1.0 * counts[i] * mean[i] * mean[i]) / sqrt(counts[i]);
    }

    printf("%u %u\n", naxes[0], naxes[1]);
    create_2Dimage_ID(ID_out_name, naxes[0], naxes[1]);
    IDout = image_ID(ID_out_name);
    for(uint32_t jj = 0; jj < naxes[1]; jj++)
        for(uint32_t ii = 0; ii < naxes[0]; ii++)
        {
            distance = sqrt((1.0 * ii - xcenter) * (1.0 * ii - xcenter) +
                            (1.0 * jj - ycenter) * (1.0 * jj - ycenter));
            i = (long)(1.0 * distance / step);
            ifloat = 1.0 * distance / step;
            x = ifloat - i;

            if((i + 1) < nb_step)
            {
                data.image[IDout].array.F[jj * naxes[0] + ii] = ((1.0 - x) * mean[i] + x *
                        mean[i + 1]);
            }
        }



    free(counts);
    free(dist);
    free(mean);
    free(rms);

    return(IDout);
}




/* rotation that keeps photometry - angle is in radians */
int basic_rotate2(
    const char *ID_name_in,
    const char *ID_name_out,
    float angle
)
{
    imageID ID_in;
    imageID ID_out, ID_wout;
    uint32_t naxes[2];
    uint32_t naxes2[2];
    uint64_t nelements;
    float rotangle;
    float *pixcorner_x;
    float *pixcorner_y;
    float x, y;
    uint32_t NB_step = 20;
    uint32_t i, j;
    float f1, f2, f3, f4, f5, f6, f7, f8, f9;
    /*  float *f1a;
        float *f2a;
        float *f3a;
        float *f4a;
        float *f5a;
        float *f6a;
        float *f7a;
        float *f8a;
        float *f9a;*/
    int *f1a;
    int *f2a;
    int *f3a;
    int *f4a;
    int *f5a;
    int *f6a;
    int *f7a;
    int *f8a;
    int *f9a;
    float pixcx, pixcy;
    float total;
    int xint, yint;
    float ccos, ssin;

    printf("rotating %s by %f radians ...\n", ID_name_in, angle);
    fflush(stdout);
    rotangle = angle;
    while(rotangle < 0)
    {
        rotangle += 2.0 * M_PI;
    }
    while(rotangle > 2 * M_PI)
    {
        rotangle -= 2.0 * M_PI;
    }
    /* now the angle is between 0 and 2*PI */
    while(rotangle > (M_PI / 2))
    {
        basic_rotate90(ID_name_in, "tmprot");
        delete_image_ID(ID_name_in);
        copy_image_ID("tmprot", ID_name_in, 0);
        delete_image_ID("tmprot");
        rotangle -= M_PI / 2.0;
    }

    /* now the angle is between 0 and PI/2 */

    ID_in = image_ID(ID_name_in);
    naxes[0] = data.image[ID_in].md[0].size[0];
    naxes[1] = data.image[ID_in].md[0].size[1];
    nelements = naxes[0] * naxes[1];
    printf("creating temporary arrays\n");
    fflush(stdout);
    f1a = (int *) calloc(NB_step * NB_step, sizeof(int));
    f2a = (int *) calloc(NB_step * NB_step, sizeof(int));
    f3a = (int *) calloc(NB_step * NB_step, sizeof(int));
    f4a = (int *) calloc(NB_step * NB_step, sizeof(int));
    f5a = (int *) calloc(NB_step * NB_step, sizeof(int));
    f6a = (int *) calloc(NB_step * NB_step, sizeof(int));
    f7a = (int *) calloc(NB_step * NB_step, sizeof(int));
    f8a = (int *) calloc(NB_step * NB_step, sizeof(int));
    f9a = (int *) calloc(NB_step * NB_step, sizeof(int));

    printf("filling up calibration array ... ");
    fflush(stdout);
    total = 1.0 / NB_step / NB_step;
    ccos = cos(rotangle);
    ssin = sin(rotangle);

    for(uint32_t ii = 0; ii < NB_step; ii++)
        for(uint32_t jj = 0; jj < NB_step; jj++)
        {
            pixcx = 1.0 * ii / NB_step;
            pixcy = 1.0 * jj / NB_step;

            for(i = 0; i < NB_step; i++)
                for(j = 0; j < NB_step; j++)
                {
                    x = pixcx + 1.0 * (0.5 + i) / NB_step * ccos - 1.0 * (0.5 + j) / NB_step * ssin;
                    y = pixcy + 1.0 * (0.5 + i) / NB_step * ssin + 1.0 * (0.5 + j) / NB_step * ccos;
                    if(x < 0)
                    {
                        if(y < 1)
                        {
                            f1a[jj * NB_step + ii]++;
                        }
                        else
                        {
                            if(y > 2)
                            {
                                f7a[jj * NB_step + ii]++;
                            }
                            else
                            {
                                f4a[jj * NB_step + ii]++;
                            }
                        }
                    }
                    else
                    {
                        if(x > 1)
                        {
                            if(y < 1)
                            {
                                f3a[jj * NB_step + ii]++;
                            }
                            else
                            {
                                if(y > 2)
                                {
                                    f9a[jj * NB_step + ii]++;
                                }
                                else
                                {
                                    f6a[jj * NB_step + ii]++;
                                }
                            }
                        }
                        else
                        {
                            if(y < 1)
                            {
                                f2a[jj * NB_step + ii]++;
                            }
                            else
                            {
                                if(y > 2)
                                {
                                    f8a[jj * NB_step + ii]++;
                                }
                                else
                                {
                                    f5a[jj * NB_step + ii]++;
                                }
                            }
                        }
                    }
                }
        }
    printf("done\n");
    fflush(stdout);

    pixcorner_x = (float *) malloc(sizeof(float) * nelements);
    if(pixcorner_x == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    pixcorner_y = (float *) malloc(sizeof(float) * nelements);
    if(pixcorner_y == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    for(uint32_t jj = 0; jj < naxes[1]; jj++)
        for(uint32_t ii = 0; ii < naxes[0]; ii++)
        {
            pixcorner_x[jj * naxes[0] + ii] = ii * ccos - jj * ssin + ssin * naxes[1] + 1.0;
            pixcorner_y[jj * naxes[0] + ii] = ii * ssin + jj * ccos;
        }

    naxes2[0] = (long)(sin(rotangle) * naxes[1] + cos(rotangle) * naxes[0] + 2.0);
    naxes2[1] = (long)(cos(rotangle) * naxes[1] + sin(rotangle) * naxes[0] + 2.0);

    create_2Dimage_ID(ID_name_out, naxes2[0], naxes2[1]);
    create_2Dimage_ID("wtmp", naxes2[0], naxes2[1]);
    ID_out = image_ID(ID_name_out);
    ID_wout = image_ID("wtmp");

    for(uint32_t jj = 0; jj < naxes[1]; jj++)
        for(uint32_t ii = 0; ii < naxes[0]; ii++)
        {
            x = pixcorner_x[jj * naxes[0] + ii];
            y = pixcorner_y[jj * naxes[0] + ii];
            /*printf("%ld %ld %d %d %f %f %f %f\n",ii,jj,i,j,pixcorner_x[jj*naxes[0]+ii],pixcorner_y[jj*naxes[0]+ii],x,y);*/
            xint = (int)(((x + 3 * naxes[0]) - (int)(x + 3 * naxes[0])) * NB_step);
            yint = (int)((y - (int) y) * NB_step);
            /*	printf("%d %d\n",xint,yint);*/
            f1 = 1.0 * f1a[yint * NB_step + xint];
            f2 = 1.0 * f2a[yint * NB_step + xint];
            f3 = 1.0 * f3a[yint * NB_step + xint];
            f4 = 1.0 * f4a[yint * NB_step + xint];
            f5 = 1.0 * f5a[yint * NB_step + xint];
            f6 = 1.0 * f6a[yint * NB_step + xint];
            f7 = 1.0 * f7a[yint * NB_step + xint];
            f8 = 1.0 * f8a[yint * NB_step + xint];
            f9 = 1.0 * f9a[yint * NB_step + xint];

            data.image[ID_out].array.F[((long) y)*naxes2[0] + ((long)(
                                           x)) - 1] += f1 * data.image[ID_in].array.F[jj * naxes[0] + ii];
            data.image[ID_out].array.F[((long) y)*naxes2[0] + ((long)(
                                           x))] += f2 * data.image[ID_in].array.F[jj * naxes[0] + ii];
            data.image[ID_out].array.F[((long) y)*naxes2[0] + ((long)(
                                           x)) + 1] += f3 * data.image[ID_in].array.F[jj * naxes[0] + ii];

            data.image[ID_out].array.F[((long) y + 1)*naxes2[0] + ((long)(
                                           x)) - 1] += f4 * data.image[ID_in].array.F[jj * naxes[0] + ii];
            data.image[ID_out].array.F[((long) y + 1)*naxes2[0] + ((long)(
                                           x))] += f5 * data.image[ID_in].array.F[jj * naxes[0] + ii];
            data.image[ID_out].array.F[((long) y + 1)*naxes2[0] + ((long)(
                                           x)) + 1] += f6 * data.image[ID_in].array.F[jj * naxes[0] + ii];

            data.image[ID_out].array.F[((long) y + 2)*naxes2[0] + ((long)(
                                           x)) - 1] += f7 * data.image[ID_in].array.F[jj * naxes[0] + ii];
            data.image[ID_out].array.F[((long) y + 2)*naxes2[0] + ((long)(
                                           x))] += f8 * data.image[ID_in].array.F[jj * naxes[0] + ii];
            data.image[ID_out].array.F[((long) y + 2)*naxes2[0] + ((long)(
                                           x)) + 1] += f9 * data.image[ID_in].array.F[jj * naxes[0] + ii];

            data.image[ID_wout].array.F[((long) y)*naxes2[0] + ((long)(x)) - 1] += f1;
            data.image[ID_wout].array.F[((long) y)*naxes2[0] + ((long)(x))] += f2;
            data.image[ID_wout].array.F[((long) y)*naxes2[0] + ((long)(x)) + 1] += f3;

            data.image[ID_wout].array.F[((long) y + 1)*naxes2[0] + ((long)(x)) - 1] += f4;
            data.image[ID_wout].array.F[((long) y + 1)*naxes2[0] + ((long)(x))] += f5;
            data.image[ID_wout].array.F[((long) y + 1)*naxes2[0] + ((long)(x)) + 1] += f6;

            data.image[ID_wout].array.F[((long) y + 2)*naxes2[0] + ((long)(x)) - 1] += f7;
            data.image[ID_wout].array.F[((long) y + 2)*naxes2[0] + ((long)(x))] += f8;
            data.image[ID_wout].array.F[((long) y + 2)*naxes2[0] + ((long)(x)) + 1] += f9;

        }

    for(uint32_t jj = 0; jj < naxes2[1]; jj++)
        for(uint32_t ii = 0; ii < naxes2[0]; ii++)
        {
            if(data.image[ID_wout].array.F[jj * naxes2[0] + ii] > (0.9 * NB_step * NB_step))
            {
                data.image[ID_out].array.F[jj * naxes2[0] + ii] /=
                    data.image[ID_wout].array.F[jj * naxes2[0] + ii];
            }
            else
            {
                data.image[ID_out].array.F[jj * naxes2[0] + ii] *= total;
            }
        }

    delete_image_ID("wtmp");

    free(pixcorner_x);
    free(pixcorner_y);

    free(f1a);
    free(f2a);
    free(f3a);
    free(f4a);
    free(f5a);
    free(f6a);
    free(f7a);
    free(f8a);
    free(f9a);

    printf("done\n");
    fflush(stdout);

    return(0);
}



/* this routine uses the Owen algorith - the image has to be square and power of 2 on the side */
int basic_rotate3(
    const char *ID_name_in,
    const char *ID_name_out,
    float angle
)
{
    long Np, Npp;
    float delta;
    long N;
    imageID ID;
    /* IDout; */
    long i, j;
    float tmp;
    imageID IDre, IDim, IDfre, IDfim;

    ID = image_ID(ID_name_in);
    N = data.image[ID].md[0].size[0];

    printf("%s\n", ID_name_out);

    Np = 2 * N;
    Npp = 2 * N;

    /* step 1 : padding of the image */
    create_2Dimage_ID("im_im", Np, Npp / 2);
    basic_add("im_im", ID_name_in, "im_re", 0, 0);
    IDre = image_ID("im_re");
    IDim = image_ID("im_im");

    /* step 2 & 3 : FFT on all rows and phase-shifting */
    for(i = 0; i < Npp / 2; i++)
    {
        basic_extract("im_re", "rowre", Np, 1, 0, i);
        basic_extract("im_im", "rowim", Np, 1, 0, i);
        pupfft("rowre", "rowim", "rowfre", "rowfim", "-reim");
        /*      permut("rowfre");
          permut("rowfim");*/
        delete_image_ID("rowre");
        delete_image_ID("rowim");
        IDfre = image_ID("rowfre");
        IDfim = image_ID("rowfim");
        delta = -tan(angle / 2) * i;
        for(j = 0; j < Np; j++)
        {
            tmp = 2.0 * M_PI * j * delta / Np;
            data.image[IDre].array.F[i * Np + j] = (float)(data.image[IDfre].array.F[j] *
                                                   cos(tmp) - data.image[IDfim].array.F[j] * sin(tmp));
            data.image[IDim].array.F[i * Np + j] = (float)(data.image[IDfre].array.F[j] *
                                                   sin(tmp) + data.image[IDfim].array.F[j] * cos(tmp));
        }
        delete_image_ID("rowfre");
        delete_image_ID("rowfim");
    }

    /* step 4 : FFT on columns */
    printf("Step 4\n");
    for(i = 0; i < Np; i++)
    {
        basic_extract("im_re", "colre", 1, Npp / 2, i, 0);
        basic_extract("im_im", "colim", 1, Npp / 2, i, 0);

        pupfft("colre", "colim", "colfre", "colfim", "-reim");
        /*permut("colfre");
        permut("colfim");*/
        delete_image_ID("colre");
        delete_image_ID("colim");
        IDfre = image_ID("colfre");
        IDfim = image_ID("colfim");

        fflush(stdout);
        for(j = 0; j < Npp / 2; j++)
        {
            data.image[IDre].array.F[j * Np + i] = data.image[IDfre].array.F[j];
            data.image[IDim].array.F[j * Np + i] = data.image[IDfim].array.F[j];
        }
        delete_image_ID("colfre");
        delete_image_ID("colfim");
    }
    printf("Step 4 done\n");
    fflush(stdout);

    /* step 5 : Doubling image vertically */
    printf("Step 5\n");
    create_2Dimage_ID("nre", Np, Npp);
    create_2Dimage_ID("nim", Np, Npp);
    IDre = image_ID("nre");
    IDim = image_ID("nim");
    for(i = 0; i < Npp / 2; i++)
    {
        for(j = 0; j < Np; j++)
        {

        }
    }
    return(0);
}

int basic_stretch(const char *name_in, const char *name_out, float coeff,
                  long Xcenter, long Ycenter)
{

    long ii, jj;
    long naxes[2];
    long ID_in, ID_out;
    long i, j;

    ID_in = image_ID(name_in);
    naxes[0] = data.image[ID_in].md[0].size[0];
    naxes[1] = data.image[ID_in].md[0].size[1];

    ID_out = create_2Dimage_ID(name_out, naxes[0], naxes[1]);

    for(ii = 0; ii < naxes[0]; ii++)
        for(jj = 0; jj < naxes[0]; jj++)
        {
            i = Xcenter + (long)(1.0 * (ii - Xcenter) * coeff);
            j = Ycenter + (long)(1.0 * (jj - Ycenter) * coeff);
            if((i < naxes[0]) && (j < naxes[1]) && (i > -1) && (j > -1))
            {
                data.image[ID_out].array.F[jj * naxes[0] + ii] += data.image[ID_in].array.F[j *
                        naxes[0] + i] / coeff / coeff;
            }
        }

    arith_image_cstmult_inplace(name_out,
                                arith_image_total(name_in) / arith_image_total(name_out));

    return(0);
}




int basic_stretch_range(
    const char *name_in,
    const char *name_out,
    float       coeff1,
    float       coeff2,
    long        Xcenter,
    long        Ycenter,
    long        NBstep,
    float       ApoCoeff
)
{
    // ApoCoeff should be between 0 and 1
    uint32_t naxes[2];
    imageID ID_in, ID_out;
    long i, j;
    float coeff;
    long step;
    float mcoeff;
    float x, y;
    float eps = 1.0e-5;
    float u, t, tmp;


    ID_in = image_ID(name_in);
    naxes[0] = data.image[ID_in].md[0].size[0];
    naxes[1] = data.image[ID_in].md[0].size[1];

    ID_out = create_2Dimage_ID(name_out, naxes[0], naxes[1]);



    for(step = 0; step < NBstep; step ++)
    {
        fprintf(stdout, ".");
        fflush(stdout);
        coeff = coeff1 + (coeff2 - coeff1) * (1.0 * step / (NBstep - 1));
        x = (coeff - (coeff1 + coeff2) / 2.0) / ((coeff2 - coeff1) / 2.0);
        // x goes from -1 to 1
        if(ApoCoeff > eps)
        {
            mcoeff = pow((1.0 - pow((fabs(x) - (1.0 - ApoCoeff)) / ApoCoeff, 2.0)), 4.0);
        }
        else
        {
            mcoeff = 1.0;
        }

        if((1.0 - x * x) < eps)
        {
            mcoeff = 0.0;
        }
        if(fabs(x) < ApoCoeff)
        {
            mcoeff = 1.0;
        }
        //      fprintf(stdout,"(%f %f %f %f %f)",coeff,coeff1,coeff2,x,mcoeff);

        for(uint32_t ii = 0; ii < naxes[0]; ii++)
            for(uint32_t jj = 0; jj < naxes[1]; jj++)
            {
                x = (1.0 * (ii - Xcenter) * coeff) + Xcenter;
                y = (1.0 * (jj - Ycenter) * coeff) + Ycenter;
                i = (long) x;
                j = (long) y;
                u = x - i;
                t = y - j;
                if((i < naxes[0] - 1) && (j < naxes[1] - 1) && (i > -1) && (j > -1))
                {
                    tmp = (1.0 - u) * (1.0 - t) * data.image[ID_in].array.F[j * naxes[0] + i];
                    tmp += (1.0 - u) * t * data.image[ID_in].array.F[(j + 1) * naxes[0] + i];
                    tmp += u * (1.0 - t) * data.image[ID_in].array.F[j * naxes[0] + i + 1];
                    tmp += u * t * data.image[ID_in].array.F[(j + 1) * naxes[0] + i + 1];
                    data.image[ID_out].array.F[jj * naxes[0] + ii] += mcoeff * tmp / coeff / coeff;
                }
            }
    }

    fprintf(stdout, "\n");
    arith_image_cstmult_inplace(name_out,
                                arith_image_total(name_in) / arith_image_total(name_out));

    return(0);
}



int basic_stretchc(
    const char *name_in,
    const char *name_out,
    float coeff
)
{

    uint32_t naxes[2];
    imageID ID_in, ID_out;
    long i, j;
    long Xcenter, Ycenter;

    ID_in = image_ID(name_in);
    naxes[0] = data.image[ID_in].md[0].size[0];
    naxes[1] = data.image[ID_in].md[0].size[1];
    Xcenter = naxes[0] / 2;
    Ycenter = naxes[1] / 2;

    ID_out = create_2Dimage_ID(name_out, naxes[0], naxes[1]);

    for(uint32_t ii = 0; ii < naxes[0]; ii++)
        for(uint32_t jj = 0; jj < naxes[0]; jj++)
        {
            i = Xcenter + (long)(1.0 * (ii - Xcenter) * coeff);
            j = Ycenter + (long)(1.0 * (jj - Ycenter) * coeff);
            if((i < naxes[0]) && (j < naxes[1]) && (i > -1) && (j > -1))
            {
                data.image[ID_out].array.F[jj * naxes[0] + ii] += data.image[ID_in].array.F[j *
                        naxes[0] + i] / coeff / coeff;
            }
        }

    /*  basic_mult(name_out,arith_image_total(name_in)/arith_image_total(name_out));*/

    return(0);
}





int gauss_histo_image(
    const char *ID_name,
    const char *ID_out_name,
    float sigma,
    float center
)
{
    imageID ID, ID_out;
    uint32_t naxes[2];
    long k, k1;
    float x;
    long N = 100000;
    float *histo = NULL;
    float *imp = NULL;
    float *impr = NULL;
    float *imprinv = NULL;

    ID = image_ID(ID_name);
    naxes[0] = data.image[ID].md[0].size[0];
    naxes[1] = data.image[ID].md[0].size[1];

    histo = (float *) malloc(sizeof(float) * N);
    if(histo == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    imp = (float *) malloc(sizeof(float) * N);
    if(imp == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    impr = (float *) malloc(sizeof(float) * N);
    if(impr == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    imprinv = (float *) malloc(sizeof(float) * N);
    if(imprinv == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    for(uint32_t ii = 0; ii < N; ii++)
    {
        histo[ii] = 0.0;
        imp[ii] = 0.0;
    }

    for(uint32_t  jj = 0; jj < naxes[1]; jj++)
        for(uint32_t  ii = 0; ii < naxes[0]; ii++)
        {
            k = (long)(data.image[ID].array.F[jj * naxes[0] + ii] * N);
            if(k < 0)
            {
                k = 0;
            }
            if(k > N - 1)
            {
                k = N - 1;
            }
            histo[k]++;
        }
    for(k = 0; k < N; k++)
    {
        histo[k] *= 1.0 / naxes[1] / naxes[0];
    }

    imp[0] = histo[0];
    for(k = 1; k < N; k++)
    {
        imp[k] = imp[k - 1] + histo[k];
    }
    for(k = 0; k < N; k++)
    {
        imp[k] /= imp[N - 1];
    }


    printf("SIGMA = %f, CENTER = %f\n", sigma, center);

    for(uint32_t  ii = 0; ii < N; ii++)
    {
        x = 2.0 * (1.0 * ii / N - center);
        histo[ii] = exp(-(x * x) / sigma / sigma);
        impr[ii] = 0.0;
    }
    impr[0] = histo[0];
    for(k = 1; k < N; k++)
    {
        impr[k] = impr[k - 1] + histo[k];
    }
    for(k = 0; k < N; k++)
    {
        impr[k] /= impr[N - 1];
    }

    k = 0;
    for(k1 = 0; k1 < N; k1++)
    {
        x = 1.0 * k1 / N;
        while(impr[k] < x)
        {
            k++;
        }
        if(k > N - 1)
        {
            k = N - 1;
        }
        imprinv[k1] = 1.0 * k / N;
    }

    ID_out = create_2Dimage_ID(ID_out_name, naxes[0], naxes[1]);
    for(uint32_t jj = 0; jj < naxes[1]; jj++)
        for(uint32_t ii = 0; ii < naxes[0]; ii++)
        {
            k1 = (long)(data.image[ID].array.F[jj * naxes[0] + ii] * N);
            if(k1 < 0)
            {
                k1 = 0;
            }
            if(k1 > N - 1)
            {
                k1 = N - 1;
            }
            k = (long)(imp[k1] * N);
            if(k < 0)
            {
                k = 0;
            }
            if(k > N - 1)
            {
                k = N - 1;
            }
            data.image[ID_out].array.F[jj * naxes[0] + ii] = imprinv[k];
        }

    free(histo);
    free(imp);
    free(impr);
    free(imprinv);

    return(0);
}





// load all images matching strfilter + .fits
// return number of images loaded
// image name in buffer is same as file name without extension
long load_fitsimages(
    const char *strfilter
)
{
    long cnt = 0;
    char fname[STRINGMAXLEN_FILENAME];
    char fname1[STRINGMAXLEN_FILENAME];
    FILE *fp;

	EXECUTE_SYSTEM_COMMAND("ls %s.fits > flist.tmp\n", strfilter);


    if((fp = fopen("flist.tmp", "r")) == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("fopen() error");
        exit(0);
    }

    while(fgets(fname, STRINGMAXLEN_FILENAME, fp) != NULL)
    {
        fname[strlen(fname) - 1] = '\0';
        strncpy(fname1, fname, STRINGMAXLEN_FILENAME);
        fname1[strlen(fname) - 5] = '\0';
        load_fits(fname, fname1, 1);
        printf("[%ld] Image %s loaded -> %s\n", cnt, fname, fname1);
        fflush(stdout);
        cnt++;
    }

    fclose(fp);
    
    EXECUTE_SYSTEM_COMMAND("rm flist.tmp");

    printf("%ld images loaded\n", cnt);

    return(cnt);
}





// load all images matching strfilter + .fits into a data cube
// return number of images loaded
// image name in buffer is same as file name without extension
long load_fitsimages_cube(
    const char *strfilter,
    const char *ID_out_name
)
{
    long cnt = 0;
    char fname[STRINGMAXLEN_FILENAME];
    char fname1[STRINGMAXLEN_FILENAME];
    FILE *fp;
    uint32_t xsize, ysize;
    imageID ID;
    imageID IDout;

    printf("Filter = %s\n", strfilter);

	EXECUTE_SYSTEM_COMMAND("ls %s > flist.tmp\n", strfilter);

    xsize = 0;
    ysize = 0;

    if((fp = fopen("flist.tmp", "r")) == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("fopen() error");
        exit(0);
    }

    while(fgets(fname, STRINGMAXLEN_FILENAME, fp) != NULL)
    {
        fname[strlen(fname) - 1] = '\0';
        if(cnt == 0)
        {
            ID = load_fits(fname, "imtmplfc", 1);
            xsize = data.image[ID].md[0].size[0];
            ysize = data.image[ID].md[0].size[1];
            delete_image_ID("imtmplfc");
        }

        ID = load_fits(fname, "imtmplfc", 1);
        if((data.image[ID].md[0].size[0] != xsize)
                || (data.image[ID].md[0].size[1] != ysize))
        {
            fprintf(stderr,
                    "ERROR in load_fitsimages_cube: not all images have the same size\n");
            exit(0);
        }
        delete_image_ID("imtmplfc");
        cnt++;
    }
    fclose(fp);

    printf("Creating 3D cube ... ");
    fflush(stdout);
    IDout = create_3Dimage_ID(ID_out_name, xsize, ysize, cnt);
    printf("\n");
    fflush(stdout);

    cnt = 0;
    if((fp = fopen("flist.tmp", "r")) == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("fopen() error");
        exit(0);
    }


    while(fgets(fname, STRINGMAXLEN_FILENAME, fp) != NULL)
    {
        fname[strlen(fname) - 1] = '\0';
        strncpy(fname1, fname, STRINGMAXLEN_FILENAME);
        fname1[strlen(fname) - 5] = '\0';
        load_fits(fname, fname1, 1);
        printf("Image %s loaded -> %s\n", fname, fname1);
        ID = image_ID(fname1);
        for(uint64_t ii = 0; ii < xsize * ysize; ii++)
        {
            data.image[IDout].array.F[xsize * ysize * cnt + ii] =
                data.image[ID].array.F[ii];
        }
        delete_image_ID(fname1);
        cnt++;
    }

    fclose(fp);
    /*  n = snprintf(command,SBUFFERSIZE,"rm flist.tmp");
      if(n >= SBUFFERSIZE)
          PRINT_ERROR("Attempted to write string buffer with too many characters");

      if(system(command)==-1)
      {
          printf("WARNING: system(\"%s\") failed [function: %s  file: %s  line: %d ]\n",command,__func__,__FILE__,__LINE__);
          //exit(0);
      }

    */
    printf("%ld images loaded into cube %s\n", cnt, ID_out_name);

    return(cnt);
}





// recenter cube frames such that the photocenter is on the central pixel
// images are recentered by integer number of pixels
imageID basic_cube_center(
    const char *ID_in_name,
    const char *ID_out_name
)
{
    imageID IDin, IDout;
    long xsize, ysize, ksize;
    long ii, jj, kk, ii1, jj1;
    double tot, totii, totjj;
    long index0, index1, index;
    double v;
    long *tx = NULL;
    long *ty = NULL;

    IDin = image_ID(ID_in_name);
    xsize = data.image[IDin].md[0].size[0];
    ysize = data.image[IDin].md[0].size[1];
    ksize = data.image[IDin].md[0].size[2];

    tx = (long *) malloc(sizeof(long) * ksize);
    if(tx == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    ty = (long *) malloc(sizeof(long) * ksize);
    if(ty == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }


    IDout = create_3Dimage_ID(ID_out_name, xsize, ysize, ksize);

    for(kk = 0; kk < ksize; kk++)
    {
        tot = 0.0;
        totii = 0.0;
        totjj = 0.0;
        index0 = kk * xsize * ysize;

        for(jj = 0; jj < ysize; jj++)
        {
            index1 = index0 + jj * xsize;
            for(ii = 0; ii < xsize; ii++)
            {
                index = index1 + ii;
                v = data.image[IDin].array.F[index];
                totii += v * ii;
                totjj += v * jj;
                tot += v;
            }
        }
        totii /= tot;
        totjj /= tot;
        tx[kk] = ((long)(totii + 0.5)) - xsize / 2;
        ty[kk] = ((long)(totjj + 0.5)) - ysize / 2;

        for(ii = 0; ii < xsize; ii++)
            for(jj = 0; jj < ysize; jj++)
            {
                ii1 = ii + tx[kk];
                jj1 = jj + ty[kk];
                if((ii1 > -1) && (ii1 < xsize) && (jj1 > -1) && (jj1 < ysize))
                {
                    data.image[IDout].array.F[index0 + jj * xsize + ii] =
                        data.image[IDin].array.F[index0 + jj1 * xsize + ii1];
                }
                else
                {
                    data.image[IDout].array.F[index0 + jj * xsize + ii] = 0.0;
                }
            }
    }

    free(tx);
    free(ty);

    return IDout;
}




//
// average frames in a cube
// excludes point which are more than alpha x sigma
// writes an rms value frame as rmsim
//
imageID cube_average(
    const char *ID_in_name,
    const char *ID_out_name,
    float alpha
)
{
    imageID IDin, IDout, IDrms;
    long xsize, ysize, ksize;
    long ii, kk;
    double *array = NULL;
    double ave, ave1, rms;
    long cnt;
    long cnt1;

    IDin = image_ID(ID_in_name);
    xsize = data.image[IDin].md[0].size[0];
    ysize = data.image[IDin].md[0].size[1];
    ksize = data.image[IDin].md[0].size[2];

    IDout = create_2Dimage_ID(ID_out_name, xsize, ysize);
    IDrms = create_2Dimage_ID("rmsim", xsize, ysize);

    array = (double *) malloc(sizeof(double) * ksize);
    if(array == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    cnt1 = 0;
    for(ii = 0; ii < xsize * ysize; ii++)
    {
        for(kk = 0; kk < ksize; kk++)
        {
            array[kk] = (double) data.image[IDin].array.F[kk * xsize * ysize + ii];
        }

        ave = 0.0;
        for(kk = 0; kk < ksize; kk++)
        {
            ave += array[kk];
        }
        ave /= ksize;

        rms = 0.0;
        for(kk = 0; kk < ksize; kk++)
        {
            rms += (array[kk] - ave) * (array[kk] - ave);
        }
        rms = sqrt(rms / ksize);

        data.image[IDrms].array.F[ii] = (float) rms;

        ave1 = 0.0;
        cnt = 0;
        for(kk = 0; kk < ksize; kk++)
        {
            if(fabs(array[kk] - ave) < alpha * rms)
            {
                ave1 += array[kk];
                cnt ++;
            }
        }
        if(cnt > 0.5)
        {
            data.image[IDout].array.F[ii] = (float)(ave1 / cnt);
        }
        else
        {
            data.image[IDout].array.F[ii] = (float) ave;
        }
        cnt1 += cnt;
    }

    free(array);

    printf("(alpha = %f) fraction of pixel values selected = %ld/%ld = %.20g\n",
           alpha, cnt1, xsize * ysize * ksize,
           (double)(1.0 * cnt1 / (xsize * ysize * ksize)));
    printf("RMS written into image rmsim\n");

    return(IDout);
}





imageID cube_collapse(
    const char *ID_in_name,
    const char *ID_out_name
)
{
    imageID IDin, IDout;
    long xsize, ysize, ksize;
    long ii, kk;

    IDin = image_ID(ID_in_name);
    xsize = data.image[IDin].md[0].size[0];
    ysize = data.image[IDin].md[0].size[1];
    ksize = data.image[IDin].md[0].size[2];

    IDout = create_2Dimage_ID(ID_out_name, xsize, ysize);

    for(ii = 0; ii < xsize * ysize; ii++)
    {
        for(kk = 0; kk < ksize; kk++)
        {
            data.image[IDout].array.F[ii] += data.image[IDin].array.F[kk * xsize * ysize +
                                             ii];
        }
    }

    return(IDout);
}




imageID image_basic_indexmap(
    char *ID_index_name,
    char *ID_values_name,
    char *IDout_name
)
{
    imageID IDindex, IDvalues;
    imageID IDout;
    long xsize, ysize, xysize;
    long val_xsize, val_ysize, val_xysize;
    uint8_t datatype;
    uint8_t val_datatype;
    float *arrayf;
    long ii, i;

    IDindex = image_ID(ID_index_name);
    IDvalues = image_ID(ID_values_name);

    xsize = data.image[IDindex].md[0].size[0];
    ysize = data.image[IDindex].md[0].size[1];
    xysize = xsize * ysize;
    datatype = data.image[IDindex].md[0].datatype;

    val_xsize = data.image[IDvalues].md[0].size[0];
    val_ysize = data.image[IDvalues].md[0].size[1];
    val_xysize = val_xsize * val_ysize;
    val_datatype = data.image[IDindex].md[0].datatype;

    IDout = create_2Dimage_ID(IDout_name, xsize, ysize);

    if(val_datatype == _DATATYPE_FLOAT)
    {
        arrayf = data.image[IDvalues].array.F;
    }
    else
    {
        arrayf = (float *) malloc(sizeof(float) * val_xysize);
        for(i = 0; i < val_xysize; i++)
        {
            arrayf[i] = (float) data.image[IDvalues].array.D[i];
        }
    }

    switch(datatype)
    {

        case _DATATYPE_FLOAT:
            for(ii = 0; ii < xysize; ii++)
            {
                i = (long)(data.image[IDindex].array.F[ii] + 0.1);
                if((i > -1) && (i < val_xysize))
                {
                    data.image[IDout].array.F[ii] = arrayf[i];
                }
            }
            break;

        case _DATATYPE_DOUBLE:
            for(ii = 0; ii < xysize; ii++)
            {
                i = (long)(data.image[IDindex].array.D[ii] + 0.1);
                if((i > -1) && (i < val_xysize))
                {
                    data.image[IDout].array.F[ii] = arrayf[i];
                }
            }
            break;



        case _DATATYPE_UINT8:
            for(ii = 0; ii < xysize; ii++)
            {
                i = (long) data.image[IDindex].array.UI8[ii];
                if((i > -1) && (i < val_xysize))
                {
                    data.image[IDout].array.F[ii] = arrayf[i];
                }
            }
            break;

        case _DATATYPE_INT8:
            for(ii = 0; ii < xysize; ii++)
            {
                i = (long) data.image[IDindex].array.SI8[ii];
                if((i > -1) && (i < val_xysize))
                {
                    data.image[IDout].array.F[ii] = arrayf[i];
                }
            }
            break;

        case _DATATYPE_UINT16:
            for(ii = 0; ii < xysize; ii++)
            {
                i = (long) data.image[IDindex].array.UI16[ii];
                if((i > -1) && (i < val_xysize))
                {
                    data.image[IDout].array.F[ii] = arrayf[i];
                }
            }
            break;

        case _DATATYPE_INT16:
            for(ii = 0; ii < xysize; ii++)
            {
                i = (long) data.image[IDindex].array.SI16[ii];
                if((i > -1) && (i < val_xysize))
                {
                    data.image[IDout].array.F[ii] = arrayf[i];
                }
            }
            break;

        case _DATATYPE_UINT32:
            for(ii = 0; ii < xysize; ii++)
            {
                i = (long) data.image[IDindex].array.UI32[ii];
                if((i > -1) && (i < val_xysize))
                {
                    data.image[IDout].array.F[ii] = arrayf[i];
                }
            }
            break;

        case _DATATYPE_INT32:
            for(ii = 0; ii < xysize; ii++)
            {
                i = (long) data.image[IDindex].array.SI32[ii];
                if((i > -1) && (i < val_xysize))
                {
                    data.image[IDout].array.F[ii] = arrayf[i];
                }
            }
            break;

        case _DATATYPE_UINT64:
            for(ii = 0; ii < xysize; ii++)
            {
                i = (long) data.image[IDindex].array.UI64[ii];
                if((i > -1) && (i < val_xysize))
                {
                    data.image[IDout].array.F[ii] = arrayf[i];
                }
            }
            break;

        case _DATATYPE_INT64:
            for(ii = 0; ii < xysize; ii++)
            {
                i = (long) data.image[IDindex].array.SI64[ii];
                if((i > -1) && (i < val_xysize))
                {
                    data.image[IDout].array.F[ii] = arrayf[i];
                }
            }
            break;

        default:
            printf("ERROR: datatype not supported\n");
            return EXIT_FAILURE;
            break;
    }



    if(val_datatype != _DATATYPE_FLOAT)
    {
        free(arrayf);
    }

    return(IDout);
}





// coadd all images matching strfilter + .fits
// return number of images added
long basic_addimagesfiles(
    const char *strfilter,
    const char *outname
)
{
    long cnt = 0;
    char fname[STRINGMAXLEN_FILENAME];
    char fname1[STRINGMAXLEN_FILENAME];
    FILE *fp;
    imageID ID;
    int init = 0; // becomes 1 when first image encountered

	EXECUTE_SYSTEM_COMMAND("ls %s.fits > flist.tmp\n", strfilter);
  

    if((fp = fopen("flist.tmp", "r")) == NULL)
    {
        PRINT_ERROR("fopen() error");
        exit(0);
    }
    while(fgets(fname, STRINGMAXLEN_FILENAME, fp) != NULL)
    {
        fname[strlen(fname) - 1] = '\0';
        strncpy(fname1, fname, STRINGMAXLEN_FILENAME);

        fname1[strlen(fname) - 5] = '\0';
        ID = load_fits(fname, fname1, 1);
        printf("Image %s loaded -> %s\n", fname, fname1);
        if(init == 0)
        {
            init = 1;
            copy_image_ID(data.image[ID].name, outname, 0);
        }
        else
        {
            arith_image_add_inplace(outname, data.image[ID].name);
        }
        delete_image_ID(fname1);
        printf("Image %s added\n", data.image[ID].name);
        cnt++;
    }

    fclose(fp);

    EXECUTE_SYSTEM_COMMAND("rm flist.tmp");

    printf("%ld images coadded (stored in variable imcnt) -> %s\n", cnt, outname);
    create_variable_ID("imcnt", 1.0 * cnt);

    return(cnt);
}




// coadd all images matching strfilter + .fits
// return number of images added
long basic_aveimagesfiles(
    const char *strfilter,
    const char *outname
)
{
    long cnt;

    cnt = basic_addimagesfiles(strfilter, outname);
    arith_image_cstmult_inplace(outname, 1.0 / cnt);

    return(cnt);
}



// add all images starting with prefix
// return number of images added
long basic_addimages(
    const char *prefix,
    const char *ID_out
)
{
    long i;
    int init = 0; // becomes 1 when first image encountered
    long cnt = 0;

    for(i = 0; i < data.NB_MAX_IMAGE; i++)
        if(data.image[i].used == 1)
        {
            if(strncmp(prefix, data.image[i].name, strlen(prefix)) == 0)
            {
                if(init == 0)
                {
                    init = 1;
                    copy_image_ID(data.image[i].name, ID_out, 0);
                }
                else
                {
                    arith_image_add_inplace(ID_out, data.image[i].name);
                }
                printf("Image %s added\n", data.image[i].name);
                cnt ++;
            }
        }

    return(cnt);
}


// paste all images starting with prefix
long basic_pasteimages(
    const char *prefix,
    long        NBcol,
    const char *IDout_name
)
{
    long i;
    long cnt = 0;
    long row = 0;
    long col = 0;
    long colmax = 0;
    long xsizeout = 0;
    long ysizeout = 0;
    long xsize1max = 0;
    long ysize1max = 0;
    long xsize1, ysize1;
    long iioffset, jjoffset;
    long ii, jj, ii1, jj1;
    imageID IDout;

    for(i = 0; i < data.NB_MAX_IMAGE; i++)
        if(data.image[i].used == 1)
        {
            if(strncmp(prefix, data.image[i].name, strlen(prefix)) == 0)
            {
                if(data.image[i].md[0].size[0] > xsize1max)
                {
                    xsize1max = data.image[i].md[0].size[0];
                }
                if(data.image[i].md[0].size[1] > ysize1max)
                {
                    ysize1max = data.image[i].md[0].size[1];
                }

                if(col == NBcol)
                {
                    col = 0;
                    row ++;
                }
                if(col > colmax)
                {
                    colmax = col;
                }

                printf("Image %s[%ld] will be pasted at [%ld %ld]\n", data.image[i].name, cnt,
                       row, col);
                col ++;
            }
        }
    xsizeout = (colmax + 1) * xsize1max;
    ysizeout = (row + 1) * ysize1max;
    IDout = create_2Dimage_ID(IDout_name, xsizeout, ysizeout);


    col = 0;
    row = 0;
    for(i = 0; i < data.NB_MAX_IMAGE; i++)
        if(data.image[i].used == 1)
        {
            if(strncmp(prefix, data.image[i].name, strlen(prefix)) == 0)
            {
                if(col == NBcol)
                {
                    col = 0;
                    row ++;
                }

                iioffset = col * xsize1max;
                jjoffset = row * ysize1max;

                xsize1 = data.image[i].md[0].size[0];
                ysize1 = data.image[i].md[0].size[1];
                for(ii = 0; ii < xsize1; ii++)
                    for(jj = 0; jj < ysize1; jj++)
                    {
                        ii1 = ii + iioffset;
                        jj1 = jj + jjoffset;
                        data.image[IDout].array.F[jj1 * xsizeout + ii1] = data.image[i].array.F[jj *
                                xsize1 + ii];
                    }

                printf("Image %s[%ld] pasted at [%ld %ld]\n", data.image[i].name, cnt, row,
                       col);
                col ++;
            }
        }

    return(cnt);
}



// average all images starting with prefix
// return number of images added
long basic_averageimages(
    const char *prefix,
    const char *ID_out
)
{
    long cnt;

    cnt = basic_addimages(prefix, ID_out);
    arith_image_cstmult_inplace(ID_out, (float)(1.0 / cnt));

    return(cnt);
}



/* ----------------------------------------------------------------------
 *
 * resize image using bilinear interpolation
 *
 *
 * ---------------------------------------------------------------------- */


long basic_resizeim(
    const char *imname_in,
    const char *imname_out,
    long xsizeout,
    long ysizeout
)
{
    imageID ID, IDout;
    long naxis = 2;
    uint32_t naxes[2];
    uint32_t naxesout[2];
    float xf, yf, xf1, yf1, uf, tf, v00f, v01f, v10f, v11f;
    double xd, yd, xd1, yd1, ud, td, v00d, v01d, v10d, v11d;
    uint8_t datatype;
    long ii, jj, ii1, jj1;


    ID = image_ID(imname_in);
    datatype = data.image[ID].md[0].datatype;
    naxes[0] = data.image[ID].md[0].size[0];
    naxes[1] = data.image[ID].md[0].size[1];
    naxesout[0] = xsizeout;
    naxesout[1] = ysizeout;


    if(datatype == _DATATYPE_FLOAT)
    {
        IDout = create_image_ID(imname_out, naxis, naxesout, datatype, 0, 0);
        for(ii = 0; ii < naxesout[0]; ii++)
            for(jj = 0; jj < naxesout[1]; jj++)
            {
                xf = (float)(1.0 * ii / naxesout[0]);
                yf = (float)(1.0 * jj / naxesout[1]);
                xf1 = xf * (float) naxes[0];
                yf1 = yf * (float) naxes[1];
                ii1 = (long) xf1;
                jj1 = (long) yf1;
                uf = xf1 - (float) ii1;
                tf = yf1 - (float) jj1;
                if((ii1 > -1) && (ii1 + 1 < naxes[0]) && (jj1 > -1) && (jj1 + 1 < naxes[1]))
                {
                    v00f = data.image[ID].array.F[jj1 * naxes[0] + ii1];
                    v01f = data.image[ID].array.F[(jj1 + 1) * naxes[0] + ii1];
                    v10f = data.image[ID].array.F[jj1 * naxes[0] + ii1 + 1];
                    v11f = data.image[ID].array.F[(jj1 + 1) * naxes[0] + ii1 + 1];
                    data.image[IDout].array.F[jj * naxesout[0] + ii] = (float)(v00f * (1.0 - uf) *
                            (1.0 - tf) + v10f * uf * (1.0 - tf) + v01f * (1.0 - uf) * tf + v11f * uf * tf);
                }
            }
    }
    else if(datatype == _DATATYPE_DOUBLE)
    {
        IDout = create_image_ID(imname_out, naxis, naxesout, datatype, 0, 0);
        for(ii = 0; ii < naxesout[0] - 1; ii++)
            for(jj = 0; jj < naxesout[1] - 1; jj++)
            {
                xd = 1.0 * ii / naxesout[0];
                yd = 1.0 * jj / naxesout[1];
                xd1 = xd * naxes[0];
                yd1 = yd * naxes[1];
                ii1 = (long) xd1;
                jj1 = (long) yd1;
                ud = xd1 - (float) ii1;
                td = yd1 - (float) jj1;
                v00d = data.image[ID].array.D[jj1 * naxes[0] + ii1];
                v01d = data.image[ID].array.D[(jj1 + 1) * naxes[0] + ii1];
                v10d = data.image[ID].array.D[jj1 * naxes[0] + ii1 + 1];
                v11d = data.image[ID].array.D[(jj1 + 1) * naxes[0] + ii1 + 1];
                data.image[IDout].array.D[jj * naxesout[0] + ii] = (double)(v00d *
                        (1.0 - ud) * (1.0 - td) + v10d * ud * (1.0 - td) + v01d *
                        (1.0 - ud) * td + v11d * ud * td);
            }
    }
    else
    {
        PRINT_ERROR("Wrong image type(s)\n");
        exit(0);
    }


    return(0);
}








/* ----------------------------------------------------------------------
 *
 * turns a 3D image into a 2D image by collapsing first 2 axis
 *
 *
 * ---------------------------------------------------------------------- */


imageID image_basic_3Dto2D(
    const char *IDname
)
{
    imageID ID;

    ID = image_ID(IDname);
    if(data.image[ID].md[0].naxis != 3)
    {
        printf("ERROR: image needs to have 3 axis\n");
    }
    else
    {
        data.image[ID].md[0].size[0] *= data.image[ID].md[0].size[1];
        data.image[ID].md[0].size[1] =  data.image[ID].md[0].size[2];
        data.image[ID].md[0].naxis = 2;
    }

    return ID;
}





imageID image_basic_SwapAxis2D(
    const char *IDin_name,
    const char *IDout_name
)
{
    imageID IDin;
    imageID IDout = -1;

    IDin = image_ID(IDin_name);
    if(data.image[IDin].md[0].naxis != 2)
    {
        printf("ERROR: image needs to have 2 axis\n");
    }
    else
    {
        IDout = create_2Dimage_ID(IDout_name, data.image[IDin].md[0].size[1],
                                  data.image[IDin].md[0].size[0]);
        for(uint32_t ii = 0; ii < data.image[IDin].md[0].size[0]; ii++)
            for(uint32_t jj = 0; jj < data.image[IDin].md[0].size[1]; jj++)
            {
                data.image[IDout].array.F[ii * data.image[IDin].md[0].size[1] + jj] =
                    data.image[IDin].array.F[jj * data.image[IDin].md[0].size[0] + ii];
            }
    }

    return IDout;
}













/* ----------------------------------------------------------------------
 *
 * turns a list of 2D points into an image by interpolation
 *
 *
 * ---------------------------------------------------------------------- */

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
)
{
    FILE *fp;
    imageID ID;
    float x, y;
    long ii, jj;
    double tot, cnt, cntx, cnty, totx, toty, slx, sly, xave, yave, vave, totsx,
           totsy;
    long i;
    long NBpts;
    double *xarray = NULL;
    double *yarray = NULL;
    double *varray = NULL;
    void *ptree = NULL;
    double buf[2];
    double pt[2];
    double radius, radius0;
    double *pv = NULL;
    struct kdres *presults = NULL;
    double dist;
    double *pos;
    //  double tmp1;
    int ok;

    long long cnttotal = 0;
    long long cntrejected = 0;
    double valm, val0, val1;

    // nearest points
    long NBnpt, NBnptmax;
    double *pt_x = NULL;
    double *pt_y = NULL;
    double *pt_val = NULL;
    float *pt_val_cp;
    double *pt_coeff = NULL;
    double *pt_coeff1 = NULL;

    long IDslx, IDsly, IDxerr, IDyerr;

    double radiusmax = 50.0;

    printf("table : %s\n", fname);
    printf("range : %f -> %f    %f -> %f\n", xmin, xmax, ymin, ymax);
    printf("output: %s (%ld x %ld)\n", ID_name, xsize, ysize);
    printf("kernel size = %f\n", convsize);
    printf("radiusmax = %f\n", radiusmax);

    pos = (double *) malloc(sizeof(double) * 2);
    if(pos == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }
    pos[0] = 0.0;
    pos[1] = 0.0;

    // load table into array
    NBpts = file_number_lines(fname);
    xarray = (double *) malloc(sizeof(double) * NBpts);
    if(xarray == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    yarray = (double *) malloc(sizeof(double) * NBpts);
    if(yarray == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    varray = (double *) malloc(sizeof(double) * NBpts);
    if(varray == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    if((fp = fopen(fname, "r")) == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("fopen() error");
        exit(0);
    }

    for(i = 0; i < NBpts; i++)
    {
        if(fscanf(fp, "%lf %lf %lf\n", &xarray[i], &yarray[i], &varray[i]) != 3)
        {
            fprintf(stderr, "%c[%d;%dm ERROR: fscanf [ %s  %s  %d ] %c[%d;m\n", (char) 27,
                    1, 31, __FILE__, __func__, __LINE__, (char) 27, 0);
            exit(0);
        }
    }
    fclose(fp);

    printf("%ld points read\n", NBpts);
    fflush(stdout);


    /* create a k-d tree for 2-dimensional points */
    ptree = kd_create(2);


    /* add nodes to the tree */
    for(i = 0; i < NBpts; i++)
    {
        buf[0] = xarray[i];
        buf[1] = yarray[i];
        kd_insert(ptree, buf, &varray[i]);
    }

    ID = create_2Dimage_ID(ID_name, xsize, ysize);

    IDslx = create_2Dimage_ID("tmp2dinterpslx", xsize, ysize);
    IDsly = create_2Dimage_ID("tmp2dinterpsly", xsize, ysize);
    IDxerr = create_2Dimage_ID("tmp2dinterpxerr", xsize, ysize);
    IDyerr = create_2Dimage_ID("tmp2dinterpyerr", xsize, ysize);

    // automatically set radius0 such that if points are randomly distributed, a circle of radius radius0 includes sqrt(NBpts) points
    //  tmp1 = sqrt(NBpts);
    //  if(tmp1<100.0)
    //   tmp1 = 100.0;
    // if(tmp1>1000.0)
    //  tmp1 = 1000.0;
    // radius0 = sqrt(tmp1/PI)*sqrt((xmax-xmin)*(ymax-ymin))/sqrt(NBpts);
    radius0 = 5.0 * convsize / sqrt(1.0 * xsize * ysize);
    radius0 *= sqrt((xmax - xmin) * (ymax - ymin));

    NBnpt = 15000;
    NBnptmax = NBnpt;
    pt_x = (double *) malloc(sizeof(double) * NBnpt);
    if(pt_x == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    pt_y = (double *) malloc(sizeof(double) * NBnpt);
    if(pt_y == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    pt_val = (double *) malloc(sizeof(double) * NBnpt);
    if(pt_val == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    pt_coeff = (double *) malloc(sizeof(double) * NBnpt);
    if(pt_coeff == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    pt_coeff1 = (double *) malloc(sizeof(double) * NBnpt);
    if(pt_coeff1 == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }

    pt_val_cp = (float *) malloc(sizeof(float) * NBnpt);
    if(pt_val_cp == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc() error");
        exit(0);
    }


    pt_x[0] = 0.0;
    pt_y[0] = 0.0;
    pt_val[0] = 0.0;
    pt_coeff[0] = 0.0;
    pt_coeff1[0] = 0.0;
    pt_val_cp[0] = 0.0;


    printf("radius = %g\n", radius0);
    fflush(stdout);

    printf("\n");
    for(ii = 0; ii < xsize; ii++)
    {
        printf("\r[%ld/%ld]   ", ii, xsize);
        fflush(stdout);
        for(jj = 0; jj < ysize; jj++)
        {
            //	printf("[%ld %ld]\n",ii,jj);
            //	fflush(stdout);

            x = (float)(1.0 * xmin + 1.0 * (xmax - xmin) * ii / xsize);
            y = (float)(1.0 * ymin + 1.0 * (ymax - ymin) * jj / ysize);


            /* find points closest to the origin and within distance radius */
            pt[0] = x;
            pt[1] = y;

            radius = radius0;
            ok = 0;
            while(ok == 0)
            {
                presults = kd_nearest_range(ptree, pt, radius);

                //presults = kd_nearest( ptree, pt );

                /* print out all the points found in results */
                //  printf( "[%g %g] found %d results (radius = %f):\n", x,y, kd_res_size(presults), radius );
                if((kd_res_size(presults) < 30) && (radius < radiusmax))
                {
                    radius *= 1.5;
                    //	  printf("        radius -> %f\n",radius);
                    //  fflush(stdout);
                }
                else
                {
                    ok = 1;
                }
            }

            if(radius < 0.99 * radiusmax)
            {
                NBnpt = kd_res_size(presults);
                //printf("NBnpt = %ld\n",NBnpt);
                //fflush(stdout);
                if(NBnpt > NBnptmax)
                {
                    pt_x = realloc(pt_x, sizeof(double) * NBnpt);
                    pt_y = realloc(pt_y, sizeof(double) * NBnpt);
                    pt_val = realloc(pt_val, sizeof(double) * NBnpt);
                    pt_coeff = realloc(pt_coeff, sizeof(double) * NBnpt);
                    pt_coeff1 = realloc(pt_coeff1, sizeof(double) * NBnpt);
                    NBnptmax = NBnpt;
                    //  printf("Reallocation to %ld points\n",NBnpt);
                    // fflush(stdout);
                }


                i = 0;
                while(kd_res_end(presults) == 0)
                {
                    /* get the data and position of the current result item */
                    pv = (double *) kd_res_item(presults, pos);

                    /* compute the distance of the current result from the pt */
                    dist = sqrt((pos[0] - pt[0]) * (pos[0] - pt[0]) + (pos[1] - pt[1]) *
                                (pos[1] - pt[1]));

                    pt_x[i] = pos[0];
                    pt_y[i] = pos[1];
                    pt_val[i] = *pv;
                    pt_val_cp[i] = (float) pt_val[i];
                    pt_coeff[i] = pow((1.0 + cos(M_PI * dist / radius0)) / 2.0, 2.0);
                    pt_coeff1[i] = pow(dist / radius0,
                                       2.0) * (1.0 + cos(M_PI * dist / radius0)) / 2.0;
                    /* go to the next entry */
                    free(pv);
                    kd_res_next(presults);

                    i++;
                }


                // reject outliers
                // sort values
                quick_sort_float(pt_val_cp, NBnpt);
                valm = pt_val_cp[(long)(NBnpt / 2)]; // median
                val0 = pt_val_cp[(long)(0.3 * NBnpt)];
                val1 = pt_val_cp[(long)(0.7 * NBnpt)];
                for(i = 0; i < NBnpt; i++)
                {
                    cnttotal ++;
                    if(fabs(pt_val[i] - valm) > 3.0 * (val1 - val0))
                    {
                        pt_coeff[i] = 0.0;
                        pt_coeff1[i] = 0.0;
                        cntrejected ++;
                    }
                }



                tot = 0.0;
                totx = 0.0;
                toty = 0.0;
                cnt = 0.0;
                for(i = 0; i < NBnpt; i++)
                {
                    tot += pt_val[i] * pt_coeff[i];
                    totx += pt_x[i] * pt_coeff[i];
                    toty += pt_y[i] * pt_coeff[i];
                    cnt += pt_coeff[i];
                }

                xave = totx / cnt;
                yave = toty / cnt;
                vave = tot / cnt;

                totsx = 0.0;
                totsy = 0.0;
                cntx = 0.0;
                cnty = 0.0;

                for(i = 0; i < NBnpt; i++)
                {
                    if(fabs(pt_x[i] - xave) > 0.01 * radius0)
                    {
                        cntx += pt_coeff1[i];
                        totsx += (pt_val[i] - vave) / (pt_x[i] - xave) * pt_coeff1[i];
                    }
                    if(fabs(pt_y[i] - yave) > 0.01 * radius0)
                    {
                        cnty += pt_coeff1[i];
                        totsy += (pt_val[i] - vave) / (pt_y[i] - yave) * pt_coeff1[i];
                    }
                }
                if(cntx < 0.0001)
                {
                    cntx = 0.0001;
                }
                if(cnty < 0.0001)
                {
                    cnty = 0.0001;
                }
                slx = totsx / cntx;
                sly = totsy / cnty;

                data.image[ID].array.F[jj * xsize + ii] = (float)
                        vave; //vave + (x-xave)*slx + (y-yave)*sly;

                data.image[IDxerr].array.F[jj * xsize + ii] = (float)(x - xave);
                data.image[IDyerr].array.F[jj * xsize + ii] = (float)(y - yave);
                data.image[IDslx].array.F[jj * xsize + ii] = (float)(slx);
                data.image[IDsly].array.F[jj * xsize + ii] = (float)(sly);
            }
        }
    }

    free(pos);
    printf("\n");

    printf("fraction of points rejected = %g\n",
           (double)(1.0 * cntrejected / cnttotal));

    free(pt_x);
    free(pt_y);
    free(pt_val);
    free(pt_val_cp);
    free(pt_coeff);
    free(pt_coeff1);

    free(xarray);
    free(yarray);
    free(varray);
    if(presults != NULL)
    {
        kd_res_free(presults);
    }
    if(presults != NULL)
    {
        PRINT_ERROR("presults was not freed by kd_res_free. Attempting free(presults) and continuing.");
        free(presults);
    }
    kd_free(ptree);
    if(ptree != NULL)
    {
        PRINT_ERROR("ptree was not freed by kd_free. Attempting free(ptree) and continuing.");
        free(ptree);
    }
    save_fl_fits(ID_name, "!tmp2dinterp.fits");

    make_gauss("kerg", xsize, ysize, convsize,
               (float) 1.0); //(long) (10.0*convsize+2.0));

    fconvolve_padd("tmp2dinterpxerr", "kerg", (long)(10.0 * convsize + 2.0),
                   "tmp2dinterpxerrg");
    fconvolve_padd("tmp2dinterpyerr", "kerg", (long)(10.0 * convsize + 2.0),
                   "tmp2dinterpyerrg");
    fconvolve_padd("tmp2dinterpslx", "kerg", (long)(10.0 * convsize + 2.0),
                   "tmp2dinterpslxg");
    fconvolve_padd("tmp2dinterpsly", "kerg", (long)(10.0 * convsize + 2.0),
                   "tmp2dinterpslyg");

    delete_image_ID("tmp2dinterpxerr");
    delete_image_ID("tmp2dinterpyerr");
    delete_image_ID("tmp2dinterpslx");
    delete_image_ID("tmp2dinterpsly");

    IDxerr = image_ID("tmp2dinterpxerrg");
    IDyerr = image_ID("tmp2dinterpyerrg");
    IDslx = image_ID("tmp2dinterpslxg");
    IDsly = image_ID("tmp2dinterpslyg");
    ID = image_ID(ID_name);

    for(ii = 0; ii < xsize; ii++)
        for(jj = 0; jj < ysize; jj++)
        {
            //xerr = data.image[IDxerr].array.F[jj*xsize+ii];
            //yerr = data.image[IDyerr].array.F[jj*xsize+ii];
            slx = data.image[IDslx].array.F[jj * xsize + ii];
            sly = data.image[IDsly].array.F[jj * xsize + ii];
            //	data.image[ID].array.F[jj*xsize+ii] += xerr*slx+yerr*sly;
        }

    delete_image_ID("tmp2dinterpxerrg");
    delete_image_ID("tmp2dinterpyerrg");
    delete_image_ID("tmp2dinterpslxg");
    delete_image_ID("tmp2dinterpslyg");


    ID = image_ID(ID_name);
    delete_image_ID("kerg");

    return(ID);
}





imageID basic_2Dextrapolate_nearestpixel(
    const char *IDin_name,
    const char *IDmask_name,
    const char *IDout_name
)
{
    imageID IDin, IDmask, IDout;
    long ii, jj, ii1, jj1, k;
    double bdist, dist;
    long naxes[2];


    long *maskii = NULL;
    long *maskjj = NULL;
    long NBmaskpts;

    long IDmask1;



    IDin = image_ID(IDin_name);
    IDmask = image_ID(IDmask_name);

    list_image_ID();
    IDmask1 = image_ID("_mask1");
    if(IDmask1 != -1)
    {
        printf("USING MASK\n");
    }

    naxes[0] = data.image[IDin].md[0].size[0];
    naxes[1] = data.image[IDin].md[0].size[1];

    NBmaskpts = 0;
    for(ii = 0; ii < naxes[0]; ii++)
        for(jj = 0; jj < naxes[1]; jj++)
            if(data.image[IDmask].array.F[jj * naxes[0] + ii] > 0.5)
            {
                NBmaskpts++;
            }

    maskii = (long *) malloc(sizeof(long) * NBmaskpts);
    if(maskii == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc error");
        exit(0);
    }
    maskii[0] = 0; // avoids warning about unused maskii

    maskjj = (long *) malloc(sizeof(long) * NBmaskpts);
    if(maskjj == NULL)
    {
        C_ERRNO = errno;
        PRINT_ERROR("malloc error");
        exit(0);
    }
    maskjj[0] = 0; // avoids warning about unused maskjj

    NBmaskpts = 0;
    for(ii = 0; ii < naxes[0]; ii++)
        for(jj = 0; jj < naxes[1]; jj++)
            if(data.image[IDmask].array.F[jj * naxes[0] + ii] > 0.5)
            {
                maskii[NBmaskpts] = ii;
                maskjj[NBmaskpts] = jj;
                NBmaskpts++;
            }

    IDout  = create_2Dimage_ID(IDout_name, naxes[0], naxes[1]);
    printf("imout = %s\n", IDout_name);
    printf("\n");

    for(ii = 0; ii < naxes[0]; ii++)
    {
        printf("\r%ld / %ld  ", ii, naxes[0]);
        fflush(stdout);

        for(jj = 0; jj < naxes[1]; jj++)
        {
            /*if(IDmask1==-1)
                OKpix = 1;
            else
            {
                if(data.image[IDmask1].array.F[jj*naxes[1]+ii]>0.5)
                    OKpix = 1;
                else
                    OKpix = 0;
            }*/
            bdist = (double)(naxes[0] + naxes[1]);
            bdist = bdist * bdist;
            for(k = 0; k < NBmaskpts; k++)
            {
                ii1 = maskii[k];
                jj1 = maskjj[k];
                dist = 1.0 * ((ii1 - ii) * (ii1 - ii) + (jj1 - jj) * (jj1 - jj));
                if(dist < bdist)
                {
                    bdist = dist;
                    data.image[IDout].array.F[jj * naxes[0] + ii] = data.image[IDin].array.F[jj1 *
                            naxes[0] + ii1];
                }
            }
        }
    }

    printf("\n");

    free(maskii);
    free(maskjj);

    return(IDout);
}



// measure offset between 2 images

double basic_measure_transl(
    const char *ID_name1,
    const char *ID_name2,
    long tmax
)
{
    imageID ID1, ID2, ID;
    imageID IDout, IDcnt;
    long dx, dy, ii1, jj1, ii2, jj2, iio, jjo;
    long sx_out, sy_out;
    long size1x, size1y;
    long size2x, size2y;
    double val;
    double tmp, v1, v2;
    int SCALE = 64; // must be power of 2
    long step1 = 1;
    long step2 = 1;
    double vmin;
    double vdx, vdy;
    long ii2min, ii2max, jj2min, jj2max;
    long dxmin, dymin;
    int SCALEindex;
    long dsize;
    double vmincnt;
    long dx1, dy1;
    int QUICKMODE = 0;
    long ii1min, ii1max, jj1min, jj1max;
    long iiomin, iiomax, jjomin, jjomax;
    imageID ID1mask;
    long xsizemask, ysizemask;
    double vlim;
    long contractfactor;
    long ii;
    long ii1m, jj1m;
    double Mlim;

    double fitval = 0.0;

    step1 = SCALE;
    step2 = SCALE;

    ID1 = image_ID(ID_name1);
    size1x = data.image[ID1].md[0].size[0];
    size1y = data.image[ID1].md[0].size[1];

    ID2 = image_ID(ID_name2);
    size2x = data.image[ID2].md[0].size[0];
    size2y = data.image[ID2].md[0].size[1];

    sx_out = 2 * tmax;
    sy_out = 2 * tmax;
    IDout = create_2Dimage_ID("TranslMap", sx_out, sy_out);
    IDcnt = create_2Dimage_ID("TranslMapcnt", sx_out, sy_out);
    for(iio = 0; iio < sx_out; iio++)
        for(jjo = 0; jjo < sy_out; jjo++)
        {
            data.image[IDout].array.F[jjo * sx_out + iio] = 0.0;
            data.image[IDcnt].array.F[jjo * sx_out + iio] = 0.0;
        }

    dxmin = 0;
    dymin = 0;
    SCALEindex = 1;


    // STEP 1 : quickly identify regions of image 1 where flux gradient is large
    // select 30% of image pixels
    contractfactor = 2;
    basic_contract(ID_name1, "_im1C", contractfactor, contractfactor);
    gauss_filter("_im1C", "_im1Cg", 5.0, 10);
    execute_arith("_im1HF=_im1C-_im1Cg");
    execute_arith("_im1HF2=_im1HF*_im1HF");
    gauss_filter("_im1HF2", "_im1mask", 5.0, 10);
    delete_image_ID("_im1C");
    delete_image_ID("_im1HF");
    delete_image_ID("_im1Cg");
    delete_image_ID("_im1HF2");

    vlim = (double) img_percentile("_im1mask", 0.8);
    printf("vlim = %g\n", vlim);
    save_fl_fits("_im1mask", "!_im1mask.0.fits");
    ID1mask = image_ID("_im1mask");
    xsizemask = data.image[ID1mask].md[0].size[0];
    ysizemask = data.image[ID1mask].md[0].size[1];

    for(ii = 0; ii < xsizemask * ysizemask; ii++)
    {
        if(data.image[ID1mask].array.F[ii] > vlim)
        {
            data.image[ID1mask].array.F[ii] = 1.0;
        }
        else
        {
            data.image[ID1mask].array.F[ii] = 0.0;
        }
    }

    save_fl_fits("_im1mask", "!_im1mask.fits");
    //exit(0);

    dsize = tmax * 2;
    while(SCALE != 0)
    {
        step1 = SCALE;
        step2 = 1; //SCALE;

        dsize /= 2; //(long) (1.0*tmax/pow(SCALEindex,2.0));
        if(dsize < 1.2 * SCALE)
        {
            dsize = (long)(1.2 * SCALE);
        }

        //      if(SCALE>1)
        //Mlim = -1;
        // else
        Mlim = 0.5;

        ii1min = 0;
        ii1max = size1x;
        jj1min = 0;
        jj1max = size1y;

        if(QUICKMODE == 1)
        {
            step1 *= 5;
            step2 *= 3;
        }
        if(SCALE == 1)
        {
            step1 = 1;
            step2 = 1;
        }

        for(ii1 = ii1min; ii1 < ii1max; ii1 += step1)
            for(jj1 = jj1min; jj1 < jj1max; jj1 += step1)
            {
                ii1m = (long)(ii1 / contractfactor);
                jj1m = (long)(jj1 / contractfactor);
                if(data.image[ID1mask].array.F[jj1m * xsizemask + ii1m] > Mlim)
                {
                    v1 = data.image[ID1].array.F[jj1 * size1x + ii1];

                    ii2min = ii1 + dxmin - dsize;
                    ii2max = ii1 + dxmin + dsize;
                    while(ii2min < 0)
                    {
                        ii2min += step2;
                    }
                    while(ii2min > size2x - 1)
                    {
                        ii2min -= step2;
                    }
                    while(ii2max < 0)
                    {
                        ii2max += step2;
                    }
                    while(ii2max > size2x - 1)
                    {
                        ii2max -= step2;
                    }

                    jj2min = jj1 + dymin - dsize;
                    jj2max = jj1 + dymin + dsize;
                    while(jj2min < 0)
                    {
                        jj2min += step2;
                    }
                    while(jj2min > size2y - 1)
                    {
                        jj2min -= step2;
                    }
                    while(jj2max < 0)
                    {
                        jj2max += step2;
                    }
                    while(jj2max > size2y - 1)
                    {
                        jj2max -= step2;
                    }

                    for(ii2 = ii2min; ii2 < ii2max; ii2 += step2)
                        for(jj2 = jj2min; jj2 < jj2max; jj2 += step2)
                        {
                            dx = ii2 - ii1;
                            dy = jj2 - jj1;
                            dx1 = dx - dxmin;
                            dy1 = dy - dymin;
                            if(dx1 * dx1 + dy1 * dy1 < 1.0 * dsize * dsize)
                            {
                                iio = dx + tmax;
                                jjo = dy + tmax;
                                if((iio > -1) && (iio < sx_out) && (jjo > -1) && (jjo < sy_out))
                                {
                                    v2 = data.image[ID2].array.F[jj2 * size2x + ii2];
                                    tmp = (v1 - v2);
                                    data.image[IDout].array.F[jjo * sx_out + iio] += tmp * tmp;
                                    data.image[IDcnt].array.F[jjo * sx_out + iio] += 1.0;
                                    //   if((iio == 87)&&(jjo == 100))
                                    //printf("%g (%ld %ld %g) (%ld %ld %g)\n",data.image[IDcnt].array.F[jjo*sx_out+iio], ii1, jj1, v1, ii2, jj2, v2);
                                }
                            }
                        }
                }
            }

        vmin =  1.0e100;
        for(iio = 0; iio < sx_out; iio++)
            for(jjo = 0; jjo < sy_out; jjo++)
            {
                if(data.image[IDcnt].array.F[jjo * sx_out + iio] > 0.1)
                {
                    val = data.image[IDout].array.F[jjo * sx_out + iio] /
                          data.image[IDcnt].array.F[jjo * sx_out + iio];
                    if(val < vmin)
                    {
                        vmin = val;
                        vmincnt = data.image[IDcnt].array.F[jjo * sx_out + iio];
                        vdx = 1.0 * iio - tmax;
                        vdy = 1.0 * jjo - tmax;
                    }
                }
            }
        printf("------- SCALE = %d [%ld] --------\n", SCALE, dsize);
        printf("vdx = %g  (%ld)\n", vdx, dxmin);
        printf("vdy = %g  (%ld)\n", vdy, dymin);
        printf("vmin = %g [%g]\n", vmin, vmincnt);

        dxmin = (long)(vdx + 0.5 + 10000) - 10000;
        dymin = (long)(vdy + 0.5 + 10000) - 10000;


        printf("-------- %ld %ld --------\n", dxmin, dymin);

        if(SCALE == 1)
        {
            SCALE = 0;
        }
        else
        {
            SCALEindex ++;
            SCALE /= 2;
        }

    }

    for(iio = 0; iio < sx_out; iio++)
        for(jjo = 0; jjo < sy_out; jjo++)
        {
            if(data.image[IDcnt].array.F[jjo * sx_out + iio] > 0.1)
            {
                data.image[IDout].array.F[jjo * sx_out + iio] /= data.image[IDcnt].array.F[jjo *
                        sx_out + iio];
            }
        }

    ID = gauss_filter("TranslMap", "TranslMapg", 5.0, 10);

    vmin =  1.0e100;

    iiomin = sx_out / 2 + dxmin - 20;
    if(iiomin < 0)
    {
        iiomin = 0;
    }
    iiomax = sx_out / 2 + dxmin + 20;
    if(iiomax > sx_out - 1)
    {
        iiomax = sx_out - 1;
    }

    jjomin = sy_out / 2 + dymin - 20;
    if(jjomin < 0)
    {
        jjomin = 0;
    }
    jjomax = sy_out / 2 + dymin + 20;
    if(jjomax > sy_out - 1)
    {
        jjomax = sy_out - 1;
    }


    for(iio = iiomin; iio < iiomax; iio++)
        for(jjo = jjomin; jjo < jjomax; jjo++)
        {
            if(data.image[IDcnt].array.F[jjo * sx_out + iio] > 0.1)
            {
                val = data.image[ID].array.F[jjo * sx_out + iio];
                if(val < vmin)
                {
                    vmin = val;
                    vdx = 1.0 * iio - tmax;
                    vdy = 1.0 * jjo - tmax;
                }
            }
        }

    create_variable_ID("vdx", vdx);
    create_variable_ID("vdy", vdy);
    printf("-------- %f %f --------\n", vdx, vdy);

    save_fl_fits("TranslMapg", "!_TranslMap.fits");
    save_fl_fits("TranslMapcnt", "!_TranslMapcnt.fits");

    delete_image_ID("TranslMap");
    delete_image_ID("TranslMapg");
    delete_image_ID("TranslMapcnt");
    //  exit(0);

    return(fitval);
}





/** coadd frames from image stream
 *  output is by default float type
 * mode :
 *   0 : simple average
 *   1 : average + std dev (std dev in "imgstreamrms")
 *   2 : average + std dev -> badpix map for detector calibration ("badpixmap")
 *   3 : same as 1
 *
 * @note Averaging will stop when receiving signal SIGUSR1
 * @note If semindex<0, use counter instead of semaphore
 *
 * */
imageID IMAGE_BASIC_streamaverage(
    const char *IDname,
    long        NBcoadd,
    const char *IDoutname,
    int         mode,
    int         semindex
)
{
    imageID ID;
    unsigned long cnt = 0;
    long k;
    long xsize, ysize;
    long IDcube;
    uint32_t *imsize;
    uint8_t datatype;
    char *ptrv;
    char *ptrcv;
    long xysize;
    long k1;
    long IDout;
    long ii;
    imageID IDrms;
    imageID IDbadpix;
    float rmsmean;
    float vmin, vmax;
    float darkp20, darkp80;
    int createim;
    long offset;

    int CounterWatch; // 1 if using cnt0, 0 if using semaphore

    double *sumsqarray; // sum squared
    double *sumarray;


    ID = image_ID(IDname);
    xsize = data.image[ID].md[0].size[0];
    ysize = data.image[ID].md[0].size[1];
    xysize = xsize * ysize;


    imsize = (uint32_t *) malloc(sizeof(uint32_t) * 3);
    imsize[0] = xsize;
    imsize[1] = ysize;
    imsize[2] = NBcoadd;
    datatype = data.image[ID].md[0].datatype;

    if(mode > 0)
    {
        IDrms = create_2Dimage_ID("imgstreamrms", xsize, ysize);
        sumsqarray = calloc(xsize * ysize, sizeof(double));
    }

    createim = 0;
    IDcube = image_ID("tmpstrcoadd");
    if(IDcube != -1)
    {
        if((data.image[IDcube].md[0].naxis == 3)
                && (data.image[IDcube].md[0].size[0] == imsize[0])
                && (data.image[IDcube].md[0].size[1] == imsize[1])
                && (data.image[IDcube].md[0].size[2] == imsize[2]))
        {
            createim = 0;
        }
        else
        {
            delete_image_ID("tmpstrcoadd");
            createim = 1;
        }
    }
    else
    {
        createim = 1;
    }

    if(createim == 1)
    {
        IDcube = create_image_ID("tmpstrcoadd", 3, imsize, datatype, 0, 0);
    }


    IDout = create_2Dimage_ID(IDoutname, xsize, ysize);
    sumarray = calloc(xsize * ysize, sizeof(double));


    // if semindex out of range, use counter
    CounterWatch = 0;
    if((semindex > data.image[ID].md[0].sem - 1) || (semindex < 0))
    {
        printf("Using counter\n");
        fflush(stdout);

        CounterWatch = 1;
    }

    if(CounterWatch == 0)
    {
        if(data.image[ID].md[0].sem > 0) // drive semaphore to zero
            while(sem_trywait(data.image[ID].semptr[semindex]) == 0) {}
    }

    // printf("\n\n");
    k = 0;

    while((k < NBcoadd) && (data.signal_USR1 == 0))
    {
        printf("\r ID %ld   image number %8ld     ", ID, k);
        fflush(stdout);
        if(CounterWatch == 1)
        {
            while(data.image[ID].md[0].cnt0 == cnt) // test if new frame exists
            {
                usleep(5);
                // do nothing, wait
            }
            cnt = data.image[ID].md[0].cnt0;
        }
        else
        {
            printf("[sem]...");
            sem_wait(data.image[ID].semptr[semindex]);
        }

        if(data.image[ID].md[0].naxis == 3)
        {
            k1 = data.image[ID].md[0].cnt1;
        }
        else
        {
            k1 = 0;
        }

        offset = k * xysize;

        switch(datatype)
        {

            case _DATATYPE_UINT8:
                ptrv = (char *) data.image[ID].array.UI8;
                ptrv += sizeof(char) * k1 * xysize;
                ptrcv = (char *) data.image[IDcube].array.UI8;
                ptrcv += sizeof(char) * k * xysize;
                memcpy(ptrcv, ptrv, sizeof(char)*xysize);

                if(mode > 0)
                {
                    for(ii = 0; ii < xysize; ii++)
                    {
                        sumsqarray[ii] += (double)(data.image[IDcube].array.UI8[offset + ii] *
                                                   data.image[IDcube].array.UI8[offset + ii]);
                    }
                }
                for(ii = 0; ii < xysize; ii++)
                {
                    sumarray[ii] += (double) data.image[IDcube].array.UI8[offset + ii];
                }
                break;

            case _DATATYPE_INT32:
                ptrv = (char *) data.image[ID].array.SI32;
                ptrv += sizeof(int) * k1 * xysize;
                ptrcv = (char *) data.image[IDcube].array.SI32;
                ptrcv += sizeof(int) * k * xysize;
                memcpy(ptrcv, ptrv, sizeof(int)*xysize);
                if(mode > 0)
                {
                    for(ii = 0; ii < xysize; ii++)
                    {
                        sumsqarray[ii] += (double)(data.image[IDcube].array.SI32[offset + ii] *
                                                   data.image[IDcube].array.SI32[offset + ii]);
                    }
                }
                for(ii = 0; ii < xysize; ii++)
                {
                    sumarray[ii] += (double) data.image[IDcube].array.SI32[offset + ii];
                }
                break;

            case _DATATYPE_FLOAT:
                ptrv = (char *) data.image[ID].array.F;
                ptrv += sizeof(float) * k1 * xysize;
                ptrcv = (char *) data.image[IDcube].array.F;
                ptrcv += sizeof(float) * k * xysize;
                memcpy(ptrcv, ptrv, sizeof(float)*xysize);

                if(mode > 0)
                {
                    for(ii = 0; ii < xysize; ii++)
                    {
                        sumsqarray[ii] += (double)(data.image[IDcube].array.F[offset + ii] *
                                                   data.image[IDcube].array.F[offset + ii]);
                    }
                }
                for(ii = 0; ii < xysize; ii++)
                {
                    sumarray[ii] += (double) data.image[IDcube].array.F[offset + ii];
                }
                break;

            case _DATATYPE_DOUBLE:
                ptrv = (char *) data.image[ID].array.D;
                ptrv += sizeof(double) * k1 * xysize;
                ptrcv = (char *) data.image[IDcube].array.D;
                ptrcv += sizeof(double) * k * xysize;
                memcpy(ptrcv, ptrv, sizeof(double)*xysize);
                if(mode > 0)
                {
                    for(ii = 0; ii < xysize; ii++)
                    {
                        sumsqarray[ii] += (double)(data.image[IDcube].array.D[offset + ii] *
                                                   data.image[IDcube].array.D[offset + ii]);
                    }
                }
                for(ii = 0; ii < xysize; ii++)
                {
                    sumarray[ii] += (double) data.image[IDcube].array.D[offset + ii];
                }
                break;

            case _DATATYPE_INT16:
                ptrv = (char *) data.image[ID].array.SI16;
                ptrv += sizeof(uint16_t) * k1 * xysize;
                ptrcv = (char *) data.image[IDcube].array.SI16;
                ptrcv += sizeof(uint16_t) * k * xysize;
                memcpy(ptrcv, ptrv, sizeof(uint16_t)*xysize);
                if(mode > 0)
                {
                    for(ii = 0; ii < xysize; ii++)
                    {
                        sumsqarray[ii] += (double)(data.image[IDcube].array.SI16[offset + ii] *
                                                   data.image[IDcube].array.SI16[offset + ii]);
                    }
                }
                for(ii = 0; ii < xysize; ii++)
                {
                    sumarray[ii] += (double) data.image[IDcube].array.SI16[offset + ii];
                }
                break;

            case _DATATYPE_UINT16:
                ptrv = (char *) data.image[ID].array.UI16;
                ptrv += sizeof(uint16_t) * k1 * xysize;
                ptrcv = (char *) data.image[IDcube].array.UI16;
                ptrcv += sizeof(uint16_t) * k * xysize;
                memcpy(ptrcv, ptrv, sizeof(uint16_t)*xysize);
                if(mode > 0)
                {
                    for(ii = 0; ii < xysize; ii++)
                    {
                        sumsqarray[ii] += (double)(data.image[IDcube].array.UI16[offset + ii] *
                                                   data.image[IDcube].array.UI16[offset + ii]);
                    }
                }
                for(ii = 0; ii < xysize; ii++)
                {
                    sumarray[ii] += (double) data.image[IDcube].array.UI16[offset + ii];
                }
                break;

            default :
                printf("ERROR: Data type not supported for function IMAGE_BASIC_streamaverage\n");
                exit(EXIT_FAILURE);
                break;
        }

        k++;
    }
    //  printf("\n Processing...\n");
    //  fflush(stdout);



    for(ii = 0; ii < xysize; ii++)
    {
        data.image[IDout].array.F[ii] = (float)(sumarray[ii] / k);
    }

    if(mode > 0)
    {
        for(ii = 0; ii < xysize; ii++)
        {
            data.image[IDrms].array.F[ii] = (float) sqrt(sumsqarray[ii] / k -
                                            (sumarray[ii] / k) * (sumarray[ii] / k));
        }
    }

    if(mode == 2)
    {
        IDbadpix = create_2Dimage_ID("badpixmap", xsize, ysize);

        // RMS
        // measure median pixel stddev
        rmsmean = img_percentile_float("imgstreamrms", 0.5);
        vmin = 0.3 * rmsmean;
        vmax = 3.0 * rmsmean;
        for(ii = 0; ii < xysize; ii++)
        {
            if(data.image[IDrms].array.F[ii] < vmin)
            {
                data.image[IDbadpix].array.F[ii] = 1.0;
            }
            if(data.image[IDrms].array.F[ii] > vmax)
            {
                data.image[IDbadpix].array.F[ii] = 1.0;
            }
        }
        // DARK
        darkp20 = img_percentile_float(IDoutname, 0.1);
        darkp80 = img_percentile_float(IDoutname, 0.9);
        vmin = darkp20 - 5.0 * (darkp80 - darkp20);
        vmax = darkp80 + 5.0 * (darkp80 - darkp20);
        for(ii = 0; ii < xysize; ii++)
        {
            if(data.image[IDout].array.F[ii] < vmin)
            {
                data.image[IDbadpix].array.F[ii] = 1.0;
            }
            if(data.image[IDout].array.F[ii] > vmax)
            {
                data.image[IDbadpix].array.F[ii] = 1.0;
            }
        }
    }

    free(sumarray);
    if(mode > 0)
    {
        free(sumsqarray);
    }

    return(IDout);
}







// feed image to data stream
// only works on slice #1 out output
long IMAGE_BASIC_streamfeed(
    const char *IDname,
    const char *streamname,
    float       frequ
)
{
    imageID ID, IDs;
    long xsize, ysize, xysize, zsize;
    long k;
    long tdelay;
    int RT_priority = 95; //any number from 0-99
    struct sched_param schedpar;
    int semval;
    const char *ptr0;
    const char *ptr1;
    int loopOK;
    long ii;

    schedpar.sched_priority = RT_priority;
#ifndef __MACH__
    if(seteuid(data.euid) != 0)     //This goes up to maximum privileges
    {
        PRINT_ERROR("seteuid error");
    }
    sched_setscheduler(0, SCHED_FIFO,
                       &schedpar); //other option is SCHED_RR, might be faster
    if(seteuid(data.ruid) != 0)     //Go back to normal privileges
    {
        PRINT_ERROR("seteuid error");
    }
#endif

    ID = image_ID(IDname);
    xsize = data.image[ID].md[0].size[0];
    ysize = data.image[ID].md[0].size[1];
    xysize = xsize * ysize;

    tdelay = (long)(1000000.0 / frequ);

    printf("frequ = %f Hz\n", frequ);
    printf("tdelay = %ld us\n", tdelay);

    IDs = image_ID(streamname);
    if((xsize != data.image[IDs].md[0].size[0])
            || (ysize != data.image[IDs].md[0].size[1]))
    {
        printf("ERROR: images have different x and y sizes");
        exit(0);
    }
    zsize = data.image[ID].md[0].size[2];

    ptr1 = (char *) data.image[IDs].array.F; // destination



    if(sigaction(SIGINT, &data.sigact, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    if(sigaction(SIGTERM, &data.sigact, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    if(sigaction(SIGBUS, &data.sigact, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    if(sigaction(SIGSEGV, &data.sigact, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    if(sigaction(SIGABRT, &data.sigact, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    if(sigaction(SIGHUP, &data.sigact, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    if(sigaction(SIGPIPE, &data.sigact, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }


    k = 0;
    loopOK = 1;
    while(loopOK == 1)
    {
        ptr0 = (char *) data.image[ID].array.F;
        ptr0 += sizeof(float) * xysize * k;
        data.image[IDs].md[0].write = 1;
        memcpy((void *) ptr1, (void *) ptr0, sizeof(float)*xysize);

        data.image[IDs].md[0].write = 0;
        data.image[IDs].md[0].cnt0++;
        COREMOD_MEMORY_image_set_sempost_byID(IDs, -1);

        usleep(tdelay);
        k++;
        if(k == zsize)
        {
            k = 0;
        }

        if((data.signal_INT == 1) || (data.signal_TERM == 1) || (data.signal_ABRT == 1)
                || (data.signal_BUS == 1) || (data.signal_SEGV == 1) || (data.signal_HUP == 1)
                || (data.signal_PIPE == 1))
        {
            loopOK = 0;
        }
    }


    data.image[IDs].md[0].write = 1;
    for(ii = 0; ii < xysize; ii++)
    {
        data.image[IDs].array.F[ii] = 0.0;
    }
    if(data.image[IDs].md[0].sem > 0)
    {
        sem_getvalue(data.image[IDs].semptr[0], &semval);
        if(semval < SEMAPHORE_MAXVAL)
        {
            sem_post(data.image[IDs].semptr[0]);
        }
    }
    data.image[IDs].md[0].write = 0;
    data.image[IDs].md[0].cnt0++;


    return(0);
}



// works only for floats
//
imageID IMAGE_BASIC_streamrecord(
    const char *streamname,
    long        NBframes,
    const char *IDname
)
{
    imageID ID;
    imageID IDstream;
    long xsize, ysize, zsize, xysize;
    unsigned long cnt;
    long waitdelayus = 50;
    long kk;
    char *ptr;


    IDstream = image_ID(streamname);
    xsize = data.image[IDstream].md[0].size[0];
    ysize = data.image[IDstream].md[0].size[1];
    zsize = NBframes;
    xysize = xsize * ysize;

    ID = create_3Dimage_ID(IDname, xsize, ysize, zsize);
    cnt = data.image[IDstream].md[0].cnt0;

    kk = 0;

    ptr = (char *) data.image[ID].array.F;
    while(kk != NBframes)
    {
        while(cnt > data.image[IDstream].md[0].cnt0)
        {
            usleep(waitdelayus);
        }

        cnt++;

        printf("\r%ld / %ld  [%ld %ld]      ", kk, NBframes, cnt,
               data.image[ID].md[0].cnt0);
        fflush(stdout);

        memcpy(ptr, data.image[IDstream].array.F, sizeof(float)*xysize);
        ptr += sizeof(float) * xysize;
        kk++;
    }
    printf("\n\n");

    return(ID);
}


