/** @file streamave.c
 */

#include <math.h>
#include <sched.h>


#include "CommandLineInterface/CLIcore.h"
#include "COREMOD_memory/COREMOD_memory.h"
#include "info/info.h"

// ==========================================
// Forward declaration(s)
// ==========================================

imageID IMAGE_BASIC_streamaverage(
    const char *__restrict__ IDname,
    long        NBcoadd,
    const char *__restrict__ IDoutname,
    int         mode,
    int         semindex
);



// ==========================================
// Command line interface wrapper function(s)
// ==========================================


static errno_t image_basic_streamaverage_cli()
{
    if(0
            + CLI_checkarg(1, 4)
            + CLI_checkarg(2, 2)
            + CLI_checkarg(3, 3)
            + CLI_checkarg(4, 2) +
            CLI_checkarg(5, 2)
            == 0)
    {
        IMAGE_BASIC_streamaverage(
            data.cmdargtoken[1].val.string,
            data.cmdargtoken[2].val.numl,
            data.cmdargtoken[3].val.string,
            data.cmdargtoken[4].val.numl,
            data.cmdargtoken[5].val.numl);
        return CLICMD_SUCCESS;
    }
    else
    {
        return CLICMD_INVALID_ARG;
    }
}




// ==========================================
// Register CLI command(s)
// ==========================================

errno_t __attribute__ ((cold)) streamave_addCLIcmd()
{

    RegisterCLIcommand(
        "imgstreamave",
        __FILE__,
        image_basic_streamaverage_cli,
        "average stream of images",
        "imgstreamave <imin> <NBcoadd [long]> <imout> <mode> <semindex>",
        "imgstreamave im 100 imave 0 -1",
        "long IMAGE_BASIC_streamaverage(const char *IDname, long NBcoadd, const char *IDoutname, int mode, int semindex)"
    );

    return RETURN_SUCCESS;
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
    const char *__restrict__ IDname,
    long        NBcoadd,
    const char *__restrict__ IDoutname,
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
    if(imsize == NULL) {
        PRINT_ERROR("malloc returns NULL pointer");
        abort();
    }
    imsize[0] = xsize;
    imsize[1] = ysize;
    imsize[2] = NBcoadd;
    datatype = data.image[ID].md[0].datatype;

    if(mode > 0)
    {
        create_2Dimage_ID("imgstreamrms", xsize, ysize, &IDrms);
        sumsqarray = calloc(xsize * ysize, sizeof(double));
        if(sumsqarray == NULL) {
            PRINT_ERROR("calloc returns NULL pointer");
            abort();
        }
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
            delete_image_ID("tmpstrcoadd", DELETE_IMAGE_ERRMODE_WARNING);
            createim = 1;
        }
    }
    else
    {
        createim = 1;
    }

    if(createim == 1)
    {
        create_image_ID("tmpstrcoadd", 3, imsize, datatype, 0, 0, 0, &IDcube);
    }

    free(imsize);

    create_2Dimage_ID(IDoutname, xsize, ysize, &IDout);
    sumarray = calloc(xsize * ysize, sizeof(double));
    if(sumarray == NULL) {
        PRINT_ERROR("calloc returns NULL pointer");
        abort();
    }


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
        create_2Dimage_ID("badpixmap", xsize, ysize, &IDbadpix);

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

    return IDout;
}




