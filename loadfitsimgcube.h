/** @file loadfitsimgcube.h
 */

errno_t __attribute__((cold)) loadfitsimgcube_addCLIcmd();


long load_fitsimages_cube(
    const char *__restrict__ strfilter,
    const char *__restrict__ ID_out_name
);
